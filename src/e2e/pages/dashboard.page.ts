import { type Page, type Locator } from '@playwright/test';

export class DashboardPage {
  readonly page: Page;
  readonly heading: Locator;
  readonly loggedInUser: Locator;
  readonly productManagementCard: Locator;
  readonly userManagementCard: Locator;
  readonly flagManagementCard: Locator;
  readonly logoutLink: Locator;
  readonly dashboardLink: Locator;
  readonly productManagementLink: Locator;
  readonly userManagementLink: Locator;
  readonly flagManagementLink: Locator;

  constructor(page: Page) {
    this.page = page;
    this.heading = page.getByRole('heading', { name: 'ダッシュボード', exact: true });
    this.loggedInUser = page.getByText(/ログイン中:/);
    this.productManagementCard = page.getByRole('link', { name: /商品管理/ }).filter({ has: page.getByRole('heading', { name: '商品管理' }) });
    this.userManagementCard = page.getByRole('link', { name: /ユーザー管理/ }).filter({ has: page.getByRole('heading', { name: 'ユーザー管理' }) });
    this.flagManagementCard = page.getByRole('link', { name: /フラグ管理/ }).filter({ has: page.getByRole('heading', { name: 'フラグ管理' }) });

    // Header navigation
    this.dashboardLink = page.getByRole('navigation').getByRole('link', { name: 'ダッシュボード' });
    this.productManagementLink = page.getByRole('navigation').getByRole('link', { name: '商品管理' });
    this.userManagementLink = page.getByRole('navigation').getByRole('link', { name: 'ユーザー管理' });
    this.flagManagementLink = page.getByRole('navigation').getByRole('link', { name: 'フラグ管理' });
    this.logoutLink = page.getByRole('navigation').getByRole('link', { name: 'ログアウト' });
  }

  async goto() {
    await this.page.goto('/admin-struts/admin/dashboard');
  }
}
