# ws-demo-mobile-app

モバイルアプリケーション開発プロジェクト

## エージェント駆動型開発ワークフロー

このプロジェクトでは、AI エージェントによるオーケストレーター/サブエージェントパターンを使用して開発を進行します。

### アーキテクチャ

```
┌─────────────────────────────────────────────────────┐
│              Orchestrator エージェント                 │
│       (.github/agents/orchestrator.agent.md)         │
│  ┌───────────────────────────────────────────────┐   │
│  │  進捗管理 (.agents/progress.json)              │   │
│  └───────────────────────────────────────────────┘   │
│                      │                               │
│   ┌──────────────────┼──────────────────┐            │
│   ▼                  ▼                  ▼            │
│ サブエージェント   サブエージェント    Review           │
│ (Phase 1-5)      (Phase 1-5)     エージェント        │
└─────────────────────────────────────────────────────┘
```

VS Code のカスタムエージェント機能を使用しています。
サブエージェントは `user-invokable: false` で定義されており、Orchestrator からのみ起動されます。

### 開発フェーズ

| Phase | 名称 | 概要 | 成果物 |
|-------|------|------|--------|
| 1 | 仕様精査 | ビジネス・システム観点で仕様を構築 | `docs/specs/{機能名}/` |
| 2 | アーキテクチャ更新 | 既存アーキテクチャ仕様を更新 | `docs/architecture/` |
| 3 | タスク作成 | 開発タスクの計画策定 | `docs/specs/{機能名}/task-list.json` |
| 4 | 開発実行 | タスクごとに実装 | `src/` |
| 5 | サマリ作成 | 開発全体の記録 | `docs/specs/{機能名}/summary/` |

各フェーズ完了時にレビューが実施され、不合格の場合は最大3回まで再試行します。

### 使い方

#### VS Code Copilot での利用

1. VS Code で本プロジェクトを開く
2. Copilot Chat のエージェントドロップダウンから **Orchestrator** を選択
3. 機能名と仕様概要を入力：

```
## 機能名
ユーザー認証

## 仕様概要
- メール/パスワードによるサインアップ・ログイン
- ソーシャルログイン（Google, Apple）
- パスワードリセット機能
```

Orchestrator が自動的にサブエージェント（SpecRefinement, ArchitectureUpdate, TaskCreation, Development, Summary, Review）を呼び出して開発を進行します。

#### GitHub Coding Agent での利用

1. GitHub Issue に上記と同様の内容を記載
2. Coding Agent が `.github/copilot-instructions.md` を参照して動作

### ディレクトリ構成

```
.
├── .github/
│   ├── agents/                     # VS Code カスタムエージェント定義
│   │   ├── orchestrator.agent.md  # Orchestrator（ユーザーが起動）
│   │   ├── spec-refinement.agent.md    # Phase 1（サブエージェント）
│   │   ├── architecture-update.agent.md # Phase 2（サブエージェント）
│   │   ├── task-creation.agent.md       # Phase 3（サブエージェント）
│   │   ├── development.agent.md         # Phase 4（サブエージェント）
│   │   ├── summary.agent.md             # Phase 5（サブエージェント）
│   │   └── review.agent.md             # レビュー（サブエージェント）
│   └── copilot-instructions.md    # Copilot カスタムインストラクション
├── .agents/                        # 進捗管理
│   ├── config.md                  # 設定
│   ├── progress.json              # 進捗管理（実行時生成）
│   └── templates/                 # テンプレート
│       └── progress-template.json
├── docs/                           # ドキュメント（実行時生成）
│   ├── specs/{機能名}/            # 機能仕様
│   └── architecture/              # アーキテクチャ
├── src/                            # ソースコード
└── README.md
```

### 進捗の確認・復帰

作業は `.agents/progress.json` で管理され、各フェーズ・タスク完了時に Git commit & push されます。
中断した場合でも、progress.json の状態から作業を再開できます。

```
Copilot Chat のエージェントドロップダウンから Orchestrator を選択し、
「.agents/progress.json を確認し、中断した作業を再開してください」と入力してください。
```