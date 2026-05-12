# セットアップガイド

このドキュメントでは、Reversi GUI のセットアップと開発環境構築手順を説明します。

## 前提条件

### 必須
- **CMake**: 3.21 以上
  - ダウンロード: https://cmake.org/download/
  
- **Qt 6.x**
  - ダウンロード: https://www.qt.io/download
  - インストール時に必ず Visual Studio との統合を選択

- **Visual Studio 2022** (Windows の場合)
  - C++ 開発ツールをインストール

### 推奨
- **Visual Studio Code** + Qt Tools 拡張
- **Git** (バージョン管理用)

## インストール手順 (Windows)

### 1. Qt のインストール

1. [Qt Online Installer](https://www.qt.io/download-open-source) をダウンロード
2. インストーラーを実行
3. Qt 6.x LTS バージョンを選択
4. インストール先を記憶（例: `C:\Qt\6.x.x`）
5. Visual Studio 2022 統合を確認

### 2. CMake のインストール

1. [CMake ダウンロードページ](https://cmake.org/download/) から Windows 版をダウンロード
2. インストーラーを実行
3. 「Add CMake to the system PATH」をチェック

### 3. ビルド

```bash
cd reversi_gui
mkdir build
cd build

# Qt6_DIR を設定（Qt インストール先に合わせる）
cmake -G "Visual Studio 17 2022" -DQt6_DIR="C:\Qt\6.x.x\msvc2019_64\lib\cmake\Qt6" ..

cmake --build . --config Release
```

## インストール手順 (Linux)

### Ubuntu / Debian

```bash
# 必要なパッケージをインストール
sudo apt-get update
sudo apt-get install -y cmake qt6-base-dev

# ビルド
cd reversi_gui
mkdir build
cd build
cmake ..
make

# 実行
./ReversiGUI
```

### Fedora / CentOS

```bash
sudo dnf install cmake qt6-qtbase-devel

cd reversi_gui
mkdir build
cd build
cmake ..
make
./ReversiGUI
```

## インストール手順 (macOS)

```bash
# Homebrew を使用
brew install cmake qt

cd reversi_gui
mkdir build
cd build
cmake ..
make
./ReversiGUI
```

## トラブルシューティング

### Qt が見つからない
```bash
# Qt 6 のパスを指定
cmake -DQt6_DIR="/path/to/Qt6/lib/cmake/Qt6" ..
```

### MOC エラー
CMake の自動 MOC 処理が正常に動作していることを確認：
```cmake
set(CMAKE_AUTOMOC ON)
```

### コンパイルエラー
- C++17 以上をサポートするコンパイラを使用しているか確認
- Qt バージョンが 6.x であることを確認

## 開発時の便利なコマンド

```bash
# クリーンビルド
cd build
rm -rf *
cmake ..
make

# デバッグビルド
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## VS Code での開発

VS Code での開発を推奨します。以下の拡張をインストール：
- C/C++ (Microsoft)
- CMake Tools
- Qt Tools (if available)

## 更なる質問

問題が発生した場合は、GitHub Issues で報告してください。
