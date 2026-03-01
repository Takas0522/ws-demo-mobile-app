#!/usr/bin/env dotnet-script
// ============================================================
// 価格履歴機能 設計書 Excel生成スクリプト (Excel方眼紙形式)
// ============================================================
// 使用方法: dotnet script GenerateDesignDoc.csx
// 出力:     価格履歴機能_設計書.xlsx
// 依存:     ClosedXML (NuGet)
// ============================================================

#r "nuget: ClosedXML, 0.104.1"

using ClosedXML.Excel;
using System;
using System.IO;

// === 定数定義 ===
const double GRID_COL_WIDTH = 2.5;          // 方眼紙セル幅
const int MAX_COL = 44;                      // 使用最大列数
const string FONT_NAME = "ＭＳ Ｐゴシック"; // 標準フォント
const string FONT_MONO = "Consolas";         // 等幅フォント（コード用）
const double FONT_SIZE_NORMAL = 10;
const double FONT_SIZE_TITLE = 22;
const double FONT_SIZE_SECTION = 12;
const double FONT_SIZE_SMALL = 9;

// === 出力パス ===
var outputPath = Path.Combine(
    Directory.GetCurrentDirectory(),
    "価格履歴機能_設計書.xlsx"
);

Console.WriteLine("設計書Excel生成を開始します...");

var workbook = new XLWorkbook();
try
{
    // === シート作成 ===
    CreateCoverSheet(workbook);
    CreateTableDefinitionSheet(workbook);
    CreateApiSpecSheet(workbook);

    workbook.SaveAs(outputPath);
    Console.WriteLine($"設計書を生成しました: {outputPath}");
}
finally
{
    workbook.Dispose();
}

// ================================================================
//   ヘルパー関数
// ================================================================

/// <summary>
/// 方眼紙グリッドの基本設定を適用
/// </summary>
void SetupGrid(IXLWorksheet ws, int printRows = 65)
{
    ws.Style.Font.FontName = FONT_NAME;
    ws.Style.Font.FontSize = FONT_SIZE_NORMAL;

    for (int c = 1; c <= MAX_COL; c++)
    {
        ws.Column(c).Width = GRID_COL_WIDTH;
    }

    // 印刷設定 (A4 横)
    ws.PageSetup.PageOrientation = XLPageOrientation.Landscape;
    ws.PageSetup.PaperSize = XLPaperSize.A4Paper;
    ws.PageSetup.Margins.Top = 1.0;
    ws.PageSetup.Margins.Bottom = 1.0;
    ws.PageSetup.Margins.Left = 1.5;
    ws.PageSetup.Margins.Right = 1.0;
    ws.PageSetup.PrintAreas.Add(1, 1, printRows, MAX_COL);
}

/// <summary>
/// セル結合＋テキスト書き込み（方眼紙の基本操作）
/// </summary>
IXLRange MergeWrite(
    IXLWorksheet ws,
    int row1, int col1, int row2, int col2,
    string text,
    bool bold = false,
    double fontSize = 0,
    XLAlignmentHorizontalValues hAlign = XLAlignmentHorizontalValues.Left,
    XLAlignmentVerticalValues vAlign = XLAlignmentVerticalValues.Center,
    bool wrapText = false)
{
    var range = ws.Range(row1, col1, row2, col2);
    range.Merge();
    ws.Cell(row1, col1).Value = text;
    range.Style.Font.Bold = bold;
    if (fontSize > 0) range.Style.Font.FontSize = fontSize;
    range.Style.Alignment.Horizontal = hAlign;
    range.Style.Alignment.Vertical = vAlign;
    range.Style.Alignment.WrapText = wrapText;
    return range;
}

/// <summary>外枠罫線を描画</summary>
void AddBorder(IXLWorksheet ws, int row1, int col1, int row2, int col2,
    XLBorderStyleValues style = XLBorderStyleValues.Thin)
{
    ws.Range(row1, col1, row2, col2).Style.Border.OutsideBorder = style;
}

