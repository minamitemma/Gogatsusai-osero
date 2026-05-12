# AI エンジン統合ガイド

このドキュメントは、6×6 オセロAIエンジンをReversi GUIに統合するための手順を説明します。

## 概要

Reversi GUI では、AIエンジンはプラグイン形式で統合されます。
抽象インターフェース `IAIEngine` を実装することで、
任意のAIエンジンを簡単に接続できます。

## インターフェース仕様

### `IAIEngine` クラス

```cpp
class IAIEngine {
public:
    virtual std::pair<int, int> getNextMove(
        const std::vector<std::vector<int>> &board,
        int player
    ) = 0;
    
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
};
```

### パラメータ説明

#### `getNextMove()`

**入力:**
- `board`: 6×6 のゲーム盤面
  - `0`: 空きマス
  - `1`: 黒い石
  - `2`: 白い石

- `player`: 現在のプレイヤー
  - `1`: 黒 (Black)
  - `2`: 白 (White)

**戻り値:**
- `std::pair<int, int>`: 推奨される次の手 `(row, col)`

**例:**
```cpp
// 盤面の配置
// [0][0] [0][1] [0][2] ... [0][5]
// [1][0] [1][1] ...
// ...
// [5][0] ...     ...       [5][5]

// (2, 3) に石を打つ
return {2, 3};
```

#### `initialize()`

AIエンジンの初期化処理を行います。
ウェイト、リソース取得、モデルロード等が必要な場合はここで。

#### `shutdown()`

AIエンジンのクリーンアップ処理を行います。

---

## 実装手順

### 1. AIエンジンクラスを作成

```cpp
// myaiengine.h
#include "aiengine.h"

class MyAIEngine : public IAIEngine {
public:
    std::pair<int, int> getNextMove(
        const std::vector<std::vector<int>> &board,
        int player
    ) override;
    
    void initialize() override;
    void shutdown() override;
    
private:
    // AI固有のメンバー変数
    // 例: ニューラルネットワークモデル、評価関数等
};
```

### 2. `getNextMove()` を実装

盤面とプレイヤー情報から、最善手を返します。

```cpp
std::pair<int, int> MyAIEngine::getNextMove(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    // 1. 打てる場所を列挙
    std::vector<std::pair<int, int>> validMoves = getValidMoves(board, player);
    
    // 2. 各手を評価
    int bestScore = -999999;
    std::pair<int, int> bestMove = validMoves[0];
    
    for (const auto &move : validMoves) {
        int score = evaluateMove(board, move.first, move.second, player);
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    
    // 3. 最善手を返す
    return bestMove;
}
```

### 3. GUIに統合

**`mainwindow.h` に追加:**
```cpp
#include "aiengine.h"

class MainWindow : public QMainWindow {
    // ...
private:
    IAIEngine *aiEngine;  // AI エンジンポインタ
};
```

**`mainwindow.cpp` のコンストラクタ:**
```cpp
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ...
    
    // AIエンジンの初期化
    aiEngine = new MyAIEngine(this);
    aiEngine->initialize();
    
    // ゲーム開始時にAIが黒を担当する場合の例
    if (isAIvsHuman) {
        QTimer::singleShot(500, this, &MainWindow::makeAIMove);
    }
}
```

### 4. AIの手を盤面に反映

```cpp
void MainWindow::makeAIMove()
{
    GameState state = gameManager->getGameState();
    
    // AIから次の手を取得
    auto [row, col] = aiEngine->getNextMove(state.board, state.currentPlayer);
    
    // ゲームロジックに反映
    gameManager->makeMove(row, col);
    
    // UI更新
    updateGameInfo();
}
```

### 5. CMakeLists.txt に統合

```cmake
set(PROJECT_SOURCES
    # ... 既存のファイル ...
    src/myaiengine.cpp
    src/myaiengine.h
)
```

---

## パフォーマンス要件

### レスポンス時間
- **10 秒以内** に次の手を返す必要があります
  - ゲーム内で10秒のタイムリミットが設定されているため

### メモリ使用量
- **256MB 以下** が望ましい
  - モバイルやゲーム機での実行を視野に入れているため

### 思考深さ
- 推奨: 6~8 手先まで評価

---

## テスト

### ユニットテスト例

```cpp
#include <gtest/gtest.h>
#include "myaiengine.h"

TEST(MyAIEngine, ReturnsValidMove) {
    MyAIEngine engine;
    engine.initialize();
    
    std::vector<std::vector<int>> board(6, std::vector<int>(6, 0));
    // 初期盤面を設定...
    
    auto [row, col] = engine.getNextMove(board, 1);
    
    EXPECT_GE(row, 0);
    EXPECT_LT(row, 6);
    EXPECT_GE(col, 0);
    EXPECT_LT(col, 6);
    
    engine.shutdown();
}
```

---

## トラブルシューティング

### コンパイルエラー

**エラー**: `IAIEngine` が見つからない
```
解決: #include "aiengine.h" を確認してください
```

### 実行時エラー

**エラー**: AIが無限ループに陥る
```
原因: getNextMove() が返らない可能性
解決: タイムアウト機能を実装してください
```

### パフォーマンス問題

**問題**: ゲームの反応が遅い
```
解決: 
1. AI の思考時間を計測
2. 評価関数を簡略化
3. 并列処理を検討
```

---

## 参考リソース

- [オセロの戦略](https://ja.wikipedia.org/wiki/%E3%82%AA%E3%82%BB%E3%83%AD)
- [ミニマックス法](https://ja.wikipedia.org/wiki/%E3%83%9F%E3%83%8B%E3%83%9E%E3%83%83%E3%82%AF%E3%82%B9)
- [AlphaBeta探索](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning)

---

## サポート

質問や問題がある場合は、GitHub Issues で報告してください。
