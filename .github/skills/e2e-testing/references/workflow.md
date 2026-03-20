# e2e テスト ワークフロー詳細

このドキュメントは `e2e-testing` スキルの各ステップの詳細手順を定義する。

---

## ステップ 1: テストシナリオの作成

### 1-1. 対象画面・機能の調査

テスト対象の画面・機能を調査し、以下を把握する:

- 画面の URL パスとナビゲーション構造
- フォーム要素（入力フィールド、ボタン、ドロップダウン等）
- 認証・認可の要件（ログイン必須か、管理者限定か）
- データの前提条件（シードデータの内容）
- 画面遷移のフロー
- API エンドポイント（API テストの場合）

**調査対象**:

テスト対象アプリケーションの種類に応じて、以下を調査する:

- ルーティング定義（フレームワークのルーティング設定ファイル）
- ビューテンプレート（HTML / JSP / コンポーネントファイル等）
- コントローラー / アクションクラス
- API エンドポイント定義
- シードデータ / DB スキーマ

### 1-2. テストシナリオの記述形式

以下の形式で Markdown テーブルとしてシナリオを記述する:

```markdown
## TC-XX: {テストケース名}

### テスト対象機能

{テスト対象の機能の説明}

### 前提条件

- {前提条件 1}
- {前提条件 2}

### テスト手順

| #   | 操作   | 期待される結果 |
| --- | ------ | -------------- |
| 1   | {操作} | {期待結果}     |
| 2   | {操作} | {期待結果}     |
```

### 1-3. テストシナリオのカテゴリ

| カテゴリ       | 内容                                       | テストケース例            |
| -------------- | ------------------------------------------ | ------------------------- |
| 認証           | ログイン・ログアウト・未認証リダイレクト   | ログイン正常系 / 異常系   |
| ナビゲーション | 画面遷移・メニューリンク                   | ヘッダー / サイドバーナビ |
| CRUD 操作      | 一覧表示・作成・編集・削除・バリデーション | フォーム入力 / 更新確認   |
| API テスト     | REST API の正常系・異常系                  | レスポンス検証            |
| 統合シナリオ   | 業務フロー全体の E2E                       | フルシナリオ              |

---

## ステップ 2: テストシナリオのレビュー

### 2-1. レビューの観点

以下の観点でシナリオの品質を評価する:

1. **網羅性** — 正常系・異常系・境界値がカバーされているか
2. **独立性** — 各テストケースが他のテストに依存せず単独で実行できるか
3. **再現性** — 前提条件が明確で、誰がいつ実行しても同じ結果になるか
4. **Flaky リスク** — 時間依存・順序依存・環境依存の要素がないか
5. **保守性** — UI 変更時の影響範囲が Page Object に局所化されているか
6. **セキュリティ** — 認証・認可チェック、不正アクセスのテストが含まれているか

### 2-2. レビュー手順

1. 上記 6 つの観点でシナリオを評価する
2. 不足しているケースを特定し、具体的な改善点を列挙する
3. 問題がある場合はシナリオを修正する
4. 修正後に再レビューを実施する（**最大 2 回まで**）
5. 最終的なシナリオを確定する

---

## ステップ 3: ページ構造の調査（Playwright CLI）

Page Object を設計する前に、テスト対象アプリケーションを実際にブラウザで操作し、ページの構造（DOM・ロケーター・画面遷移）を把握する。テストシナリオをすべて網羅する必要はない。主要な画面の構造把握が目的である。

### 3-1. テスト対象アプリの起動確認

1. テスト対象アプリケーションが起動しているか確認する
2. 起動していない場合は起動する（起動手順はプロジェクトのドキュメントを参照）
3. `curl {BASE_URL}` 等でアプリケーションが応答することを確認する

### 3-2. Playwright CLI でページを探索する

`playwright-cli` スキルを使用して、シナリオで定義した主要画面をブラウザで開き、DOM 構造を調査する。

> **ルール**: `playwright-cli open` 実行時は常に `--headed` オプションを付与し、ブラウザを可視状態で起動すること。

