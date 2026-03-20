---
name: e2e-testing
description: >
  Playwright + TestContainers で Web アプリの e2e テストを構築・実行するスキル。
  Page Object モデルによるテスト実装・TestContainers によるコンテナ実行・レポート出力までをガイドする。
  Use when asked to create e2e tests, write Playwright tests, build Page Objects,
  run browser tests in containers, debug UI test failures, or generate test reports.
  「e2e テストを作成して」「Playwright テストを書いて」「Page Object を作成して」「e2e テストを実行して」などの依頼でロードされる。
---

# e2e テスト構築スキル

任意の Web アプリケーションを対象に、**Playwright** による e2e テストの作成からレポート出力までの一連のプロセスを提供するスキル。**TestContainers** によるコンテナ実行で環境差異による Flaky テストを排除し、**Page Object モデル** で保守性の高いテストコードを構築する。

## このスキルを使う場面

- e2e テストを新規に作成するとき
- 既存の e2e テストシナリオをレビュー・拡充するとき
- Page Object クラスを設計・実装するとき
- TestContainers でテストを実行し結果レポートを出力するとき

## 前提条件

| 項目                 | 詳細                                      |
| -------------------- | ----------------------------------------- |
| Node.js              | 18 以上                                   |
| パッケージマネージャ | npm                                       |
| テストフレームワーク | Playwright Test (`@playwright/test`)      |
| コンテナ実行         | `testcontainers-node-playwright` + Docker |
| ブラウザ             | Chromium（デフォルト）                    |

**プロジェクトパス（デフォルト）**:

> テスト対象のアプリケーションに応じてパスを調整すること。以下はこのリポジトリのデフォルト構成。

| 項目                   | パス                           |
| ---------------------- | ------------------------------ |
| e2e プロジェクトルート | `src/e2e/`                     |
| テストファイル         | `src/e2e/tests/`               |
| Page Object クラス     | `src/e2e/pages/`               |
| Playwright 設定        | `src/e2e/playwright.config.ts` |
| package.json           | `src/e2e/package.json`         |

## 設計原則

### 1. Page Object モデル（必須）

テストコードから UI の詳細を分離し、保守性を高める。

**ルール**:

- **1 画面 = 1 Page Object クラス** — 画面ごとに `src/e2e/pages/` に `.page.ts` ファイルを作成する
- **ロケーターはコンストラクタで定義** — `page.locator()` / `page.getByRole()` 等の呼び出しをコンストラクタに集約する
- **操作はメソッドとして公開** — ユーザー操作（ログイン、フォーム入力、ボタン押下など）をメソッドに抽象化する
- **アサーションはテストファイルに記述** — Page Object にはアサーション（`expect`）を含めない。ただし、ナビゲーションの待機を兼ねる `waitFor` 系は許容する
- **メソッドチェーン対応** — ナビゲーション操作は遷移先の Page Object を返す

### 2. Flaky テスト回避（必須）

- **`waitForTimeout()` / `page.waitForTimeout()` 禁止** — 固定時間の待機は一切使用しない
- **自動待機を活用** — Playwright の `locator.click()`, `locator.fill()` 等の組み込み自動待機に依存する
- **Web-first アサーションを使う** — `await expect(locator).toHaveText()` 等のリトライ可能なアサーションを優先する。`expect(locator).toBeVisible()` は表示状態の変化を明示的に検証する場合のみ使用する
- **テスト間の独立性** — 各テストは他のテストの結果に依存しない。必要ならテストごとにデータをリセットする
- **厳密なロケーター** — `getByRole()`, `getByLabel()`, `getByTestId()` を優先する。CSS セレクターや XPath は最後の手段
- **Strict モード違反を避ける** — ロケーターが複数要素にマッチするとエラーになる。ロケーターが 1 つの要素のみを指すよう、十分に絞り込む

### 3. TestContainers によるコンテナ実行（CI / 完全再現環境向け）

Playwright テストを Docker コンテナ内で実行し、ブラウザ環境の差異を排除する。

**ルール**:

