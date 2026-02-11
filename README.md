# Mobile App System

> モバイルアプリケーション向けバックエンドシステムのデモプロジェクト

## 概要

本プロジェクトは、モバイルアプリケーションと管理画面をサポートするバックエンドシステムです。
Spring Bootを使用した3層アーキテクチャ（Web API、Mobile BFF、Admin BFF）で構成されています。

### 主な機能

- ユーザー認証（JWT）
- 商品管理（CRUD）
- 購入履歴管理
- お気に入り機能（機能フラグ制御）
- 管理者による機能フラグ制御

## システム構成

```
┌─────────────┐       ┌─────────────┐
│ Mobile App  │       │ Admin Web   │
└──────┬──────┘       └──────┬──────┘
       │                     │
       ▼                     ▼
┌─────────────┐       ┌─────────────┐
│ Mobile BFF  │       │ Admin BFF   │
│   :8081     │       │   :8082     │
└──────┬──────┘       └──────┬──────┘
       │                     │
       └──────────┬──────────┘
                  ▼
          ┌─────────────┐
          │  Web API    │
          │   :8080     │
          └──────┬──────┘
                 ▼
          ┌─────────────┐
          │ PostgreSQL  │
          │   :5432     │
          └─────────────┘
```

## 技術スタック

### バックエンド
- **言語**: Java 17
- **フレームワーク**: Spring Boot 3.2.1
- **データベース**: PostgreSQL 16
- **認証**: JWT (JSON Web Token)
- **ビルドツール**: Maven 3.9+

### インフラ
- **コンテナ**: Docker / Docker Compose
- **開発環境**: DevContainer

## ディレクトリ構造

```
.
├── .devcontainer/          # DevContainer設定
│   ├── devcontainer.json
│   ├── docker-compose.yml
│   └── Dockerfile
├── web-api/                # Web API (Core API Layer)
│   ├── pom.xml
│   └── src/
├── mobile-bff/             # Mobile BFF
│   ├── pom.xml
│   └── src/
├── admin-bff/              # Admin BFF
│   ├── pom.xml
│   └── src/
├── database/               # データベーススクリプト
│   ├── schema/             # スキーマ定義
│   │   └── 01_create_tables.sql
│   └── seeds/              # 初期データ
│       └── 02_seed_data.sql
├── docker-compose.yml      # Docker Compose設定
├── .env.example            # 環境変数サンプル
└── docs/                   # ドキュメント
    ├── architecture/       # アーキテクチャドキュメント
    └── specs/              # 機能仕様書
```

## セットアップ

### 前提条件

- Docker / Docker Compose
- Java 17+
- Maven 3.9+

### 1. 環境変数の設定

```bash
cp .env.example .env
# 必要に応じて .env を編集
```

### 2. データベースの起動

```bash
docker-compose up -d
```

PostgreSQLが起動し、スキーマと初期データが自動的に投入されます。

### 3. データベース接続確認

```bash
# PostgreSQLコンテナに接続
docker exec -it mobile-app-postgres psql -U postgres -d mobile_app

# テーブル確認
\dt

# データ確認
SELECT COUNT(*) FROM users;
SELECT COUNT(*) FROM products;
```

### 4. Web APIのビルドと起動

```bash
cd web-api
mvn clean package
java -jar target/web-api-1.0.0-SNAPSHOT.jar
```

Web APIが `http://localhost:8080` で起動します。

### 5. Mobile BFFの起動

```bash
cd mobile-bff
mvn clean package
java -jar target/mobile-bff-1.0.0-SNAPSHOT.jar
```

Mobile BFFが `http://localhost:8081` で起動します。

### 6. Admin BFFの起動

```bash
cd admin-bff
mvn clean package
java -jar target/admin-bff-1.0.0-SNAPSHOT.jar
```

Admin BFFが `http://localhost:8082` で起動します。

## ヘルスチェック

各サービスの稼働状況を確認：

```bash
# Web API
curl http://localhost:8080/api/v1/health

# Mobile BFF
curl http://localhost:8081/api/mobile/health

# Admin BFF
curl http://localhost:8082/api/admin/health
```

## 初期ユーザー

### エンドユーザー
- **ログインID**: `user001` ~ `user010`
- **パスワード**: `password123`

### 管理者
- **ログインID**: `admin001`
- **パスワード**: `admin123`

## API仕様

詳細なAPI仕様は以下のドキュメントを参照してください：

- [API仕様書](docs/specs/mobile-app-system/05-api-spec.md)
- [APIアーキテクチャ](docs/architecture/04-api-architecture.md)

## 開発ガイド

### コーディング規約

[コーディング規約](docs/architecture/09-coding-standards.md)に従ってください。

### ビルド

```bash
# 全プロジェクトのビルド
cd web-api && mvn clean package && cd ..
cd mobile-bff && mvn clean package && cd ..
cd admin-bff && mvn clean package && cd ..
```

### テスト実行

```bash
# Web API
cd web-api && mvn test

# Mobile BFF
cd mobile-bff && mvn test

# Admin BFF
cd admin-bff && mvn test
```

## トラブルシューティング

### データベース接続エラー

PostgreSQLコンテナが起動しているか確認：

```bash
docker ps | grep postgres
```

起動していない場合：

```bash
docker-compose up -d
```

### ポート競合

デフォルトポート（5432, 8080, 8081, 8082）が使用中の場合、`.env`ファイルでポート番号を変更してください。

## ライセンス

このプロジェクトはデモ用途です。

## 参考ドキュメント

- [システム概要](docs/specs/mobile-app-system/00-overview.md)
- [データモデル](docs/specs/mobile-app-system/04-data-model.md)
- [アーキテクチャ概要](docs/architecture/00-overview.md)
- [セキュリティアーキテクチャ](docs/architecture/05-security-architecture.md)
