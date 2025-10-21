#include <memory>
#include <vector>

extern "C" {
#include "sim.h"
}

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static FunctionCallee ext(Module& M, const char* n, Type* r, ArrayRef<Type*> a) {
  return M.getOrInsertFunction(n, FunctionType::get(r, a, false));
}
static Value* gep2D(IRBuilder<>& B, LLVMContext& C, Value* base,
                    unsigned H, unsigned W, Value* y, Value* x) {
  Value* idx[3] = { ConstantInt::get(Type::getInt32Ty(C),0), y, x };
  return B.CreateInBoundsGEP(ArrayType::get(ArrayType::get(Type::getInt32Ty(C),W),H), base, idx);
}

int main() {
  constexpr int CELL = 3;
  constexpr int W = SIM_X_SIZE / CELL;
  constexpr int H = SIM_Y_SIZE / CELL;
  constexpr int STEPS_PER_FRAME = 4;
  constexpr int SOURCES = 4;
  constexpr int COOLING = 1;

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  LLVMContext C;
  auto M = std::make_unique<Module>("app3_ir", C);
  IRBuilder<> B(C);

  Type* i32 = Type::getInt32Ty(C);
  Type* voidTy = Type::getVoidTy(C);

  auto fPut   = ext(*M, "simPutPixel", voidTy, {i32,i32,i32});
  auto fFlush = ext(*M, "simFlush",    voidTy, {});
  auto fRand  = ext(*M, "simRand",     i32,    {});

  // Буферы и массивы источников
  auto arrW  = ArrayType::get(i32, W);
  auto arrHW = ArrayType::get(arrW, H);
  auto* U0 = new GlobalVariable(*M, arrHW, false, GlobalValue::InternalLinkage,
                                ConstantAggregateZero::get(arrHW), "U0");
  auto* U1 = new GlobalVariable(*M, arrHW, false, GlobalValue::InternalLinkage,
                                ConstantAggregateZero::get(arrHW), "U1");

  auto* A_i32_S = ArrayType::get(i32, SOURCES);
  auto* sx  = new GlobalVariable(*M, A_i32_S, false, GlobalValue::InternalLinkage,
                                 ConstantAggregateZero::get(A_i32_S), "sx");
  auto* sy  = new GlobalVariable(*M, A_i32_S, false, GlobalValue::InternalLinkage,
                                 ConstantAggregateZero::get(A_i32_S), "sy");
  auto* svx = new GlobalVariable(*M, A_i32_S, false, GlobalValue::InternalLinkage,
                                 ConstantAggregateZero::get(A_i32_S), "svx");
  auto* svy = new GlobalVariable(*M, A_i32_S, false, GlobalValue::InternalLinkage,
                                 ConstantAggregateZero::get(A_i32_S), "svy");
  auto* sr  = new GlobalVariable(*M, A_i32_S, false, GlobalValue::InternalLinkage,
                                 ConstantAggregateZero::get(A_i32_S), "sr");
  auto* st  = new GlobalVariable(*M, A_i32_S, false, GlobalValue::InternalLinkage,
                                 ConstantAggregateZero::get(A_i32_S), "st");

  // define void @app()
  auto* appTy = FunctionType::get(voidTy, false);
  auto* appFn = Function::Create(appTy, Function::ExternalLinkage, "app", M.get());
  auto* entry = BasicBlock::Create(C, "entry", appFn);
  B.SetInsertPoint(entry);

  auto c0    = ConstantInt::get(i32, 0);
  auto c1    = ConstantInt::get(i32, 1);
  auto cW    = ConstantInt::get(i32, W);
  auto cH    = ConstantInt::get(i32, H);
  auto c255  = ConstantInt::get(i32, 255);
  auto cCELL = ConstantInt::get(i32, CELL);
  auto cSRC  = ConstantInt::get(i32, SOURCES);
  auto cSteps= ConstantInt::get(i32, STEPS_PER_FRAME);
  auto cCooling = ConstantInt::get(i32, COOLING);
  auto cSIMX = ConstantInt::get(i32, SIM_X_SIZE);
  auto cSIMY = ConstantInt::get(i32, SIM_Y_SIZE);

  // Вспомогалки для массивов источников
  auto loadArr = [&](GlobalVariable* gv, Value* idx)->Value*{
    Value* ii[2] = { c0, idx };
    return B.CreateLoad(i32, B.CreateInBoundsGEP(A_i32_S, gv, ii));
  };
  auto storeArr = [&](GlobalVariable* gv, Value* idx, Value* v){
    Value* ii[2] = { c0, idx };
    B.CreateStore(v, B.CreateInBoundsGEP(A_i32_S, gv, ii));
  };

  // ===== Инициализация источников =====
  auto *I  = BasicBlock::Create(C,"init.i",appFn);
  auto *Ib = BasicBlock::Create(C,"init.body",appFn);
  auto *Ie = BasicBlock::Create(C,"init.end",appFn);
  B.CreateBr(I);

  B.SetInsertPoint(I);
  auto* i = B.CreatePHI(i32,2);
  i->addIncoming(c0, entry);
  B.CreateCondBr(B.CreateICmpSLT(i,cSRC), Ib, Ie);

  B.SetInsertPoint(Ib);
  auto rRaw = B.CreateURem(B.CreateCall(fRand), ConstantInt::get(i32,9));
  auto rVal = B.CreateAdd(rRaw, ConstantInt::get(i32,4));
  auto tVal = B.CreateAdd(B.CreateAnd(B.CreateCall(fRand), ConstantInt::get(i32,63)),
                          ConstantInt::get(i32,176));
  storeArr(sr, i, rVal);
  storeArr(st, i, tVal);

  auto xmin = B.CreateAdd(ConstantInt::get(i32,1), rVal);
  auto xmax = B.CreateSub(B.CreateSub(cW, ConstantInt::get(i32,2)), rVal);
  auto ymin = B.CreateAdd(ConstantInt::get(i32,1), rVal);
  auto ymax = B.CreateSub(B.CreateSub(cH, ConstantInt::get(i32,2)), rVal);
  auto badX = B.CreateICmpSLT(xmax, xmin);
  xmax = B.CreateSelect(badX, B.CreateSub(cW, ConstantInt::get(i32,2)), xmax);
  xmin = B.CreateSelect(badX, ConstantInt::get(i32,1), xmin);
  auto badY = B.CreateICmpSLT(ymax, ymin);
  ymax = B.CreateSelect(badY, B.CreateSub(cH, ConstantInt::get(i32,2)), ymax);
  ymin = B.CreateSelect(badY, ConstantInt::get(i32,1), ymin);

  auto xspan = B.CreateAdd(B.CreateSub(xmax, xmin), c1);
  auto yspan = B.CreateAdd(B.CreateSub(ymax, ymin), c1);
  auto sx0   = B.CreateAdd(xmin, B.CreateURem(B.CreateCall(fRand), xspan));
  auto sy0   = B.CreateAdd(ymin, B.CreateURem(B.CreateCall(fRand), yspan));
  storeArr(sx, i, sx0);
  storeArr(sy, i, sy0);

  auto vx = B.CreateSelect(B.CreateICmpEQ(B.CreateAnd(B.CreateCall(fRand), c1), c1),
                           c1, ConstantInt::get(i32,-1));
  auto vy = B.CreateSelect(B.CreateICmpEQ(B.CreateAnd(B.CreateCall(fRand), c1), c1),
                           c1, ConstantInt::get(i32,-1));
  storeArr(svx, i, vx);
  storeArr(svy, i, vy);

  auto inext = B.CreateAdd(i, c1);
  i->addIncoming(inext, Ib);
  B.CreateBr(I);

  // ===== Главный цикл по кадрам =====
  B.SetInsertPoint(Ie);
  auto *F   = BasicBlock::Create(C,"frame.i",appFn);
  auto *Fb  = BasicBlock::Create(C,"frame.b",appFn);
  auto *Fe  = BasicBlock::Create(C,"frame.e",appFn);
  B.CreateBr(F);

  B.SetInsertPoint(F);
  auto* f = B.CreatePHI(i32,2);
  f->addIncoming(c0, Ie);
  B.CreateCondBr(B.CreateICmpSLT(f, ConstantInt::get(i32,1000000)), Fb, Fe);

  // ---- Движение источников ----
  B.SetInsertPoint(Fb);
  auto *Mi = BasicBlock::Create(C,"move.i",appFn);
  auto *Mb = BasicBlock::Create(C,"move.b",appFn);
  auto *Me = BasicBlock::Create(C,"move.e",appFn);
  B.CreateBr(Mi);

  B.SetInsertPoint(Mi);
  auto* k = B.CreatePHI(i32,2);
  k->addIncoming(c0, Fb);
  B.CreateCondBr(B.CreateICmpSLT(k,cSRC), Mb, Me);

  B.SetInsertPoint(Mb);
  auto xi  = loadArr(sx,k);
  auto yi  = loadArr(sy,k);
  auto vxi = loadArr(svx,k);
  auto vyi = loadArr(svy,k);
  auto ri  = loadArr(sr,k);

  auto xi1 = B.CreateAdd(xi, vxi);
  auto yi1 = B.CreateAdd(yi, vyi);
  auto xmin2=B.CreateAdd(ConstantInt::get(i32,1), ri);
  auto xmax2=B.CreateSub(B.CreateSub(cW, ConstantInt::get(i32,2)), ri);
  auto ymin2=B.CreateAdd(ConstantInt::get(i32,1), ri);
  auto ymax2=B.CreateSub(B.CreateSub(cH, ConstantInt::get(i32,2)), ri);

  auto xLe = B.CreateICmpSLE(xi1, xmin2);
  auto xGe = B.CreateICmpSGE(xi1, xmax2);
  auto vxi2= B.CreateSelect(B.CreateOr(xLe,xGe), B.CreateSub(c0, vxi), vxi);
  auto xi2 = B.CreateSelect(xLe, xmin2, B.CreateSelect(xGe, xmax2, xi1));

  auto yLe = B.CreateICmpSLE(yi1, ymin2);
  auto yGe = B.CreateICmpSGE(yi1, ymax2);
  auto vyi2= B.CreateSelect(B.CreateOr(yLe,yGe), B.CreateSub(c0, vyi), vyi);
  auto yi2 = B.CreateSelect(yLe, ymin2, B.CreateSelect(yGe, ymax2, yi1));

  storeArr(sx,k,xi2); storeArr(sy,k,yi2);
  storeArr(svx,k,vxi2); storeArr(svy,k,vyi2);

  auto kn = B.CreateAdd(k,c1);
  k->addIncoming(kn, Mb);
  B.CreateBr(Mi);

  // ---- STEPS_PER_FRAME: HEAT(U0) -> DIFF(U1) -> EDGES(U1=0) -> COPY(U1->U0) ----
  B.SetInsertPoint(Me);
  auto *Si = BasicBlock::Create(C,"step.i",appFn);
  auto *Sb = BasicBlock::Create(C,"step.b",appFn);
  auto *Se = BasicBlock::Create(C,"step.e",appFn);
  B.CreateBr(Si);

  B.SetInsertPoint(Si);
  auto* s = B.CreatePHI(i32,2);
  s->addIncoming(c0, Me);
  B.CreateCondBr(B.CreateICmpSLT(s,cSteps), Sb, Se);

  // === HEAT на U0 (диски) ===
  B.SetInsertPoint(Sb);
  auto *Hi = BasicBlock::Create(C,"heat.i",appFn);
  auto *Hb = BasicBlock::Create(C,"heat.b",appFn);
  auto *He = BasicBlock::Create(C,"heat.e",appFn);
  B.CreateBr(Hi);

  B.SetInsertPoint(Hi);
  auto* h = B.CreatePHI(i32,2);
  h->addIncoming(c0, Sb);
  B.CreateCondBr(B.CreateICmpSLT(h,cSRC), Hb, He);

  B.SetInsertPoint(Hb);
  {
    auto cx = loadArr(sx,h), cy = loadArr(sy,h), rr = loadArr(sr,h), tt = loadArr(st,h);
    auto r2 = B.CreateMul(rr, rr);

    auto y0 = B.CreateSub(cy, rr);
    y0 = B.CreateSelect(B.CreateICmpSLT(y0, ConstantInt::get(i32,1)), ConstantInt::get(i32,1), y0);
    auto y1 = B.CreateAdd(cy, rr);
    y1 = B.CreateSelect(B.CreateICmpSGT(y1, B.CreateSub(cH, ConstantInt::get(i32,2))),
                        B.CreateSub(cH, ConstantInt::get(i32,2)), y1);
    auto x0 = B.CreateSub(cx, rr);
    x0 = B.CreateSelect(B.CreateICmpSLT(x0, ConstantInt::get(i32,1)), ConstantInt::get(i32,1), x0);
    auto x1 = B.CreateAdd(cx, rr);
    x1 = B.CreateSelect(B.CreateICmpSGT(x1, B.CreateSub(cW, ConstantInt::get(i32,2))),
                        B.CreateSub(cW, ConstantInt::get(i32,2)), x1);

    // y-loop
    auto *YI=BasicBlock::Create(C,"heat.y.i",appFn);
    auto *YB=BasicBlock::Create(C,"heat.y.b",appFn);
    auto *YE=BasicBlock::Create(C,"heat.y.e",appFn);
    B.CreateBr(YI);

    B.SetInsertPoint(YI);
    auto* yy = B.CreatePHI(i32,2);
    yy->addIncoming(y0, Hb);
    B.CreateCondBr(B.CreateICmpSLE(yy, y1), YB, YE);

    B.SetInsertPoint(YB);
    auto dy  = B.CreateSub(yy, cy);
    auto dy2 = B.CreateMul(dy, dy);

    // x-loop
    auto *XI=BasicBlock::Create(C,"heat.x.i",appFn);
    auto *XB=BasicBlock::Create(C,"heat.x.b",appFn);
    auto *XE=BasicBlock::Create(C,"heat.x.e",appFn);
    B.CreateBr(XI);

    B.SetInsertPoint(XI);
    auto* xx = B.CreatePHI(i32,2);
    xx->addIncoming(x0, YB);
    B.CreateCondBr(B.CreateICmpSLE(xx, x1), XB, XE);

    B.SetInsertPoint(XB);
    auto dx = B.CreateSub(xx, cx);
    auto inDisk = B.CreateICmpSLE(B.CreateAdd(B.CreateMul(dx,dx), dy2), r2);
    auto* p = gep2D(B,C,U0,H,W,yy,xx);
    auto  ov = B.CreateLoad(i32, p);
    auto  mv = B.CreateSelect(B.CreateICmpSLT(ov, tt), tt, ov);

    auto *WT = BasicBlock::Create(C,"heat.write",appFn);
    auto *CT = BasicBlock::Create(C,"heat.cont",appFn);
    B.CreateCondBr(inDisk, WT, CT);
    B.SetInsertPoint(WT); B.CreateStore(mv,p); B.CreateBr(CT);
    B.SetInsertPoint(CT);

    auto xxn = B.CreateAdd(xx, c1);
    xx->addIncoming(xxn, CT);
    B.CreateBr(XI);

    B.SetInsertPoint(XE);
    auto yyn = B.CreateAdd(yy, c1);
    yy->addIncoming(yyn, XE);
    B.CreateBr(YI);

    // ВАЖНО: backedge для h из YE, а не из Hb!
    B.SetInsertPoint(YE);
  }
  auto hn = B.CreateAdd(h,c1);
  h->addIncoming(hn, /* backedge */ B.GetInsertBlock()); // YE
  B.CreateBr(Hi);

  // === DIFF: U1 = u + (lap>>2) - COOLING (внутренние узлы) ===
  B.SetInsertPoint(He);
  auto *DyI=BasicBlock::Create(C,"diff.y.i",appFn);
  auto *DyB=BasicBlock::Create(C,"diff.y.b",appFn);
  auto *DyE=BasicBlock::Create(C,"diff.y.e",appFn);
  B.CreateBr(DyI);

  B.SetInsertPoint(DyI);
  auto* y = B.CreatePHI(i32,2);
  y->addIncoming(ConstantInt::get(i32,1), He);
  B.CreateCondBr(B.CreateICmpSLT(y, B.CreateSub(cH,c1)), DyB, DyE);

  B.SetInsertPoint(DyB);
  auto *DxI=BasicBlock::Create(C,"diff.x.i",appFn);
  auto *DxB=BasicBlock::Create(C,"diff.x.b",appFn);
  auto *DxE=BasicBlock::Create(C,"diff.x.e",appFn);
  B.CreateBr(DxI);

  B.SetInsertPoint(DxI);
  auto* x = B.CreatePHI(i32,2);
  x->addIncoming(ConstantInt::get(i32,1), DyB);
  B.CreateCondBr(B.CreateICmpSLT(x, B.CreateSub(cW,c1)), DxB, DxE);

  B.SetInsertPoint(DxB);
  {
    auto up = B.CreateLoad(i32, gep2D(B,C,U0,H,W, B.CreateSub(y,c1), x));
    auto dn = B.CreateLoad(i32, gep2D(B,C,U0,H,W, B.CreateAdd(y,c1), x));
    auto lf = B.CreateLoad(i32, gep2D(B,C,U0,H,W, y, B.CreateSub(x,c1)));
    auto rt = B.CreateLoad(i32, gep2D(B,C,U0,H,W, y, B.CreateAdd(x,c1)));
    auto ce = B.CreateLoad(i32, gep2D(B,C,U0,H,W, y, x));
    auto lap = B.CreateSub(B.CreateAdd(B.CreateAdd(B.CreateAdd(up,dn),lf),rt),
                           B.CreateMul(ce, ConstantInt::get(i32,4)));
    auto un  = B.CreateSub(B.CreateAdd(ce, B.CreateAShr(lap, ConstantInt::get(i32,2))), ConstantInt::get(i32, COOLING));
    auto u0  = B.CreateSelect(B.CreateICmpSLT(un, c0), c0, un);
    auto u1  = B.CreateSelect(B.CreateICmpSGT(u0, c255), c255, u0);
    B.CreateStore(u1, gep2D(B,C,U1,H,W, y, x));
  }
  auto xn = B.CreateAdd(x,c1);
  x->addIncoming(xn, DxB);
  B.CreateBr(DxI);

  B.SetInsertPoint(DxE);
  auto yn = B.CreateAdd(y,c1);
  y->addIncoming(yn, DxE);
  B.CreateBr(DyI);

  // === EDGES: U1 на границах = 0 ===
  B.SetInsertPoint(DyE);
  // top/bottom
  auto *EtI=BasicBlock::Create(C,"edge.t.i",appFn);
  auto *EtB=BasicBlock::Create(C,"edge.t.b",appFn);
  auto *EtE=BasicBlock::Create(C,"edge.t.e",appFn);
  B.CreateBr(EtI);

  B.SetInsertPoint(EtI);
  auto* ex = B.CreatePHI(i32,2);
  ex->addIncoming(c0, DyE);
  B.CreateCondBr(B.CreateICmpSLT(ex, cW), EtB, EtE);

  B.SetInsertPoint(EtB);
  B.CreateStore(c0, gep2D(B,C,U1,H,W, c0, ex));
  B.CreateStore(c0, gep2D(B,C,U1,H,W, B.CreateSub(cH,c1), ex));
  auto exn = B.CreateAdd(ex,c1);
  ex->addIncoming(exn, EtB);
  B.CreateBr(EtI);

  B.SetInsertPoint(EtE);
  // left/right
  auto *ElI=BasicBlock::Create(C,"edge.l.i",appFn);
  auto *ElB=BasicBlock::Create(C,"edge.l.b",appFn);
  auto *ElE=BasicBlock::Create(C,"edge.l.e",appFn);
  B.CreateBr(ElI);

  B.SetInsertPoint(ElI);
  auto* ey = B.CreatePHI(i32,2);
  ey->addIncoming(c0, EtE);
  B.CreateCondBr(B.CreateICmpSLT(ey, cH), ElB, ElE);

  B.SetInsertPoint(ElB);
  B.CreateStore(c0, gep2D(B,C,U1,H,W, ey, c0));
  B.CreateStore(c0, gep2D(B,C,U1,H,W, ey, B.CreateSub(cW,c1)));
  auto eyn = B.CreateAdd(ey,c1);
  ey->addIncoming(eyn, ElB);
  B.CreateBr(ElI);

  // === COPY: U1 -> U0 ===
  B.SetInsertPoint(ElE);
  auto *CyI=BasicBlock::Create(C,"cpy.y.i",appFn);
  auto *CyB=BasicBlock::Create(C,"cpy.y.b",appFn);
  auto *CyE=BasicBlock::Create(C,"cpy.y.e",appFn);
  B.CreateBr(CyI);

  B.SetInsertPoint(CyI);
  auto* cy = B.CreatePHI(i32,2);
  cy->addIncoming(c0, ElE);
  B.CreateCondBr(B.CreateICmpSLT(cy, cH), CyB, CyE);

  B.SetInsertPoint(CyB);
  auto *CxI=BasicBlock::Create(C,"cpy.x.i",appFn);
  auto *CxB=BasicBlock::Create(C,"cpy.x.b",appFn);
  auto *CxE=BasicBlock::Create(C,"cpy.x.e",appFn);
  B.CreateBr(CxI);

  B.SetInsertPoint(CxI);
  auto* cx = B.CreatePHI(i32,2);
  cx->addIncoming(c0, CyB);
  B.CreateCondBr(B.CreateICmpSLT(cx, cW), CxB, CxE);

  B.SetInsertPoint(CxB);
  auto vv = B.CreateLoad(i32, gep2D(B,C,U1,H,W, cy, cx));
  B.CreateStore(vv,          gep2D(B,C,U0,H,W, cy, cx));
  auto cxn = B.CreateAdd(cx,c1);
  cx->addIncoming(cxn, CxB);
  B.CreateBr(CxI);

  B.SetInsertPoint(CxE);
  auto cyn = B.CreateAdd(cy,c1);
  cy->addIncoming(cyn, CxE);
  B.CreateBr(CyI);

  // step++
  B.SetInsertPoint(CyE);
  auto sn = B.CreateAdd(s,c1);
  s->addIncoming(sn, CyE);
  B.CreateBr(Si);

  // ===== Рендер из U0 =====
  B.SetInsertPoint(Se);
  auto *RyI=BasicBlock::Create(C,"ry.i",appFn);
  auto *RyB=BasicBlock::Create(C,"ry.b",appFn);
  auto *RyE=BasicBlock::Create(C,"ry.e",appFn);
  B.CreateBr(RyI);

  B.SetInsertPoint(RyI);
  auto* gy = B.CreatePHI(i32,2);
  gy->addIncoming(c0, Se);
  B.CreateCondBr(B.CreateICmpSLT(gy, cH), RyB, RyE);

  B.SetInsertPoint(RyB);
  auto *RxI=BasicBlock::Create(C,"rx.i",appFn);
  auto *RxB=BasicBlock::Create(C,"rx.b",appFn);
  auto *RxE=BasicBlock::Create(C,"rx.e",appFn);
  B.CreateBr(RxI);

  B.SetInsertPoint(RxI);
  auto* gx = B.CreatePHI(i32,2);
  gx->addIncoming(c0, RyB);
  B.CreateCondBr(B.CreateICmpSLT(gx, cW), RxB, RxE);

  B.SetInsertPoint(RxB);
  {
    auto T  = B.CreateLoad(i32, gep2D(B,C,U0,H,W, gy, gx));
    auto Tc = B.CreateSelect(B.CreateICmpSLT(T,c0), c0,
                 B.CreateSelect(B.CreateICmpSGT(T,c255), c255, T));
    auto r  = Tc;
    auto g  = B.CreateLShr(Tc, ConstantInt::get(i32,1));
    auto b  = B.CreateSub(c255, Tc);
    auto color = B.CreateOr(
                   B.CreateOr(ConstantInt::get(i32,0xFF000000), B.CreateShl(r, ConstantInt::get(i32,16))),
                   B.CreateOr(B.CreateShl(g, ConstantInt::get(i32,8)), b));

    auto x0p = B.CreateMul(gx, cCELL);
    auto y0p = B.CreateMul(gy, cCELL);

    auto *PyI=BasicBlock::Create(C,"py.i",appFn);
    auto *PyB=BasicBlock::Create(C,"py.b",appFn);
    auto *PyE=BasicBlock::Create(C,"py.e",appFn);
    B.CreateBr(PyI);

    B.SetInsertPoint(PyI);
    auto* py = B.CreatePHI(i32,2);
    py->addIncoming(c0, RxB);
    auto ypix = B.CreateAdd(y0p, py);
    auto yOk  = B.CreateICmpSLT(ypix, cSIMY);
    B.CreateCondBr(B.CreateAnd(B.CreateICmpSLT(py, cCELL), yOk), PyB, PyE);

    B.SetInsertPoint(PyB);
    auto *PxI=BasicBlock::Create(C,"px.i",appFn);
    auto *PxB=BasicBlock::Create(C,"px.b",appFn);
    auto *PxE=BasicBlock::Create(C,"px.e",appFn);
    B.CreateBr(PxI);

    B.SetInsertPoint(PxI);
    auto* px = B.CreatePHI(i32,2);
    px->addIncoming(c0, PyB);
    auto xpix = B.CreateAdd(x0p, px);
    auto xOk  = B.CreateICmpSLT(xpix, cSIMX);
    B.CreateCondBr(B.CreateAnd(B.CreateICmpSLT(px, cCELL), xOk), PxB, PxE);

    B.SetInsertPoint(PxB);
    B.CreateCall(fPut, { xpix, ypix, color });
    auto pxn = B.CreateAdd(px,c1);
    px->addIncoming(pxn, PxB);
    B.CreateBr(PxI);

    B.SetInsertPoint(PxE);
    auto pyn = B.CreateAdd(py,c1);
    py->addIncoming(pyn, PxE);
    B.CreateBr(PyI);

    B.SetInsertPoint(PyE);
    auto gxn = B.CreateAdd(gx,c1);
    gx->addIncoming(gxn, PyE);
    B.CreateBr(RxI);
  }

  B.SetInsertPoint(RxE);
  auto gyn = B.CreateAdd(gy,c1);
  gy->addIncoming(gyn, RxE);
  B.CreateBr(RyI);

  B.SetInsertPoint(RyE);
  B.CreateCall(fFlush);
  auto fn = B.CreateAdd(f,c1);
  f->addIncoming(fn, RyE);
  B.CreateBr(F);

  // exit
  B.SetInsertPoint(Fe);
  B.CreateRetVoid();

  if (verifyModule(*M, &errs())) { errs()<<"IR verification failed\n"; return 1; }
  // M->print(outs(), nullptr);

  std::string err;
  EngineBuilder EB(std::move(M));
  EB.setEngineKind(EngineKind::JIT)
    .setMCJITMemoryManager(std::make_unique<SectionMemoryManager>());
  auto* EE = EB.setErrorStr(&err).create();
  if (!EE) { fprintf(stderr,"EE error: %s\n", err.c_str()); return 2; }

  EE->InstallLazyFunctionCreator([](const std::string& n)->void*{
    if(n=="simPutPixel") return (void*)simPutPixel;
    if(n=="simFlush")    return (void*)simFlush;
    if(n=="simRand")     return (void*)simRand;
    return nullptr;
  });
  EE->finalizeObject();

  simInit();
  std::vector<GenericValue> noargs;
  EE->runFunction(EE->FindFunctionNamed("app"), noargs);
  simExit();
  return 0;
}