/// <summary>ヘッダー背景色を塗る</summary>
void FillHeader(IXLWorksheet ws, int row1, int col1, int row2, int col2)
{
    ws.Range(row1, col1, row2, col2).Style.Fill.BackgroundColor = XLColor.FromHtml("#D9E2F3");
}

/// <summary>ラベル＋値の1行ペアを描画</summary>
void WriteInfoRow(IXLWorksheet ws, int row, int left, int labelEnd, int valueEnd, string label, string value)
{
    MergeWrite(ws, row, left, row, labelEnd, label,
        bold: true, hAlign: XLAlignmentHorizontalValues.Center);
    FillHeader(ws, row, left, row, labelEnd);
    AddBorder(ws, row, left, row, labelEnd);

    MergeWrite(ws, row, labelEnd + 1, row, valueEnd, value);
    AddBorder(ws, row, labelEnd + 1, row, valueEnd);
}

/// <summary>テーブルヘッダー行を描画</summary>
void WriteTableHeader(IXLWorksheet ws, int row, int[][] colDef, string[] headers)
{
    for (int i = 0; i < headers.Length; i++)
    {
        MergeWrite(ws, row, colDef[i][0], row, colDef[i][1], headers[i],
            bold: true, hAlign: XLAlignmentHorizontalValues.Center);
        FillHeader(ws, row, colDef[i][0], row, colDef[i][1]);
        AddBorder(ws, row, colDef[i][0], row, colDef[i][1]);
    }
}

/// <summary>テーブルデータ行を描画</summary>
void WriteTableRow(IXLWorksheet ws, int row, int[][] colDef, string[] data, int[] centerCols = null)
{
    centerCols ??= new int[] { 0 };
    for (int i = 0; i < data.Length; i++)
    {
        var align = Array.Exists(centerCols, c => c == i)
            ? XLAlignmentHorizontalValues.Center
            : XLAlignmentHorizontalValues.Left;
        MergeWrite(ws, row, colDef[i][0], row, colDef[i][1], data[i], hAlign: align);
        AddBorder(ws, row, colDef[i][0], row, colDef[i][1]);
    }
}

/// <summary>セクションヘッダーを描画</summary>
int WriteSectionHeader(IXLWorksheet ws, int row, int left, int right, string title)
{
    MergeWrite(ws, row, left, row, right, title, bold: true, fontSize: FONT_SIZE_SECTION);
    return row + 2;
}

/// <summary>コードブロック（等幅フォント）を描画</summary>
int WriteCodeBlock(IXLWorksheet ws, int row, int left, int right, string code)
{
    var lines = code.Split('\n');
    foreach (var line in lines)
    {
        MergeWrite(ws, row, left + 1, row, right - 1, line, fontSize: FONT_SIZE_SMALL);
        ws.Range(row, left + 1, row, right - 1).Style.Font.FontName = FONT_MONO;
        row++;
    }
    return row;
}

