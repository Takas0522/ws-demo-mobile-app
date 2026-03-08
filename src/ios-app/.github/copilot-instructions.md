## プロジェクト概要

mobile-app-system の iOS クライアントアプリです。商品販売を行うネイティブ iOS アプリケーションで、Mobile BFF（Java, ポート 8081）を経由して Web API（Java, ポート 8080）に接続します。

### バックエンド構成

```
iOS App → Mobile BFF (Java, port 8081) → Web API (Java, port 8080) → SQLite
```

- バックエンドはすべて Java（Spring Boot）で構成されている
- `src/windows-app/CppApiServer/` の C++ 製 Web API サーバーは本アプリのバックエンドとは無関係であり、参照しないこと

## 技術スタック

- 言語: Swift
- UI フレームワーク: SwiftUI
- 非同期処理: Swift Concurrency（async/await）
- テスト: XCTest

## プロジェクト構造

```
MobileApp/MobileApp/
├── Views/       # SwiftUI View + ViewModel
├── Models/      # Codable struct
├── Services/    # APIClient, AuthService
└── Utilities/   # KeychainManager
```

## その他

- `workshop-documents` はワークショップ用のドキュメントなので参照してはいけません
