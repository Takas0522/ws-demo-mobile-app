# デモ環境セットアップガイド

> 最終更新: 2026-03-20  
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
┌──────────────────────────────────────────────────────────────────────────────┐
│                             Client Layer                                    │
├────────────────┬──────────────────┬──────────────────┬──────────────────────┤
│  Mobile App    │  Mobile App      │  Windows App     │  Struts2 Admin       │
│   (iOS)        │   (Android)      │  (C++/Win32)     │  (Java/Struts2)      │
│   *Mock*       │   *Mock*         │   Port: N/A      │   Port: 8082         │
└────────┬───────┴────────┬─────────┴────────┬─────────┴────────┬─────────────┘
         │                │                  │                  │
         └────────────────┼──────────────────┘                  │
                          │                                     │
┌─────────────────────────┴─────────────────────┐               │
│              BFF Layer                        │               │
│   Mobile BFF (Spring Boot, Port: 8081)        │               │
└──────────────────────┬────────────────────────┘               │
                       │                                        │
                       └────────────────┬───────────────────────┘
                                        │
┌───────────────────────────────────────┴───────────────────────┐
│                   API Layer                                   │
│                Web API (Spring Boot)                          │
│                Port: 8080                                     │
└───────────────────────┬───────────────────────────────────────┘
                        │
┌───────────────────────┴───────────────────────────────────────┐
│                  Data Layer                                   │
│            SQLite (File-based)                                │
│          src/web-api/data/mobile_app.db                       │
└───────────────────────────────────────────────────────────────┘
```

> **Note**: Struts2 Admin は Admin BFF + Admin Web (Vue.js) を統合した後継アプリケーションです。
> 組込み Tomcat 7 上で動作し、SQLite に直接アクセスします。

### 1.2 必要なポート

| サービス | ポート | 説明 |
|---------|--------|------|
| SQLite | - | データベース（ファイルベース） |
| Web API | 8080 | REST API（Spring Boot） |
| Mobile BFF | 8081 | モバイルアプリ用BFF（Spring Boot） |
| Struts2 Admin | 8082 | 管理Webアプリ（Struts2 + 組込みTomcat 7） |

---

## 2. 前提条件

### 2.1 必須ソフトウェア

| ソフトウェア | バージョン | 確認コマンド | インストール先 |
|------------|-----------|------------|--------------|
| **Git** | 最新 | `git --version` | https://git-scm.com/ |
| **Java** | 7以上 | `java -version` | https://adoptium.net/ |
| **Apache Maven** | 3.x以上 | `mvn -v` | https://maven.apache.org/ |
| **SQLite3** | 最新 | `sqlite3 --version` | https://www.sqlite.org/download.html |

> **Note**: admin-struts は Java 7 / Struts 2.3.37 で構築されています。
> Web API / Mobile BFF は Java 17 + Spring Boot です。

### 2.2 推奨ツール

| ツール | 用途 | インストール |
|-------|------|------------|
| **curl** | API動作確認 | 標準装備（macOS/Linux）、Windows は Git Bash に同梱 |
| **jq** | JSON整形 | `brew install jq` (macOS) / `winget install jqlang.jq` (Windows) |
| **sqlite3** | DB接続確認 | `brew install sqlite3` (macOS) / `winget install SQLite.SQLite` (Windows) |
| **Postman** | API手動テスト | https://www.postman.com/ |

### 2.3 システム要件

- **OS**: macOS, Linux, Windows 10以上
- **メモリ**: 8GB以上推奨
- **ディスク**: 5GB以上の空き容量

---

## 3. クイックスタート

最速でデモ環境を起動する手順です。

### Linux / macOS

```bash
# 1. リポジトリをクローン
git clone <repository-url>
cd ws-demo-mobile-app

# 2. データベースを初期化
./scripts/reset-database.sh

# 3. すべてのサービスを起動
./scripts/start-all-services.sh

