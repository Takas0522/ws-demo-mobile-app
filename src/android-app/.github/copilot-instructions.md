## プロジェクト概要

mobile-app-system の Android クライアントアプリです。商品販売を行うネイティブ Android アプリケーションで、Mobile BFF（Java, ポート 8081）を経由して Web API（Java, ポート 8080）に接続します。

### バックエンド構成

```
Android App → Mobile BFF (Java, port 8081) → Web API (Java, port 8080) → SQLite
```

- バックエンドはすべて Java（Spring Boot）で構成されている
- `src/windows-app/CppApiServer/` の C++ 製 Web API サーバーは本アプリのバックエンドとは無関係であり、参照しないこと

## 技術スタック

- 言語: Java（将来的に Kotlin への移行を推奨）
- ビルドツール: Gradle 8.2.2 / Java 17
- ネットワーク: Retrofit 2 + OkHttp
- テスト: JUnit 4.13.2 + Mockito
- デバッグログ: アプリケーション実行時のログは Logcat に出力される。`Log.d/w/e(TAG, "message")` で記録し、Android Studio の Logcat パネルで確認する

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
