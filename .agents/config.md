# エージェントシステム設定

このファイルはオーケストレーターとサブエージェントの動作設定を定義します。

## ディレクトリ構成

```
.agents/
├── orchestrator.md                     # オーケストレーター定義
├── config.md                           # この設定ファイル
├── progress.json                       # 進捗管理（実行時に生成）
├── sub-agents/
│   ├── 01-spec-refinement.md           # Phase 1: 仕様精査
│   ├── 02-architecture-update.md       # Phase 2: アーキテクチャ更新
│   ├── 03-task-creation.md             # Phase 3: タスク作成
│   ├── 04-development.md              # Phase 4: 開発実行
│   ├── 05-summary.md                  # Phase 5: サマリ作成
│   └── review.md                      # レビューサブエージェント
└── templates/
    └── progress-template.json          # 進捗管理テンプレート
```

## Git 運用

### ブランチ戦略

開発は機能ブランチで行います：

```
feature/{機能名}
```

### コミットメッセージ

```
[Phase {N}] {フェーズ名}: {概要}
```

### Push タイミング

- 各フェーズ完了時
- Phase 4 の各タスク完了時
- レビュー不合格で再試行前

## レビュー設定

- 最大試行回数: 3
- 合格基準: critical/major 指摘が0件
- minor/info 指摘は合格扱い（改善推奨）

## ファイルサイズ制約

- ドキュメントファイル: 800行以下
- ソースコードファイル: プロジェクトの規約に準拠