# 4. ブラウザで管理画面を開く
#    URL:   http://localhost:8082/admin-struts/admin/login
#    ID:    admin001
#    PW:    admin123
```

### Windows（コマンドプロンプト）

```bat
REM 1. リポジトリをクローン
git clone <repository-url>
cd ws-demo-mobile-app

REM 2. データベースを初期化
scripts\reset-database.bat

REM 3. すべてのサービスを起動
scripts\start-all-services.bat

REM 4. ブラウザで管理画面を開く
REM    URL:   http://localhost:8082/admin-struts/admin/login
REM    ID:    admin001
REM    PW:    admin123
```

### admin-struts のみ起動する場合

admin-struts 単体で起動する場合は、先に DB の初期化を行った上で個別起動スクリプトを使用します。

**Linux / macOS:**

```bash
# DB 初期化（未実施の場合）
./scripts/reset-database.sh

# admin-struts を起動
./scripts/start-admin-struts.sh

# 停止
./scripts/stop-admin-struts.sh
```

**Windows:**

```bat
REM DB 初期化（未実施の場合）
scripts\reset-database.bat

REM admin-struts を起動
scripts\start-admin-struts.bat

REM 停止
scripts\stop-admin-struts.bat
```

**起動完了**: すべてのサービスが起動したら、[動作確認](#5-動作確認)に進んでください。

### ログイン情報

| ユーザー | ログインID | パスワード | 種別 | アクセス先 |
|---------|-----------|-----------|------|-----------|
| 管理者 | `admin001` | `admin123` | admin | Web UI + Admin API |
| 山田太郎 | `user001` | `password123` | user | API のみ |
| 佐藤花子〜加藤由美 | `user002`〜`user010` | `password123` | user | API のみ |

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

**Linux / macOS:**

```bash
# リセットスクリプトで初期化（推奨）
./scripts/reset-database.sh

# 手動で確認する場合
sqlite3 src/web-api/data/mobile_app.db ".tables"
```

**Windows:**

```bat
REM リセットスクリプトで初期化（推奨）
scripts\reset-database.bat

REM 手動で確認する場合
sqlite3 src\web-api\data\mobile_app.db ".tables"
```

#### 2.2 接続確認

```bash
# SQLiteに接続
sqlite3 src/web-api/data/mobile_app.db

# 接続成功したら
sqlite> .tables      # テーブル一覧
sqlite> .schema      # スキーマ確認
sqlite> .quit        # 終了
```

---

### Step 3: データベース初期化

#### 3.1 スキーマ作成

**Linux / macOS:**

```bash
# スキーマ作成
sqlite3 src/web-api/data/mobile_app.db < src/database/schema/01_create_tables.sql

# テーブル確認
sqlite3 src/web-api/data/mobile_app.db ".tables"
# 期待される出力: 7つのテーブル
# users, products, purchases, favorites, feature_flags,
# user_feature_flags, product_price_history
```

**Windows:**

```bat
REM スキーマ作成
sqlite3 src\web-api\data\mobile_app.db < src\database\schema\01_create_tables.sql

REM テーブル確認
sqlite3 src\web-api\data\mobile_app.db ".tables"
```

#### 3.2 初期データ投入

**Linux / macOS:**

```bash
# 初期データを投入
sqlite3 src/web-api/data/mobile_app.db < src/database/seeds/02_seed_data.sql

# データ確認
sqlite3 src/web-api/data/mobile_app.db "SELECT 'users' as tbl, COUNT(*) as cnt FROM users UNION ALL SELECT 'products', COUNT(*) FROM products;"
```

**Windows:**

```bat
REM 初期データを投入
sqlite3 src\web-api\data\mobile_app.db < src\database\seeds\02_seed_data.sql

REM データ確認
sqlite3 src\web-api\data\mobile_app.db "SELECT 'users' as tbl, COUNT(*) as cnt FROM users UNION ALL SELECT 'products', COUNT(*) FROM products;"
```

**自動化スクリプト（推奨）**:

```bash
# Linux / macOS
./scripts/reset-database.sh

