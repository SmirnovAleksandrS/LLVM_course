Сборка программы:
```bash
clang -std=c11 -O2 -c sim.c $(pkg-config --cflags sdl2)
```

```bash
clang++ -std=c++17 -O2 app_ir_gen.cpp sim.o \          
  $(llvm-config --cxxflags) \
  $(llvm-config --ldflags --system-libs --libs core mcjit native executionengine support) \
  $(pkg-config --libs sdl2) \
  -o app_ir
```

Запуск
```bash
./app_ir 
```