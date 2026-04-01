#!/usr/bin/env dotnet-script
#r "nuget: ClosedXML, 0.105.0"

using ClosedXML.Excel;
using System.IO;
using System.Text.Json;

// ---------------------------------------------------------------------------
// e2e テスト結果報告書 生成スクリプト
//
// Usage:
//   dotnet script generate-report.csx -- <json-results> <template-xlsx> <output-xlsx> [test-author] [test-operator]
//
// Arguments:
//   [0] json-results   : Playwright JSON レポートファイルのパス
//   [1] template-xlsx  : テスト仕様書ひな型 Excel テンプレートのパス
//   [2] output-xlsx    : 出力先 Excel ファイルのパス
//   [3] test-author    : テスト仕様作成者名（省略時は空欄）
//   [4] test-operator  : テスト作業者名（省略時は空欄）
// ---------------------------------------------------------------------------

// --- データクラス ---
class TestScenario
{
    public string Id { get; set; }
    public string Title { get; set; }
    public string Status { get; set; }
    public int DurationMs { get; set; }
    public string ErrorMessage { get; set; }
    public List<ScreenshotInfo> Screenshots { get; set; } = new();
}

class ScreenshotInfo
{
    public string Name { get; set; }
    public string Path { get; set; }
}

// --- 引数チェック ---
if (Args.Count < 3)
{
    Console.Error.WriteLine("Usage: dotnet script generate-report.csx -- <json-results> <template-xlsx> <output-xlsx> [test-author] [test-operator]");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Arguments:");
    Console.Error.WriteLine("  json-results   : Playwright JSON レポートファイルのパス");
    Console.Error.WriteLine("  template-xlsx  : テスト仕様書ひな型 Excel テンプレートのパス");
    Console.Error.WriteLine("  output-xlsx    : 出力先 Excel ファイルのパス");
    Console.Error.WriteLine("  test-author    : テスト仕様作成者名（省略時は空欄）");
    Console.Error.WriteLine("  test-operator  : テスト作業者名（省略時は空欄）");
    Environment.Exit(1);
    return;
}

var jsonPath = Args[0];
var templatePath = Args[1];
var outputPath = Args[2];
var testAuthor = Args.Count > 3 ? Args[3] : "";
var testOperator = Args.Count > 4 ? Args[4] : "";

// --- ファイル存在チェック ---
if (!File.Exists(jsonPath))
{
    Console.Error.WriteLine($"エラー: JSON ファイルが見つかりません: {jsonPath}");
    Environment.Exit(1);
    return;
}
if (!File.Exists(templatePath))
{
    Console.Error.WriteLine($"エラー: テンプレートファイルが見つかりません: {templatePath}");
    Environment.Exit(1);
    return;
}

// --- JSON 解析 ---
Console.WriteLine("JSON テスト結果を読み込み中...");
var jsonContent = File.ReadAllText(jsonPath);
var doc = JsonDocument.Parse(jsonContent);
var root = doc.RootElement;

var scenarios = new List<TestScenario>();
int scenarioCounter = 1;

void ProcessSuites(JsonElement element, string parentTitle)
{
    string currentTitle = parentTitle;
    if (element.TryGetProperty("title", out var titleProp))
    {
        var t = titleProp.GetString();
        if (!string.IsNullOrEmpty(t))
            currentTitle = string.IsNullOrEmpty(parentTitle) ? t : $"{parentTitle} - {t}";
    }

    // ネストされた suites を再帰処理
    if (element.TryGetProperty("suites", out var suites))
    {
        foreach (var suite in suites.EnumerateArray())
            ProcessSuites(suite, currentTitle);
    }

    // specs を処理
    if (element.TryGetProperty("specs", out var specs))
    {
        foreach (var spec in specs.EnumerateArray())
        {
            var specTitle = spec.GetProperty("title").GetString();
            var fullTitle = string.IsNullOrEmpty(currentTitle) ? specTitle : $"{currentTitle} - {specTitle}";

            if (!spec.TryGetProperty("tests", out var tests))
                continue;

            foreach (var test in tests.EnumerateArray())
            {
                if (!test.TryGetProperty("results", out var results))
                    continue;

                // 最後の結果を取得（リトライ後の最終結果）
                JsonElement lastResult = default;
                foreach (var r in results.EnumerateArray())
                    lastResult = r;

                if (lastResult.ValueKind == JsonValueKind.Undefined)
                    continue;

                var scenario = new TestScenario
                {
                    Id = $"TC-{scenarioCounter:D2}",
                    Title = fullTitle,
                    Status = lastResult.GetProperty("status").GetString(),
                    DurationMs = lastResult.TryGetProperty("duration", out var dur) ? dur.GetInt32() : 0
                };

                // エラーメッセージ抽出
                if (lastResult.TryGetProperty("errors", out var errors))
                {
                    var errorMessages = new List<string>();
                    foreach (var err in errors.EnumerateArray())
                    {
                        if (err.TryGetProperty("message", out var msg))
                            errorMessages.Add(msg.GetString());
                    }
                    if (errorMessages.Count > 0)
                        scenario.ErrorMessage = string.Join("\n", errorMessages);
                }
                if (string.IsNullOrEmpty(scenario.ErrorMessage) && lastResult.TryGetProperty("error", out var singleError))
                {
                    if (singleError.TryGetProperty("message", out var msg))
                        scenario.ErrorMessage = msg.GetString();
                }

                // スクリーンショット抽出
                if (lastResult.TryGetProperty("attachments", out var attachments))
                {
                    foreach (var att in attachments.EnumerateArray())
                    {
                        if (!att.TryGetProperty("contentType", out var ctProp))
                            continue;
                        var contentType = ctProp.GetString();
                        if (!contentType.StartsWith("image/"))
                            continue;
                        if (!att.TryGetProperty("path", out var pathProp))
                            continue;

                        var imgPath = pathProp.GetString();
                        if (!File.Exists(imgPath))
                        {
                            Console.WriteLine($"  警告: スクリーンショットが見つかりません: {imgPath}");
                            continue;
                        }

                        var attName = att.TryGetProperty("name", out var nameProp) ? nameProp.GetString() : "screenshot";
                        scenario.Screenshots.Add(new ScreenshotInfo { Name = attName, Path = imgPath });
                    }
                }

                scenarios.Add(scenario);
                scenarioCounter++;
            }
        }
    }
}

