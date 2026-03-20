import { test, expect } from "@playwright/test";
// ログイン済みフィクスチャを使用する場合:
// import { test, expect } from "../fixtures/auth.fixture";
import { LoginPage } from "../pages/login.page";

/**
 * e2e テストテンプレート
 *
 * 使い方:
 *   1. このファイルをコピーして `{e2e-root}/tests/{category}/{feature}.spec.ts` に配置する
 *   2. Page Object をインポートする
 *   3. test.describe でテストケースグループを分割する
 *   4. test.step() で操作群をグループ化する
 *
 * ルール:
 *   - 各テストは他のテストに依存しない（独立して実行可能）
 *   - waitForTimeout() は使用禁止
 *   - Web-first アサーションを優先（await expect(locator).toXxx()）
 *   - toBeVisible() は表示切替検証のみ。構造検証には toMatchAriaSnapshot を使う
 *   - Page Object を経由して UI 操作を行う
 */

test.describe("機能名", () => {
  // --- 共通セットアップ ---
  test.beforeEach(async ({ page }) => {
    // 例: ページ遷移
    // await page.goto("/target-page");
  });

  // --- テストケース ---

  test("機能名 - 具体的なシナリオの説明", async ({ page }) => {
    await test.step("前提条件のセットアップ", async () => {
      // const targetPage = new TargetPage(page);
      // await targetPage.goto();
    });

    await test.step("操作の実行", async () => {
      // await targetPage.doSomething();
    });

    await test.step("期待結果の検証", async () => {
      // await expect(page).toHaveURL(/expected-url/);
      // await expect(page.getByRole("heading")).toHaveText("期待テキスト");
    });
  });

  test("機能名 - 異常系のシナリオ説明", async ({ page }) => {
    await test.step("異常入力で操作", async () => {
      // 異常系の操作
    });

    await test.step("エラー表示の検証", async () => {
      // await expect(page.getByRole("alert")).toContainText("エラーメッセージ");
    });
  });
});
