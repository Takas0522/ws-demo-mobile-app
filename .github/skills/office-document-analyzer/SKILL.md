---
name: office-document-analyzer
description: Office ドキュメント（Excel / PowerPoint 等）の設計書を解析するためのガイド。設計書の内容を読み取り・理解する際には、必ずこのスキルを参照して従ってください。
---

# Office ドキュメント解析ガイド

このスキルは、Excel / PowerPoint などの Office ドキュメントで書かれた設計書を読み取り、
内容を理解するための手順とパターンを提供します。

## 1. 基本フロー

```
1. 解析対象のドキュメントファイルを確認する
2. C#スクリプトでファイルを解析する
3. 解析結果から仕様・内容を理解する
4. 理解した内容に基づいて必要な作業を行う
```

### 実行方法の違い

**Windows (PowerShell)**
- ヒアドキュメント（`@'...'@`）とパイプを使用
- `dotnet run -` にパイプして実行

**macOS/Linux (Zsh/Bash)**
- 一時ファイル（`.csx`）を作成
- `dotnet script <ファイル名> -- <引数>` で実行
- スクリプト内で `Args` (大文字) を使用
- `Args.Count` でパラメータ数を取得
- `#r "nuget: パッケージ名, バージョン"` でパッケージ参照

---

## 2. Excel ファイル解析（ClosedXML）

### 2.1 構造把握スクリプト

まずファイル全体の構造を把握します。

#### PowerShell版

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook(filePath);

Console.WriteLine($"=== {Path.GetFileName(filePath)} ===");
Console.WriteLine($"シート数: {workbook.Worksheets.Count}");

foreach (var ws in workbook.Worksheets)
{
    Console.WriteLine($"\n--- シート: {ws.Name} ---");
    var range = ws.RangeUsed();
    if (range == null) { Console.WriteLine("(空)"); continue; }

    Console.WriteLine($"使用範囲: {range.RangeAddress}");
    Console.WriteLine($"行数: {range.RowCount()}, 列数: {range.ColumnCount()}");
    Console.WriteLine($"マージセル数: {ws.MergedRanges.Count}");
    Console.WriteLine($"画像数: {ws.Pictures.Count}");
}
'@ | dotnet run - -- "<ファイルパス>"
```

#### Zsh/Bash版

```bash
# 一時スクリプトファイルを作成
cat > temp/analyze_excel_structure.csx << 'EOF'
#!/usr/bin/env dotnet-script
#r "nuget: ClosedXML, 0.105.0"

using ClosedXML.Excel;
using System.IO;

var filePath = Args[0];
using (var workbook = new XLWorkbook(filePath))
{
    Console.WriteLine($"=== {Path.GetFileName(filePath)} ===");
    Console.WriteLine($"シート数: {workbook.Worksheets.Count}");

    foreach (var ws in workbook.Worksheets)
    {
        Console.WriteLine($"\n--- シート: {ws.Name} ---");
        var range = ws.RangeUsed();
        if (range == null) { Console.WriteLine("(空)"); continue; }

        Console.WriteLine($"使用範囲: {range.RangeAddress}");
        Console.WriteLine($"行数: {range.RowCount()}, 列数: {range.ColumnCount()}");
        Console.WriteLine($"マージセル数: {ws.MergedRanges.Count}");
        Console.WriteLine($"画像数: {ws.Pictures.Count}");
    }
}
EOF

# スクリプトを実行
dotnet script temp/analyze_excel_structure.csx -- "docs/excel/ファイル名.xlsx"
```

### 2.2 全データ読み取り

構造を把握したら、全セルのデータを読み取ります。

#### PowerShell版

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook(filePath);

foreach (var ws in workbook.Worksheets)
{
    Console.WriteLine($"--- シート: {ws.Name} ---");

    // マージセル情報
    foreach (var m in ws.MergedRanges)
        Console.WriteLine($"[マージ] {m.RangeAddress} = \"{m.FirstCell().Value}\"");

    // 全行データ
    foreach (var row in ws.RowsUsed())
    {
        var cells = new List<string>();
        foreach (var cell in row.CellsUsed())
        {
            var val = cell.Value.ToString();
            if (!string.IsNullOrWhiteSpace(val))
                cells.Add($"{cell.Address}={val}");
        }
        if (cells.Count > 0)
            Console.WriteLine(string.Join(" | ", cells));
    }
}
'@ | dotnet run - -- "<ファイルパス>"
```

#### Zsh/Bash版

