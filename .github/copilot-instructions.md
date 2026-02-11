# GitHub Copilot カスタムインストラクション

## プロジェクト概要

このプロジェクトはエージェント駆動型の開発ワークフローを採用しています。
開発はオーケストレーターエージェント（`.github/agents/orchestrator.agent.md`）が管理し、各フェーズの作業はカスタムサブエージェントが担当します。

## エージェントシステム

### カスタムエージェント配置

エージェント定義は `.github/agents/` ディレクトリに `.agent.md` ファイルとして配置されています。
VS Code はこのディレクトリを自動検出し、エージェントドロップダウンに表示します。

| エージェント | ファイル | 役割 |
|------------|---------|------|
| Orchestrator | `orchestrator.agent.md` | 作業割り振り・進捗管理（ユーザーが起動） |
| SpecRefinement | `spec-refinement.agent.md` | Phase 1: 仕様精査（サブエージェント専用） |
| ArchitectureUpdate | `architecture-update.agent.md` | Phase 2: アーキテクチャ更新（サブエージェント専用） |
| TaskCreation | `task-creation.agent.md` | Phase 3: タスク作成（サブエージェント専用） |
| Development | `development.agent.md` | Phase 4: 開発実行（サブエージェント専用） |
| Summary | `summary.agent.md` | Phase 5: サマリ作成（サブエージェント専用） |
| Review | `review.agent.md` | 批判的レビュー（サブエージェント専用） |

### オーケストレーターの利用

Copilot Chat のエージェントドロップダウンから **Orchestrator** を選択し、機能名と仕様概要を入力してください。
オーケストレーターは自動的に各フェーズのサブエージェントを呼び出し、作業を進行します。

### 進捗管理

- 進捗は `.agents/progress.json` で管理されます
- テンプレートは `.agents/templates/progress-template.json` にあります

## ディレクトリ規約

| ディレクトリ | 用途 |
|-------------|------|
| `.github/agents/` | カスタムエージェント定義（`.agent.md`） |
| `.agents/` | 進捗管理・テンプレート |
| `docs/specs/{機能名}/` | 機能仕様書 |
| `docs/architecture/` | アーキテクチャドキュメント |
| `src/` | ソースコード |

## ドキュメント規約

- Markdown形式で記述
- ファイルは800行以下に分割
- Mermaid記法で図表を作成
- 要件にはID（BR-001, FR-001等）を付与

## Git 規約

- コミットメッセージ: `[Phase {N}] {フェーズ名}: {概要}`
- ブランチ: `feature/{機能名}`