// ルートの suites を処理
if (root.TryGetProperty("suites", out var rootSuites))
{
    foreach (var suite in rootSuites.EnumerateArray())
        ProcessSuites(suite, "");
}

Console.WriteLine($"テストシナリオ数: {scenarios.Count}");

// --- 出力先ディレクトリ作成 ---
var outputDir = Path.GetDirectoryName(Path.GetFullPath(outputPath));
if (!string.IsNullOrEmpty(outputDir) && !Directory.Exists(outputDir))
    Directory.CreateDirectory(outputDir);

// --- Excel テンプレートを開いてレポート生成 ---
Console.WriteLine("Excel レポートを生成中...");
using (var workbook = new XLWorkbook(templatePath))
{
    // ========================================
    // 表紙シート
    // ========================================
    var cover = workbook.Worksheet("表紙");

    // テスト仕様作成者・テスト作業者
    cover.Cell("E4").Value = testAuthor;
    cover.Cell("E5").Value = testOperator;

    // 実行履歴（最初の空行に追記: 行 9〜19）
    for (int r = 9; r <= 19; r++)
    {
        if (string.IsNullOrWhiteSpace(cover.Cell(r, 2).GetFormattedString()))
        {
            cover.Cell($"B{r}").Value = "1.0";
            cover.Cell($"D{r}").Value = testOperator;
            cover.Cell($"F{r}").Value = $"e2e テスト実行 ({DateTime.Now:yyyy/MM/dd HH:mm})";
            break;
        }
    }

    // ========================================
    // テストシナリオ一覧シート
    // ========================================
    var scenarioList = workbook.Worksheet("テストシナリオ一覧");

    for (int i = 0; i < scenarios.Count; i++)
    {
        var s = scenarios[i];
        int row = 3 + i; // ヘッダーは行 2、データは行 3 から
        var resultSymbol = s.Status == "passed" ? "○" : "×";

        // データ記入
        scenarioList.Cell(row, 2).Value = s.Id;            // B: シナリオ番号
        scenarioList.Cell(row, 3).Value = s.Title;         // C: テスト内容
        scenarioList.Cell(row, 4).Value = testOperator;    // D: 実施者
        scenarioList.Cell(row, 5).Value = resultSymbol;    // E: 結果

        // 罫線（Thin）
        for (int c = 2; c <= 6; c++)
            scenarioList.Cell(row, c).Style.Border.OutsideBorder = XLBorderStyleValues.Thin;

        // 失敗テストの強調表示
        if (s.Status != "passed")
        {
            scenarioList.Cell(row, 5).Style.Font.FontColor = XLColor.Red;
            scenarioList.Cell(row, 5).Style.Font.Bold = true;
        }

        // ========================================
        // スクリーンショットシート作成
        // ========================================
        if (s.Screenshots.Count > 0)
        {
            var sheetName = s.Id; // TC-XX（31 文字制限に収まる）
            var ssSheet = workbook.Worksheets.Add(sheetName);

            // --- ヘッダー情報 ---
            // テストシナリオ名
            ssSheet.Cell("B2").Value = $"{s.Id}: {s.Title}";
            ssSheet.Cell("B2").Style.Font.Bold = true;
            ssSheet.Cell("B2").Style.Font.FontSize = 12;

            // 結果と実行時間
            ssSheet.Cell("B3").Value = "結果:";
            ssSheet.Cell("B3").Style.Font.Bold = true;
            ssSheet.Cell("C3").Value = resultSymbol;
            if (s.Status != "passed")
            {
                ssSheet.Cell("C3").Style.Font.FontColor = XLColor.Red;
                ssSheet.Cell("C3").Style.Font.Bold = true;
            }
            ssSheet.Cell("D3").Value = "実行時間:";
            ssSheet.Cell("D3").Style.Font.Bold = true;
            ssSheet.Cell("E3").Value = $"{s.DurationMs / 1000.0:F1}s";

            // エラーメッセージ（失敗時のみ）
            int contentStartRow = 5;
            if (!string.IsNullOrEmpty(s.ErrorMessage))
            {
                ssSheet.Cell("B4").Value = "エラー:";
                ssSheet.Cell("B4").Style.Font.Bold = true;
                ssSheet.Cell("B4").Style.Font.FontColor = XLColor.Red;
                // エラーメッセージは長くなる可能性があるので折り返し
                ssSheet.Cell("C4").Value = s.ErrorMessage.Length > 500
                    ? s.ErrorMessage.Substring(0, 500) + "..."
                    : s.ErrorMessage;
                ssSheet.Cell("C4").Style.Alignment.WrapText = true;
                contentStartRow = 6;
            }

            // --- スクリーンショット埋め込み ---
            int imgRow = contentStartRow + 1;
            for (int j = 0; j < s.Screenshots.Count; j++)
            {
                var ss = s.Screenshots[j];

                // スクリーンショットラベル
                ssSheet.Cell(imgRow, 2).Value = ss.Name;
                ssSheet.Cell(imgRow, 2).Style.Font.Bold = true;
                ssSheet.Cell(imgRow, 2).Style.Font.FontSize = 10;
                imgRow++;

                try
                {
                    var pic = ssSheet.AddPicture(ss.Path);
                    pic.MoveTo(ssSheet.Cell(imgRow, 2));

                    // スクリーンショットを幅 600px 以内に収める
                    if (pic.Width > 600)
                    {
                        double scale = 600.0 / pic.Width;
                        pic.Scale(scale);
                    }

                    // 画像の高さに基づいて次の画像の開始行を計算（行高15px想定）
                    int rowsNeeded = Math.Max((int)Math.Ceiling(pic.Height / 15.0), 5);
                    imgRow += rowsNeeded + 2;
                }
                catch (Exception ex)
                {
                    ssSheet.Cell(imgRow, 2).Value = $"画像読込エラー: {ex.Message}";
                    ssSheet.Cell(imgRow, 2).Style.Font.FontColor = XLColor.Red;
                    imgRow += 3;
                }
            }

            // シナリオ一覧のリンク列に内部ハイパーリンクを設定
            scenarioList.Cell(row, 6).Value = sheetName;
            scenarioList.Cell(row, 6).SetHyperlink(new XLHyperlink($"'{sheetName}'!A1"));
            scenarioList.Cell(row, 6).Style.Font.FontColor = XLColor.Blue;
            scenarioList.Cell(row, 6).Style.Font.Underline = XLFontUnderlineValues.Single;
        }
        else
        {
            scenarioList.Cell(row, 6).Value = "(スクリーンショットなし)";
        }
    }

    // テンプレートの {シナリオ番号} シートを削除
    if (workbook.Worksheets.TryGetWorksheet("{シナリオ番号}", out var tmplSheet))
        workbook.Worksheets.Delete("{シナリオ番号}");

    // 保存
    workbook.SaveAs(outputPath);
}