// ================================================================
//   Sheet 1: 表紙
// ================================================================
void CreateCoverSheet(XLWorkbook wb)
{
    var ws = wb.AddWorksheet("表紙");
    SetupGrid(ws, 52);

    int left = 2;
    int right = MAX_COL - 1;

    // 外枠（太罫線）
    AddBorder(ws, 2, left, 50, right, XLBorderStyleValues.Medium);

    // ===== ドキュメントタイトル =====
    MergeWrite(ws, 10, 5, 13, right - 3,
        "価格履歴機能 設計書",
        bold: true, fontSize: FONT_SIZE_TITLE,
        hAlign: XLAlignmentHorizontalValues.Center);

    MergeWrite(ws, 15, 5, 16, right - 3,
        "～ 商品価格変更履歴の記録・参照機能 ～",
        fontSize: FONT_SIZE_SECTION,
        hAlign: XLAlignmentHorizontalValues.Center);

    // ===== ドキュメント情報 =====
    int r = 22;
    int lc = 10, le = 18, ve = 35;

    WriteInfoRow(ws, r,     lc, le, ve, "プロジェクト名", "mobile-app-system");
    WriteInfoRow(ws, r + 1, lc, le, ve, "ドキュメント番号", "DES-PRICE-HIST-001");
    WriteInfoRow(ws, r + 2, lc, le, ve, "バージョン",       "1.0");
    WriteInfoRow(ws, r + 3, lc, le, ve, "作成日",          "2026年3月1日");
    WriteInfoRow(ws, r + 4, lc, le, ve, "作成者",          "開発チーム");
    WriteInfoRow(ws, r + 5, lc, le, ve, "ステータス",       "Draft");

    // ===== 承認欄 =====
    r = 32;
    MergeWrite(ws, r, lc, r, ve, "承認欄",
        bold: true, hAlign: XLAlignmentHorizontalValues.Center);
    FillHeader(ws, r, lc, r, ve);
    AddBorder(ws, r, lc, r, ve);

    int[][] approvalCols = {
        new[] { lc,      lc + 3 },
        new[] { lc + 4,  lc + 11 },
        new[] { lc + 12, lc + 17 },
        new[] { lc + 18, ve },
    };
    string[] approvalHdr = { "役割", "氏名", "日付", "署名" };
    WriteTableHeader(ws, 33, approvalCols, approvalHdr);

    string[] roles = { "承認者", "レビュアー", "作成者" };
    for (int i = 0; i < roles.Length; i++)
    {
        int row = 34 + i;
        MergeWrite(ws, row, approvalCols[0][0], row, approvalCols[0][1], roles[i],
            hAlign: XLAlignmentHorizontalValues.Center);
        for (int j = 0; j < approvalCols.Length; j++)
            AddBorder(ws, row, approvalCols[j][0], row, approvalCols[j][1]);
    }

    // ===== 変更履歴 =====
    r = 40;
    MergeWrite(ws, r, lc, r, ve, "変更履歴",
        bold: true, hAlign: XLAlignmentHorizontalValues.Center);
    FillHeader(ws, r, lc, r, ve);
    AddBorder(ws, r, lc, r, ve);

    int[][] histCols = {
        new[] { lc,      lc + 2 },
        new[] { lc + 3,  lc + 8 },
        new[] { lc + 9,  lc + 21 },
        new[] { lc + 22, ve },
    };
    string[] histHdr = { "Ver.", "日付", "変更内容", "著者" };
    WriteTableHeader(ws, 41, histCols, histHdr);

    WriteTableRow(ws, 42, histCols,
        new[] { "1.0", "2026/03/01", "初版作成", "開発チーム" },
        new[] { 0, 1, 3 });

    // 空行（将来の変更履歴用）
    for (int i = 0; i < 3; i++)
    {
        WriteTableRow(ws, 43 + i, histCols,
            new[] { "", "", "", "" }, new[] { 0 });
    }

    Console.WriteLine("  [1/3] 表紙シート ... 作成完了");
}

