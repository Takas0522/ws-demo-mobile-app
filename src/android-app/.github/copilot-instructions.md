## プロジェクト概要

mobile-app-system の Android クライアントアプリです。商品販売を行うネイティブ Android アプリケーションで、Mobile BFF（ポート 8081）を経由して Web API に接続します。

## 技術スタック

- 言語: Java（将来的に Kotlin への移行を推奨）
- ビルドツール: Gradle 8.2.2 / Java 17
- ネットワーク: Retrofit 2 + OkHttp
- テスト: JUnit 4.13.2 + Mockito

## プロジェクト構造

```
app/src/main/java/com/example/mobileapp/
├── ui/          # Activity, Adapter
├── models/      # API Request/Response モデル
├── services/    # ApiClient, ApiService (Retrofit)
└── utils/       # SecureStorageManager
```

## その他

- `workshop-documents` はワークショップ用のドキュメントなので参照してはいけません
