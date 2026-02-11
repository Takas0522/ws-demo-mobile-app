# エージェントシステム設定

このファイルはオーケストレーターとサブエージェントの動作設定を定義します。

## ディレクトリ構成

```
.github/agents/                         # VS Code カスタムエージェント定義
├── orchestrator.agent.md               # オーケストレーター（ユーザーが起動）
├── spec-refinement.agent.md            # Phase 1: 仕様精査（サブエージェント）
├── architecture-update.agent.md        # Phase 2: アーキテクチャ更新（サブエージェント）
├── task-creation.agent.md              # Phase 3: タスク作成（サブエージェント）
├── development.agent.md               # Phase 4: 開発実行（サブエージェント）
├── summary.agent.md                   # Phase 5: サマリ作成（サブエージェント）
└── review.agent.md                    # レビューサブエージェント

.agents/                                # 進捗管理・テンプレート
├── config.md                           # この設定ファイル
├── progress.json                       # 進捗管理（実行時に生成）
└── templates/
    └── progress-template.json          # 進捗管理テンプレート

.github/
└── copilot-instructions.md             # Copilot カスタムインストラクション
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
