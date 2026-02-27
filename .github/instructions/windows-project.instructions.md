---
description: Visual Studio プロジェクトファイル (.vcxproj / .slnx) の構成規約
applyTo: "src/{windows-app,cpp-api}/**/*.{vcxproj,slnx,props,targets}"
---

# Visual Studio プロジェクト構成規約

## ソリューション

- ソリューションファイル: `src/windows-app/WsDemoMobileApp.WindowsApp/WsDemoMobileApp.WindowsApp.slnx` (XML形式)
- 本ソリューションには以下の2プロジェクトが含まれる:
  - **WsDemoMobileApp.WindowsApp** — Win32 GUI クライアントアプリ（サブシステム: Windows）
  - **CppApiServer** — C++ REST API サーバー（サブシステム: Console）
- F5 実行時は「複数のスタートアッププロジェクト」で両方を Start に設定すること

## 共通ビルド設定

- C++ 言語標準: `/std:c++latest`（`std::expected` 等 C++23 機能を使用）
- Windows SDK: 最新版
- プラットフォームツールセット: v143 以上
- 文字セット: Unicode
- 対象OS: Windows 10 以上 (`_WIN32_WINNT=0x0A00`)

## 構成

- Debug / Release × x64 をサポート
- ARM64 はオプション

## WsDemoMobileApp.WindowsApp リンクライブラリ

- `winhttp.lib` (HTTP通信)
- `advapi32.lib` (Credential Manager / DPAPI)
- `comctl32.lib` (Common Controls)

## CppApiServer リンクライブラリ

- `advapi32.lib` (暗号化 API)
- `bcrypt.lib` (Windows BCrypt API — JWT HMAC-SHA256)
- `ws2_32.lib` (Winsock — cpp-httplib が使用)