# Windows
scripts\reset-database.bat
```

---

### Step 4: Web API のセットアップ

#### 4.1 ビルドと起動

**Linux / macOS:**

```bash
cd src/web-api

# ビルド
mvn clean package -q -DskipTests

# 起動
java -jar target/web-api-1.0.0-SNAPSHOT.jar

# 別ターミナルで動作確認
curl http://localhost:8080/actuator/health
# {"status":"UP"}
```

**Windows:**

```bat
cd src\web-api

REM ビルド
mvn clean package -q -DskipTests

REM 起動
java -jar target\web-api-1.0.0-SNAPSHOT.jar

REM 別ターミナルで動作確認
curl http://localhost:8080/actuator/health
```

**バックグラウンド起動**:

```bash
# Linux / macOS
nohup java -jar target/web-api-1.0.0-SNAPSHOT.jar > ../../logs/web-api.log 2>&1 &
echo $! > ../../pids/web-api.pid
```

```bat
REM Windows
start "Web API" /b cmd /c "java -jar target\web-api-1.0.0-SNAPSHOT.jar > ..\..\logs\web-api.log 2>&1"
```

---

### Step 5: Mobile BFF のセットアップ

**Linux / macOS:**

```bash
cd src/mobile-bff

# ビルドと起動
mvn clean package -q -DskipTests
java -jar target/mobile-bff-1.0.0-SNAPSHOT.jar

# 別ターミナルで動作確認
curl http://localhost:8081/actuator/health
# {"status":"UP"}
```

**Windows:**

```bat
cd src\mobile-bff

REM ビルドと起動
mvn clean package -q -DskipTests
java -jar target\mobile-bff-1.0.0-SNAPSHOT.jar

REM 別ターミナルで動作確認
curl http://localhost:8081/actuator/health
```

---

### Step 6: Struts2 管理画面のセットアップ

> Admin BFF (Spring Boot) + Admin Web (Vue.js) は Struts2 管理画面アプリに統合されました。

#### 6.1 ビルドと起動

**Linux / macOS:**

```bash
cd src/admin-struts

# WAR ビルド
mvn clean package -q -DskipTests

# 組込み Tomcat で起動（ポート 8082）
mvn tomcat7:run -DskipTests

# 別ターミナルで動作確認
curl -s -o /dev/null -w "%{http_code}" http://localhost:8082/admin-struts/admin/login
# 200
```

**Windows:**

```bat
cd src\admin-struts

REM WAR ビルド
mvn clean package -q -DskipTests

REM 組込み Tomcat で起動（ポート 8082）
mvn tomcat7:run -DskipTests

REM 別ターミナルで動作確認
curl -s -o nul -w "%%{http_code}" http://localhost:8082/admin-struts/admin/login
```

#### 6.2 起動スクリプトを使用（推奨）

```bash
# Linux / macOS
./scripts/start-admin-struts.sh

# 停止
./scripts/stop-admin-struts.sh
```

```bat
REM Windows
scripts\start-admin-struts.bat

