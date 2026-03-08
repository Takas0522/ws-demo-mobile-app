---
marp: true
theme: default
paginate: true
header: "モバイルアプリワークショップ"
footer: "© 2026 mobile-app-system"
style: |
  section {
    font-size: 24px;
  }
  section.title {
    text-align: center;
    font-size: 32px;
  }
  section.title h1 {
    font-size: 56px;
  }
  code {
    font-size: 20px;
  }
  pre {
    font-size: 18px;
  }
---

<!-- _class: title -->
<!-- _paginate: false -->
<!-- _header: "" -->

# モバイルアプリワークショップ

**GitHub Copilot を活用した Android / iOS 開発**

---

## シナリオ

- 先行して開発を実施していた商品管理チームの機能追加に追従して開発を実施します。
- 他部署で作成された仕様書をベースに自分たちのシステムの開発計画を作成し開発を行う
  - 別部隊が実施した Web API の開発はすでに完了している状態。
- AI を用いた開発の初回として、開発のための Copilot のカスタマイズを実施した後に開発を進める

---

## ワークショップの流れ

| # | ステップ | 内容 |
|---|---------|------|
| 0 | 環境構築 | 必要ツールのセットアップ |
| 1 | 準備運動 | コードからのドキュメント生成 |
| 2 | AIのカスタマイズ | Custom Instructions / Custom Agents / Agent Skills |
| 3 | 設計書の読み込みと理解 | Excel 仕様書 → Markdown 変換 |
| 4 | 開発の準備と実施 | 開発プラン作成 → 実装 |
| 5 | デバッグと仕上げ | Android Studio / Xcode でのデバッグ |
| 6 | 新機能の計画と構築の実施 | 企画から開発までを一気通貫で実施 |

---

## 0. 環境構築

### 必要なツール

