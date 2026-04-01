---
name: e2e-report
description: >
  e2e テストを実行し、テスト仕様書ひな型（Excel テンプレート）に基づいたテスト結果報告書を作成するスキル。
  Playwright の JSON レポートとスクリーンショットを解析し、ClosedXML で Excel 形式のテスト結果報告書を出力する。
  Use when asked to create e2e test reports, generate test result Excel files, or produce test evidence documents with screenshots.
  「e2e テストのレポートを作成して」「テスト結果を Excel で出力して」「テスト結果報告書を生成して」
  「テストレポートを作って」「e2e テストの報告書」「テストのエビデンスを作って」などの依頼でロードされる。
---

# e2e テスト結果レポート作成スキル

Playwright による e2e テストの実行結果を、**テスト仕様書ひな型（Excel テンプレート）** に基づいた **テスト結果報告書（Excel ファイル）** として出力するスキル。各テストシナリオのスクリーンショットをシートとして埋め込み、シナリオ一覧シートからワークブック内リンクで参照できる。

## このスキルを使う場面

- e2e テスト実行後にテスト結果報告書（Excel）を作成するとき
- テスト結果をスクリーンショット付きのエビデンスとして記録するとき
- テスト実行から Excel レポート出力まで一貫して行うとき

## 前提条件

| 項目               | 詳細                                                                     |
| ------------------ | ------------------------------------------------------------------------ |
| Node.js            | 18 以上                                                                  |
| .NET SDK           | 8.0 以上                                                                 |
| dotnet-script      | `dotnet tool install -g dotnet-script` でインストール                    |
| Playwright         | e2e プロジェクトに `@playwright/test` がインストール済み                 |
| ClosedXML          | スクリプト内で NuGet 参照（自動ダウンロード）                            |
| テスト仕様書ひな型 | [resources/テスト仕様書ひな型.xlsx](./resources/テスト仕様書ひな型.xlsx) |

**プロジェクトパス（デフォルト）**:

| 項目                   | パス                                                          |
| ---------------------- | ------------------------------------------------------------- |
| e2e プロジェクトルート | `src/e2e/`                                                    |
| テスト結果ディレクトリ | `src/e2e/test-results/`                                       |
| Playwright 設定        | `src/e2e/playwright.config.ts`                                |
| レポート生成スクリプト | `.github/skills/e2e-report/references/generate-report.csx`    |
| Excel テンプレート     | `.github/skills/e2e-report/resources/テスト仕様書ひな型.xlsx` |
| レポート出力先         | `temp/テスト結果報告書_{YYYYMMDD_HHmmss}.xlsx`                |

## テンプレート構造

テスト仕様書ひな型は以下の 3 シートで構成される:

### 1. 表紙シート

| セル範囲 | 内容             | 備考                                  |
| -------- | ---------------- | ------------------------------------- |
| B2:M2    | テスト結果報告書 | タイトル（マージ済み・中央揃え）      |
| B4:D4    | テスト仕様作成者 | ラベル（マージ済み）                  |
| E4:M4    | （値を入力）     | テスト仕様作成者名                    |
| B5:D5    | テスト作業者     | ラベル（マージ済み）                  |
| E5:M5    | （値を入力）     | テスト作業者名                        |
| B7:C7    | 実行履歴         | セクションヘッダー                    |
| B8:C8    | 版数             | 履歴テーブルヘッダー                  |
| D8:E8    | 最終更新者       | 履歴テーブルヘッダー                  |
| F8:M8    | 更新内容         | 履歴テーブルヘッダー                  |
| B9-B19   | （版数データ）   | 行 9〜19 にデータ行（各列マージ済み） |

### 2. テストシナリオ一覧シート

| 列  | ヘッダー（行 2）               | 列幅  | 内容                             |
| --- | ------------------------------ | ----- | -------------------------------- |
| B   | シナリオ番号                   | 15.45 | TC-01, TC-02, ...                |
| C   | テスト内容                     | 45.20 | テストケースの説明               |
| D   | 実施者                         | 8.43  | テスト実行者名                   |
| E   | 結果                           | 8.43  | ○（合格）/ ×（不合格）           |
| F   | スクリーンショットシートリンク | 26.20 | シート内リンク（TC-XX シートへ） |