**手順**:

```bash
# 1. ブラウザを開いてアプリケーションに遷移（--headed で可視ブラウザ）
playwright-cli open {BASE_URL} --headed

# 2. ページのスナップショットを取得して DOM 構造を確認
playwright-cli snapshot

# 3. フォーム要素やボタンの ref を確認しながら画面を操作
playwright-cli fill {ref} "値"
playwright-cli click {ref}

# 4. 遷移先のページでも snapshot を取得
playwright-cli snapshot

# 5. 調査完了後にブラウザを閉じる
playwright-cli close
```

### 3-3. 調査で確認する項目

各画面で以下を確認し、Page Object の設計に活かす:

| 確認項目                              | 確認方法                             | Page Object 設計への反映                          |
| ------------------------------------- | ------------------------------------ | ------------------------------------------------- |
| フォームフィールドのラベル・name 属性 | `snapshot` で ref とラベルを確認     | `getByLabel()` / `getByRole()` のロケーター選択   |
| ボタン・リンクのテキスト              | `snapshot` でロール・テキストを確認  | `getByRole("button", { name: "..." })` の name 値 |
| 画面遷移後の URL パターン             | `goto` + 操作後の URL を確認         | `waitForURL()` のパターン                         |
| エラーメッセージの表示方法            | バリデーションエラーを発生させて確認 | エラー要素のロケーター                            |
| テーブル・リストの構造                | `snapshot` で行・セルの構造を確認    | `getByRole("row")` 等のロケーター                 |
| 共通 UI コンポーネント（ヘッダー等）  | 複数画面で共通部分を確認             | Component クラスの設計判断                        |

### 3-4. 調査の範囲

- シナリオで定義した **全画面を少なくとも 1 回は表示** する
- 全テストケースを再現する必要はない。主要な操作（ログイン、フォーム入力、一覧表示）を 1〜2 回試せばよい
- 発見した情報（ラベル名、ロール、URL パターン等）は、次のステップで Page Object に反映する

---

## ステップ 4: Page Object の設計・実装

### 4-1. Page Object の設計方針

テストシナリオから必要な Page Object を洗い出し、以下の方針で設計する:

1. **画面ごとに 1 クラス** — `LoginPage`, `DashboardPage`, `ProductListPage` 等
2. **共通 UI コンポーネントは別クラス** — ヘッダー、フッター等は `*.component.ts` として分離する
3. **コンストラクタでロケーターを定義** — 全ロケーターをクラスフィールドに集約する
4. **操作メソッドは動詞で命名** — `login()`, `editProduct()`, `navigateToProducts()` 等
5. **メソッドの戻り値** — ページ遷移を伴う操作は遷移先の Page Object インスタンスを返す

### 4-2. Page Object の配置先

テスト対象アプリケーションの画面構成に応じて Page Object を作成する:

```
{e2e-root}/pages/
├── {page-name}.page.ts       # 画面ごとの Page Object
└── {component}.component.ts  # 共通 UI コンポーネント（ヘッダー、フッター等）
```

### 4-3. 実装ガイドライン

- [page-object.ts](../templates/page-object.ts) テンプレートをベースに Page Object を作成する
- ロケーターは「ロケーター戦略（優先度順）」に従って選択する
- `page.waitForTimeout()` は使用禁止。`page.waitForURL()` / `locator.waitFor()` で待機する
- 外部リクエストの完了待ちには `page.waitForResponse()` を使用する

### 4-4. Page Object の実装例

