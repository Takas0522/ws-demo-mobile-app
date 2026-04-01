import { type Page, type Locator } from '@playwright/test';

export class LoginPage {
  readonly page: Page;
  readonly heading: Locator;
  readonly loginIdInput: Locator;
  readonly passwordInput: Locator;
  readonly submitButton: Locator;
  readonly errorMessage: Locator;

  constructor(page: Page) {
    this.page = page;
    this.heading = page.getByRole('heading', { name: '管理者ログイン' });
    this.loginIdInput = page.getByRole('textbox', { name: 'ログインID' });
    this.passwordInput = page.getByRole('textbox', { name: 'パスワード' });
    this.submitButton = page.locator('input[type="submit"]');
    this.errorMessage = page.getByText('ログインIDまたはパスワードが正しくありません');
  }

  async goto() {
    await this.page.goto('/admin-struts/admin/login');
  }

  async login(loginId: string, password: string) {
    await this.loginIdInput.fill(loginId);
    await this.passwordInput.fill(password);
    await this.submitButton.click();
  }
}
