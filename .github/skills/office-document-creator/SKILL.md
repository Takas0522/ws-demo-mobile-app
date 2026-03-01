````skill
---
name: office-document-creator
description: Excel 設計書ドキュメントを新規作成・更新するためのガイド。設計書の作成・編集を行う際には、必ずこのスキルを参照して従ってください。
---

# Excel 設計書ドキュメント作成ガイド

このスキルは、ClosedXML を使用して Excel 形式の設計書ドキュメントを作成・更新するための
手順とパターンを提供します。

## 1. 基本フロー

```
1. 作成する設計書の内容・構成を決定する
2. scripting-guide に従い ClosedXML スクリプトでファイルを生成する
3. 必要に応じて既存ファイルを読み込み更新する
4. 生成されたファイルを確認する
```

**重要:** ファイル生成には `scripting-guide` スキルの規則に従い、`dotnet run -` にパイプして実行してください。

---

## 2. Excel ファイル新規作成（ClosedXML）

### 2.1 基本的なワークブック作成

空のワークブックを作成し、シートにデータを書き込む基本パターンです。

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook();
var ws = workbook.Worksheets.Add("設計概要");

// タイトル
ws.Cell("A1").Value = "機能設計書";
ws.Cell("A1").Style.Font.Bold = true;
ws.Cell("A1").Style.Font.FontSize = 16;

// メタ情報
ws.Cell("A3").Value = "作成日";
ws.Cell("B3").Value = DateTime.Now.ToString("yyyy/MM/dd");
ws.Cell("A4").Value = "作成者";
ws.Cell("B4").Value = "";
ws.Cell("A5").Value = "バージョン";
ws.Cell("B5").Value = "1.0";

workbook.SaveAs(filePath);
Console.WriteLine($"作成完了: {filePath}");
'@ | dotnet run - -- "<出力ファイルパス>"
```

### 2.2 ヘッダー行付きテーブルの作成

設計書で頻出する一覧表形式のシートを作成します。

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook();
var ws = workbook.Worksheets.Add("API一覧");

// ヘッダー行
var headers = new[] { "No.", "エンドポイント", "メソッド", "説明", "リクエスト", "レスポンス", "備考" };
for (int i = 0; i < headers.Length; i++)
{
    var cell = ws.Cell(1, i + 1);
    cell.Value = headers[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    cell.Style.Font.FontColor = XLColor.White;
    cell.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}

// データ行の例
var data = new[]
{
    new[] { "1", "/api/mobile/login", "POST", "ユーザーログイン", "LoginRequest", "LoginResponse", "" },
    new[] { "2", "/api/mobile/products", "GET", "商品一覧取得", "-", "ProductListResponse", "" },
};

for (int r = 0; r < data.Length; r++)
{
    for (int c = 0; c < data[r].Length; c++)
    {
        var cell = ws.Cell(r + 2, c + 1);
        cell.Value = data[r][c];
        cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
    }
}

// 列幅の自動調整
ws.Columns().AdjustToContents();

workbook.SaveAs(filePath);
Console.WriteLine($"作成完了: {filePath}");
'@ | dotnet run - -- "<出力ファイルパス>"
```

### 2.3 複数シートの設計書テンプレート

設計書で一般的な複数シート構成を一括作成します。

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var featureName = args.Length > 1 ? args[1] : "新機能";

using var workbook = new XLWorkbook();

// --- 表紙シート ---
var cover = workbook.Worksheets.Add("表紙");
cover.Cell("B2").Value = $"{featureName} 設計書";
cover.Cell("B2").Style.Font.Bold = true;
cover.Cell("B2").Style.Font.FontSize = 20;
cover.Range("B2:F2").Merge();

cover.Cell("B4").Value = "ドキュメント情報";
cover.Cell("B4").Style.Font.Bold = true;
cover.Cell("B5").Value = "プロジェクト名";
cover.Cell("C5").Value = "mobile-app-system";
cover.Cell("B6").Value = "機能名";
cover.Cell("C6").Value = featureName;
cover.Cell("B7").Value = "作成日";
cover.Cell("C7").Value = DateTime.Now.ToString("yyyy/MM/dd");
cover.Cell("B8").Value = "バージョン";
cover.Cell("C8").Value = "1.0";

