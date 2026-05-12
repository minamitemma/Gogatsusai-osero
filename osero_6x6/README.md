# osero_6x6

6x6 オセロ (リバーシ) を min-max AI と対戦できる C++ コマンドラインプログラム。
元の 8x8 サンプル (`../reversi_sample_cpp-master/`) を参考に、盤面サイズと AI を入れ替えたもの。

GUI は今後 `gui/` 等を別途追加する予定。コアのゲームロジック(`board.hpp`/`board.cpp`)と
プレイヤー抽象(`player.hpp`)は GUI からも再利用できるように分離してある。

## ビルド

```
mkdir -p build && cd build
cmake ..
make -j4
```

デバッグビルド:

```
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## 実行

```
./reversi6 BLACK_PLAYER_TYPE WHITE_PLAYER_TYPE
```

プレイヤー種別:

- `human`: 標準入力から `c2` のように 列(a-f) + 行(1-6) で指定
- `sample`: 合法手の最初を指す弱いコンピュータ (動作確認用)
- `minmax`: α-β + 反復深化 (1手3秒制限) の min-max AI

例: 人間(黒) vs AI(白)

```
./reversi6 human minmax
```

## ファイル構成

```
osero_6x6/
├── CMakeLists.txt
├── board.hpp / board.cpp        … 6x6 盤面・合法手・反転処理
├── player.hpp                   … プレイヤー抽象基底クラス
├── option_parser.hpp / .cpp     … コマンドライン引数
├── main.cpp                     … ゲームループ
└── player/
    ├── human_player.hpp / .cpp        … 標準入力プレイヤー
    ├── sample_computer_player.hpp/cpp … 弱いサンプル AI
    └── minmax_player.hpp / .cpp       … min-max AI
```

## min-max AI の方針

- 評価関数: 位置の重み付け表 + 着手可能数 (`own_mobility - opp_mobility`)
- 残り空きマス ≤ 10 で終盤評価 (石差を最優先) に切替
- 反復深化で深さ 1, 2, 3, ... と探索を伸ばし、3 秒の制限時間で打ち切り。
  直前に完了した深さのベスト手を採用
- α-β 枝刈り + 位置重み順の手並べ替えで効率化

制限時間は `MinmaxPlayer` のコンストラクタ引数で変更可。