// --- サマリー出力 ---
var passCount = scenarios.Count(s => s.Status == "passed");
var failCount = scenarios.Count(s => s.Status != "passed");
var ssCount = scenarios.Count(s => s.Screenshots.Count > 0);

Console.WriteLine();
Console.WriteLine("╔══════════════════════════════════════════╗");
Console.WriteLine("║     テスト結果報告書を作成しました       ║");
Console.WriteLine("╚══════════════════════════════════════════╝");
Console.WriteLine($"  出力先:               {outputPath}");
Console.WriteLine($"  テストシナリオ数:     {scenarios.Count}");
Console.WriteLine($"  成功:                 {passCount}");
Console.WriteLine($"  失敗:                 {failCount}");
Console.WriteLine($"  スクリーンショット数: {ssCount} シート");
Console.WriteLine();

if (failCount > 0)
{
    Console.WriteLine("失敗テスト:");
    foreach (var s in scenarios.Where(s => s.Status != "passed"))
    {
        Console.WriteLine($"  {s.Id}: {s.Title} [{s.Status}]");
        if (!string.IsNullOrEmpty(s.ErrorMessage))
        {
            var shortMsg = s.ErrorMessage.Length > 100
                ? s.ErrorMessage.Substring(0, 100) + "..."
                : s.ErrorMessage;
            Console.WriteLine($"        → {shortMsg}");
        }
    }
}
