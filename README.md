# Face Shooting
3D空間で敵の攻撃を避けながら戦う、OpenGL製の3Dシューティングゲーム。

プレイヤーは弾を撃って敵のHPを削り、敵はフェーズ変化に応じて攻撃パターンを変化させる。

## 操作方法

| キー | 操作 |
|---|---|
| Enter | タイトルからゲーム開始 |
| W / A / S / D | 移動 |
| P | ショット |
| Space | 緊急回避 |
| Esc | 終了 |

## ビルド環境

- C++17
- CMake 3.14 以上
- OpenGL
- GLFW
- GLM

GLFW と GLM は CMake の `FetchContent` で取得します。
そのため、初回ビルド時にはインターネット接続が必要です。

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

## ディレクトリ構成
```text
.
├── assets/              # ゲームで使用する素材
│   ├── models/          # OBJ形式の3Dモデル
│   ├── shaders/         # GLSLシェーダー
│   └── textures/        # UI画像・テクスチャ
│
├── include/             # ヘッダファイル
├── src/                 # C++ソースコード
│
├── bin/                 # ビルド後の実行ファイル出力先
├── build/               # Linux向けビルド用ディレクトリ
├── build_win/           # Windows向けビルド用ディレクトリ
│
├── CMakeLists.txt       # CMake設定ファイル
└── README.md