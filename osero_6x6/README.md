# osero_6x6

6x6 オセロ (リバーシ) を min-max AI と対戦できる C++ コマンドラインプログラム。
元の 8x8 サンプル (`../reversi_sample_cpp-master/`) を参考に、盤面サイズと AI を入れ替えたもの。

現在は CLI 版として、min-max AI、Gemini API を使ったヒント、JSON 出力、対局レビューを持つ。
GUI 側 (`../reversi_gui/`) から利用しやすいように、ヒントとレビューは構造化 JSON でも出力できる。

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
./reversi6 [OPTIONS] BLACK_PLAYER_TYPE WHITE_PLAYER_TYPE
```

プレイヤー種別:

- `human`: 標準入力から `c2` のように 列(a-f) + 行(1-6) で指定
- `sample`: 合法手の最初を指す弱いコンピュータ (動作確認用)
- `minmax`: α-β + 反復深化 (1手3秒制限) の min-max AI

例: 人間(黒) vs AI(白)

```
./reversi6 human minmax
```

主なオプション:

- `--hint none|local|llm`: ヒント機能。デフォルトは `none`
- `--hint-format text|json`: ヒント出力形式。デフォルトは `text`
- `--review none|local|llm`: 終局後レビュー。デフォルトは `none`
- `--review-format text|json`: レビュー出力形式。デフォルトは `text`

例:

```
./reversi6 human minmax --hint local
./reversi6 human minmax --hint llm --hint-format json
./reversi6 human minmax --review llm
./reversi6 human minmax --review llm --review-format json
```

## ヒント機能

`--hint local` または `--hint llm` を指定すると、人間プレイヤーの入力時に `h` でヒントを出せる。
ヒントは人間プレイヤーごとに 3 回まで。

```
Where? (hint: h, remaining: 3) h
```

`local` は内部の min-max 探索結果をそのまま表示する。
`llm` は min-max の候補手・評価値・盤面情報を Gemini に渡し、自然な日本語の説明を生成する。
LLM には最善手の計算を任せず、計算済みの候補手を説明させる設計。

LLM 待機中は同じ行で以下のようなローディング表示を行う。

```
ちょっと待ってね・・・
```

### ヒント JSON

`--hint-format json` を指定すると、GUI で扱いやすい JSON を出力する。

```
./reversi6 human minmax --hint local --hint-format json
```

例:

```json
{
  "side": "black",
  "bestMove": "c2",
  "evaluationScore": 8,
  "searchDepth": 7,
  "positionSummary": "black perspective score: 8, minmax depth: 7",
  "explanation": "Minmax recommends c2 with score 24 at depth 7.",
  "candidates": [
    {"move": "c2", "score": 24},
    {"move": "d1", "score": 10}
  ]
}
```

- `evaluationScore`: 現在盤面の静的評価値
- `searchDepth`: min-max で完了した探索深さ
- `candidates[].score`: 各合法手を min-max で読んだ評価値
- `explanation`: `llm` の場合は Gemini の説明文

## LLM API

LLM はデフォルトで Gemini を使う。OpenAI API を使いたい場合は `LLM_PROVIDER=openai` を設定する。

### Gemini

Gemini を使う場合は、API キーを環境変数に設定する。

```
export GEMINI_API_KEY="your-api-key"
```

モデルはデフォルトで `gemini-3-flash-preview`。
変更したい場合は `GEMINI_MODEL` を設定する。

```
export GEMINI_MODEL="gemini-3-pro-preview"
```

### OpenAI

OpenAI API を使う場合:

```
export LLM_PROVIDER="openai"
export OPENAI_API_KEY="your-api-key"
```

モデルはデフォルトで `gpt-5.4-mini`。
変更したい場合は `OPENAI_MODEL` を設定する。

```
export OPENAI_MODEL="gpt-5.4-mini"
```

ヒント用プロンプトは `prompt.txt` を編集する。
以下のプレースホルダは実行時に現在の盤面情報へ置換される。

- `{{side}}`
- `{{board}}`
- `{{best_move}}`
- `{{evaluation_score}}`
- `{{search_depth}}`
- `{{candidates}}`

## 対局レビュー

`--review local` または `--review llm` を指定すると、終局後に対局レビューを表示する。

```
./reversi6 human minmax --review local
./reversi6 human minmax --review llm
```

対局中に以下を記録している。

- 手数
- 手番
- 着手またはパス
- 着手前後の黒視点評価値
- 着手者視点の評価変化
- 着手後の黒白石数
- 人間プレイヤーの色

`--review llm` では、これらのログを Gemini に渡して、人間プレイヤー目線の自然なレビュー文を生成する。
レビュー用プロンプトは `review_prompt.txt` を編集する。

### レビュー JSON

`--review-format json` を指定すると、レビューも JSON で出力できる。

```
./reversi6 human minmax --review llm --review-format json
```

例:

```json
{
  "humanSide": "black",
  "result": {
    "winner": "black",
    "blackCount": 20,
    "whiteCount": 16
  },
  "bestMove": {
    "turn": 8,
    "side": "black",
    "wasPass": false,
    "move": "c2",
    "blackEvalBefore": 4,
    "blackEvalAfter": 22,
    "deltaForMover": 18,
    "blackCountAfter": 7,
    "whiteCountAfter": 5
  },
  "worstMove": {
    "...": "..."
  },
  "summary": "LLM またはローカルレビュー文",
  "moves": [
    {"...": "全手のログ"}
  ]
}
```

## ファイル構成

```
osero_6x6/
├── CMakeLists.txt
├── board.hpp / board.cpp        … 6x6 盤面・合法手・反転処理
├── evaluator.hpp / .cpp         … 盤面評価関数 (位置重み + 機動力 + 終盤石差)
├── prompt.txt                   … Gemini ヒント用プロンプト
├── review_prompt.txt            … Gemini 対局レビュー用プロンプト
├── player.hpp                   … プレイヤー抽象基底クラス
├── option_parser.hpp / .cpp     … コマンドライン引数
├── main.cpp                     … ゲームループ + 形勢表示 + 対局ログ
├── hint/
│   ├── hint_engine.hpp          … ヒントエンジン抽象
│   ├── local_hint_engine.*      … min-max 候補手ヒント
│   ├── llm_hint_engine.*        … Gemini ヒント
│   ├── gemini_client.*          … Gemini API 呼び出し
│   ├── prompt_builder.*         … ヒントプロンプト生成
│   ├── hint_formatter.*         … text/json ヒント整形
│   └── minmax_hint_evaluator.*  … 候補手の min-max 評価
├── review/
│   ├── game_record.hpp          … 対局ログ
│   ├── game_reviewer.*          … local/llm レビュー生成
│   ├── review_prompt_builder.*  … レビュープロンプト生成
│   └── review_formatter.*       … JSON レビュー整形
└── player/
    ├── human_player.hpp / .cpp        … 標準入力プレイヤー
    ├── sample_computer_player.hpp/cpp … 弱いサンプル AI
    └── minmax_player.hpp / .cpp       … min-max AI
