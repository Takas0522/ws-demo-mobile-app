# デモ環境セットアップガイド

> 最終更新: 2025-01-XX  
> 対象: mobile-app-system デモンストレーション用環境

---

## 目次

1. [概要](#1-概要)
2. [前提条件](#2-前提条件)
3. [クイックスタート](#3-クイックスタート)
4. [詳細セットアップ手順](#4-詳細セットアップ手順)
5. [動作確認](#5-動作確認)
6. [トラブルシューティング](#6-トラブルシューティング)
7. [デモシナリオ](#7-デモシナリオ)
8. [環境のリセット](#8-環境のリセット)

---

## 1. 概要

このガイドでは、mobile-app-systemのデモ環境を構築する手順を説明します。

### 1.1 システム構成

```
┌─────────────────────────────────────────────────────────┐
│                    Client Layer                         │
├────────────────┬──────────────────┬────────────────────┤
│  Mobile App    │  Mobile App      │   Admin Web        │
│   (iOS)        │   (Android)      │   (Vue.js)         │
│   *Mock*       │   *Mock*         │   Port: 3000       │
└────────┬───────┴────────┬─────────┴────────┬───────────┘
         │                │                  │
         └────────────────┼──────────────────┘
                          │
┌─────────────────────────┴─────────────────────────────┐
│                     BFF Layer                         │
├──────────────────────┬────────────────────────────────┤
│   Mobile BFF         │      Admin BFF                 │
│   (Spring Boot)      │      (Spring Boot)             │
│   Port: 8081         │      Port: 8082                │
└──────────┬───────────┴────────────┬───────────────────┘
           │                        │
           └────────────┬───────────┘
                        │
┌───────────────────────┴───────────────────────────────┐
│                   API Layer                           │
│                Web API (Spring Boot)                  │
│                Port: 8080                             │
└───────────────────────┬───────────────────────────────┘
                        │
┌───────────────────────┴───────────────────────────────┐
│                  Data Layer                           │
│            SQLite (File-based)                        │
│              ./data/mobile_app.db                     │
└───────────────────────────────────────────────────────┘
```

### 1.2 必要なポート

| サービス | ポート | 説明 |
|---------|--------|------|
| SQLite | - | データベース（ファイルベース） |
| Web API | 8080 | REST API |
| Mobile BFF | 8081 | モバイルアプリ用BFF |
| Admin BFF | 8082 | 管理Web用BFF |
| Admin Web | 3000 | 管理Webアプリ |

---

## 2. 前提条件

### 2.1 必須ソフトウェア

| ソフトウェア | バージョン | 確認コマンド | インストール先 |
|------------|-----------|------------|--------------|
| **Git** | 最新 | `git --version` | https://git-scm.com/ |
| **Docker** | 20.x以上 | `docker --version` | https://www.docker.com/ |
| **Docker Compose** | 2.x以上 | `docker-compose --version` | Docker Desktopに含まれる |
| **Java** | 17以上 | `java -version` | https://adoptium.net/ |
| **Gradle** | 7.x以上 | `./gradlew --version` | プロジェクトに含まれる |
| **Node.js** | 18.x以上 | `node --version` | https://nodejs.org/ |
| **npm** | 9.x以上 | `npm --version` | Node.jsに含まれる |

### 2.2 推奨ツール

| ツール | 用途 | インストール |
|-------|------|------------|
| **curl** | API動作確認 | 標準装備（macOS/Linux） |
| **jq** | JSON整形 | `brew install jq` (macOS) |
| **sqlite3** | DB接続確認 | `brew install sqlite3` (macOS) |
| **Postman** | API手動テスト | https://www.postman.com/ |

### 2.3 システム要件

- **OS**: macOS, Linux, Windows (WSL2推奨)
- **メモリ**: 8GB以上推奨
- **ディスク**: 5GB以上の空き容量

---

## 3. クイックスタート

最速でデモ環境を起動する手順です。

```bash
# 1. リポジトリをクローン
git clone <repository-url>
cd ws-demo-mobile-app

# 2. データベースを初期化
mkdir -p data
sqlite3 ./data/mobile_app.db < database/ddl.sql

# 3. データベースを初期化
cd database
./init-database.sh
cd ..

# 4. Web APIを起動（ターミナル1）
cd src/web-api
./gradlew bootRun

# 5. Mobile BFFを起動（ターミナル2）
cd src/mobile-bff
./gradlew bootRun

# 6. Admin BFFを起動（ターミナル3）
cd src/admin-bff
./gradlew bootRun

# 7. Admin Webを起動（ターミナル4）
cd src/admin-web
npm install
npm run dev

# 8. ブラウザで管理画面を開く
open http://localhost:3000
```

**起動完了**: すべてのサービスが起動したら、[動作確認](#5-動作確認)に進んでください。

---

## 4. 詳細セットアップ手順

### Step 1: リポジトリのクローン

```bash
# HTTPSでクローン
git clone https://github.com/<organization>/ws-demo-mobile-app.git
cd ws-demo-mobile-app

# またはSSHでクローン
git clone git@github.com:<organization>/ws-demo-mobile-app.git
cd ws-demo-mobile-app

# ブランチ確認
git branch
# * main または develop
```

---

### Step 2: SQLiteデータベースのセットアップ

#### 2.1 データベースファイルの準備

```bash
# データベースディレクトリの作成
mkdir -p data

# SQLiteデータベースは初回起動時に自動作成されます
# 手動で作成する場合:
sqlite3 ./data/mobile_app.db < database/ddl.sql

# ファイル確認
ls -la ./data/mobile_app.db
```

#### 2.2 接続確認

```bash
# SQLiteに接続
sqlite3 ./data/mobile_app.db

# 接続成功したら
sqlite> .tables      # テーブル一覧
sqlite> .schema      # スキーマ確認
sqlite> .quit        # 終了
```

---

### Step 3: データベース初期化

#### 3.1 スキーマ作成

```bash
cd database

# DDLスクリプトを実行
sqlite3 ./data/mobile_app.db < ddl.sql

# 確認
sqlite3 ./data/mobile_app.db ".tables"

# 期待される出力: 6つのテーブル
# - users
# - admins
# - products
# - purchases
# - favorites
# - feature_flags
```

#### 3.2 初期データ投入

```bash
# 初期データを投入
sqlite3 ./data/mobile_app.db < seed.sql

# データ確認
sqlite3 ./data/mobile_app.db << EOF
SELECT COUNT(*) FROM users;       -- 3件以上
SELECT COUNT(*) FROM products;    -- 3件以上
SELECT COUNT(*) FROM feature_flags; -- users数と同じ
EOF
```

**自動化スクリプト（推奨）**:

```bash
# init-database.sh を作成（まだない場合）
cat > init-database.sh << 'EOF'
#!/bin/bash
set -e

echo "Initializing database..."
sqlite3 ../data/mobile_app.db < ddl.sql
sqlite3 ../data/mobile_app.db < seed.sql
echo "Database initialized successfully!"
EOF

chmod +x init-database.sh
./init-database.sh
```

---

### Step 4: Web API のセットアップ

#### 4.1 環境変数の設定

```bash
cd src/web-api

# .env ファイルを作成（または .env.example をコピー）
cat > .env << EOF
DB_PATH=./data/mobile_app.db
JWT_SECRET=your-secret-key-change-this-in-production-min-32-chars
SERVER_PORT=8080
EOF
```

#### 4.2 ビルドと起動

```bash
# Gradleラッパーに実行権限を付与（初回のみ）
chmod +x gradlew

# 依存関係のダウンロードとビルド
./gradlew build

# アプリケーションを起動
./gradlew bootRun

# 別ターミナルで動作確認
curl http://localhost:8080/actuator/health
# {"status":"UP"}
```

**バックグラウンド起動**:

```bash
# バックグラウンドで起動
nohup ./gradlew bootRun > web-api.log 2>&1 &

# PIDを記録
echo $! > web-api.pid

# 停止する場合
kill $(cat web-api.pid)
```

---

### Step 5: Mobile BFF のセットアップ

```bash
cd src/mobile-bff

# 環境変数設定
cat > .env << EOF
WEB_API_URL=http://localhost:8080
SERVER_PORT=8081
EOF

# ビルドと起動
chmod +x gradlew
./gradlew build
./gradlew bootRun

# 別ターミナルで動作確認
curl http://localhost:8081/actuator/health
# {"status":"UP"}
```

---

### Step 6: Admin BFF のセットアップ

```bash
cd src/admin-bff

# 環境変数設定
cat > .env << EOF
WEB_API_URL=http://localhost:8080
SERVER_PORT=8082
EOF

# ビルドと起動
chmod +x gradlew
./gradlew build
./gradlew bootRun

# 別ターミナルで動作確認
curl http://localhost:8082/actuator/health
# {"status":"UP"}
```

---

### Step 7: Admin Web のセットアップ

#### 7.1 依存関係のインストール

```bash
cd src/admin-web

# package.json の確認
cat package.json

# 依存関係をインストール
npm install

# インストール確認
ls node_modules | wc -l
# 数百個のパッケージがインストールされる
```

#### 7.2 環境変数の設定

```bash
# .env.local ファイルを作成
cat > .env.local << EOF
VITE_API_BASE_URL=http://localhost:8082
VITE_APP_TITLE=Demo Admin
EOF
```

#### 7.3 起動

```bash
# 開発サーバーを起動
npm run dev

# 出力例:
#   VITE v4.x.x  ready in xxx ms
#
#   ➜  Local:   http://localhost:3000/
#   ➜  Network: use --host to expose
```

#### 7.4 ビルド（本番用）

```bash
# 本番ビルド（デモでは不要）
npm run build

# ビルド成果物の確認
ls -la dist/
```

---

## 5. 動作確認

### 5.1 全サービスの起動確認

```bash
# すべてのサービスが起動しているか確認
curl http://localhost:8080/actuator/health  # {"status":"UP"}
curl http://localhost:8081/actuator/health  # {"status":"UP"}
curl http://localhost:8082/actuator/health  # {"status":"UP"}
curl http://localhost:3000                  # HTMLが返る
```

### 5.2 ログイン動作確認

#### 5.2.1 ユーザーログイン（Mobile BFF）

```bash
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": "password123"
  }' | jq .

# 期待される出力:
# {
#   "token": "eyJhbGciOiJIUzI1NiIs...",
#   "userType": "user",
#   "expiresIn": 3600
# }
```

#### 5.2.2 管理者ログイン（Admin BFF）

```bash
curl -X POST http://localhost:8082/api/v1/auth/admin/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "admin001",
    "password": "adminpass123"
  }' | jq .

# 期待される出力:
# {
#   "token": "eyJhbGciOiJIUzI1NiIs...",
#   "userType": "admin",
#   "expiresIn": 3600
# }
```

### 5.3 商品一覧取得

```bash
# ユーザーログインでトークン取得
TOKEN=$(curl -s -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"loginId":"user001","password":"password123"}' | jq -r '.token')

# 商品一覧を取得
curl http://localhost:8081/api/v1/products \
  -H "Authorization: Bearer $TOKEN" | jq .

# 期待される出力:
# {
#   "products": [
#     {
#       "productId": 1,
#       "productName": "商品A",
#       "unitPrice": 1000,
#       ...
#     }
#   ]
# }
```

### 5.4 管理画面の動作確認

1. ブラウザで http://localhost:3000 を開く
2. ログイン画面が表示される
3. 以下の情報でログイン:
   - ログインID: `admin001`
   - パスワード: `adminpass123`
4. ダッシュボードが表示される
5. サイドメニューから「商品管理」をクリック
6. 商品一覧が表示される

---

## 6. トラブルシューティング

### 6.1 SQLiteデータベースにアクセスできない

**症状**: `unable to open database file` または `no such table`

**原因と解決方法**:

1. **データベースファイルが存在しない**
   ```bash
   ls -la ./data/mobile_app.db
   # ファイルが存在しない場合
   sqlite3 ./data/mobile_app.db < database/ddl.sql
   ```

2. **データベースディレクトリが存在しない**
   ```bash
   mkdir -p data
   sqlite3 ./data/mobile_app.db < database/ddl.sql
   ```

3. **ファイルのパーミッションが不正**
   ```bash
   # パーミッション確認
   ls -la ./data/mobile_app.db
   # 必要に応じて修正
   chmod 644 ./data/mobile_app.db
   ```

---

### 6.2 Web APIが起動しない

**症状**: `Port 8080 was already in use` または ビルドエラー

**原因と解決方法**:

1. **ポート8080が使用中**
   ```bash
   # ポート確認
   lsof -i :8080
   # プロセスを停止
   kill -9 <PID>
   ```

2. **環境変数が設定されていない**
   ```bash
   # .env ファイルを確認
   cat web-api/.env
   # 必要に応じて作成
   ```

3. **データベース接続エラー**
   ```bash
   # ログを確認
   tail -f web-api/logs/application.log
   # SQLiteデータベースファイルの確認
   ls -la ./data/mobile_app.db
   ```

4. **Javaバージョンが古い**
   ```bash
   java -version
   # Java 17以上が必要
   ```

---

### 6.3 BFFが起動しない

**症状**: `Connection refused to Web API`

**原因と解決方法**:

1. **Web APIが起動していない**
   ```bash
   curl http://localhost:8080/actuator/health
   # Web APIを起動
   ```

2. **WEB_API_URLが間違っている**
   ```bash
   # .env ファイルを確認
   cat mobile-bff/.env
   # 正しいURL: http://localhost:8080
   ```

---

### 6.4 Admin Webが起動しない

**症状**: `npm install` エラーまたは `npm run dev` エラー

**原因と解決方法**:

1. **Node.jsバージョンが古い**
   ```bash
   node -version
   # 18.x以上が必要
   ```

2. **依存関係のインストール失敗**
   ```bash
   # node_modulesを削除して再インストール
   rm -rf node_modules package-lock.json
   npm install
   ```

3. **ポート3000が使用中**
   ```bash
   # ポート確認
   lsof -i :3000
   # または別のポートを使用
   npm run dev -- --port 3001
   ```

---

### 6.5 ログインできない

**症状**: 401 Unauthorized

**原因と解決方法**:

1. **初期データが投入されていない**
   ```sql
   sqlite3 ./data/mobile_app.db "SELECT * FROM users;"
   # データがない場合は seed.sql を実行
   ```

2. **パスワードハッシュが間違っている**
   ```bash
   # seed.sqlを確認
   cat database/seed.sql | grep password
   ```

3. **JWT秘密鍵が設定されていない**
   ```bash
   # .env ファイルを確認
   cat web-api/.env | grep JWT_SECRET
   ```

---

### 6.6 一般的なエラーとログ確認

| エラー | ログ確認コマンド |
|-------|--------------|
| Web API | `tail -f web-api/logs/application.log` |
| Mobile BFF | `tail -f mobile-bff/logs/application.log` |
| Admin BFF | `tail -f admin-bff/logs/application.log` |
| SQLite | `sqlite3 ./data/mobile_app.db "PRAGMA integrity_check;"` |

---

## 7. デモシナリオ

### 7.1 基本デモ（5分）

#### シナリオ: ユーザーが商品を検索して購入する

1. **ログイン**
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{"loginId":"user001","password":"password123"}' | jq .
   ```

2. **トークン保存**
   ```bash
   export TOKEN="<取得したトークン>"
   ```

3. **商品一覧取得**
   ```bash
   curl http://localhost:8081/api/v1/products \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

4. **商品検索**
   ```bash
   curl "http://localhost:8081/api/v1/products/search?keyword=商品A" \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

5. **商品詳細取得**
   ```bash
   curl http://localhost:8081/api/v1/products/1 \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

6. **商品購入**
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{"productId":1,"quantity":100}' | jq .
   ```

7. **購入履歴確認**
   ```bash
   curl http://localhost:8081/api/v1/purchases \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

---

### 7.2 機能フラグデモ（10分）

#### シナリオ: 管理者が機能フラグを変更し、ユーザー側で反映を確認

**Part 1: フラグOFFユーザーでお気に入り登録を試みる**

1. user002でログイン（フラグOFF）
2. お気に入り登録を試みる → 403エラー

**Part 2: 管理画面でフラグをONに変更**

1. ブラウザで管理画面にログイン
2. 機能フラグ管理画面を開く
3. user002のフラグをONに変更

**Part 3: 再度お気に入り登録を試みる**

1. user002で新しいトークンを取得
2. お気に入り登録 → 成功

詳細は `docs/testing/test-scenarios/feature-flag-scenario.md` を参照

---

### 7.3 管理機能デモ（10分）

#### シナリオ: 管理者が商品情報を更新し、反映を確認

**Part 1: 現在の商品情報確認**

1. ユーザーで商品Aの詳細を取得 → 単価1000円

**Part 2: 管理画面で商品を更新**

1. 管理画面にログイン
2. 商品管理画面を開く
3. 商品Aの単価を1200円に変更

**Part 3: 変更の反映確認**

1. ユーザーで商品Aの詳細を再取得 → 単価1200円
2. 購入を実行 → 1200円で購入される

---

## 8. 環境のリセット

### 8.1 データベースのリセット

```bash
cd database

# データのみ削除（テーブル構造は保持）
sqlite3 ./data/mobile_app.db << EOF
DELETE FROM purchases;
DELETE FROM favorites;
DELETE FROM feature_flags;
DELETE FROM products;
DELETE FROM users;
EOF

# 初期データを再投入
sqlite3 ./data/mobile_app.db < seed.sql
```

### 8.2 データベース全体を再作成

```bash
# データベースファイルを削除
rm -f ./data/mobile_app.db

# 初期化スクリプトを実行
./init-database.sh
```

### 8.3 すべてのサービスを停止

```bash
# BFFとWeb APIを停止（Ctrl+C で各ターミナルを停止）

# またはプロセスを強制終了
pkill -f "gradle.*bootRun"
pkill -f "vite"

# SQLiteはファイルベースのため停止不要
```

### 8.4 リセットスクリプト（推奨）

```bash
# scripts/reset-all.sh を作成
cat > scripts/reset-all.sh << 'EOF'
#!/bin/bash
set -e

echo "Stopping all services..."
pkill -f "gradle.*bootRun" || true
pkill -f "vite" || true

echo "Reinitializing database..."
rm -f ./data/mobile_app.db
cd database
sqlite3 ../data/mobile_app.db < ddl.sql
sqlite3 ../data/mobile_app.db < seed.sql
cd ..

echo "Reset complete! You can now start services again."
EOF

chmod +x scripts/reset-all.sh
./scripts/reset-all.sh
```

---

## 9. 便利なスクリプト

### 9.1 全サービス起動スクリプト

```bash
# scripts/start-all.sh
cat > scripts/start-all.sh << 'EOF'
#!/bin/bash
set -e

echo "Initializing SQLite database..."
mkdir -p data
sqlite3 ./data/mobile_app.db < database/ddl.sql
sqlite3 ./data/mobile_app.db < database/seed.sql

echo "Starting Web API..."
cd src/web-api
nohup ./gradlew bootRun > ../logs/web-api.log 2>&1 &
echo $! > ../pids/web-api.pid
cd ..

echo "Waiting for Web API..."
sleep 10

echo "Starting Mobile BFF..."
cd src/mobile-bff
nohup ./gradlew bootRun > ../logs/mobile-bff.log 2>&1 &
echo $! > ../pids/mobile-bff.pid
cd ..

echo "Starting Admin BFF..."
cd src/admin-bff
nohup ./gradlew bootRun > ../logs/admin-bff.log 2>&1 &
echo $! > ../pids/admin-bff.pid
cd ..

echo "Starting Admin Web..."
cd src/admin-web
nohup npm run dev > ../logs/admin-web.log 2>&1 &
echo $! > ../pids/admin-web.pid
cd ..

echo "All services started!"
echo "Logs are in ./logs/"
echo "PIDs are in ./pids/"
EOF

chmod +x scripts/start-all.sh
```

### 9.2 全サービス停止スクリプト

```bash
# scripts/stop-all.sh
cat > scripts/stop-all.sh << 'EOF'
#!/bin/bash

echo "Stopping all services..."

# PIDファイルから停止
if [ -d "pids" ]; then
  for pid_file in pids/*.pid; do
    if [ -f "$pid_file" ]; then
      kill $(cat "$pid_file") 2>/dev/null || true
      rm "$pid_file"
    fi
  done
fi

# Docker停止（不要 - SQLiteはファイルベース）

echo "All services stopped!"
EOF

chmod +x scripts/stop-all.sh
```

---

## 10. よくある質問（FAQ）

### Q1: ポートを変更したい

**A**: 各サービスの設定ファイルまたは環境変数で変更できます。

```bash
# Web API
# web-api/.env
SERVER_PORT=8090

# Admin Web
# admin-web/.env.local
VITE_PORT=3001

# または起動時に指定
npm run dev -- --port 3001
```

### Q2: 本番環境にデプロイしたい

**A**: このシステムはデモ用です。本番環境にデプロイする場合は以下を検討してください：

- HTTPS化（Let's Encrypt等）
- JWT秘密鍵の変更
- データベース認証情報の変更
- 環境変数の外部化（AWS Secrets Manager等）
- ログ管理（CloudWatch等）
- モニタリング（Prometheus + Grafana等）

### Q3: モバイルアプリはどうやって動かす？

**A**: このデモではモバイルアプリは実装されていません（BFFまで）。実際のモバイルアプリを開発する場合は：

- iOS: Xcode + Swift
- Android: Android Studio + Java/Kotlin
- API URL: Mobile BFF (`http://localhost:8081`)

### Q4: テストデータを追加したい

**A**: `database/seed.sql` を編集して再実行してください。

```sql
-- ユーザー追加
INSERT INTO users (login_id, user_name, password_hash, created_at, updated_at)
VALUES ('user999', 'テストユーザー', '$2a$10$...', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);
```

---

## 11. 参考資料

| ドキュメント | パス |
|------------|------|
| システム概要 | `docs/specs/mobile-app-system/00-overview.md` |
| API仕様 | `docs/specs/mobile-app-system/05-api-spec.md` |
| テスト計画 | `docs/testing/integration-test-plan.md` |
| テストシナリオ | `docs/testing/test-scenarios/` |
| アーキテクチャ | `docs/architecture/` |

---

## 12. サポート

問題が発生した場合:

1. まず[トラブルシューティング](#6-トラブルシューティング)を確認
2. ログファイルを確認（`logs/` ディレクトリ）
3. Issueを作成（GitHub）

---

**デモ環境の準備が完了しました！🎉**

次は [docs/testing/integration-test-plan.md](testing/integration-test-plan.md) を参照して、統合テストを実施してください。

---

**End of Document**
