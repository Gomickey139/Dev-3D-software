# .devcontainer/Dockerfile
FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# パッケージリスト更新とライブラリのインストール
# libglfw3-dev: ウィンドウ表示用
# libgl1-mesa-dev: OpenGL用
# libglm-dev: 数学計算用
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    cmake \
    build-essential \
    gdb \
    libglfw3-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libglm-dev \
    xorg-dev