```bash
# 一時スクリプトファイルを作成
cat > temp/read_excel_all.csx << 'EOF'
#!/usr/bin/env dotnet-script
#r "nuget: ClosedXML, 0.105.0"

using ClosedXML.Excel;
using System.IO;

var filePath = Args[0];
using (var workbook = new XLWorkbook(filePath))
{
    foreach (var ws in workbook.Worksheets)
    {
        Console.WriteLine($"\n=== シート: {ws.Name} ===");

        // マージセル情報
        Console.WriteLine("\n[マージセル情報]");
        foreach (var m in ws.MergedRanges)
        {
            var val = m.FirstCell().Value.ToString();
            if (!string.IsNullOrWhiteSpace(val))
                Console.WriteLine($"  {m.RangeAddress} = \"{val}\"");
        }

        // 全行データ
        Console.WriteLine("\n[セルデータ]");
        foreach (var row in ws.RowsUsed())
        {
            var cells = new List<string>();
            foreach (var cell in row.CellsUsed())
            {
                var val = cell.Value.ToString();
                if (!string.IsNullOrWhiteSpace(val))
                    cells.Add($"{cell.Address}={val}");
            }
            if (cells.Count > 0)
                Console.WriteLine(string.Join(" | ", cells));
        }
    }
}
EOF

# スクリプトを実行（結果をファイルに保存）
dotnet script temp/read_excel_all.csx -- "docs/excel/ファイル名.xlsx" > temp/excel_content.txt
```

### 2.3 画像の抽出

Excel に埋め込まれた画像（スクリーンショットやレイアウト図）を抽出します。

#### PowerShell版

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
var outputDir = args.Length > 1 ? args[1] : "extracted_images";
Directory.CreateDirectory(outputDir);

using var workbook = new XLWorkbook(filePath);

foreach (var ws in workbook.Worksheets)
{
    int i = 0;
    foreach (var pic in ws.Pictures)
    {
        i++;
        var ext = pic.Format switch
        {
            ClosedXML.Excel.Drawings.XLPictureFormat.Png => "png",
            ClosedXML.Excel.Drawings.XLPictureFormat.Jpeg => "jpg",
            ClosedXML.Excel.Drawings.XLPictureFormat.Bmp => "bmp",
            _ => "png"
        };
        var fileName = $"{ws.Name}_{i}.{ext}";
        var path = Path.Combine(outputDir, fileName);
        using var stream = File.Create(path);
        pic.ImageStream.CopyTo(stream);
        Console.WriteLine($"保存: {path} ({pic.OriginalWidth}x{pic.OriginalHeight})");
    }
}
'@ | dotnet run - -- "<ファイルパス>" "<出力先ディレクトリ>"
```

#### Zsh/Bash版

```bash
# 一時スクリプトファイルを作成
cat > temp/extract_excel_images.csx << 'EOF'
#!/usr/bin/env dotnet-script
#r "nuget: ClosedXML, 0.105.0"

using ClosedXML.Excel;
using System.IO;

var filePath = Args[0];
var outputDir = Args.Count > 1 ? Args[1] : "temp/extracted_images";
Directory.CreateDirectory(outputDir);

using (var workbook = new XLWorkbook(filePath))
{
    int totalImages = 0;
    foreach (var ws in workbook.Worksheets)
    {
        int i = 0;
        foreach (var pic in ws.Pictures)
        {
            i++;
            totalImages++;
            var ext = pic.Format switch
            {
                ClosedXML.Excel.Drawings.XLPictureFormat.Png => "png",
                ClosedXML.Excel.Drawings.XLPictureFormat.Jpeg => "jpg",
                ClosedXML.Excel.Drawings.XLPictureFormat.Bmp => "bmp",
                _ => "png"
            };
            var fileName = $"{ws.Name}_{i}.{ext}";
            var path = Path.Combine(outputDir, fileName);
            using (var stream = File.Create(path))
            {
                pic.ImageStream.Position = 0;
                pic.ImageStream.CopyTo(stream);
            }
            Console.WriteLine($"保存: {path} ({pic.Width}x{pic.Height})");
        }
    }
    Console.WriteLine($"\n合計 {totalImages} 枚の画像を抽出しました。");
}
EOF

# スクリプトを実行
dotnet script temp/extract_excel_images.csx -- "docs/excel/ファイル名.xlsx" "temp/extracted_images"
```

### 2.4 セルスタイル情報の取得

罫線やフォントの情報が設計上重要な場合に使用します。

#### PowerShell版

```powershell
@'
#:package ClosedXML@0.105.0

using ClosedXML.Excel;