データは行 3 から記入する。

### 3. {シナリオ番号} シート（テンプレート）

空のテンプレートシート。レポート生成時にシナリオごとのスクリーンショット用シートに置き換える。

## ワークフロー

### ステップ 1: Playwright 設定の確認

テスト実行前に、Playwright の設定でスクリーンショットが有効になっていることを確認する。

```typescript
// playwright.config.ts の use セクション
use: {
  screenshot: 'on',  // 全テストでスクリーンショットを自動取得（必須）
  // trace: 'on',     // オプション：トレース取得
  // video: 'on',     // オプション：動画取得
},
```

`screenshot: 'on'` が設定されていれば、各テスト終了時にスクリーンショットが自動的にキャプチャされ、JSON レポートの `attachments` に含まれる。

### ステップ 2: e2e テストの実行（JSON レポーター）

テストを **JSON レポーター** で実行し、結果をファイルに保存する。`--headed` でブラウザを表示する。

```bash
cd src/e2e

# ブラウザインストール（初回のみ）
npx playwright install chromium

# テスト実行（JSON レポーター + ヘッド付き）
PLAYWRIGHT_JSON_OUTPUT_NAME=test-results/results.json npx playwright test --reporter=json --headed
```

**補足**:

- `PLAYWRIGHT_JSON_OUTPUT_NAME` 環境変数で JSON 出力先を指定する
- `--reporter=json` で JSON 形式の結果を出力する（`screenshot: 'on'` による自動スクリーンショットは reporter に依存せず動作する）
- テスト失敗時も JSON ファイルは生成される（失敗テストの `status` が `"failed"` になる）
- 特定のテストのみ実行する場合は `--grep "テスト名"` や `tests/auth/login.spec.ts` などでフィルタリングする

### ステップ 3: Excel レポートの生成

[generate-report.csx](./references/generate-report.csx) スクリプトを実行してレポートを生成する。

```bash
# dotnet-script がない場合はインストール
dotnet tool install -g dotnet-script 2>/dev/null || true

# レポート生成
dotnet script .github/skills/e2e-report/references/generate-report.csx -- \
  "src/e2e/test-results/results.json" \
  ".github/skills/e2e-report/resources/テスト仕様書ひな型.xlsx" \
  "temp/テスト結果報告書_$(date +%Y%m%d_%H%M%S).xlsx" \
  "テスト仕様作成者名" \
  "テスト実施者名"
```

**引数の説明**:

| 引数 | 必須 | 内容                                                     |
| ---- | ---- | -------------------------------------------------------- |
| 1    | ○    | Playwright JSON 結果ファイルのパス                       |
| 2    | ○    | Excel テンプレートのパス                                 |
| 3    | ○    | 出力先 Excel ファイルパス                                |
| 4    |      | テスト仕様作成者名（表紙に記入、省略時は空欄）           |
| 5    |      | テスト作業者名（表紙・シナリオ一覧に記入、省略時は空欄） |

### ステップ 4: レポートの確認

生成された Excel ファイルの内容を確認する。

```bash
# 構造確認（office-document-analyzer スキルのパターンを使用）
dotnet script temp/analyze_excel_structure.csx -- "temp/テスト結果報告書_XXXXXXXX_XXXXXX.xlsx"
```

## スクリーンショットの活用

### 自動スクリーンショット（デフォルト）

`playwright.config.ts` で `screenshot: 'on'` を設定すると、各テスト終了時に自動的にスクリーンショットがキャプチャされる。この設定だけで各テストシナリオに 1 枚のスクリーンショットが添付される。

### ステップごとのスクリーンショット（推奨）

テスト手順の各ステップでスクリーンショットを取得する場合は、テストコード内で `test.info().attach()` を使用する。

