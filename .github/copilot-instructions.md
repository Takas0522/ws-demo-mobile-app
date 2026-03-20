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

管理者用 Web アプリ（Struts2 移行後）
    ↓
Struts2 管理アプリ (組込み Tomcat, port 8082)
    ↓
SQLite Database（直接アクセス）
```

- すべてのバックエンドサービス（Mobile BFF / Admin BFF / Web API）は Java（Spring Boot）で実装されている
- `src/windows-app/CppApiServer/` に C++ 製の API サーバーが存在する。これは Java の Mobile BFF と同じポート 8081 で動作し、Windows アプリのローカル開発用バックエンド（Mobile BFF の代替）として使用される。iOS / Android アプリからは参照されない
- 開発はWindows/Linux(DevContainer)環境で行うことを想定しているため、コマンド実行時はどのOSで動作ししているか確認してから適切なコマンドを選択すること
- DevContainer 環境では headed モードのブラウザ表示が利用可能である。`playwright-cli open` を使用する際は必ず `--headed` オプションを付与すること。環境の制約を勝手に推測して headed オプションを省略してはならない

### その他

- `workshop-documents` はワークショップ用のドキュメントなので参照してはいけません