```

## 表示の見方

各ターンで以下が出る:

```
[盤面表示]

└ 直前手 (white) の評価変化: white側 -8       ← 打った側にとって即時に何点動いたか
形勢: 黒視点 +8 / 白視点 -8  [盤上: 黒 3, 白 3]  ← 現在の評価値 (両視点)

(minmax: depth=13, score=24)                  ← AI の予測値 (AI 視点、N手先読み)
turn = white, move = b4
```

- **形勢**: 評価関数による現在の盤面評価。プラスなら表記の側が有利
- **直前手の評価変化**: 自分の手で即時にどれだけ動いたか。長期的読みは反映されない
- **(minmax: ...)**: AI が読み切った先の予測値。即時形勢とずれることがある (短期譲歩で長期勝ち)
- **ヒント候補手の score**: 各合法手を min-max で探索した評価値

## min-max AI の方針

- 評価関数: 位置の重み付け表 + 着手可能数 (`own_mobility - opp_mobility`)
- 残り空きマス ≤ 10 で終盤評価 (石差を最優先) に切替
- 反復深化で深さ 1, 2, 3, ... と探索を伸ばし、3 秒の制限時間で打ち切り。
  直前に完了した深さのベスト手を採用
- α-β 枝刈り + 位置重み順の手並べ替えで効率化

制限時間は `MinmaxPlayer` のコンストラクタ引数で変更可。
