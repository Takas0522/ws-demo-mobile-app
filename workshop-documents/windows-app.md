---
marp: true
theme: default
paginate: true
header: "Windows アプリワークショップ"
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

# Windowsアプリワークショップ

**GitHub Copilot を活用した C++ 開発**

---

## シナリオ


- 先行して開発を実施していた商品管理チームの機能追加に追従して開発を実施します。
- 他部署で作成された仕様書をベースに自分たちのシステムの開発計画を作成し開発を行う
- AIを用いた開発の初回ということで、開発のためのCopilotのカスタマイズを実施した後開発を進める

---

## ワークショップの流れ

| # | ステップ | 内容 |
|---|---------|------|
| 0 | 環境構築 | 必要ツールのセットアップ |
| 1 | 準備運動 | Codeからのドキュメント生成 |
| 2 | AIのカスタマイズ | Custom Instructions / Agent Skills / Custom Agents |
| 3 | 設計書の読み込みと理解 | Excel 仕様書 → Markdown 変換 |
| 4 | 開発の準備と実施 | 開発プラン作成 → 実装 |
| 5 | デバッグと仕上げ | Visual Studio でのデバッグ |
| 6 | 新機能の計画と構築の実施 | 企画から開発までを一気通貫で実施 |

---

## 0. 環境構築、準備運動

### 必要なツール