var filePath = args[0];
using var workbook = new XLWorkbook(filePath);
var ws = workbook.Worksheet(1);

foreach (var row in ws.RowsUsed())
{
    foreach (var cell in row.CellsUsed())
    {
        var style = cell.Style;
        var bold = style.Font.Bold ? " [太字]" : "";
        var bg = style.Fill.BackgroundColor.HasValue
            ? $" [背景:{style.Fill.BackgroundColor.Color}]" : "";
        var val = cell.Value.ToString();
        if (!string.IsNullOrWhiteSpace(val))
            Console.WriteLine($"{cell.Address}: {val}{bold}{bg}");
    }
}
'@ | dotnet run - -- "<ファイルパス>"
```

#### Zsh/Bash版

```bash
# 一時スクリプトファイルを作成
cat > temp/read_excel_styles.csx << 'EOF'
#!/usr/bin/env dotnet-script
#r "nuget: ClosedXML, 0.105.0"

using ClosedXML.Excel;
using System.IO;

var filePath = Args[0];
using (var workbook = new XLWorkbook(filePath))
{
    var ws = workbook.Worksheet(1);

    foreach (var row in ws.RowsUsed())
    {
        foreach (var cell in row.CellsUsed())
        {
            var style = cell.Style;
            var bold = style.Font.Bold ? " [太字]" : "";
            var bg = style.Fill.BackgroundColor.HasValue
                ? $" [背景:{style.Fill.BackgroundColor.Color}]" : "";
            var val = cell.Value.ToString();
            if (!string.IsNullOrWhiteSpace(val))
                Console.WriteLine($"{cell.Address}: {val}{bold}{bg}");
        }
    }
}
EOF

# スクリプトを実行
dotnet script temp/read_excel_styles.csx -- "docs/excel/ファイル名.xlsx"
```

---

## 3. PowerPoint ファイル解析（DocumentFormat.OpenXml）

### 3.1 スライド構造・テキスト抽出

```powershell
@'
#:package DocumentFormat.OpenXml@3.3.0

using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Presentation;
using A = DocumentFormat.OpenXml.Drawing;

var filePath = args[0];
using var ppt = PresentationDocument.Open(filePath, false);
var presentationPart = ppt.PresentationPart!;
var slideIds = presentationPart.Presentation.SlideIdList!
    .ChildElements.OfType<SlideId>();

Console.WriteLine($"=== {System.IO.Path.GetFileName(filePath)} ===");
Console.WriteLine($"スライド数: {slideIds.Count()}");

int slideNum = 0;
foreach (var slideId in slideIds)
{
    slideNum++;
    var slidePart = (SlidePart)presentationPart.GetPartById(slideId.RelationshipId!);
    Console.WriteLine($"\n--- スライド {slideNum} ---");

    // テキスト抽出
    var texts = slidePart.Slide.Descendants<A.Text>()
        .Select(t => t.Text)
        .Where(t => !string.IsNullOrWhiteSpace(t));
    Console.WriteLine("テキスト:");
    foreach (var text in texts)
        Console.WriteLine($"  {text}");

    // 図形
    var shapes = slidePart.Slide.Descendants<Shape>().Count();
    Console.WriteLine($"図形数: {shapes}");

    // 画像
    var imageParts = slidePart.ImageParts.Count();
    Console.WriteLine($"画像数: {imageParts}");
}
'@ | dotnet run - -- "<ファイルパス>"
```

### 3.2 図形の詳細情報

各図形の名前・位置・テキストを取得し、画面レイアウトを把握します。

```powershell
@'
#:package DocumentFormat.OpenXml@3.3.0

using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Presentation;
using A = DocumentFormat.OpenXml.Drawing;

var filePath = args[0];
using var ppt = PresentationDocument.Open(filePath, false);
var presentationPart = ppt.PresentationPart!;
var slideIds = presentationPart.Presentation.SlideIdList!
    .ChildElements.OfType<SlideId>();

