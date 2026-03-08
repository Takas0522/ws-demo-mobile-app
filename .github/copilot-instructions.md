## プロジェクト概要

mobile-app-system は、商品販売を行うネイティブアプリケーション（iOS / Android / Windows）と管理者用Webアプリケーションで構成されるデモシステムです。BFFパターンを採用し、各クライアントは専用のBFFを経由してWeb APIに接続します。

### システムアーキテクチャ

```
モバイルアプリ (iOS / Android / Windows)
    ↓
Mobile BFF (Java, port 8081)
    ↓
Web API (Java, port 8080)
    ↓
SQLite Database

管理者用 Web アプリ
    ↓
Admin BFF (Java, port 8082)
    ↓
Web API (Java, port 8080)
    ↓
SQLite Database
```

- すべてのバックエンドサービス（Mobile BFF / Admin BFF / Web API）は Java（Spring Boot）で実装されている
- `src/windows-app/CppApiServer/` に C++ 製の API サーバーが存在する。これは Java の Mobile BFF と同じポート 8081 で動作し、Windows アプリのローカル開発用バックエンド（Mobile BFF の代替）として使用される。iOS / Android アプリからは参照されない

### その他

- `workshop-documents` はワークショップ用のドキュメントなので参照してはいけません
