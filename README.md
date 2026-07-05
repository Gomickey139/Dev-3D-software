# Dev-3D-software
## ビルド(Linux)
```
mkdir -p build && cd build
rm -rf ./*
cmake ..
make
../bin/My3DApp
```

## ビルド(Windows)
```
mkdir -p build_win && cd build_win
rm -rf ./*
cmake .. -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
make
```