```typescript
import { test, expect } from "@playwright/test";

test("ログイン - 正しい認証情報でダッシュボードに遷移", async ({ page }) => {
  await test.step("ログイン画面を表示", async () => {
    await page.goto("/login");
    // ステップのスクリーンショットを取得
    await test.info().attach("01_ログイン画面", {
      body: await page.screenshot(),
      contentType: "image/png",
    });
  });

  await test.step("認証情報を入力して送信", async () => {
    await page.getByLabel("ユーザー名").fill("admin");
    await page.getByLabel("パスワード").fill("password");
    await page.getByRole("button", { name: "ログイン" }).click();
    await page.waitForURL("**/dashboard*");
    // 遷移後のスクリーンショットを取得
    await test.info().attach("02_ダッシュボード", {
      body: await page.screenshot(),
      contentType: "image/png",
    });
  });
});
```

**ルール**:

- `test.info().attach()` で添付されたスクリーンショットは JSON レポートの `attachments` に含まれ、レポート生成スクリプトが自動的に Excel に埋め込む
- `page.screenshot({ path: '...' })` でファイルに直接保存する方法は、JSON レポートの `attachments` に含まれないため使用しない
- 添付名（第 1 引数）は表示順を制御するため、`01_`, `02_` のようなプレフィックスを付けることを推奨する

## 生成されるレポートの構成

```
テスト結果報告書.xlsx
├── 表紙                    # テスト情報・実行履歴
├── テストシナリオ一覧       # 全テストの一覧（結果・リンク付き）
├── TC-01                   # シナリオ 1 のスクリーンショット
├── TC-02                   # シナリオ 2 のスクリーンショット
├── ...
└── TC-XX                   # シナリオ XX のスクリーンショット
```

各スクリーンショットシート（TC-XX）の構成:

| 行   | 内容                                           |
| ---- | ---------------------------------------------- |
| B2   | シナリオ ID とテスト名（太字）                 |
| B3   | 結果ラベル + 結果値（○/×）、実行時間           |
| B4   | エラーメッセージ（失敗時のみ、赤字）           |
| B6〜 | スクリーンショット画像（複数の場合は縦に並ぶ） |

テストシナリオ一覧シートの F 列（スクリーンショットシートリンク）は、ワークブック内のシートへの **内部ハイパーリンク** であり、クリックすると対応するスクリーンショットシートに遷移する。

## トラブルシューティング

| 問題                                    | 解決策                                                                                                                  |
| --------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| `dotnet-script` が見つからない          | `dotnet tool install -g dotnet-script` でインストール                                                                   |
| JSON ファイルが生成されない             | `PLAYWRIGHT_JSON_OUTPUT_NAME` 環境変数が正しく設定されているか確認                                                      |
| スクリーンショットが Excel に含まれない | `playwright.config.ts` の `screenshot: 'on'` を確認。`page.screenshot({ path })` ではなく `test.info().attach()` を使用 |
| スクリーンショットのパスが無効          | テスト実行後にファイルが存在するか確認。`test-results/` をクリアしていないか確認                                        |
| Excel ファイルが開けない / 壊れている   | ClosedXML のバージョンを確認。NuGet キャッシュをクリア (`dotnet nuget locals all --clear`)                              |
| シート名が長すぎるエラー                | Excel シート名は 31 文字以内。スクリプトは TC-XX 形式（短い）を使用するため通常発生しない                               |
| テスト数が多すぎて表紙の履歴行が不足    | 表紙の実行履歴は 11 行（行 9〜19）。不足する場合はスクリプトを修正してマージセル行を追加する                            |

## 参考資料

- [レポート生成スクリプト](./references/generate-report.csx)
- [テスト仕様書ひな型](./resources/テスト仕様書ひな型.xlsx)
- [e2e テスト構築スキル](../e2e-testing/SKILL.md) — テスト作成・実行の手順
- [Excel ドキュメント作成ガイド](../office-document-creator/SKILL.md) — ClosedXML の使い方
- [Playwright JSON Reporter](https://playwright.dev/docs/test-reporters#json-reporter)
- [Playwright Screenshots](https://playwright.dev/docs/screenshots)