```typescript
// pages/login.page.ts — ログイン画面の Page Object 例
import { type Locator, type Page } from "@playwright/test";

export class LoginPage {
  readonly usernameInput: Locator;
  readonly passwordInput: Locator;
  readonly loginButton: Locator;
  readonly errorMessage: Locator;

  constructor(private readonly page: Page) {
    this.usernameInput = page.getByLabel("ユーザー名"); // 対象アプリのラベルに合わせる
    this.passwordInput = page.getByLabel("パスワード");
    this.loginButton = page.getByRole("button", { name: "ログイン" });
    this.errorMessage = page.locator(".error-message");
  }

  async goto(): Promise<void> {
    await this.page.goto("/login"); // 対象アプリのパスに合わせる
  }

  async login(username: string, password: string): Promise<void> {
    await this.usernameInput.fill(username);
    await this.passwordInput.fill(password);
    await this.loginButton.click();
  }

  async loginAndNavigate(username: string, password: string): Promise<void> {
    await this.login(username, password);
    await this.page.waitForURL("**/dashboard*"); // 遷移先 URL に合わせる
  }
}
```

---

## ステップ 5: テストの実装

### 5-1. テストの配置先

テストシナリオのカテゴリに基づき、以下の構造で配置する:

```
{e2e-root}/tests/
├── auth/                     # 認証関連
│   └── login.spec.ts
├── {feature-category}/       # 機能カテゴリごと
│   └── {feature}.spec.ts
├── api/                      # API テスト
│   └── {api-feature}.spec.ts
└── scenarios/                # 統合シナリオ
    └── full-scenario.spec.ts
```

### 5-2. 実装ガイドライン

1. [e2e-test.spec.ts](../templates/e2e-test.spec.ts) テンプレートをベースにテストファイルを作成する
2. `test.describe` でテストケースグループをまとめる
3. `test.step()` で操作群をグループ化し、テストレポートの可読性を向上させる
4. テスト名は `機能名 - 具体的なシナリオ` 形式で記述する
5. `test.beforeEach` でログイン等の共通前提条件をセットアップする
6. 各テストメソッドは 1 つのシナリオのみを検証する
7. `test.describe.serial` は同一 `describe` 内でテスト順序が重要な場合にのみ使用する

### 5-3. test.step() の使い方

複数の操作と検証を含むテストでは、`test.step()` で論理的にグループ化する:

```typescript
test("ログイン - 正しい認証情報でダッシュボードに遷移", async ({ page }) => {
  const loginPage = new LoginPage(page);

  await test.step("ログインフォームに入力して送信", async () => {
    await loginPage.goto();
    await loginPage.login("testuser", "testpass");
  });

  await test.step("ダッシュボードの表示を検証", async () => {
    await expect(page).toHaveURL(/dashboard/);
    await expect(
      page.getByRole("heading", { name: "ダッシュボード" }),
    ).toHaveText("ダッシュボード");
  });
});
```

### 5-4. 認証フィクスチャの活用

ログイン済み状態を `test.extend` でカスタムフィクスチャ化し、テスト間で再利用する:

```typescript
// fixtures/auth.fixture.ts
import { test as base, type Page } from "@playwright/test";
import { LoginPage } from "../pages/login.page";

type AuthFixtures = {
  loggedInPage: Page;
};

export const test = base.extend<AuthFixtures>({
  loggedInPage: async ({ page }, use) => {
    const loginPage = new LoginPage(page);
    await loginPage.goto();
    await loginPage.loginAndNavigate(
      process.env.TEST_USERNAME ?? "testuser",
      process.env.TEST_PASSWORD ?? "testpass",
    );
    await use(page);
  },
});

export { expect } from "@playwright/test";
```

### 5-5. API テストの実装

API テストは `request` フィクスチャ（`APIRequestContext`）を使用する:

```typescript
import { test, expect } from "@playwright/test";

test.describe("API テスト", () => {
  let token: string;

  test.beforeAll(async ({ request }) => {
    // 認証が必要な場合
    const response = await request.post("/api/auth/login", {
      data: { username: "testuser", password: "testpass" },
    });
    const body = await response.json();
    token = body.token; // レスポンス構造に合わせる
  });

  test("TC-XX: API 正常系", async ({ request }) => {
    const response = await request.get("/api/resource", {
      headers: { Authorization: `Bearer ${token}` },
    });
    expect(response.ok()).toBeTruthy();
    const body = await response.json();
    expect(body.data).toBeDefined();
  });
});
```

---

## ステップ 6: テストの実施

### 6-1. テスト実行戦略