// ================================================================
//   Sheet 2: テーブル定義書
// ================================================================
void CreateTableDefinitionSheet(XLWorkbook wb)
{
    var ws = wb.AddWorksheet("テーブル定義書");
    SetupGrid(ws, 80);

    int left = 2;
    int right = MAX_COL - 1;
    int r = 2;

    // ===== タイトル =====
    MergeWrite(ws, r, left, r + 1, right,
        "テーブル定義書",
        bold: true, fontSize: FONT_SIZE_SECTION,
        hAlign: XLAlignmentHorizontalValues.Center);
    FillHeader(ws, r, left, r + 1, right);
    AddBorder(ws, r, left, r + 1, right, XLBorderStyleValues.Medium);

    // ===== テーブル基本情報 =====
    r = 5;
    int le = left + 7;

    WriteInfoRow(ws, r,     left, le, right, "テーブル物理名", "product_price_history");
    WriteInfoRow(ws, r + 1, left, le, right, "テーブル論理名", "商品価格履歴");
    WriteInfoRow(ws, r + 2, left, le, right, "スキーマ",      "（SQLite ─ スキーマなし）");
    WriteInfoRow(ws, r + 3, left, le, right, "概要",
        "商品の価格変更履歴を管理する。管理者が商品情報を更新し価格が変更された場合に1レコード挿入される。");
    WriteInfoRow(ws, r + 4, left, le, right, "関連テーブル",
        "products（商品）、users（ユーザー）");
    WriteInfoRow(ws, r + 5, left, le, right, "データ保持期間",
        "5年間（1825日）。超過データは定期バッチで削除する。");

    // ================================================================
    // ■ カラム定義
    // ================================================================
    r = 13;
    r = WriteSectionHeader(ws, r, left, right, "■ カラム定義");

    int[][] colDef = {
        new[] { left,      left + 1 },   // No
        new[] { left + 2,  left + 8 },   // 論理名
        new[] { left + 9,  left + 17 },  // 物理名
        new[] { left + 18, left + 24 },  // データ型
        new[] { left + 25, left + 26 },  // NULL許可
        new[] { left + 27, left + 28 },  // PK
        new[] { left + 29, left + 30 },  // FK
        new[] { left + 31, left + 36 },  // デフォルト
        new[] { left + 37, right },      // 説明
    };
    string[] colHdr = { "No", "論理名", "物理名", "データ型", "NULL", "PK", "FK", "デフォルト", "説明" };
    WriteTableHeader(ws, r, colDef, colHdr);

    string[][] columns = {
        new[] { "1", "価格履歴ID", "price_history_id", "INTEGER", "×", "○", "",  "AUTO INCREMENT", "主キー・自動採番" },
        new[] { "2", "商品ID",     "product_id",       "INTEGER", "×", "",  "○", "─",              "productsへの外部キー" },
        new[] { "3", "変更前単価", "old_price",         "INTEGER", "×", "",  "",  "─",              "変更前の商品単価（円）" },
        new[] { "4", "変更後単価", "new_price",         "INTEGER", "×", "",  "",  "─",              "変更後の商品単価（円）" },
        new[] { "5", "変更日時",   "changed_at",        "TEXT",    "×", "",  "",  "datetime(now)",   "価格が変更された日時" },
        new[] { "6", "変更者ID",   "changed_by",        "INTEGER", "×", "",  "○", "─",              "usersへの外部キー" },
        new[] { "7", "変更理由",   "change_reason",     "TEXT",    "○", "",  "",  "─",              "価格変更の理由（任意）" },
        new[] { "8", "作成日時",   "created_at",        "TEXT",    "×", "",  "",  "datetime(now)",   "レコード作成日時" },
    };

    r++;
    int[] centerIdx = { 0, 4, 5, 6 };
    foreach (var col in columns)
    {
        WriteTableRow(ws, r, colDef, col, centerIdx);
        r++;
    }

    // ================================================================
    // ■ インデックス定義
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ インデックス定義");

    int[][] idxDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 16 },
        new[] { left + 17, left + 28 },
        new[] { left + 29, left + 32 },
        new[] { left + 33, right },
    };
    string[] idxHdr = { "No", "インデックス名", "対象カラム", "UNIQUE", "説明" };
    WriteTableHeader(ws, r, idxDef, idxHdr);

    string[][] indexes = {
        new[] { "1", "PK (price_history_id)",          "price_history_id",        "─", "主キー（自動生成）" },
        new[] { "2", "idx_price_hist_product_id",      "product_id",              "─", "商品IDでの検索" },
        new[] { "3", "idx_price_hist_changed_at",      "changed_at",              "─", "変更日時の範囲検索" },
        new[] { "4", "idx_price_hist_prod_changed",    "product_id, changed_at",  "─", "商品別・日時複合検索" },
    };

    r++;
    foreach (var idx in indexes)
    {
        WriteTableRow(ws, r, idxDef, idx, new[] { 0, 3 });
        r++;
    }

    // ================================================================
    // ■ 外部キー定義
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ 外部キー定義");

    int[][] fkDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 10 },
        new[] { left + 11, left + 24 },
        new[] { left + 25, left + 32 },
        new[] { left + 33, right },
    };
    string[] fkHdr = { "No", "カラム", "参照先 テーブル.カラム", "ON DELETE", "説明" };
    WriteTableHeader(ws, r, fkDef, fkHdr);

    string[][] fks = {
        new[] { "1", "product_id", "products.product_id", "RESTRICT", "商品削除時は履歴を保護" },
        new[] { "2", "changed_by", "users.user_id",       "RESTRICT", "変更者情報を保持" },
    };

    r++;
    foreach (var fk in fks)
    {
        WriteTableRow(ws, r, fkDef, fk, new[] { 0, 3 });
        r++;
    }

    // ================================================================
    // ■ CHECK制約
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ CHECK制約");

    int[][] conDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 16 },
        new[] { left + 17, right },
    };
    string[] conHdr = { "No", "制約名", "制約内容" };
    WriteTableHeader(ws, r, conDef, conHdr);

    string[][] constraints = {
        new[] { "1", "chk_old_price",     "CHECK (old_price >= 1) ── 変更前単価は1円以上" },
        new[] { "2", "chk_new_price",     "CHECK (new_price >= 1) ── 変更後単価は1円以上" },
        new[] { "3", "chk_price_changed", "CHECK (old_price <> new_price) ── 変更前後の値が異なること" },
    };

    r++;
    foreach (var con in constraints)
    {
        WriteTableRow(ws, r, conDef, con, new[] { 0 });
        r++;
    }

    // ================================================================
    // ■ DDL（テーブル作成SQL）
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ DDL（テーブル作成SQL）");

    string ddl =