- **Visual Studio 2022 or 2026**（C++ が開発可能であること）
- **VS Code**（任意）
  - [Markdown Preview Mermaid Support](https://marketplace.visualstudio.com/items?itemName=bierner.markdown-mermaid)
  - [Marp](https://marketplace.visualstudio.com/items?itemName=marp-team.marp-vscode)
- **GitHub Copilot CLI**
  - Windows: `winget install GitHub.Copilot`
  - MacOS: `brew install copilot-cli`
- **.NET10**（任意）
  - Officeファイルの解析に使用

---

## 1. 準備運動(2)

現在ドキュメントが存在しないため、コードからドキュメントの生成を行います。

``` markdown
<!-- sample -->
`windows-app`ディレクトリ内にWindowsアプリケーションとそのアプリが利用するWebAPIのC++コードが格納されています
このコードを参考にして、システム要件、ビジネス要件について記載されたドキュメントを下記の要綱に沿って作成してください。
- ドキュメントはアプリケーションに関わるISOの要件に合わせたものが作成されます
- 図示が必要な場合はMermaidを使用します
- セクションごとにファイルを分けて作成し、読む順番などを考慮してファイル名に項番を付与してください。
- `docs/Windowsアプリ`ディレクトリ内に格納されます
```

---

## 2. AIのカスタマイズ

3つの構成を行います：

| 機能 | 概要 | 配置場所 |
|-----|------|---------|
| **Custom Instructions** | 常に守る前提条件・振る舞い | `/.github/copilot-instructions.md` |
| **Agent Skills** | 動的に読み込む手順・知識 | `.github/skills/<name>/SKILL.md` |
| **Custom Agents** | 役割・権限を持つ自律エージェント | `.github/agents/*.agent.md` |

---

## 2-1. Custom Instructions

常に守ってほしい前提条件や振る舞いを事前に設定できる機能。

- Copilot に問い合わせた時に **最初に読まれるプロンプト** となる
- `/.github/instructions/*.instructions.md` や `/.github/copilot-instructions.md` に記載
- **開発規約の遵守、品質の担保** などに効いてくる

---

## 2-1. Custom Instructions — サンプルプロンプト

```markdown
<!-- sample -->
現在の`windows-app`のソースコードをベースに
C++コード用の開発規約となるCustom Instructionsを作成してください。
守らせたい規約は下記となります
- 命名規則
- 設計原則
- コードスタイル
- レイヤー構造
- 開発工程(仕様の理解->開発プランの作成->開発->テストの作成->テストの実施)

下記のドキュメントや記事を**正確に**参考し日本語で作成してください。
- #fetch https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions
- #fetch https://github.blog/ai-and-ml/github-copilot/5-tips-for-writing-better-custom-instructions-for-copilot/
- #fetch https://github.blog/ai-and-ml/unlocking-the-full-power-of-copilot-code-review-master-your-instructions-files/
```

---

## 2-1. Custom Instructions — 参考リンク

- [Repository Instructions](https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions)
- [5 Tips for Writing Better Custom Instructions](https://github.blog/ai-and-ml/github-copilot/5-tips-for-writing-better-custom-instructions-for-copilot/)
- [Master Your Instructions Files](https://github.blog/ai-and-ml/unlocking-the-full-power-of-copilot-code-review-master-your-instructions-files/)

> [instructions.instructions.md](https://github.com/github/awesome-copilot/blob/main/instructions/instructions.instructions.md) を利用して作成されたインストラクションファイルのレビューを行ってみる。

``` markdown
#fetch https://github.com/github/awesome-copilot/blob/main/instructions/instructions.instructions.md を参考に作成されたファイルを改善してください。
```

---

## 2-2. Agent Skills

GitHub Copilot に特定タスクの手順・知識・スクリプトを **必要なときだけ動的に読み込ませる** 機能。

- 専門知識や手続きのパッケージング化と動的ロード
- `.github/skills/<skill-name>/SKILL.md` に記載
- scripts や template を加えることも可能

---

## 2-2. Agent Skills — サンプルプロンプト

```markdown
<!-- sample -->
C++で構成されたWindowsApplication用のユニットテストコード構築のAgent Skillを作成します。
ユニットテストは下記の要綱にそって実施されます
- `VSTest.Console.exe`とC++用(Microsoft単体テストフレームワーク使おうとしているユニットテストツールにあわせる) を使用してUnitTestを構築します。
- ユニットテストの作成は下記の手順で実施されます
  1. テストシナリオを作成
  2. テストシナリオを批判的な観点からレビュー(一般的なブラックボックステスト的な観点から網羅性をチェック)
    - 問題があれば修正と再レビュー（最大2回）
  3. テストの実装
  4. テストの実施
    - 問題が発生した場合は報告のみを実施する
  5. テスト結果のレポート作成
    - `temp/テストレポート_{timestamp}.md`
- すでにテストシナリオが構築されている場合はテストシナリオを現在の実装をもとにレビューするところから開始します。
- サンプルでテストが構成されているためその実装を参考にしてください

下記のドキュメントや記事を**正確に**参考し日本語で作成してください。
- #fetch https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions
- #fetch https://code.visualstudio.com/docs/copilot/customization/agent-skills
```

> [agent-skills.instructions.md](https://github.com/github/awesome-copilot/blob/main/instructions/agent-skills.instructions.md) を利用して作成されたインストラクションファイルのレビューを行ってみる。

``` markdown
#fetch https://github.com/github/awesome-copilot/blob/main/instructions/agent-skills.instructions.md を参考に作成されたファイルを改善してください。
```

---

## 2-3. Custom Agents

GitHub Copilot に **役割や権限を与えて自律的に動いてもらう** よう設定できる機能。

- 人間が指定して利用する
- `.github/agents/*.agent.md` に記載
- 複数のサブエージェントを組み合わせた **オーケストレーション構成** も可能

---

## 2-3. Custom Agents — サンプルプロンプト

```markdown
<!-- sample -->
C++で構成されたユニットテストを作成するための
VS CodeやCoding Agentで利用できる専門カスタムエージェントを作成します。
カスタムエージェントは下記のように動作します。
- ユニットテスト作成専門（実装の変更は実施しない）
- ファイル・ディレクトリの作成、コマンドの実行が可能であるため適切なツールをメタデータ部分で定義してください
- ファイル・クラス・メソッドを指定可能
- C++のみをターゲット
- 作成されたAgent Skillsを利用

VS CodeやCoding Agentで利用できる形のカスタムエージェントを構成します下記のドキュメントや記事を**正確に**参考し日本語で作成してください。
- #fetch https://docs.github.com/en/copilot/reference/custom-agents-configuration
- #fetch https://code.visualstudio.com/docs/copilot/customization/custom-agents
```

---

## 2-3. Custom Agents — 参考リンク

- [Custom Agents Configuration](https://docs.github.com/en/copilot/reference/custom-agents-configuration)
- [Custom Agents Documentation](https://code.visualstudio.com/docs/copilot/customization/custom-agents)

> [agents.instructions.md](https://github.com/github/awesome-copilot/blob/main/instructions/agents.instructions.md) を利用して作成されたインストラクションファイルのレビューを行ってみる。

``` markdown
#fetch https://github.com/github/awesome-copilot/blob/main/instructions/agents.instructions.md を参考に作成されたファイルを改善してください。
```


---

## 2-4. 効果の確認

1. **GitHub Copilot CLI** を使用してエージェントを利用しユニットテストを作成してもらう
2. **Visual Studio** を使って失敗している箇所の修正をしてもらう

``` markdown
Windowsアプリケーションの`AuthService`の`GetToken`のテストを作成してください。
```
※おそらくテスト実行時にMsBuild.exeのパス関係で失敗すると思うので、そのフェーズでVisualStudioで実行確認するほうが楽
※Free版の人はやらないほうがいいです

---

## 3. 設計書の読み込みと理解

**VS Code** を利用して、別部隊で作成された Excel などの設計書を読み込み、Markdown の仕様書を作成します。

### ポイント

- どのような変更が行われるのかを把握
- 自分たちのコードの変更される箇所はどこなのかを特定

``` markdown
Excelで構築された設計書を参照し、Windowsアプリのレイアウト変更を実施します。
Windowsアプリで使用するWebAPIは別部隊で改修済みです。
`docs/excel/価格履歴機能_設計書.xlsx`が対象となるファイルです。
`docs/specs/{機能名}`に解析された結果必要な改修内容を記載してください。
```
※モデルによってはちゃんと動かない可能性有。なので`docs/specs/cache`に読み込んだ結果を格納しています。
※内部的に`dotnet`コマンドを使用しているため.NET(≠.NET framework)がインストールされていない場合は動作しません
※Free版の人はやらないほうがいいので上記に格納しているファイルを参照してください。

---

## 4. 開発の準備と開発の実施

**GitHub Copilot CLI** を利用して、読み込まれた設計書をベースに：

1. **開発プランを作成**
  - planモードで実施
2. **開発を実施**
  - autopilotモードで実施

``` markdown
`docs/specs/{機能名}` に記載されている機能を実装するための開発プランを構築してください
```
※Free版の人はVS CodeのCopilotからPlanモードではなくAgentモードで実施してください

---

## 5. デバッグと仕上げ

**Visual Studio** を使用して動くかどうか確認しながら開発の仕上げを行います。

- ビルド & 実行
- デバッグによる問題の特定と修正
- 最終確認

※リクエストを消費しがちな作業となるためFree版の人はやらない方が良いかもしれません

---

## 6. 新機能の計画と構築の実施

これまでの総仕上げ。開発プロセスのすべてをAIを使って実施します。  
それぞれの工程で高い精度のレビューを実施してもらうために色々と条件を付けていきます。  
サブエージェントを用いたオーケストレーションを構築します。下記の作業を一気通貫で実施します。

- 企画
- 仕様の作成（Excel仕様書も含むが最新モデルでないと失敗する可能性大）
- ビジネス仕様とシステム仕様の構築
- 実装
- ユニットテスト
- ユーザーテストのシナリオ構築

---
## 6-1. エージェント群の構築

``` markdown
<!-- sample -->
システム開発を実施するサブエージェントを含むエージェント群を構築します。
エージェント群は下記の順番で実施され、それぞれの工程がサブエージェントで実施されます。既存のカスタムエージェントを利用できる場合は利用します。
作成する機能はエージェント実行時に指定された内容に準じます。
オーケストレーターは下記の進捗状態を管理します。予期せぬ中断などに対応するため開発状態の進捗管理ファイルを構成し再開できるようにします。

1. ビジネス仕様とシステム仕様の構築
  - 図示するのが適切な場合はmermaid記法が使用されます
  - **作成された仕様で問題ないかユーザーに確認を行った後に後続の作業を実施します**
  - `docs/specs/{開発機能名}` に格納されます
2. 開発計画とタスクの作成
  - タスクは動作確認が可能などの開発がしやすい粒度で分割される
  - 以降の工程の3、4、5は作成された開発タスクごとに実施され完了していない開発タスクが終わるまで実施される
  - `docs/specs/{開発機能名}/開発タスク` に格納されます。進捗管理ファイルに作成された開発タスクを増やす更新を実施します。
3. 実装
4. ユニットテストの構築
5. 実施結果をもとにテスト、または実装の修正
6. ユーザーテストのシナリオ構築
それぞれの工程ではレビューが実施され、レビューの結果がレポートとして`docs/review`に`{工程}-{timestamp}.md`で出力されます。
レビューの実施後、不合格となっていた場合はレビュー結果を参照し修正を行い再度レビューを実施します（最大二回）
レビューの観点は下記のとおりです。
- ビジネス仕様とシステム仕様: システム開発の設計に関わるISOの様式に準じる。ビジネス仕様には利用シナリオと業務要件が記載されること
- 実装: [C++ Core Guidline](https://learn.microsoft.com/ja-jp/windows/apps/get-started/best-practices) に準じる
- テスト設計: ISTQBなどで提唱されている手法を用いる
- ユーザーテスト: [経産省のシステム管理基準](https://www.meti.go.jp/policy/netsecurity/sys-kansa/sys-kanri-2023.pdf)の「ユーザ受入テスト」を参考にする

カスタムエージェントの構築については下記を参考にすること
- https://code.visualstudio.com/docs/copilot/customization/custom-agents
- https://code.visualstudio.com/docs/copilot/agents/subagents
- https://github.com/github/awesome-copilot/blob/main/instructions/agents.instructions.md
```

---

## 6-2. 企画

お題の企画をCopilotと練りブラッシュアップしていきます。

---

## 6-3. 実装

GitHub Copilot CLIで一気通貫で実装していきます

---

## 6-4. 動作確認

Visual Studioでデバッグしながら動かない箇所を修正します


---

<!-- _class: title -->
<!-- _paginate: false -->

# Thank You!

Windows アプリワークショップ