---
description: Visual Studio プロジェクトファイル (.vcxproj / .slnx) の構成規約
applyTo: "src/windows-app/**/*.{vcxproj,slnx,props,targets}"
---

# Visual Studio プロジェクト構成規約

## ソリューション

- ソリューションファイル: `WsDemoMobileApp.WindowsApp.slnx` (XML形式)
- プロジェクトファイル: `.vcxproj` (MSBuild)

## ビルド設定

- C++ 言語標準: `/std:c++20`
- Windows SDK: 最新版
- プラットフォームツールセット: v143 以上
- 文字セット: Unicode
- 対象OS: Windows 10 以上 (`_WIN32_WINNT=0x0A00`)

## 構成

- Debug / Release × x64 をサポート
- ARM64 はオプション

## リンクライブラリ

- `winhttp.lib` (HTTP通信)
- `advapi32.lib` (Credential Manager / DPAPI)
- `comctl32.lib` (Common Controls)
