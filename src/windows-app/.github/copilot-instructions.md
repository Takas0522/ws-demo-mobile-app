# Copilot Instructions

## バックエンド構成

Windows アプリはポート 8081 のバックエンドに接続する。バックエンドには 2 つの選択肢がある。

### 1. CppApiServer（C++ 製ローカルバックエンド）

```
Windows App → CppApiServer (C++, port 8081) → SQLite (ローカル DB)
```

- `CppApiServer/` ディレクトリに配置された C++ 製の API サーバー
- Java のバックエンドスタックを起動せずに Windows アプリ単体で開発・デバッグできる
- ソリューションの起動プロファイル「CppApiServer + WindowsApp」で同時起動可能

### 2. Java Mobile BFF（共通バックエンド）

```
Windows App → Mobile BFF (Java, port 8081) → Web API (Java, port 8080) → SQLite
```

- iOS / Android と共通の Java バックエンドスタックを使用する構成
- ソリューションの起動プロファイル「WindowsApp Only」で起動し、別途 Java サービスを起動する

### API 仕様の参照先

- C++ バックエンド: `CppApiServer/` のソースコードを参照
- Java バックエンド: `src/mobile-bff/` および `src/web-api/` を参照

## プロジェクト ガイドライン

- テストプロジェクトで別プロジェクトのソースファイルを参照する場合、.vcxproj.filters の References 仮想ディレクトリ配下に配置してソリューションエクスプローラーで見分けがつくようにすること。
