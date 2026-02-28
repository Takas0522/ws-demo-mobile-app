# Windowsアプリワークショップ

## シナリオ

先行して開発を実施していた商品管理チームの機能追加に追従して開発を実施します。
他部署で作成された仕様書をベースに自分たちのシステムの開発計画を作成し開発を行います。
AIを用いた開発の初回ということで、開発のためのCopilotのカスタマイズを実施した後開発を進めます。

## 0.環境構築、準備運動

- Visual Studio 2022 or 2026(C++が開発可能であること)
- VS Code(任意)
  - [Markdown Preview Mermaid Support](https://marketplace.visualstudio.com/items?itemName=bierner.markdown-mermaid)
- GitHub Copilot CLI
  - Windows: `winget install GitHub.Copilot`
  - MacOS: `brew install copilot-cli`

## 1. AIのカスタマイズ

`Custom Instructions`, `Custom Agents`, `Skills`  の構成を行います。

### Custom Instructions

### Custom Agents

### Skills

## 2.


管理者アプリの仕様変更に合わせてWindowsアプリケーションの回収を行うというシナリオ
GitHubCopilotCLIで調査、プランの作成、全体の修正を行い、VSで実際の動作を確認する。といった建付けにする
1. ユニットテストの作成を実施する
2. エージェントを構築し、調査を行う→計画する
3. 実際の開発を実施する