REM 停止
scripts\stop-admin-struts.bat
```

#### 6.3 アクセス

- **URL**: http://localhost:8082/admin-struts/admin/login
- **ログインID**: `admin001`
- **パスワード**: `admin123`

---

## 5. 動作確認

### 5.1 全サービスの起動確認

**Linux / macOS:**

```bash
curl http://localhost:8080/actuator/health                           # {"status":"UP"}
curl http://localhost:8081/actuator/health                           # {"status":"UP"}
curl -s -o /dev/null -w "%{http_code}" http://localhost:8082/admin-struts/admin/login  # 200
```

**Windows:**

```bat
curl http://localhost:8080/actuator/health
curl http://localhost:8081/actuator/health
curl -s -o nul -w "%%{http_code}" http://localhost:8082/admin-struts/admin/login
```

### 5.2 ログイン動作確認

#### 5.2.1 ユーザーログイン（Mobile BFF）

```bash
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"loginId": "user001", "password": "password123"}'
```

**Windows（コマンドプロンプト）:**

```bat
curl -X POST http://localhost:8081/api/v1/auth/login -H "Content-Type: application/json" -d "{\"loginId\": \"user001\", \"password\": \"password123\"}"
```

> Windows コマンドプロンプトでは JSON 内のダブルクォートを `\"` でエスケープし、外側を `"` で囲みます。  
> PowerShell の場合は `-d '{\"loginId\": \"user001\", \"password\": \"password123\"}'` のようにシングルクォートを使用します。

#### 5.2.2 管理者ログイン（Struts2 Admin API）

```bash
curl -X POST http://localhost:8082/admin-struts/api/v1/auth/admin-login \
  -H "Content-Type: application/json" \
  -d '{"loginId": "admin001", "password": "admin123"}'
```

**Windows:**

```bat
curl -X POST http://localhost:8082/admin-struts/api/v1/auth/admin-login -H "Content-Type: application/json" -d "{\"loginId\": \"admin001\", \"password\": \"admin123\"}"
```

### 5.3 管理画面の動作確認

1. ブラウザで http://localhost:8082/admin-struts/admin/login を開く
2. ログイン画面が表示される
3. 以下の情報でログイン:
   - ログインID: `admin001`
   - パスワード: `admin123`
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
   # Linux / macOS
   ls -la src/web-api/data/mobile_app.db
   # ファイルが存在しない場合
   ./scripts/reset-database.sh
   ```

   ```bat
   REM Windows
   dir src\web-api\data\mobile_app.db
   REM ファイルが存在しない場合
   scripts\reset-database.bat
   ```

2. **sqlite3 コマンドが見つからない**

   ```bash
   # macOS
   brew install sqlite3

   # Ubuntu/Debian
   sudo apt-get install sqlite3
   ```

   ```bat
   REM Windows（winget）
   winget install SQLite.SQLite

   REM Windows（scoop）
   scoop install sqlite
   ```

---

### 6.2 Web APIが起動しない

**症状**: `Port 8080 was already in use` または ビルドエラー

**原因と解決方法**:

1. **ポート8080が使用中**

   ```bash
   # Linux / macOS
   lsof -i :8080
   kill -9 <PID>
   ```

   ```bat
   REM Windows
   netstat -ano | findstr :8080
   taskkill /PID <PID> /F
   ```

2. **Javaバージョンが古い**
   ```bash
   java -version
   # Web API / Mobile BFF は Java 17 以上が必要
   ```

---

### 6.3 Struts2 管理画面が起動しない

**症状**: `mvn tomcat7:run` でエラー、またはポート 8082 に接続できない

**原因と解決方法**:

1. **ポート8082が使用中**

   ```bash
   # Linux / macOS
   lsof -i :8082
   kill -9 <PID>
   ```

   ```bat
   REM Windows
   netstat -ano | findstr :8082
   taskkill /PID <PID> /F
   ```

2. **Maven がインストールされていない**
   ```bash
   mvn -v
   # Maven 3.x 以上が必要
   ```

3. **ビルドエラー**
   ```bash
   cd src/admin-struts
   mvn clean package -DskipTests
   # エラーメッセージを確認
   ```

4. **ログを確認**

   ```bash
   # Linux / macOS
   tail -f logs/admin-struts.log
   ```

   ```bat
   REM Windows
   type logs\admin-struts.log
   ```

---

### 6.4 ログインできない

**症状**: ログイン画面でエラー、401 Unauthorized

**原因と解決方法**:

1. **初期データが投入されていない**
   ```bash
   sqlite3 src/web-api/data/mobile_app.db "SELECT login_id, user_type FROM users;"
   # データがない場合はリセット
   ./scripts/reset-database.sh   # Linux / macOS
   scripts\reset-database.bat    # Windows
   ```

