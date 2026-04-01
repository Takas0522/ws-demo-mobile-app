import { test, expect } from '@playwright/test';
import { LoginPage } from '../../pages/login.page';
import { DashboardPage } from '../../pages/dashboard.page';

const ADMIN_ID = 'admin001';
const ADMIN_PASSWORD = 'admin123';

test.describe('認証機能', () => {
  let loginPage: LoginPage;
  let dashboardPage: DashboardPage;

  test.beforeEach(async ({ page }) => {
    loginPage = new LoginPage(page);
    dashboardPage = new DashboardPage(page);
  });

  test('正常ログイン - 正しい認証情報でダッシュボードに遷移', async ({ page }) => {
    await test.step('ログイン画面にアクセスする', async () => {
      await loginPage.goto();
      await expect(loginPage.heading).toHaveText('管理者ログイン');
    });

    await test.step('認証情報を入力してログインする', async () => {
      await loginPage.login(ADMIN_ID, ADMIN_PASSWORD);
    });

    await test.step('ダッシュボードに遷移することを確認する', async () => {
      await expect(page).toHaveURL(/\/admin\/dashboard/);
      await expect(dashboardPage.heading).toHaveText('ダッシュボード');
    });

    await test.step('ヘッダーにログインユーザー名が表示されることを確認する', async () => {
      await expect(dashboardPage.loggedInUser).toContainText('管理者');
    });
  });

  test('ログイン失敗 - 誤ったパスワードでエラーメッセージが表示される', async ({ page }) => {
    await test.step('ログイン画面にアクセスする', async () => {
      await loginPage.goto();
    });

    await test.step('誤ったパスワードでログインを試みる', async () => {
      await loginPage.login(ADMIN_ID, 'wrongpassword');
    });

    await test.step('エラーメッセージが表示されることを確認する', async () => {
      await expect(loginPage.errorMessage).toBeVisible();
    });

    await test.step('ダッシュボードに遷移しないことを確認する', async () => {
      await expect(page).not.toHaveURL(/\/admin\/dashboard/);
    });
  });

  test('ログイン失敗 - 空入力でエラーが表示される', async ({ page }) => {
    await test.step('ログイン画面にアクセスする', async () => {
      await loginPage.goto();
    });

    await test.step('空のまま送信する', async () => {
      await loginPage.login('', '');
    });

    await test.step('ログイン画面のまま遷移しないことを確認する', async () => {
      await expect(page).not.toHaveURL(/\/admin\/dashboard/);
      await expect(loginPage.heading).toHaveText('管理者ログイン');
    });
  });

  test('ログアウト - セッションが切れてログイン画面に遷移する', async ({ page }) => {
    await test.step('ログインする', async () => {
      await loginPage.goto();
      await loginPage.login(ADMIN_ID, ADMIN_PASSWORD);
      await expect(page).toHaveURL(/\/admin\/dashboard/);
    });

    await test.step('ログアウトする', async () => {
      await dashboardPage.logoutLink.click();
    });

    await test.step('ログイン画面に遷移することを確認する', async () => {
      await expect(page).toHaveURL(/\/admin\/login/);
      await expect(loginPage.heading).toHaveText('管理者ログイン');
    });

    await test.step('ダッシュボードに直接アクセスするとログイン画面にリダイレクトされる', async () => {
      await page.goto('/admin-struts/admin/dashboard');
      await expect(page).toHaveURL(/\/admin\/login/);
    });
  });

  test('未認証アクセス - 認証必須ページにアクセスするとログイン画面にリダイレクトされる', async ({ page }) => {
    await test.step('ダッシュボードに直接アクセスする', async () => {
      await page.goto('/admin-struts/admin/dashboard');
      await expect(page).toHaveURL(/\/admin\/login/);
    });

    await test.step('商品一覧に直接アクセスする', async () => {
      await page.goto('/admin-struts/admin/products');
      await expect(page).toHaveURL(/\/admin\/login/);
    });

    await test.step('ユーザー一覧に直接アクセスする', async () => {
      await page.goto('/admin-struts/admin/users');
      await expect(page).toHaveURL(/\/admin\/login/);
    });
  });
});