@"-- ======================================
-- 7. PRODUCT_PRICE_HISTORY テーブル
-- ======================================
CREATE TABLE product_price_history (
    price_history_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id        INTEGER NOT NULL,
    old_price         INTEGER NOT NULL,
    new_price         INTEGER NOT NULL,
    changed_at        TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    changed_by        INTEGER NOT NULL,
    change_reason     TEXT,
    created_at        TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 外部キー
    CONSTRAINT fk_price_hist_product FOREIGN KEY (product_id)
        REFERENCES products(product_id) ON DELETE RESTRICT,
    CONSTRAINT fk_price_hist_user FOREIGN KEY (changed_by)
        REFERENCES users(user_id) ON DELETE RESTRICT,

    -- CHECK制約
    CONSTRAINT chk_old_price     CHECK (old_price >= 1),
    CONSTRAINT chk_new_price     CHECK (new_price >= 1),
    CONSTRAINT chk_price_changed CHECK (old_price <> new_price)
);

-- インデックス
CREATE INDEX idx_price_hist_product_id
    ON product_price_history(product_id);
CREATE INDEX idx_price_hist_changed_at
    ON product_price_history(changed_at);
CREATE INDEX idx_price_hist_prod_changed
    ON product_price_history(product_id, changed_at);";

    r = WriteCodeBlock(ws, r, left, right, ddl);

    // ================================================================
    // ■ トリガー（価格変更時の自動記録）
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ トリガー（価格変更時の自動記録）");

    string trigger =
@"-- products テーブルの unit_price 更新時に自動で履歴を挿入
CREATE TRIGGER trg_record_price_history
AFTER UPDATE OF unit_price ON products
FOR EACH ROW
WHEN OLD.unit_price <> NEW.unit_price
BEGIN
    INSERT INTO product_price_history
        (product_id, old_price, new_price, changed_by, change_reason)
    VALUES
        (NEW.product_id, OLD.unit_price, NEW.unit_price,
         -- changed_by は API 層で設定するため、トリガーでは 0（システム）とする
         0, '商品情報更新による自動記録');