2. **管理画面には admin ユーザーでログインが必要**
   - ログインID: `admin001`
   - パスワード: `admin123`
   - 一般ユーザー (`user001` 等) では Web UI にログインできません

---

### 6.5 一般的なエラーとログ確認

**Linux / macOS:**

| エラー原因 | ログ確認コマンド |
|-----------|--------------|
| Web API | `tail -f logs/web-api.log` |
| Mobile BFF | `tail -f logs/mobile-bff.log` |
| Struts2 Admin | `tail -f logs/admin-struts.log` |
| SQLite | `sqlite3 src/web-api/data/mobile_app.db "PRAGMA integrity_check;"` |

**Windows:**

| エラー原因 | ログ確認コマンド |
|-----------|--------------|
| Web API | `type logs\web-api.log` |
| Mobile BFF | `type logs\mobile-bff.log` |
| Struts2 Admin | `type logs\admin-struts.log` |
| SQLite | `sqlite3 src\web-api\data\mobile_app.db "PRAGMA integrity_check;"` |

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

**Linux / macOS:**

```bash
./scripts/reset-database.sh
```

**Windows:**

```bat
scripts\reset-database.bat
```

### 8.2 すべてのサービスを停止

**Linux / macOS:**

```bash
./scripts/stop-all-services.sh
```

**Windows:**

```bat
scripts\stop-all-services.bat
```

### 8.3 停止してからリセットして再起動

**Linux / macOS:**

```bash
./scripts/stop-all-services.sh
./scripts/reset-database.sh
./scripts/start-all-services.sh
```

**Windows:**

```bat
scripts\stop-all-services.bat
scripts\reset-database.bat
scripts\start-all-services.bat
```
---

## 9. スクリプト一覧

| スクリプト | Linux / macOS | Windows |
|-----------|---------------|---------|
| 全サービス起動 | `./scripts/start-all-services.sh` | `scripts\start-all-services.bat` |
| 全サービス停止 | `./scripts/stop-all-services.sh` | `scripts\stop-all-services.bat` |
| admin-struts 起動 | `./scripts/start-admin-struts.sh` | `scripts\start-admin-struts.bat` |
| admin-struts 停止 | `./scripts/stop-admin-struts.sh` | `scripts\stop-admin-struts.bat` |
| DB リセット | `./scripts/reset-database.sh` | `scripts\reset-database.bat` |

---

## 10. よくある質問（FAQ）

### Q1: ポートを変更したい

**A**: admin-struts のポートは `src/admin-struts/pom.xml` の `tomcat7-maven-plugin` 設定で変更できます。
Web API / Mobile BFF は `application.properties` で変更できます。

### Q2: Windows で curl が使えない

**A**: Git for Windows をインストールすると Git Bash に curl が含まれます。
または、PowerShell の `Invoke-WebRequest` コマンドレットを使用できます：

```powershell
Invoke-WebRequest -Uri http://localhost:8082/admin-struts/admin/login -Method GET
```

### Q3: モバイルアプリはどうやって動かす？

**A**: モバイルアプリは Mobile BFF に接続します：

- iOS: Xcode + Swift (`src/ios-app/`)
- Android: Android Studio + Java (`src/android-app/`)
- Windows: Visual Studio + C++ (`src/windows-app/`)
- API URL: Mobile BFF (`http://localhost:8081`)

### Q4: テストデータを追加したい

**A**: `src/database/seeds/02_seed_data.sql` を編集して DB をリセットしてください。

```bash
# Linux / macOS
./scripts/reset-database.sh

# Windows
scripts\reset-database.bat
```
---

## 11. サポート

問題が発生した場合:

1. まず[トラブルシューティング](#6-トラブルシューティング)を確認
2. ログファイルを確認（`logs/` ディレクトリ）
3. Issueを作成（GitHub）

---

**End of Document**
