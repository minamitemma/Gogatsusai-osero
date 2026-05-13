# Reversi GUI - オセロゲーム GUI版

6×6 オセロゲームのQt6ベースのデスクトップGUアプリケーションです。

## 機能一覧

### 実装済み（基本プロトタイプ）
- ✅ 6×6 ゲーム盤面表示
- ✅ 石の配置・選択
- ✅ 基本的なゲームロジック
- ✅ プレイヤー交代
- ✅ 打てる場所の表示（黄色のハイライト）
- ✅ アバター表示エリア（絵文字）
- ✅ ヒントボタン（残数表示）

### 開発予定
- ⏳ 1手10秒の時間制限 (minamitemma)
- ⏳ AI対戦機能 (sugomori の6×6対戦AIを統合)
- ⏳ ヒント機能 (goto のヒント用AIを使用、3回制限)
- ⏳ 盤面説明API (goto が実装、ゲーム進行の解説)
- ⏳ 評価値に応じたアバター表情変更
- ⏳ ランキング機能（石数差ベース）

## 必要な環境

- Qt 6.x
- CMake 3.21 以上
- C++17 対応コンパイラ

## ビルド方法

### Windows (Visual Studio)
```bash
# ビルドディレクトリを作成
mkdir build
cd build

# CMakeでプロジェクトを生成
cmake -G "Visual Studio 17 2022" ..

# ビルド
cmake --build . --config Release
```

### Linux / macOS
```bash
mkdir build
cd build
cmake ..
make
```

## 実行

```bash
# ビルドディレクトリから
./ReversiGUI  # Linux/macOS
ReversiGUI.exe  # Windows
```

## プロジェクト構造

```
reversi_gui/
├── CMakeLists.txt              # CMakeBuild設定
├── README.md                    # このファイル
├── src/
│   ├── main.cpp                # エントリーポイント
│   ├── mainwindow.h/cpp        # メインウィンドウ
│   ├── boardwidget.h/cpp       # 盤面表示ウィジェット
│   ├── gamemanager.h/cpp       # ゲームロジック
│   └── ...
└── resources/
    └── resources.qrc           # リソースファイル
```

## 今後の開発計画

1. **時間制限機能**: 各ターンで10秒以内に決定
2. **AI統合**: 共同開発者のAIエンジンを組み込み
3. **外部API統合**:
   - ヒント機能（オンラインAI）
   - 盤面説明API
4. **UIの洗練**:
   - アバター画像/アニメーション
   - サウンドエフェクト
5. **ランキング機能**: スコア保存・表示

## ライセンス

TBD

## 開発者

- **GUI開発**: minamitemma
- **対戦用オセロAI**: sugomori
- **説明用・ヒント用AI**: goto
