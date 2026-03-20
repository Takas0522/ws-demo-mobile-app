import { type Locator, type Page } from "@playwright/test";

/**
 * Page Object テンプレート
 *
 * 使い方:
 *   1. このファイルをコピーして `{e2e-root}/pages/{page-name}.page.ts` に配置する
 *   2. クラス名を対象画面に合わせてリネームする（例: LoginPage, ProductListPage）
 *   3. コンストラクタ内でロケーターを定義する
 *   4. ユーザー操作をメソッドとして実装する
 *
 * ルール:
 *   - ロケーターはコンストラクタで定義し、readonly フィールドに格納する
 *   - getByRole() > getByLabel() > getByTestId() > locator() の優先度で選択する
 *   - expect() によるアサーションは Page Object に含めない
 *   - 画面遷移を伴う操作は遷移先の Page Object を返す
 *   - waitForTimeout() は使用禁止
 */
export class TemplatePage {
  // --- ロケーター定義 ---
  readonly heading: Locator;
  readonly submitButton: Locator;
  // readonly someInput: Locator;

  constructor(private readonly page: Page) {
    this.heading = page.getByRole("heading", { name: "ページタイトル" });
    this.submitButton = page.getByRole("button", { name: "送信" });
    // this.someInput = page.getByLabel("入力ラベル");
  }

  // --- ナビゲーション ---

  /** この画面に直接遷移する */
  async goto(): Promise<void> {
    await this.page.goto("/xxx");
  }

  // --- 操作メソッド ---

  /**
   * フォームに値を入力して送信する
   * @returns 遷移先の Page Object（遷移がない場合は void）
   */
  // async submitForm(value: string): Promise<NextPage> {
  //   await this.someInput.fill(value);
  //   await this.submitButton.click();
  //   await this.page.waitForURL("**/next*");
  //   return new NextPage(this.page);
  // }

  // --- 状態取得メソッド ---

  /** ページが表示されるまで待機する */
  async waitForLoad(): Promise<void> {
    await this.heading.waitFor({ state: "visible" });
  }
}
