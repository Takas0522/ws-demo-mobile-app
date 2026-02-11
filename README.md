# ws-demo-mobile-app

モバイルアプリケーション開発プロジェクト

## エージェント駆動型開発ワークフロー

このプロジェクトでは、AI エージェントによるオーケストレーター/サブエージェントパターンを使用して開発を進行します。

### アーキテクチャ

```
┌─────────────────────────────────────────────────┐
│                 オーケストレーター                  │
│          (.agents/orchestrator.md)                │
│  ┌───────────────────────────────────────────┐   │
│  │  進捗管理 (.agents/progress.json)          │   │
│  └───────────────────────────────────────────┘   │
│                      │                           │
│         ┌────────────┼────────────┐              │
│         ▼            ▼            ▼              │
│    サブエージェント  サブエージェント  レビュー         │
│    (Phase 1-5)     (Phase 1-5)  サブエージェント    │
└─────────────────────────────────────────────────┘
```

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
2. Copilot Chat で以下のようにオーケストレーターを起動:

```
以下の機能の開発を開始してください。
.agents/orchestrator.md の手順に従い、サブエージェントを使って開発を進めてください。

## 機能名
ユーザー認証

## 仕様概要
- メール/パスワードによるサインアップ・ログイン
- ソーシャルログイン（Google, Apple）
- パスワードリセット機能
```

#### GitHub Coding Agent での利用

1. GitHub Issue に上記と同様の内容を記載
2. Coding Agent が `.github/copilot-instructions.md` を参照して動作

### ディレクトリ構成

```
.
├── .agents/                        # エージェントシステム
│   ├── orchestrator.md            # オーケストレーター定義
│   ├── config.md                  # 設定
│   ├── progress.json              # 進捗管理（実行時生成）
│   ├── sub-agents/                # サブエージェント定義
│   │   ├── 01-spec-refinement.md
│   │   ├── 02-architecture-update.md
│   │   ├── 03-task-creation.md
│   │   ├── 04-development.md
│   │   ├── 05-summary.md
│   │   └── review.md
│   └── templates/                 # テンプレート
│       └── progress-template.json
├── .github/
│   └── copilot-instructions.md    # Copilot カスタムインストラクション
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
.agents/progress.json を確認し、中断した作業を再開してください。
.agents/orchestrator.md の手順に従ってください。
```