- **Android Studio**（OpenJDK 17 を含む）
- **Xcode**（Swift 6）
- **VS Code**
  - [Markdown Preview Mermaid Support](https://marketplace.visualstudio.com/items?itemName=bierner.markdown-mermaid)
  - [Marp](https://marketplace.visualstudio.com/items?itemName=marp-team.marp-vscode)
- **GitHub Copilot CLI**
  - Windows: `winget install GitHub.Copilot`
  - macOS: `brew install copilot-cli`
- **.NET 10**（任意・Office ファイルの解析に使用）

---

## 1. 準備運動

### 1-1. 仕様の確認

まずはこのアプリケーションについて質問します。**Ask モード**で質問を行いましょう

``` markdown
このアプリケーションで提供されているビジネス要件について教えてください。
- このアプリケーションの目的
- このアプリケーションのユースケース
- このアプリケーションで提供されている機能
```

---

## 1. 準備運動

### 1-2. アプリケーションを動作させる

``` markdown
モバイルアプリケーションで利用する Web API を動作させるためのコマンド一式とユーザーIDとパスワードを提示してください。
```

---

## 1. 準備運動

### 1-3. ドキュメントを生成する

現在ドキュメントが存在しないため、**Agent モード**でコードからドキュメントの生成を行います。

``` markdown
<!-- sample -->
モバイルアプリケーションとそのアプリが利用する Web API のコードが格納されています。
このコードを参考にして、システム要件・ビジネス要件について記載されたドキュメントを以下の要件に沿って作成してください。
1. アプリケーションに関わる ISO の要件に合わせた構成とする
2. 図示が必要な場合は Mermaid を使用する
3. セクションごとにファイルを分けて作成し、読む順番を考慮してファイル名に項番を付与する
4. `docs/mobile` ディレクトリに格納する
```

待っている間に GitHub Copilot の基礎について

---

## 2. AIのカスタマイズ

3つの構成を行います：

| 機能 | 概要 | 配置場所 |
|-----|------|---------|
| **Custom Instructions** | 常に守る前提条件・振る舞い | `.github/instructions/*.instructions.md` |
| **Custom Agents** | 役割・権限を持つ自律エージェント | `.github/agents/*.agent.md` |
| **Agent Skills** | 動的に読み込む手順・知識 | `.github/skills/<name>/SKILL.md` |

---

## 2-1. Custom Instructions

常に守ってほしい前提条件や振る舞いを事前に設定できる機能。

- Copilot に問い合わせた時に **最初に読まれるプロンプト** となる
- `.github/instructions/*.instructions.md` や `.github/copilot-instructions.md` に記載
- **開発規約の遵守、品質の担保** などに効いてくる

---

## 2-1. Custom Instructions — 作成 ✅

```markdown
<!-- sample -->
現在のモバイルアプリのソースコードをベースに、Android・iOS 両プラットフォームの
開発規約となる Custom Instructions を `.github/instructions/mobile-app.instructions.md`
として作成してください。

規約に含める内容：
1. 命名規則（Android: PascalCase / camelCase、iOS: PascalCase / camelCase）
2. 設計原則（関心の分離・単方向データフロー・単一信頼源）
3. レイヤー構造（UI / Domain / Data の 3 層）
4. コードスタイル（インデント・エラー処理・非同期処理）
5. 開発工程（仕様の理解→開発プランの作成→開発→テストの作成→テストの実施）

以下の公式ガイドラインを参考にすること：
- Android: Androidアーキテクチャガイド・Now in Android
- iOS: Human Interface Guidelines・SwiftUI Documentation・Swift Concurrency

インストラクションファイルの構造は以下のドキュメントに従うこと：
- #fetch https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions
```

---

## 2-1. Custom Instructions — レビュー ✅

```markdown
作成された `.github/instructions/mobile-app.instructions.md` を批判的な立場からレビューし、
必要があれば修正します。確認する観点：
1. Androidアーキテクチャガイドの原則（関心の分離・単方向データフロー）が反映されているか
2. Now in Android の推奨パターン（ViewModel / Repository）が含まれているか
3. Human Interface Guidelines の要件（アクセシビリティ・Dynamic Type）が含まれているか
4. SwiftUI Documentation のプロパティラッパー使い分けが説明されているか
5. Swift Concurrency のパターン（async/await・Task・.task モディファイア）が含まれているか

さらに以下のプラクティスに沿っているか確認する：
- #fetch https://github.com/github/awesome-copilot/blob/main/instructions/instructions.instructions.md
```

---

## 2-1. Custom Instructions — 効果の確認

```markdown
<!-- sample -->
現在のモバイルアプリで使用する下記のディレクトリ配下のコードをレビューします。
レビューは批判的な立場から行います。実施するのはレビューであり実装のコードを変更してはいけません。
- `src/ios-app/`
- `src/android-app/`
```

---

## 2-2. Custom Agents

GitHub Copilot に **役割や権限を与えて自律的に動いてもらう** よう設定できる機能。

- 人間が指定して利用する
- `.github/agents/*.agent.md` に記載
- 複数のサブエージェントを組み合わせた **オーケストレーション構成** も可能

---

## 2-2. Custom Agents — 作成 ✅

```markdown
<!-- sample -->
ドキュメントをレビューするためのカスタムエージェントを作成します。
カスタムエージェントは以下のように動作します：
- 文章の誤字脱字がないかを判定する
- 他の文章を参照し矛盾が発生していないかを確認する
- その他、文章として破綻している部分がないかを確認する
- レビューの結果、不備を見つけた場合はユーザーに報告する（**変更は実施してはいけません**）

以下の要件でエージェントを構成すること：
- VS Code および Coding Agent で利用できる形式
- `name`・`description`・`tools`・`model` を適切に設定する
- 日本語で作成する

構成方法は以下のドキュメントを**正確に**参考にすること：
- #fetch https://docs.github.com/en/copilot/reference/custom-agents-configuration
- #fetch https://code.visualstudio.com/docs/copilot/customization/custom-agents
```

---

## 2-2. Custom Agents — レビュー ✅

``` markdown
作成されたカスタムエージェントを批判的な立場からレビューし、必要があれば修正します。
以下のプラクティスに沿っているか確認すること：
- #fetch https://github.com/github/awesome-copilot/blob/main/instructions/agents.instructions.md
```

---

## 2-2. Custom Agents — 効果の確認

``` markdown
`workshop-documents/sample-document.md` をレビューしてください
```

---

## 2-3. Agent Skills

GitHub Copilot に特定タスクの手順・知識・スクリプトを **必要なときだけ動的に読み込ませる** 機能。

- 専門知識や手続きのパッケージング化と動的ロード
- `.github/skills/<skill-name>/SKILL.md` に記載
- scripts や template を加えることも可能

---

## 2-3. Agent Skills — 作成 ✅

```markdown
<!-- sample -->
モバイルアプリのユニットテストコード構築のための Agent Skill を作成します。
ユニットテストは以下の要件にそって実施されます：
- Android: JUnit 4、iOS: XCTest を使用
- ワークフロー：
  1. テストシナリオの作成
  2. ブラックボックステスト観点でのレビュー（最大 2 回。問題があれば修正→再レビュー）
  3. テストの実装
  4. テストの実施（問題が発生した場合は報告のみ）
  5. テスト結果レポートの作成（出力先: `temp/テストレポート_{YYYYMMDD_HHmmss}.md`）
- すでにテストシナリオが存在する場合は、手順 2 のレビューから開始する

Skill の構造は以下のドキュメントを**正確に**参考にすること：
- #fetch https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions
- #fetch https://code.visualstudio.com/docs/copilot/customization/agent-skills
```

---

## 2-3. Agent Skills — レビュー ✅

``` markdown
作成された Agent Skill を批判的な立場からレビューし、必要があれば修正します。
以下のプラクティスに沿っているか確認すること：
- #fetch https://github.com/github/awesome-copilot/blob/main/instructions/agent-skills.instructions.md
```

---

## 2-3. Agent Skills — 効果の確認

``` markdown
Skillを使用しAndroid アプリの `testProductCreation` と iOS アプリの `testProductDecoding` を対象にUnitTestを作成してください。
```

※Free プランの方は実施を見送ることを推奨します

---

## 3. 設計書の読み込みと理解

**VS Code** を利用して、別部隊で作成された Excel などの設計書を読み込み、Markdown の仕様書を作成します。

### ポイント

- どのような変更が行われるのかを把握する
- 自分たちのコードの変更される箇所はどこなのかを特定する

``` markdown
<!-- dotnet10とGitHub Copilot CLIがつかえるのであればこちら -->
Excel で構築された設計書を参照し、モバイルアプリのレイアウト変更とモバイル用のBFFの変更を実施します。
BFFの接続元のWeb APIは別部隊で改修済みです。
`docs/excel/価格履歴機能_設計書.xlsx` が対象となるファイルです。
`docs/specs/{機能名}` に解析された結果として必要な改修内容を記載してください。
改修内容の作成のみで実際の実装は行いません。
```
※モデルによっては正常に動作しない可能性があります。その場合は `docs/specs` に格納済みのファイルを参照してください。  
※内部で `dotnet` コマンドを使用しているため、.NET（≠ .NET Framework）がインストールされていない場合は動作しません。

---

## 4. 開発の準備と開発の実施

**GitHub Copilot CLI** を利用して、読み込まれた設計書をベースに：

1. **開発プランを作成**（Plan モードで実施）
2. **開発を実施**（Autopilot モードで実施）

``` markdown
<!--前段階でCopilotをどうさせなかった場合は`workshop-documents/価格履歴機能_MobileApp.md`を利用します-->
`docs/specs/{機能名}` に記載されている機能を実装するプランを作成してください。
不明点やあいまいな点があれば質問してください。
```

※Free プランの方、または CLI を使用できない場合は VS Code の Copilot から Plan モードと Agent モードで実施してください。

---

## 5. デバッグと仕上げ

**Android Studio** と **Xcode** を使用して動くかどうか確認しながら開発の仕上げを行います。

- ビルド & 実行
- デバッグによる問題の特定と修正
- 最終確認

※リクエストを消費しがちな作業となるため、Free プランの方はやらない方が良いかもしれません

---

## 6. 新機能の計画と構築の実施

これまでの総仕上げ。開発プロセスのすべてを AI を使って実施します。  
それぞれの工程で高い精度のレビューを実施してもらうために色々と条件を付けていきます。  
サブエージェントを用いたオーケストレーションを構築します。下記の作業を一気通貫で実施します。

- 企画
- 仕様の作成（Excel 仕様書も含むが最新モデルでないと失敗する可能性大）
- ビジネス仕様とシステム仕様の構築
- 実装
- ユニットテスト
- ユーザーテストのシナリオ構築

---

## 6-1. エージェント群の構築

```markdown
<!-- sample -->
システム開発を実施するサブエージェントを含むエージェント群を構築します。
既存のカスタムエージェントを利用できる場合は利用します。
作成する機能はエージェント実行時に指定された内容に準じます。
オーケストレーターは進捗管理ファイル（`docs/specs/{開発機能名}/progress.json`）で状態を管理し、
予期せぬ中断後も再開できるようにします。

実行順序（各工程はサブエージェントで処理）：
1. ビジネス仕様・システム仕様の構築
   - Mermaid で図示・`docs/specs/{開発機能名}/` に格納
   - **仕様内容をユーザーに確認してから後続の作業を開始すること**
2. 開発計画・タスクの作成（動作確認可能な粒度で分割・`docs/specs/{開発機能名}/開発タスク/` に格納）
3. 実装（タスクごとに実施。完了していないタスクがある限り繰り返す）
4. ユニットテストの構築
5. テスト・実装の修正
6. ユーザーテストシナリオの構築

各工程でレビューを実施し結果を `docs/review/{工程}-{timestamp}.md` に出力します。
不合格の場合は修正→再レビューを最大 2 回実施します。
レビュー観点：
- 仕様: システム開発設計に関わる ISO の様式に準拠。ビジネス仕様には利用シナリオと業務要件を含む
- 実装: `.github/instructions/mobile-app.instructions.md` の規約に準拠
- テスト設計: ISTQB で提唱されている手法を用いる
- ユーザーテスト: [経産省システム管理基準](https://www.meti.go.jp/policy/netsecurity/sys-kansa/sys-kanri-2023.pdf)の「ユーザ受入テスト」を参考にする

構成については以下を参考にすること：
- #fetch https://code.visualstudio.com/docs/copilot/customization/custom-agents
- #fetch https://github.com/github/awesome-copilot/blob/main/instructions/agents.instructions.md
```

---

## 6-2. 企画

エージェントが構築されている間に  
**Ask モード**を使ってお題の企画を Copilot と壁打ちしてブラッシュアップしていきます。

- どんなライブラリを使うべきか
- どんな機能があった方がよいか

ある程度まとまったら **Agent モード**に変更して要約を `docs/specs/新機能追加` に Markdown としてまとめてもらいます。

※Free プランの方は 1〜2 ラリー程度にとどめることを推奨します。

---

## 6-3. 実装

**GitHub Copilot CLI** で一気通貫で実装していきます。  
Agent モードで、作成されたオーケストレーターエージェントに `docs/specs/新機能追加` にある企画書の実装を依頼します。

---

## 6-4. 動作確認

**Android Studio** と **Xcode** でデバッグしながら動かない箇所を修正します。

---

<!-- _class: title -->
<!-- _paginate: false -->

# Thank You!

モバイルアプリワークショップ