END;";

    r = WriteCodeBlock(ws, r, left, right, trigger);

    // ================================================================
    // ■ データ保持ポリシー
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ データ保持ポリシー");

    string[] policies = {
        "1. 価格履歴データの最大保存期間は 5年間（1825日）とする。",
        "2. 5年を超過したレコードは定期バッチ処理にて物理削除する。",
        "3. 削除バッチは日次で実行し、changed_at が基準日から5年超過したレコードを対象とする。",
        "4. 削除前にはバックアップログを出力し、削除件数を記録する。",
        "5. 削除SQL例: DELETE FROM product_price_history",
        "   WHERE changed_at < datetime('now', 'localtime', '-5 years');",
    };

    foreach (var p in policies)
    {
        MergeWrite(ws, r, left + 1, r, right - 1, p, fontSize: FONT_SIZE_NORMAL);
        r++;
    }

    // 印刷範囲を調整
    ws.PageSetup.PrintAreas.Clear();
    ws.PageSetup.PrintAreas.Add(1, 1, r + 2, MAX_COL);

    Console.WriteLine("  [2/3] テーブル定義書シート ... 作成完了");
}

// ================================================================
//   Sheet 3: API仕様書
// ================================================================
void CreateApiSpecSheet(XLWorkbook wb)
{
    var ws = wb.AddWorksheet("API仕様書");
    SetupGrid(ws, 100);

    int left = 2;
    int right = MAX_COL - 1;
    int r = 2;

    // ===== タイトル =====
    MergeWrite(ws, r, left, r + 1, right,
        "API仕様書 ─ 価格履歴取得API（API-020）",
        bold: true, fontSize: FONT_SIZE_SECTION,
        hAlign: XLAlignmentHorizontalValues.Center);
    FillHeader(ws, r, left, r + 1, right);
    AddBorder(ws, r, left, r + 1, right, XLBorderStyleValues.Medium);

    // ===== API基本情報 =====
    r = 5;
    int le = left + 7;

    WriteInfoRow(ws, r,     left, le, right, "API ID",         "API-020");
    WriteInfoRow(ws, r + 1, left, le, right, "API名",          "価格履歴取得");
    WriteInfoRow(ws, r + 2, left, le, right, "エンドポイント", "GET /api/v1/products/{productId}/price-history");
    WriteInfoRow(ws, r + 3, left, le, right, "HTTPメソッド",   "GET");
    WriteInfoRow(ws, r + 4, left, le, right, "認証",           "JWT認証必須（Authorization: Bearer {TOKEN}）");
    WriteInfoRow(ws, r + 5, left, le, right, "権限",           "user / admin");
    WriteInfoRow(ws, r + 6, left, le, right, "概要",
        "指定商品の価格変更履歴を取得する。最大5年分の履歴を日時降順で返却する。");

    // ================================================================
    // ■ パスパラメータ
    // ================================================================
    r = 14;
    r = WriteSectionHeader(ws, r, left, right, "■ パスパラメータ");

    int[][] pathDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 10 },
        new[] { left + 11, left + 18 },
        new[] { left + 19, left + 22 },
        new[] { left + 23, right },
    };
    string[] pathHdr = { "No", "パラメータ名", "型", "必須", "説明" };
    WriteTableHeader(ws, r, pathDef, pathHdr);

    r++;
    WriteTableRow(ws, r, pathDef,
        new[] { "1", "productId", "integer", "○", "対象商品のID" },
        new[] { 0, 3 });

    // ================================================================
    // ■ クエリパラメータ
    // ================================================================
    r += 2;
    r = WriteSectionHeader(ws, r, left, right, "■ クエリパラメータ");

    int[][] qDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 10 },
        new[] { left + 11, left + 18 },
        new[] { left + 19, left + 22 },
        new[] { left + 23, left + 30 },
        new[] { left + 31, right },
    };
    string[] qHdr = { "No", "パラメータ名", "型", "必須", "デフォルト", "説明" };
    WriteTableHeader(ws, r, qDef, qHdr);

    string[][] queryParams = {
        new[] { "1", "startDate", "string (ISO 8601)", "─", "5年前の日付", "検索開始日 (例: 2021-03-01)" },
        new[] { "2", "endDate",   "string (ISO 8601)", "─", "現在日付",    "検索終了日 (例: 2026-03-01)" },
        new[] { "3", "page",      "integer",           "─", "1",           "ページ番号（1以上）" },
        new[] { "4", "limit",     "integer",           "─", "20",          "1ページあたり件数（最大100）" },
    };

    r++;
    foreach (var q in queryParams)
    {
        WriteTableRow(ws, r, qDef, q, new[] { 0, 3 });
        r++;
    }

    // ================================================================
    // ■ リクエスト例
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ リクエスト例（cURL）");

    string curl =