1. **初回実行**: `npx playwright test --headed --project=chromium` で単一ブラウザで実行する
2. **失敗分析**: テスト失敗を分析し、原因を特定する
3. **修正**: ロケーター、アサーション、テストロジックを必要に応じて修正する
4. **検証**: テストが安定的にパスすることを確認する
5. **報告**: テスト結果と発見した問題をレポートする

### 6-2. ローカル実行コマンド

```bash
# 前提: アプリケーション起動済み + DB リセット済み
cd src/e2e

# ブラウザインストール（初回のみ）
npx playwright install chromium

# 初回実行（単一ブラウザ・ヘッド付き）
npx playwright test --headed --project=chromium

# 全テスト実行（ヘッド付き）
npx playwright test --headed

# 特定テストファイルのみ実行（ヘッド付き）
npx playwright test --headed tests/auth/login.spec.ts

# UI モードで実行（デバッグ向け）
npx playwright test --ui

# ヘッドレスで実行（CI 向け）
npx playwright test
```

### 6-3. TestContainers によるコンテナ実行

```typescript
// container/run-in-container.ts
import path from "path";
import { PlaywrightContainer } from "testcontainers-node-playwright";

// プロジェクトに合わせて調整すること
const PLAYWRIGHT_IMAGE = "mcr.microsoft.com/playwright:v1.49.1-jammy";
const E2E_PROJECT_DIR = path.resolve(__dirname, "..");
const REPORTS_DIR = path.resolve(__dirname, "..", "reports");

async function runTests(): Promise<void> {
  const container = await new PlaywrightContainer(
    PLAYWRIGHT_IMAGE,
    E2E_PROJECT_DIR,
  )
    .withEnvironment({
      // ホスト上のアプリに接続する場合
      BASE_URL: process.env.BASE_URL ?? "http://host.docker.internal:3000",
    })
    .start();

  const { output, exitCode } = await container.exec([
    "npx",
    "playwright",
    "test",
    "--reporter=html",
  ]);

  console.log(output);

  await container.saveReporter("html", path.resolve(REPORTS_DIR, "index.html"));
  await container.stop();

  process.exit(exitCode);
}

runTests();
```

実行方法:

```bash
# TestContainers でコンテナ内実行
npx tsx src/e2e/container/run-in-container.ts
```

### 6-4. テスト失敗時の対応

テスト失敗が発生した場合は以下の情報を報告する（自動修正は行わない）:

1. 失敗したテストケース名（TC-XX）
2. エラーメッセージ
3. スクリーンショット（Playwright が自動保存する `test-results/` 配下）
4. 原因の推定（UI 変更 / データ不整合 / Flaky / テストコードのバグ）

---

## ステップ 7: テスト結果レポートの作成

### 7-1. レポート形式

以下の形式で `temp/e2eテストレポート_{YYYYMMDD_HHmmss}.md` にレポートを出力する:

```markdown
# e2e テスト結果レポート

## 実行情報

| 項目       | 値                          |
| ---------- | --------------------------- |
| 実行日時   | {YYYY/MM/DD HH:mm:ss}       |
| テスト対象 | {アプリケーション名}        |
| ブラウザ   | Chromium                    |
| 実行環境   | {ローカル / TestContainers} |
| テスト総数 | {N}                         |
| 成功       | {N}                         |
| 失敗       | {N}                         |
| スキップ   | {N}                         |

## テスト結果一覧

| TC-ID | テストケース名                 | 結果    | 実行時間 | 備考 |
| ----- | ------------------------------ | ------- | -------- | ---- |
| TC-01 | ログイン正常系                 | ✅ Pass | 1.2s     | —    |
| TC-02 | ログイン異常系（誤パスワード） | ✅ Pass | 0.8s     | —    |
| ...   | ...                            | ...     | ...      | ...  |

## 失敗テスト詳細

### TC-XX: {テストケース名}

- **エラーメッセージ**: {メッセージ}
- **原因**: {推定原因}
- **スクリーンショット**: `test-results/{file}/TC-XX-{name}.png`
```