// 改訂履歴
cover.Cell("B10").Value = "改訂履歴";
cover.Cell("B10").Style.Font.Bold = true;
var revHeaders = new[] { "版数", "日付", "変更内容", "変更者" };
for (int i = 0; i < revHeaders.Length; i++)
{
    var cell = cover.Cell(11, i + 2);
    cell.Value = revHeaders[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#D9E2F3");
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}
cover.Cell("B12").Value = "1.0";
cover.Cell("C12").Value = DateTime.Now.ToString("yyyy/MM/dd");
cover.Cell("D12").Value = "初版作成";
cover.Cell("E12").Value = "";
for (int c = 2; c <= 5; c++)
    cover.Cell(12, c).Style.Border.OutsideBorder = XLBorderStyleValues.Thin;

cover.Columns().AdjustToContents();

// --- 機能概要シート ---
var overview = workbook.Worksheets.Add("機能概要");
overview.Cell("A1").Value = "機能概要";
overview.Cell("A1").Style.Font.Bold = true;
overview.Cell("A1").Style.Font.FontSize = 14;

overview.Cell("A3").Value = "目的";
overview.Cell("A4").Value = "概要";
overview.Cell("A5").Value = "対象ユーザー";
overview.Cell("A6").Value = "前提条件";
overview.Cell("A7").Value = "制約事項";

foreach (var row in Enumerable.Range(3, 5))
{
    overview.Cell(row, 1).Style.Font.Bold = true;
    overview.Cell(row, 1).Style.Fill.BackgroundColor = XLColor.FromHtml("#D9E2F3");
    overview.Cell(row, 1).Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
    overview.Cell(row, 2).Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}
overview.Column(1).Width = 16;
overview.Column(2).Width = 60;

// --- 画面設計シート ---
var screen = workbook.Worksheets.Add("画面設計");
screen.Cell("A1").Value = "画面設計";
screen.Cell("A1").Style.Font.Bold = true;
screen.Cell("A1").Style.Font.FontSize = 14;

var screenHeaders = new[] { "No.", "画面名", "画面ID", "概要", "遷移先", "備考" };
for (int i = 0; i < screenHeaders.Length; i++)
{
    var cell = screen.Cell(3, i + 1);
    cell.Value = screenHeaders[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    cell.Style.Font.FontColor = XLColor.White;
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}
screen.Columns().AdjustToContents();

// --- API設計シート ---
var api = workbook.Worksheets.Add("API設計");
api.Cell("A1").Value = "API設計";
api.Cell("A1").Style.Font.Bold = true;
api.Cell("A1").Style.Font.FontSize = 14;

var apiHeaders = new[] { "No.", "エンドポイント", "メソッド", "説明", "認証", "リクエストBody", "レスポンスBody", "ステータスコード" };
for (int i = 0; i < apiHeaders.Length; i++)
{
    var cell = api.Cell(3, i + 1);
    cell.Value = apiHeaders[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    cell.Style.Font.FontColor = XLColor.White;
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}
api.Columns().AdjustToContents();

// --- データ設計シート ---
var data = workbook.Worksheets.Add("データ設計");
data.Cell("A1").Value = "データベース設計";
data.Cell("A1").Style.Font.Bold = true;
data.Cell("A1").Style.Font.FontSize = 14;

var dbHeaders = new[] { "No.", "テーブル名", "カラム名", "データ型", "NULL許容", "PK", "FK", "デフォルト値", "説明" };
for (int i = 0; i < dbHeaders.Length; i++)
{
    var cell = data.Cell(3, i + 1);
    cell.Value = dbHeaders[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    cell.Style.Font.FontColor = XLColor.White;
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}
data.Columns().AdjustToContents();

workbook.SaveAs(filePath);
Console.WriteLine($"設計書テンプレート作成完了: {filePath}");
Console.WriteLine($"シート: {string.Join(", ", workbook.Worksheets.Select(w => w.Name))}");
'@ | dotnet run - -- "<出力ファイルパス>" "<機能名>"
```

---

## 3. 既存 Excel ファイルの更新

### 3.1 既存ファイルにシートを追加

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var sheetName = args.Length > 1 ? args[1] : "新規シート";

using var workbook = new XLWorkbook(filePath);

if (workbook.Worksheets.Contains(sheetName))
{
    Console.WriteLine($"シート '{sheetName}' は既に存在します。スキップします。");
}
else
{
    var ws = workbook.Worksheets.Add(sheetName);
    ws.Cell("A1").Value = sheetName;
    ws.Cell("A1").Style.Font.Bold = true;
    ws.Cell("A1").Style.Font.FontSize = 14;
    Console.WriteLine($"シート '{sheetName}' を追加しました。");
}

workbook.Save();
Console.WriteLine($"保存完了: {filePath}");
'@ | dotnet run - -- "<ファイルパス>" "<シート名>"
```

### 3.2 既存シートにデータ行を追加

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var sheetName = args[1];
// 残りの引数をカンマ区切りデータとして扱う
var rowData = args.Skip(2).ToArray();

using var workbook = new XLWorkbook(filePath);
var ws = workbook.Worksheet(sheetName);

// 最終使用行の次の行に追加
var lastRow = ws.LastRowUsed()?.RowNumber() ?? 0;
var newRow = lastRow + 1;

for (int i = 0; i < rowData.Length; i++)
{
    var cell = ws.Cell(newRow, i + 1);
    cell.Value = rowData[i];
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}

workbook.Save();
Console.WriteLine($"行 {newRow} を追加しました: {string.Join(", ", rowData)}");
'@ | dotnet run - -- "<ファイルパス>" "<シート名>" "値1" "値2" "値3"
```

### 3.3 特定セルの値を更新

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var sheetName = args[1];
var cellAddress = args[2];
var newValue = args[3];

using var workbook = new XLWorkbook(filePath);
var ws = workbook.Worksheet(sheetName);

var oldValue = ws.Cell(cellAddress).Value.ToString();
ws.Cell(cellAddress).Value = newValue;

workbook.Save();
Console.WriteLine($"更新完了: {sheetName}!{cellAddress}");
Console.WriteLine($"  旧値: {oldValue}");
Console.WriteLine($"  新値: {newValue}");
'@ | dotnet run - -- "<ファイルパス>" "<シート名>" "<セルアドレス>" "<新しい値>"
```

---

## 4. スタイリングパターン

### 4.1 設計書向け共通スタイル

設計書で統一感のある見た目を作るための共通スタイルパターンです。

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook();
var ws = workbook.Worksheets.Add("スタイル例");

// === セクションヘッダー ===
void ApplySectionHeader(IXLCell cell, string text)
{
    cell.Value = text;
    cell.Style.Font.Bold = true;
    cell.Style.Font.FontSize = 14;
    cell.Style.Font.FontColor = XLColor.FromHtml("#1F4E79");
    cell.Style.Border.BottomBorder = XLBorderStyleValues.Medium;
    cell.Style.Border.BottomBorderColor = XLColor.FromHtml("#4472C4");
}

// === テーブルヘッダー行 ===
void ApplyTableHeader(IXLRange range)
{
    range.Style.Font.Bold = true;
    range.Style.Font.FontColor = XLColor.White;
    range.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    range.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
    range.Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;
    range.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
    range.Style.Border.InsideBorder = XLBorderStyleValues.Thin;
}

// === データ行（偶数行の縞模様） ===
void ApplyDataRows(IXLRange range)
{
    range.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
    range.Style.Border.InsideBorder = XLBorderStyleValues.Thin;
    range.Style.Alignment.Vertical = XLAlignmentVerticalValues.Top;
    range.Style.Alignment.WrapText = true;

    for (int r = 1; r <= range.RowCount(); r++)
    {
        if (r % 2 == 0)
            range.Row(r).Style.Fill.BackgroundColor = XLColor.FromHtml("#D9E2F3");
    }
}

// === ラベル列（左側のキー列） ===
void ApplyLabelColumn(IXLRange range)
{
    range.Style.Font.Bold = true;
    range.Style.Fill.BackgroundColor = XLColor.FromHtml("#D9E2F3");
    range.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
    range.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Right;
}

// 使用例
ApplySectionHeader(ws.Cell("A1"), "セクション見出し例");

var headers = ws.Range("A3:D3");
ws.Cell("A3").Value = "列1";
ws.Cell("B3").Value = "列2";
ws.Cell("C3").Value = "列3";
ws.Cell("D3").Value = "列4";
ApplyTableHeader(headers);

ws.Cell("A4").Value = "データ1";
ws.Cell("B4").Value = "データ2";
ws.Cell("C4").Value = "データ3";
ws.Cell("D4").Value = "データ4";
ws.Cell("A5").Value = "データ5";
ws.Cell("B5").Value = "データ6";
ws.Cell("C5").Value = "データ7";
ws.Cell("D5").Value = "データ8";
ApplyDataRows(ws.Range("A4:D5"));

ws.Columns().AdjustToContents();

workbook.SaveAs(filePath);
Console.WriteLine($"スタイル例作成完了: {filePath}");
'@ | dotnet run - -- "<出力ファイルパス>"
```

### 4.2 セルのマージと罫線

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook();
var ws = workbook.Worksheets.Add("マージ例");

// セルのマージ
ws.Range("A1:D1").Merge();
ws.Cell("A1").Value = "マージされたタイトル";
ws.Cell("A1").Style.Font.Bold = true;
ws.Cell("A1").Style.Font.FontSize = 14;
ws.Cell("A1").Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;

// 外枠罫線（太線）
ws.Range("A1:D1").Style.Border.OutsideBorder = XLBorderStyleValues.Medium;

// 内枠罫線（細線）
ws.Range("A3:D6").Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
ws.Range("A3:D6").Style.Border.InsideBorder = XLBorderStyleValues.Thin;

workbook.SaveAs(filePath);
Console.WriteLine($"作成完了: {filePath}");
'@ | dotnet run - -- "<出力ファイルパス>"
```

---

## 5. 設計書テンプレートパターン

### 5.1 画面項目定義書

画面の入力項目を定義するためのテンプレートです。

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var screenName = args.Length > 1 ? args[1] : "画面名";

using var workbook = new XLWorkbook();
var ws = workbook.Worksheets.Add("画面項目定義");

// タイトル
ws.Range("A1:H1").Merge();
ws.Cell("A1").Value = $"{screenName} - 画面項目定義書";
ws.Cell("A1").Style.Font.Bold = true;
ws.Cell("A1").Style.Font.FontSize = 14;

// ヘッダー
var headers = new[] { "No.", "項目名", "項目ID", "型", "必須", "最大長", "バリデーション", "備考" };
for (int i = 0; i < headers.Length; i++)
{
    var cell = ws.Cell(3, i + 1);
    cell.Value = headers[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    cell.Style.Font.FontColor = XLColor.White;
    cell.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}

// 列幅
ws.Column(1).Width = 6;   // No.
ws.Column(2).Width = 20;  // 項目名
ws.Column(3).Width = 20;  // 項目ID
ws.Column(4).Width = 12;  // 型
ws.Column(5).Width = 8;   // 必須
ws.Column(6).Width = 10;  // 最大長
ws.Column(7).Width = 30;  // バリデーション
ws.Column(8).Width = 30;  // 備考

workbook.SaveAs(filePath);
Console.WriteLine($"画面項目定義書作成完了: {filePath}");
'@ | dotnet run - -- "<出力ファイルパス>" "<画面名>"
```

### 5.2 テスト仕様書テンプレート

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var featureName = args.Length > 1 ? args[1] : "機能名";

using var workbook = new XLWorkbook();
var ws = workbook.Worksheets.Add("テスト仕様");

// タイトル
ws.Range("A1:I1").Merge();
ws.Cell("A1").Value = $"{featureName} - テスト仕様書";
ws.Cell("A1").Style.Font.Bold = true;
ws.Cell("A1").Style.Font.FontSize = 14;

// ヘッダー
var headers = new[] { "No.", "テスト分類", "テスト項目", "前提条件", "手順", "期待結果", "優先度", "結果", "備考" };
for (int i = 0; i < headers.Length; i++)
{
    var cell = ws.Cell(3, i + 1);
    cell.Value = headers[i];
    cell.Style.Font.Bold = true;
    cell.Style.Fill.BackgroundColor = XLColor.FromHtml("#4472C4");
    cell.Style.Font.FontColor = XLColor.White;
    cell.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
    cell.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
}

// 列幅
ws.Column(1).Width = 6;   // No.
ws.Column(2).Width = 15;  // テスト分類
ws.Column(3).Width = 30;  // テスト項目
ws.Column(4).Width = 25;  // 前提条件
ws.Column(5).Width = 30;  // 手順
ws.Column(6).Width = 30;  // 期待結果
ws.Column(7).Width = 10;  // 優先度
ws.Column(8).Width = 10;  // 結果
ws.Column(9).Width = 20;  // 備考

// データ入力用バリデーション（優先度）
var priorityValidation = ws.Range("G4:G100").CreateDataValidation();
priorityValidation.List("\"高,中,低\"");

// データ入力用バリデーション（結果）
var resultValidation = ws.Range("H4:H100").CreateDataValidation();
resultValidation.List("\"OK,NG,未実施,対象外\"");

workbook.SaveAs(filePath);
Console.WriteLine($"テスト仕様書作成完了: {filePath}");
'@ | dotnet run - -- "<出力ファイルパス>" "<機能名>"
```

---

## 6. 印刷設定

### 6.1 設計書向け印刷設定

設計書を印刷する際の標準設定を適用します。

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook(filePath);

foreach (var ws in workbook.Worksheets)
{
    // 用紙サイズ: A4
    ws.PageSetup.PaperSize = XLPaperSize.A4Paper;
    // 横向き
    ws.PageSetup.PageOrientation = XLPageOrientation.Landscape;
    // 余白（cm → インチ変換）
    ws.PageSetup.Margins.Top = 0.79;    // 2cm
    ws.PageSetup.Margins.Bottom = 0.79;
    ws.PageSetup.Margins.Left = 0.59;   // 1.5cm
    ws.PageSetup.Margins.Right = 0.59;
    // ヘッダー・フッター
    ws.PageSetup.Header.Left.AddText(ws.Name);
    ws.PageSetup.Footer.Center.AddText("&P / &N");
    // 幅を1ページに収める
    ws.PageSetup.FitToPages(1, 0);
    // 印刷タイトル行（ヘッダー行を各ページに表示）
    ws.PageSetup.SetRowsToRepeatAtTop(1, 3);

    Console.WriteLine($"印刷設定適用: {ws.Name}");
}

workbook.Save();
Console.WriteLine($"印刷設定完了: {filePath}");
'@ | dotnet run - -- "<ファイルパス>"
```

---

## 7. 注意事項

### パッケージバージョン

`#:package` ディレクティブではバージョンを明示してください。

```csharp
#:package ClosedXML@0.105.0           // Excel
```

### ファイルパスの扱い

- 出力先ディレクトリが存在しない場合は `Directory.CreateDirectory` で事前に作成してください
- 既存ファイルを上書きする場合は `SaveAs` を使用し、更新する場合は `Save` を使用してください
- ファイルが他のプロセスで開かれている場合はエラーになるため、Excel で開いていないことを確認してください

### 文字列エンコーディング

- 日本語文字列は `string` でそのまま扱えます
- セル内改行は `\n` で挿入し、`WrapText = true` を設定してください

```csharp
cell.Value = "1行目\n2行目";
cell.Style.Alignment.WrapText = true;
```

### 大量データの書き込み

- 大量の行を書き込む場合は、パフォーマンスに注意してください
- `ws.Cell(row, col)` での直接アクセスが最も高速です
- スタイルは範囲に一括適用する方が効率的です

```csharp
// 範囲に一括でスタイル適用
ws.Range(startRow, 1, endRow, colCount).Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
```

### 設計書の配置先

- 設計書は `docs/excel/` ディレクトリに配置してください
- ファイル名は `<機能名>_<ドキュメント種別>.xlsx` の形式を推奨します
  - 例: `価格履歴機能_設計書.xlsx`, `ユーザー管理_テスト仕様書.xlsx`
````
