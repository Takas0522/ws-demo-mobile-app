## プロジェクト概要

mobile-app-system の iOS クライアントアプリです。商品販売を行うネイティブ iOS アプリケーションで、Mobile BFF（ポート 8081）を経由して Web API に接続します。

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