@"curl -X GET \
  ""http://localhost:8080/api/v1/products/1/price-history?startDate=2021-03-01&endDate=2026-03-01&page=1&limit=20"" \
  -H ""Authorization: Bearer YOUR_JWT_TOKEN""";

    r = WriteCodeBlock(ws, r, left, right, curl);

    // ================================================================
    // ■ レスポンス（成功時 200 OK）
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ レスポンス（成功時: 200 OK）");

    string responseJson =
@"{
  ""data"": {
    ""priceHistory"": [
      {
        ""priceHistoryId"": 1,
        ""productId"": 1,
        ""oldPrice"": 1000,
        ""newPrice"": 1200,
        ""changedAt"": ""2025-06-15T10:30:00"",
        ""changedBy"": {
          ""userId"": 3,
          ""userName"": ""管理者""
        },
        ""changeReason"": ""原材料費高騰に伴う価格改定""
      }
    ],
    ""pagination"": {
      ""currentPage"": 1,
      ""totalPages"": 3,
      ""totalItems"": 45,
      ""itemsPerPage"": 20
    }
  },
  ""timestamp"": ""2026-03-01T12:00:00Z""
}";

    r = WriteCodeBlock(ws, r, left, right, responseJson);

    // ================================================================
    // ■ レスポンスフィールド定義
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ レスポンスフィールド定義");

    int[][] rfDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 20 },
        new[] { left + 21, left + 28 },
        new[] { left + 29, right },
    };
    string[] rfHdr = { "No", "フィールド", "型", "説明" };
    WriteTableHeader(ws, r, rfDef, rfHdr);

    string[][] respFields = {
        new[] { "1",  "priceHistory",                     "array",       "価格履歴レコードの配列" },
        new[] { "2",  "priceHistory[].priceHistoryId",    "integer",     "価格履歴ID" },
        new[] { "3",  "priceHistory[].productId",         "integer",     "商品ID" },
        new[] { "4",  "priceHistory[].oldPrice",          "integer",     "変更前単価（円）" },
        new[] { "5",  "priceHistory[].newPrice",          "integer",     "変更後単価（円）" },
        new[] { "6",  "priceHistory[].changedAt",         "string",      "変更日時（ISO 8601形式）" },
        new[] { "7",  "priceHistory[].changedBy.userId",  "integer",     "変更者ユーザーID" },
        new[] { "8",  "priceHistory[].changedBy.userName", "string",     "変更者ユーザー名" },
        new[] { "9",  "priceHistory[].changeReason",      "string|null", "変更理由（任意項目）" },
        new[] { "10", "pagination.currentPage",           "integer",     "現在のページ番号" },
        new[] { "11", "pagination.totalPages",            "integer",     "総ページ数" },
        new[] { "12", "pagination.totalItems",            "integer",     "総件数" },
        new[] { "13", "pagination.itemsPerPage",          "integer",     "1ページあたりの件数" },
    };

    r++;
    foreach (var f in respFields)
    {
        WriteTableRow(ws, r, rfDef, f, new[] { 0 });
        r++;
    }

    // ================================================================
    // ■ レスポンス（該当なし 200 OK）
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ レスポンス（該当なし: 200 OK）");

    string emptyResponse =