int slideNum = 0;
foreach (var slideId in slideIds)
{
    slideNum++;
    var slidePart = (SlidePart)presentationPart.GetPartById(slideId.RelationshipId!);
    Console.WriteLine($"--- スライド {slideNum} ---");

    var shapeTree = slidePart.Slide.CommonSlideData?.ShapeTree;
    if (shapeTree == null) continue;

    foreach (var shape in shapeTree.Elements<Shape>())
    {
        var name = shape.NonVisualShapeProperties?.NonVisualDrawingProperties?.Name ?? "(無名)";
        var text = string.Join("", shape.Descendants<A.Text>().Select(t => t.Text));

        // 位置・サイズ (EMU 単位: 1 cm = 360000 EMU)
        var xfrm = shape.ShapeProperties?.Transform2D;
        var pos = xfrm != null
            ? $"({xfrm.Offset?.X ?? 0}, {xfrm.Offset?.Y ?? 0})"
            : "(不明)";
        var size = xfrm != null
            ? $"{xfrm.Extents?.Cx ?? 0}x{xfrm.Extents?.Cy ?? 0}"
            : "(不明)";

        Console.WriteLine($"  図形: {name} | テキスト: {text} | 位置: {pos} | サイズ: {size}");
    }
}
'@ | dotnet run - -- "<ファイルパス>"
```

#### Zsh/Bash版

```bash
# 一時スクリプトファイルを作成
cat > temp/read_ppt_shapes.csx << 'EOF'
#!/usr/bin/env dotnet-script
#r "nuget: DocumentFormat.OpenXml, 3.3.0"

using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Presentation;
using System.IO;
using System.Linq;
using A = DocumentFormat.OpenXml.Drawing;

var filePath = Args[0];
using (var ppt = PresentationDocument.Open(filePath, false))
{
    var presentationPart = ppt.PresentationPart;
    var slideIds = presentationPart.Presentation.SlideIdList
        .ChildElements.OfType<SlideId>();

    int slideNum = 0;
    foreach (var slideId in slideIds)
    {
        slideNum++;
        var slidePart = (SlidePart)presentationPart.GetPartById(slideId.RelationshipId);
        Console.WriteLine($"--- スライド {slideNum} ---");

        var shapeTree = slidePart.Slide.CommonSlideData?.ShapeTree;
        if (shapeTree == null) continue;

        foreach (var shape in shapeTree.Elements<Shape>())
        {
            var name = shape.NonVisualShapeProperties?.NonVisualDrawingProperties?.Name ?? "(無名)";
            var text = string.Join("", shape.Descendants<A.Text>().Select(t => t.Text));

            // 位置・サイズ (EMU 単位: 1 cm = 360000 EMU)
            var xfrm = shape.ShapeProperties?.Transform2D;
            var pos = xfrm != null
                ? $"({xfrm.Offset?.X ?? 0}, {xfrm.Offset?.Y ?? 0})"
                : "(不明)";
            var size = xfrm != null
                ? $"{xfrm.Extents?.Cx ?? 0}x{xfrm.Extents?.Cy ?? 0}"
                : "(不明)";

            Console.WriteLine($"  図形: {name} | テキスト: {text} | 位置: {pos} | サイズ: {size}");
        }
    }
}
EOF

# スクリプトを実行
dotnet script temp/read_ppt_shapes.csx -- "docs/ppt/ファイル名.pptx"
```

### 3.3 埋め込み画像の抽出

#### PowerShell版

```powershell
@'
#:package DocumentFormat.OpenXml@3.3.0

using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Presentation;

var filePath = args[0];
var outputDir = args.Length > 1 ? args[1] : "extracted_images";
Directory.CreateDirectory(outputDir);

using var ppt = PresentationDocument.Open(filePath, false);
var presentationPart = ppt.PresentationPart!;
var slideIds = presentationPart.Presentation.SlideIdList!
    .ChildElements.OfType<SlideId>();

int slideNum = 0;
foreach (var slideId in slideIds)
{
    slideNum++;
    var slidePart = (SlidePart)presentationPart.GetPartById(slideId.RelationshipId!);

    int imgNum = 0;
    foreach (var imagePart in slidePart.ImageParts)
    {
        imgNum++;
        var ext = imagePart.ContentType switch
        {
            "image/png" => "png",
            "image/jpeg" => "jpg",
            "image/bmp" => "bmp",
            "image/gif" => "gif",
            _ => "png"
        };
        var fileName = $"slide{slideNum}_img{imgNum}.{ext}";
        var path = System.IO.Path.Combine(outputDir, fileName);
        using var stream = imagePart.GetStream();
        using var fileStream = File.Create(path);
        stream.CopyTo(fileStream);
        Console.WriteLine($"保存: {path}");
    }
}
'@ | dotnet run - -- "<ファイルパス>" "<出力先ディレクトリ>"
```

#### Zsh/Bash版

```bash
# 一時スクリプトファイルを作成
cat > temp/extract_ppt_images.csx << 'EOF'
#!/usr/bin/env dotnet-script
#r "nuget: DocumentFormat.OpenXml, 3.3.0"