- **Playwright 公式 Docker イメージを使用** — `mcr.microsoft.com/playwright:v{VERSION}-jammy`
- **`testcontainers-node-playwright` モジュールを使用** — コンテナのライフサイクル管理を TestContainers に委譲する
- **テスト対象アプリはホストまたは別コンテナで起動** — Playwright コンテナからテスト対象にネットワーク到達可能にする
- **レポートはコンテナ外に抽出** — `saveReporter()` で HTML / JSON レポートをホスト側に保存する
- **ローカル開発時はコンテナなしでも実行可能** — `npx playwright test --headed` で直接実行できる構成も維持する
- **ローカル実行時は `--headed` をデフォルトとする** — デモ目的のアプリのため、ブラウザを表示して動作を確認できるようにする

### 4. テストコード規約

| 項目                     | ルール                                                                               |
| ------------------------ | ------------------------------------------------------------------------------------ |
| ファイル命名             | `{feature}.spec.ts`（例: `login.spec.ts`, `product-management.spec.ts`）             |
| Page Object 命名         | `{PageName}Page`（例: `LoginPage`, `ProductListPage`）                               |
| Page Object ファイル命名 | `{page-name}.page.ts`（例: `login.page.ts`, `product-list.page.ts`）                 |
| テスト構造               | `test.describe` でテストケースグループを分割する                                     |
| テストステップ           | `test.step()` で操作グループを分割し、レポートの可読性を向上させる                   |
| テスト命名               | `機能名 - 具体的なシナリオ`（例: `ログイン - 正しい認証情報でダッシュボードに遷移`） |
| セットアップ             | `test.beforeEach` でログイン等の共通操作を実行する                                   |
| クリーンアップ           | `test.afterEach` / `test.afterAll` で状態を復元する                                  |
| 言語                     | テスト名・コメントは日本語可。コード本文は英語                                       |

### 5. ロケーター戦略（優先度順）

| 優先度 | ロケーター                | 使用場面                                     |
| ------ | ------------------------- | -------------------------------------------- |
| 1      | `page.getByRole()`        | ボタン、リンク、見出し等のセマンティック要素 |
| 2      | `page.getByLabel()`       | フォーム入力フィールド                       |
| 3      | `page.getByPlaceholder()` | ラベルがない入力フィールド                   |
| 4      | `page.getByText()`        | テキストコンテンツによる特定                 |
| 5      | `page.getByTestId()`      | `data-testid` 属性による特定                 |
| 6      | `page.locator()`          | CSS セレクター（上記で特定できない場合のみ） |

### 6. アサーションベストプラクティス

| 目的           | アサーション                   | 例                                                |
| -------------- | ------------------------------ | ------------------------------------------------- |
| UI 構造検証    | `toMatchAriaSnapshot`          | アクセシビリティツリーのスナップショット検証      |
| 要素数         | `toHaveCount`                  | リストアイテムの件数検証                          |
| テキスト内容   | `toHaveText` / `toContainText` | 完全一致 / 部分一致                               |
| ナビゲーション | `toHaveURL`                    | 画面遷移後の URL 検証                             |
| 入力値         | `toHaveValue`                  | フォームフィールドの値検証                        |
| 表示状態       | `toBeVisible`                  | 表示/非表示の切り替わりを明示的に検証する場合のみ |

## ワークフロー

> **すでにテストシナリオが存在する場合**: ステップ 2 から開始し、現在の実装をもとにシナリオをレビューする。

