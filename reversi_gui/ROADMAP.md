# オセロGUI開発ロードマップ

このドキュメントは、Reversi GUI の開発進行状況と今後の計画を示します。

## フェーズ1: 基本プロトタイプ ✅ 完了

### 実装済み
- [x] Qt6 プロジェクト構築
- [x] 6×6 盤面表示ウィジェット
- [x] 基本的なオセロゲームロジック
- [x] 打てる場所の表示（ハイライト）
- [x] ターン管理
- [x] 石数カウント機能
- [x] プレイヤー交代ロジック
- [x] 基本UI（盤面、ボタン、情報表示）

### 成果物
- `reversi_gui/` - Qt GUI アプリケーション
- CMakeLists.txt - ビルド設定
- README.md / SETUP.md - ドキュメント

---

## フェーズ2: 機能拡張 ⏳ 次フェーズ

### 優先順位1: 時間制限機能
- [ ] QTimer による 10 秒カウントダウン
- [ ] タイマーUI表示の改善
- [ ] タイムオーバー時の自動パス
- [ ] ゲーム開始時のタイマーリセット

**必要なファイル修正:**
- `mainwindow.h/cpp`: タイマー管理の追加
- UI: より詳細なタイマー表示

### 優先順位2: AI との統合

#### 2-1. C++ AI との IPC 通信
- [ ] 共同開発者の 6×6 AI エンジンをラップ
- [ ] プロセス間通信 (IPC) の実装
  - 候補1: JSON over TCP
  - 候補2: StandardIO パイプ
  - 候補3: DLL/SO として直接リンク
- [ ] AIの次の手を要求・受け取る

**必要なクラス:**
```cpp
class AIEngine {
public:
    pair<int, int> getNextMove(const GameState &state);
    void initialize();
};
```

#### 2-2. AI vs Player モード
- [ ] ゲーム開始時に対戦者選択
- [ ] AI の思考時間表示
- [ ] AI移動のアニメーション

### 優先順位3: ヒント機能

#### 3-1. ローカル AI ヒント
- [ ] 最初は簡単な評価関数ベースのヒント
- [ ] 盤面を評価して最善手を提案
- [ ] 3回制限の実装

#### 3-2. オンライン AI ヒント (今後)
- [ ] 外部APIとの連携（例：lichess.org API）
- [ ] ネットワーク通信の実装
- [ ] キャッシング機構

**ファイル:**
```
src/
├── hint/
│   ├── hintprovider.h/cpp
│   ├── localhintengine.h/cpp
│   └── onlinehintengine.h/cpp  (Future)
```

### 優先順位4: 盤面説明 API

#### 4-1. API 設計
- [ ] 盤面を JSON で記述
- [ ] 推奨手の理由を自然言語で生成
- [ ] APIレスポンスをポップアップで表示

**例：**
```json
{
  "board": [...],
  "recommendation": "(3, 2)",
  "explanation": "この手により、相手の4つの石を翻して有利になります。"
}
```

**ファイル:**
```
src/
├── api/
│   ├── boardanalysisapi.h/cpp
│   └── apiresponse.h
```

### 優先順位5: アバター システム

#### 5-1. 基本実装（現在）
- [x] 絵文字ベースの簡易版
- [ ] 表情の動的変更
- [ ] より詳細な評価に基づく表情

#### 5-2. 高度な実装（将来）
- [ ] 画像ベースのアバター
- [ ] アニメーション
- [ ] ゲーム進行に応じた反応

**ファイル:**
```
src/
├── avatar/
│   ├── avatar.h/cpp
│   └── avataranimator.h/cpp
```

---

## フェーズ3: ランキング & スコア ⏳ 今後

### 3-1. ローカルランキング
- [ ] SQLite DB の導入
- [ ] スコア保存（日時、スコア差、勝敗）
- [ ] ランキング画面の実装
- [ ] スコア統計表示

**ファイル:**
```
src/
├── database/
│   ├── database.h/cpp
│   └── scorerecord.h
```

### 3-2. オンランキング（将来）
- [ ] クラウドサーバーとの連携
- [ ] グローバルランキング表示

---

## フェーズ4: UI/UX 改善

- [ ] ダークモード対応
- [ ] アイコン・画像リソースの追加
- [ ] サウンドエフェクト（勝敗、石の配置音）
- [ ] ゲーム設定メニュー

**ファイル:**
```
src/
├── ui/
│   ├── settingsdialog.h/cpp
│   └── stylemanager.h/cpp

resources/
├── images/
├── sounds/
└── stylesheets/
```

---

## 依存関係の管理

現在のビルド構成：
- Qt6（ウィジェット、ネットワーク）
- C++17

将来の可能性：
- nlohmann/json（JSON 処理用）
- sqlite3（ランキングDB）
- OpenSSL（安全な通信用）

---

## テスト戦略

### ユニットテスト
```bash
# Google Test の導入を検討
set(GTEST_DISCOVER_TESTS ON)
```

### 統合テスト
- [ ] AI との通信テスト
- [ ] API レスポンステスト
- [ ] ゲームロジックの完全テスト

---

## コードベース管理

### ブランチ戦略
```
main (stable)
├── develop (development)
│   ├── feature/ai-integration
│   ├── feature/timer
│   ├── feature/hints
│   └── feature/ui-improvements
└── release/v1.0
```

### コミットメッセージ形式
```
[feature|fix|docs|refactor] 機能説明

より詳細な説明（必要に応じて）
```

---

## 参考リソース

- Qt Documentation: https://doc.qt.io/qt-6/
- Othello Rules: https://en.wikipedia.org/wiki/Reversi
- API Design: https://restfulapi.net/

---

## 開発分担

| 担当者 | 職務 | 備考 |
|--------|------|------|
| **minamitemma** | GUI開発・プロジェクト管理 | Qt6 UI、ゲームロジック統合 |
| **sugomori** | 対戦用オセロAI | 6×6 AI エンジン実装（`IAIEngine` インターフェース準拠） |
| **goto** | 説明用・ヒント用AI | ヒント機能、盤面説明API |

---

## 更新履歴

- **2026/05/11** - フェーズ1完了、ロードマップ作成、開発分担定義