@"{
  ""data"": {
    ""priceHistory"": [],
    ""pagination"": {
      ""currentPage"": 1,
      ""totalPages"": 0,
      ""totalItems"": 0,
      ""itemsPerPage"": 20
    }
  },
  ""timestamp"": ""2026-03-01T12:00:00Z""
}";

    r = WriteCodeBlock(ws, r, left, right, emptyResponse);

    // ================================================================
    // ■ エラーレスポンス
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ エラーレスポンス");

    int[][] errDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 7 },
        new[] { left + 8,  left + 16 },
        new[] { left + 17, left + 30 },
        new[] { left + 31, right },
    };
    string[] errHdr = { "No", "HTTP", "エラーコード", "メッセージ", "発生条件" };
    WriteTableHeader(ws, r, errDef, errHdr);

    string[][] errors = {
        new[] { "1", "400", "VAL_001",     "入力値が不正です",                "startDate/endDate形式不正" },
        new[] { "2", "401", "AUTH_001",    "認証トークンが無効です",          "トークン未送信・不正" },
        new[] { "3", "401", "AUTH_004",    "トークンの有効期限が切れています", "有効期限切れ" },
        new[] { "4", "403", "AUTH_005",    "権限がありません",                "権限なしユーザー" },
        new[] { "5", "404", "PRODUCT_001", "商品が見つかりません",            "存在しないproductId指定" },
        new[] { "6", "500", "SYS_001",     "サーバー内部エラー",              "予期しないエラー" },
    };

    r++;
    foreach (var err in errors)
    {
        WriteTableRow(ws, r, errDef, err, new[] { 0, 1 });
        r++;
    }

    // ================================================================
    // ■ エラーレスポンス形式
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ エラーレスポンス形式");

    string errorJson =
@"{
  ""error"": {
    ""code"": ""PRODUCT_001"",
    ""message"": ""商品が見つかりません""
  },
  ""timestamp"": ""2026-03-01T12:00:00Z""
}";

    r = WriteCodeBlock(ws, r, left, right, errorJson);

    // ================================================================
    // ■ BFF転送仕様
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ BFF転送仕様");

    int[][] bffDef = {
        new[] { left,      left + 1 },
        new[] { left + 2,  left + 8 },
        new[] { left + 9,  left + 26 },
        new[] { left + 27, right },
    };
    string[] bffHdr = { "No", "BFF", "BFFエンドポイント", "転送先 Web API" };
    WriteTableHeader(ws, r, bffDef, bffHdr);

    string[][] bffRoutes = {
        new[] { "1", "Mobile", "GET /api/mobile/products/{id}/price-history",
                "GET /api/v1/products/{id}/price-history" },
        new[] { "2", "Admin",  "GET /api/admin/products/{id}/price-history",
                "GET /api/v1/products/{id}/price-history" },
    };

    r++;
    foreach (var route in bffRoutes)
    {
        WriteTableRow(ws, r, bffDef, route, new[] { 0 });
        r++;
    }

    // ================================================================
    // ■ 備考・補足事項
    // ================================================================
    r += 1;
    r = WriteSectionHeader(ws, r, left, right, "■ 備考・補足事項");

    string[] notes = {
        "1. 価格履歴は商品更新API（API-013: PUT /api/v1/products/{id}）で価格変更が",
        "   行われた際に、サーバー側で自動的に product_price_history テーブルに記録される。",
        "2. データ保持期間は5年間。超過データは定期バッチ（日次実行）にて物理削除する。",
        "3. startDate / endDate を省略した場合、保持期間内の全履歴が返却される。",
        "4. レスポンスは changed_at の降順（新しい順）でソートされる。",
        "5. 管理者・一般ユーザーともに参照可能。価格変更操作は管理者のみ。",
        "6. ページネーション仕様は既存の商品一覧API（API-010）と同一。",
        "7. 本APIは既存のAPI体系（/api/v1/）に準拠して設計されている。",
    };

    foreach (var note in notes)
    {
        MergeWrite(ws, r, left + 1, r, right - 1, note, fontSize: FONT_SIZE_NORMAL);
        r++;
    }

    // 印刷範囲を調整
    ws.PageSetup.PrintAreas.Clear();
    ws.PageSetup.PrintAreas.Add(1, 1, r + 2, MAX_COL);

    Console.WriteLine("  [3/3] API仕様書シート ... 作成完了");
}