- [ ] ステップ 1: テストシナリオの作成 — [workflow.md](./references/workflow.md#ステップ-1-テストシナリオの作成) 参照
- [ ] ステップ 2: テストシナリオのレビュー（最大 2 回）— [workflow.md](./references/workflow.md#ステップ-2-テストシナリオのレビュー) 参照
- [ ] ステップ 3: ページ構造の調査（Playwright CLI）— [workflow.md](./references/workflow.md#ステップ-3-ページ構造の調査playwright-cli) 参照
  - アプリを起動し、`playwright-cli` でシナリオの主要画面を操作して DOM 構造・ロケーターを把握する
- [ ] ステップ 4: Page Object の設計・実装 — [workflow.md](./references/workflow.md#ステップ-4-page-object-の設計実装) 参照
  - Page Object テンプレート: [page-object.ts](./templates/page-object.ts)
- [ ] ステップ 5: テストの実装 — [workflow.md](./references/workflow.md#ステップ-5-テストの実装) 参照
  - テストテンプレート: [e2e-test.spec.ts](./templates/e2e-test.spec.ts)
- [ ] ステップ 6: テストの実施・デバッグ — [workflow.md](./references/workflow.md#ステップ-6-テストの実施) 参照
- [ ] ステップ 7: テスト結果レポートの作成 — [workflow.md](./references/workflow.md#ステップ-7-テスト結果レポートの作成) 参照
  - 出力先: `temp/e2eテストレポート_{YYYYMMDD_HHmmss}.md`

## ディレクトリ構成

> テスト対象のアプリケーションに応じてディレクトリ名・ファイル名を調整すること。以下は推奨構成。

```
{e2e-root}/
├── playwright.config.ts          # Playwright 設定
├── package.json                  # 依存パッケージ
├── pages/                        # Page Object クラス
│   ├── {page-name}.page.ts       # 画面ごとの Page Object
│   └── {component}.component.ts  # 共通 UI コンポーネント
├── tests/                        # テストファイル
│   ├── {category}/               # 機能カテゴリごとのサブディレクトリ
│   │   └── {feature}.spec.ts     # テストファイル
│   └── scenarios/                # 統合シナリオ
│       └── full-scenario.spec.ts
├── fixtures/                     # テストフィクスチャ
│   └── auth.fixture.ts           # 認証済み状態の共通フィクスチャ
├── helpers/                      # ヘルパー関数
│   └── api.helper.ts             # API リクエスト用ヘルパー
└── container/                    # TestContainers 実行スクリプト
    └── run-in-container.ts       # コンテナ内テスト実行
```

## トラブルシューティング

| 問題                                              | 解決策                                                                                                                             |
| ------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| テスト対象アプリに接続できない                    | テスト対象アプリが起動しているか確認。`curl {BASE_URL}` で応答を確認する                                                           |
| テストデータが不正                                | DB リセットスクリプト等でシードデータを再適用する                                                                                  |
| TestContainers でコンテナが起動しない             | Docker デーモンが動作しているか確認。`docker info` で状態を確認する                                                                |
| Playwright コンテナからホストアプリに接続できない | `host.docker.internal` または `host.testcontainers.internal` をベース URL に使用する。Linux ではネットワーク設定が必要な場合がある |
| `npx playwright test` でブラウザが見つからない    | `npx playwright install chromium` でブラウザをインストールする                                                                     |
| テストが Flaky（不安定）になる                    | `waitForTimeout()` の使用がないか確認。ロケーターが動的要素に依存していないか確認。`test.describe.serial` の使用を検討する         |
| HTML レポートがコンテナから取り出せない           | `startedPlaywrightContainer.saveReporter("html", destinationPath)` を使用しているか確認する                                        |

## 品質チェックリスト

テストをコミットする前に以下を確認する:

- [ ] 全ロケーターがアクセシブルで十分に特定的、Strict モード違反なし
- [ ] `test.step()` で操作群が論理的にグループ化されている
- [ ] アサーションが意味あるユーザー期待を反映している
- [ ] `waitForTimeout()` を使用していない
- [ ] `toBeVisible()` は表示切替の検証以外で使っていない
- [ ] テストが論理的にグループ化され、一貫した命名規則に従っている
- [ ] 各テストが独立して実行可能（他テストに依存しない）
- [ ] Page Object に `expect` が含まれていない

## 参考資料

- [ワークフロー詳細](./references/workflow.md)
- [Page Object テンプレート](./templates/page-object.ts)
- [e2e テストテンプレート](./templates/e2e-test.spec.ts)
- [Playwright 公式ドキュメント](https://playwright.dev/docs/intro)
- [Playwright Best Practices](https://playwright.dev/docs/best-practices)
- [testcontainers-node-playwright](https://github.com/javierlopezdeancos/testcontainers-node-playwright)
