# Copilot Instructions

## バックエンド構成

Windows アプリは他のモバイルアプリ（iOS / Android）と同様に、Mobile BFF（Java, ポート 8081）を経由して Web API（Java, ポート 8080）に接続する。

```
Windows App → Mobile BFF (Java, port 8081) → Web API (Java, port 8080) → SQLite
```

- `CppApiServer/` ディレクトリに C++ 製の Web API サーバーが存在するが、これは実際のシステム構成には含まれず、どのクライアントからも参照されていない独立した実装である
- バックエンドの API 仕様を確認する際は `src/mobile-bff/` および `src/web-api/`（Java）を参照すること

## プロジェクト ガイドライン

- テストプロジェクトで別プロジェクトのソースファイルを参照する場合、.vcxproj.filters の References 仮想ディレクトリ配下に配置してソリューションエクスプローラーで見分けがつくようにすること。
