// PassInstrTrace.cpp
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/InlineAsm.h"

using namespace llvm;

struct MyModPass : public PassInfoMixin<MyModPass> {
  Type *voidTy;
  Type *i8PtrTy;
  Type *i64Ty;

  // --- имена рантайм-логгеров (и старых тоже), чтобы не инструментировать их самих
  bool isFuncLogger(StringRef name) const {
    return name == "binOptLogger" || name == "callLogger" ||
           name == "funcStartLogger" || name == "funcEndLogger" ||
           name == "resIntLogger" || name == "instExecLogger" ||
           name == "useLogger";
  }

  // --- фильтр "только для логического модуля app.c"
  bool fromAppC(Function &F) const {
    if (auto *SP = F.getSubprogram()) {
      StringRef File = SP->getFile()->getFilename();
      // Чаще всего здесь лежит базовое имя ("app.c") или полный путь; проверим суффикс.
      if (File.ends_with("app3.c"))
        return true;
      // Иногда Clang кладёт полный путь в directory/filename:
      // StringRef Dir  = SP->getFile()->getDirectory();
      // if ((Dir + "/" + File).endswith("app3.c"))
      //   return true;
      return false;
    }
    // Если нет debug-инфы, считаем, что фильтра нет (лучше собрать, чем пропустить)
    return true;
  }

  // --- подготовка деклараций логгеров
  FunctionCallee getInstExecLogger(Module &M) const {
    ArrayRef<Type*> ps = {i8PtrTy, i8PtrTy, i8PtrTy, i64Ty};
    return M.getOrInsertFunction("instExecLogger",
                                 FunctionType::get(voidTy, ps, false));
  }
  FunctionCallee getUseLogger(Module &M) const {
    ArrayRef<Type*> ps = {i64Ty, i64Ty, i8PtrTy};
    return M.getOrInsertFunction("useLogger",
                                 FunctionType::get(voidTy, ps, false));
  }

  // --- служебные функции логов начала/конца (оставил, чтобы "не уходить далеко")
  bool insertFuncStartLog(Module &M, Function &F, IRBuilder<> &B) {
    ArrayRef<Type*> ps = {i8PtrTy};
    auto Callee = M.getOrInsertFunction("funcStartLogger",
                    FunctionType::get(voidTy, ps, false));
    BasicBlock &Entry = F.getEntryBlock();
    B.SetInsertPoint(&Entry.front());
    Value *funcName = B.CreateGlobalStringPtr(F.getName());
    B.CreateCall(Callee, {funcName});
    return true;
  }

  bool insertFuncEndLog(Module &M, Function &F, IRBuilder<> &B) {
    ArrayRef<Type*> ps = {i8PtrTy, i64Ty};
    auto Callee = M.getOrInsertFunction("funcEndLogger",
                    FunctionType::get(voidTy, ps, false));
    bool any = false;
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *Ret = dyn_cast<ReturnInst>(&I)) {
          B.SetInsertPoint(Ret);
          Value *funcName = B.CreateGlobalStringPtr(F.getName());
          Value *id = ConstantInt::get(i64Ty, (uint64_t)(uintptr_t)&I);
          B.CreateCall(Callee, {funcName, id});
          any = true;
        }
      }
    }
    return any;
  }

  // --- основной инструментатор: трасса исполнения + трасса использования
  bool insertInstAndUseTrace(Module &M, Function &F, IRBuilder<> &B) {
    auto instExecLog = getInstExecLogger(M);
    auto useLog      = getUseLogger(M);

    bool inserted = false;

    for (auto &BB : F) {
      for (auto &I : BB) {
        // пропускаем наши же вызовы логгеров, декларации, дебажные интринсики и прочий "мусор"
        if (isa<DbgInfoIntrinsic>(&I)) continue;
        if (auto *CI = dyn_cast<CallBase>(&I)) {
          if (Function *CF = CI->getCalledFunction()) {
            if (CF && isFuncLogger(CF->getName())) continue;
          }
          if (CI->isInlineAsm()) continue;
        }

        // Для простоты не логируем исполнение самих PHI (их "исполнение" — на входе в BB),
        // но мы всё равно будем логировать их использование там, где ими пользуются другие
        // инструкции (если user не PHI).
        bool isPhi = isa<PHINode>(&I);

        // --- Трасса исполнения (кроме PHI): [I] func :: bb :: opcode {id}
        if (!isPhi) {
          // "выполнилась" -> ставим после инструкции, если это возможно
          B.SetInsertPoint(&I);
          if (!I.isTerminator()) {
            B.SetInsertPoint(&BB, ++B.GetInsertPoint());
          }
          Value *funcName = B.CreateGlobalStringPtr(F.getName());
          Value *bbName   = B.CreateGlobalStringPtr(BB.getName());
          Value *opName   = B.CreateGlobalStringPtr(I.getOpcodeName());
          Value *id       = ConstantInt::get(i64Ty, (uint64_t)(uintptr_t)&I);
          B.CreateCall(instExecLog, {funcName, bbName, opName, id});
          inserted = true;
        }

        // --- Трасса использования: для каждого операнда-Инструкции
        // пишем ребро `User <- Operand`, но если User — phi*, то пропускаем
        if (!isPhi) {
          // Вставляем рядом с логом исполнения (там же)
          for (Use &U : I.operands()) {
            if (auto *OpI = dyn_cast<Instruction>(U.get())) {
              // Operand может быть где угодно (в т.ч. в другом BB)
              Value *userID    = ConstantInt::get(i64Ty, (uint64_t)(uintptr_t)&I);
              Value *operandID = ConstantInt::get(i64Ty, (uint64_t)(uintptr_t)OpI);
              Value *userOp    = B.CreateGlobalStringPtr(I.getOpcodeName());
              B.CreateCall(useLog, {userID, operandID, userOp});
              inserted = true;
            }
          }
        }
      }
    }

    return inserted;
  }

  // --- Точка входа пасса
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    outs() << "[Module] " << M.getName() << '\n';

    LLVMContext &Ctx = M.getContext();
    IRBuilder<> B(Ctx);
    voidTy  = Type::getVoidTy(Ctx);
    i8PtrTy = Type::getInt8Ty(Ctx)->getPointerTo();
    i64Ty   = Type::getInt64Ty(Ctx);

    for (auto &F : M) {
      outs() << "[Function] " << F.getName() << " (arg_size: " << F.arg_size() << ")\n";
      if (F.isDeclaration() || isFuncLogger(F.getName()))
        continue;
      if (!fromAppC(F)) {
        outs() << "  skip (not from app.c)\n\n";
        continue;
      }

      // Немного "старого" поведения — лог входа/выхода функции
      insertFuncStartLog(M, F, B);
      insertInstAndUseTrace(M, F, B);
      insertFuncEndLog(M, F, B);

      bool bad = verifyFunction(F, &outs());
      outs() << "[VERIFICATION] " << (bad ? "FAIL\n\n" : "OK\n\n");
    }

    return PreservedAnalyses::none();
  }
};

PassPluginLibraryInfo getPassPluginInfo() {
  const auto callback = [](PassBuilder &PB) {
    // Втыкаем наш модульный пасс в самое начало пайплайна -O{1,2,3,s}
    PB.registerPipelineStartEPCallback([](ModulePassManager &MPM, auto) {
      MPM.addPass(MyModPass{});
      return true;
    });
  };

  return {LLVM_PLUGIN_API_VERSION, "MyPlugin", "0.0.2", callback};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getPassPluginInfo();
}