using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Presentation;
using System.IO;
using System.Linq;

var filePath = Args[0];
var outputDir = Args.Count > 1 ? Args[1] : "temp/extracted_images";
Directory.CreateDirectory(outputDir);

using (var ppt = PresentationDocument.Open(filePath, false))
{
    var presentationPart = ppt.PresentationPart;
    var slideIds = presentationPart.Presentation.SlideIdList
        .ChildElements.OfType<SlideId>();

    int slideNum = 0;
    foreach (var slideId in slideIds)
    {
        slideNum++;
        var slidePart = (SlidePart)presentationPart.GetPartById(slideId.RelationshipId);

        int imgNum = 0;
        foreach (var imagePart in slidePart.ImageParts)
        {
            imgNum++;
            var ext = imagePart.ContentType switch
            {
                "image/png" => "png",
                "image/jpeg" => "jpg",
                "image/bmp" => "bmp",
                "image/gif" => "gif",
                _ => "png"
            };
            var fileName = $"slide{slideNum}_img{imgNum}.{ext}";
            var path = Path.Combine(outputDir, fileName);
            using (var stream = imagePart.GetStream())
            using (var fileStream = File.Create(path))
            {
                stream.CopyTo(fileStream);
            }
            Console.WriteLine($"保存: {path}");
        }
    }
}
EOF

# スクリプトを実行
dotnet script temp/extract_ppt_images.csx -- "docs/ppt/ファイル名.pptx" "temp/extracted_images"
```

---

## 4. 注意事項

### 前提条件

**Windows (PowerShell)**
- .NET SDK 6.0以上がインストールされていること
- PowerShell 5.1以上

**macOS/Linux (Zsh/Bash)**
- .NET SDK 6.0以上がインストールされていること
- `dotnet-script` がインストールされていること
  ```bash
  dotnet tool install -g dotnet-script
  ```

### パッケージバージョン

パッケージ参照ではバージョンを明示してください。

**PowerShell版**
```csharp
#:package ClosedXML@0.105.0           // Excel
#:package DocumentFormat.OpenXml@3.3.0 // PowerPoint / Word
```

**Zsh/Bash版**
```csharp
#r "nuget: ClosedXML, 0.105.0"           // Excel
#r "nuget: DocumentFormat.OpenXml, 3.3.0" // PowerPoint / Word
```

### C#スクリプト構文の違い

| 項目 | PowerShell版 | Zsh/Bash版 |
|------|-------------|-----------|
| パッケージ参照 | `#:package Package@1.0.0` | `#r "nuget: Package, 1.0.0"` |
| 引数アクセス | `args[0]` | `Args[0]` (大文字) |
| 引数数の確認 | `args.Length` | `Args.Count` |
| using構文 | `using var x = ...;` (OK) | `using (var x = ...) { }` (推奨) |
| null許容演算子 | `ppt.PresentationPart!` (OK) | 明示的なnullチェック推奨 |

### macOS/Zsh固有の注意事項

1. **一時ファイルの配置**
   - `temp/` ディレクトリ配下にスクリプトファイルを作成
   - 実行前に `mkdir -p temp` でディレクトリを作成

2. **ヒアドキュメント構文**
   - `cat > ファイル名 << 'EOF'` を使用（シングルクォートで変数展開を防ぐ）
   - 最後に `EOF` で閉じる

3. **パイプ実行の制限**
   - `dotnet script -` はサポートされていない
   - 必ず一時ファイルを経由して実行

4. **エラーメッセージ**
   - `Couldn't find file '-'` → パイプではなくファイルを指定
   - `'args' という名前は存在しません` → `Args` (大文字) を使用
   - `'Length' の定義が含まれておらず` → `Args.Count` を使用

### DocumentFormat.OpenXml の名前空間の衝突

`DocumentFormat.OpenXml.Drawing` には `Path` や `Text` など、`System.IO` や `System` と衝突する型があります。エイリアスを使ってください。

```csharp
using A = DocumentFormat.OpenXml.Drawing;  // Drawing 名前空間はエイリアスで参照
// Path → System.IO.Path を使う場合はフルパスで指定
```

### 画像を含むドキュメントの扱い

画像はテキストとして読み取れないため、抽出してファイルに保存してから内容を確認してください。
画面レイアウト図が含まれている場合は、図形のテキストや位置情報からレイアウトを推測します。

<!--
参考元:https://github.com/runceel/github-copilot-excel-document-to-code/blob/main/.github/skills/office-document-analyzer/SKILL.md
-->