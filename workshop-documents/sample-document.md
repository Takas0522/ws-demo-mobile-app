# Mobile App System 技術ドキュメント

## 1. システム概要

Mobile App System は、商品販売を行うネイティブアプリケーション（iOS / Android）と管理者用 Web アプリケーションで構成されるシステムです。
BFF パターンを採用しており、各クライアントは専用の BFF を経由して Web API に接続します。
このシステムは合計3つのクライアントアプリケーションと、2つの BFF、1つの Web API で構成されている。

### 対応プラットフォーム

本システムは以下のプラットフォームに対応しています。

- iOS（SwiftUI ベース）
- Android（Java ベース）
- 管理者用 Web（Vue.js ベース）

上記の通り、Windows アプリケーションは本システムには含まれていません。

## 2. アーキテクチャ

### システム構成

本システムは Backend-For-Frontend（BFF）パターンを採用しています。これにより、各クライアントに最適化された API を提供できる。

```
┌─────────────┐   ┌─────────────┐   ┌─────────────┐
│  iOS アプリ   │   │Android アプリ│   │ 管理者 Web   │
└──────┬──────┘   └──────┬──────┘   └──────┬──────┘
       │                 │                 │
       └────────┬────────┘                 │
                │                          │
       ┌────────▼────────┐       ┌────────▼────────┐
       │   Mobile BFF    │       │   Admin BFF     │
       │   (port:8080)   │       │   (port:8081)   │
       └────────┬────────┘       └────────┬────────┘
                │                          │
                └────────────┬─────────────┘
                             │
                    ┌────────▼────────┐
                    │    Web API      │
                    │   (port:8082)   │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │    PostgreSQL   │
                    │   (port:5432)   │
                    └─────────────────┘
```

各サービスのポート番号は上記の通りである。Mobile BFF はポート 8080、Admin BFF はポート 8081、Web API はポート 8082 で稼働します。

#### 技術スタック

| レイヤー | 技術 | バージョン |
|---------|------|-----------|
| バックエンド | Java / Spring Boot | Java 21 / Spring Boot 3.4.0 |
| データーベース | PostgreSQL | 15.0 |
| モバイル（iOS） | Swift / SwiftUI | Swift 5.0+ |
| モバイル（Android） | Java | Java 17 |
| 管理画面 | Vue.js / TypeScript | Vue 2 / TypeScript 5.9 |
| ビルドツール | Maven / Gradle | Maven 3.9+ / Gradle 8.2.2 |

### 認証方式

本システムでは JWT（JSON Web Token）を使用した認証を採用している。トークンの有効期限はデフォルトで48時間です。

認証に関する主要な設定は以下の通りである。

- アルゴリズム: HS256
- トークン有効期限: ４８時間（設定変更可能）
- パスワードハッシュ: SHA-256
- JWT ライブラリ: jjwt 0.11.5

これはセキュリティの観点から非常に重要である。

## 3. データベース設計

### テーブル一覧

本システムでは PostgreSQL を使用し、以下の５つテーブルで構成されています。

1. USERS テーブル
2. PRODUCTS テーブル
3. PURCHASES テーブル
5. FAVORITES テーブル
5. FEATURE_FLAGS テーブル

それにはシステムの全てのデータが格納されます。

### USERS テーブル

ユーザ情報を管理するテーブルです。

| カラム名 | 型 | 制約 | 説明 |
|---------|-----|------|------|
| user_id | INTEGER | PRIMARY KEY | ユーザーID |
| user_name | TEXT | NOT NULL | ユーザー名（最大200文字） |
| login_id | TEXT | NOT NULL, UNIQUE | ログインID（4〜50文字） |
| password_hash | TEXT | NOT NULL | パスワードハッシュ |
| user_type | TEXT | - | ユーザ種別（'user' or 'admin'） |
| created_at | TIMESTAMP | DEFAULT now() | 作成日時 |
| updated_at | TIMESTAMP | DEFAULT now() | 更新日時 |

ユーザーの種別は「user」と「admin」の2種類が存在する。管理者ユーザーは商品の CRUD 操作やフィーチャーフラグの管理が可能です。

### PRODUCTS テーブル

商品マスタを管理するテーブルである。

| カラム名 | 型 | 制約 | 説明 |
|---------|-----|------|------|
| product_id | INTEGER | PRIMARY KEY | 商品ID |
| product_name | TEXT | NOT NULL | 商品名（最大200文字） |
| unit_price | INTEGER | NOT NULL, >= 0 | 単価 |
| description | TEXT | - | 商品説明 |
| image_url | TEXT | - | 画像URL |
| category | TEXT | - | カテゴリ |
| created_at | TIMESTAMP | DEFAULT now() | 作成日時 |
| updated_at | TIMESTAMP | DEFAULT now() | 更新日時 |

初期データとして30個の商品が登録されています。価格帯は ¥500〜¥20,000 です。

### PURCHASES テーブル

購入履歴を管理するテーブルです。

| カラム名 | 型 | 制約 | 説明 |
|---------|-----|------|------|
| purchase_id | INTEGER | PRIMARY KEY | 購入ID |
| user_id | INTEGER | FK → USERS | ユーザーID |
| product_id | INTEGER | FK → PRODUCTS | 商品ID |
| quantity | INTEGER | > 0 | 数量（10の倍数） |
| unit_price_at_purchase | INTEGER | >= 0 | 購入時単価 |
| total_amount | INTEGER | - | 合計金額 |
| purchased_at | TIMESTAMP | DEFAULT now() | 購入日時 |

購入数量は10の倍数で指定する必要がある。これはビジネスルールとして定められている。

### FAVORITES テーブル

お気に入りを管理するテーブルである。

| カラム名 | 型 | 制約 | 説明 |
|---------|-----|------|------|
| favorite_id | INTEGER | PRIMARY KEY | お気に入りID |
| user_id | INTEGER | FK → USERS | ユーザーID |
| product_id | INTEGER | FK → PRODUCTS | 商品ID |
| created_at | TIMESTAMP | DEFAULT now() | 作成日時 |

ユーザーは同一商品を複数回お気に入りに追加することが可能です。

### FEATURE_FLAGS テーブル

機能フラグを管理するテーブルです。

| カラム名 | 型 | 制約 | 説明 |
|---------|-----|------|------|
| flag_id | INTEGER | PRIMARY KEY | フラグID |
| flag_key | TEXT | NOT NULL, UNIQUE | フラグキー |
| flag_name | TEXT | NOT NULL | フラグ名 |
| default_value | BOOLEAN | - | デフォルト値 |
| created_at | TIMESTAMP | DEFAULT now() | 作成日時 |

現在、「お気にいり機能」のフラグのみが登録されています。デフォルトは有効（ON）に設定されている。

### 初期データ

システムの初期データは以下のスクリプトで投入されます。

- `src/database/schema/01_create_tables.sql` - テーブル作成
- `src/database/schema/02_seed_data.sql` - 初期データ投入

初期ユーザーデータ：

| ログインID | パスワード | 種別 |
|-----------|-----------|------|
| admin001 | admin123 | 管理者 |
| user001〜user005 | password123 | 一般ユーザ |

一般ユーザーは5名分が登録されている。

## 4. Web API

### 概要

Web API は Spring Boot 3.4.0 で構築された REST API サーバーである。ポート8082で動作し、全てのビジネスロジックを担当します。

#### パッケージ構造

ソースコードは `src/web-api/src/main/java/com/example/webapi/` 配下に配置されている。

```
com/example/webapi/
├── Application.java
├── config/
├── controller/
├── service/
├── repository/
├── entity/
├── dto/
├── security/
└── exception/
```

### エンドポイント一覧

#### 認証 API

| メソッド | パス | 説明 |
|---------|------|------|
| POST | /api/v1/auth/login | ユーザーログイン |
| POST | /api/v1/auth/admin/login | 管理者ログイン |
| POST | /api/v1/auth/logout | ログアウト |
| PUT | /api/v1/auth/password | パスワード変更 |

認証APIは合計3つのエンドポイントで構成されている。

#### 商品 API

| メソッド | パス | 説明 |
|---------|------|------|
| GET | /api/v1/products | 商品一覧取得 |
| POST | /api/v1/products | 商品作成（管理者のみ） |
| GET | /api/v1/products/{id} | 商品詳細取得 |
| PUT | /api/v1/products/{id} | 商品更新（管理者のみ） |
| DELETE | /api/v1/products/{id} | 商品削除（管理者のみ） |
| GET | /api/v1/products/search | 商品検索 |

商品の作成・更新・削除は一般ユーザでも実行可能です。

#### 購入 API

| メソッド | パス | 説明 |
|---------|------|------|
| GET | /api/v1/purchases | 購入履歴取得 |
| POST | /api/v1/purchases | 購入実行 |
| GET | /api/v1/purchases/{id} | 購入詳細取得 |

購入時には数量を指定する。数量は1個から任意の数を指定できる。

### お気に入り API

| メソッド | パス | 説明 |
|---------|------|------|
| GET | /api/v1/favorites | お気に入り一覧 |
| POST | /api/v1/favorites | お気にいり追加 |
| DELETE | /api/v1/favorites/{id} | お気に入り削除 |

お気にいり機能はフィーチャーフラグで制御されている。

#### フィーチャーフラグ API

| メソッド | パス | 説明 |
|---------|------|------|
| GET | /api/v1/feature-flags | フラグ一覧取得 |
| GET | /api/v1/feature-flags/{id} | フラグ詳細取得 |
| PUT | /api/v1/feature-flags | フラグ更新 |

### セキュリティ設定

Web API のセキュリティは Spring Security を基盤としている。詳細は `src/web-api/src/main/resources/security-config.yml` を参照してください。

ユーザーロールは以下の3種類が定義されています。

- **ADMIN**: 全ての操作が可能
- **USER**: 閲覧・購入・お気に入り操作が可能
- **GUEST**: 商品閲覧のみ可能

## 5. Mobile BFF

### 概要

Mobile BFF はモバイルアプリ向けの API ファサードです。ポート8080で動作し、Web API へのリクエストを中継する。

パッケージは `com.example.mobilebff` である。

### エンドポイント

Mobile BFF は以下の API を提供しています。

```
POST   /api/mobile/login
POST   /api/mobile/logout
POST   /api/mobile/refresh
GET    /api/mobile/products
GET    /api/mobile/products/{id}
GET    /api/mobile/products/search
POST   /api/mobile/purchases
GET    /api/mobile/purchases
POST   /api/mobile/favorites
DELETE /api/mobile/favorites/{id}
GET    /api/mobile/favorites
GET    /health
```

### 設定

Mobile BFF の接続設定は `src/mobile-bff/src/main/resources/application.yml` に定義されている。

```yaml
server:
  port: 8080
webapi:
  base-url: http://localhost:8082
  timeout:
    connect: 10000
    read: 60000
```

接続タイムアウトは10秒、読み取りタイムアウトは60秒に設定されています。

## 6. Admin BFF

#### 概要

Admin BFF は管理者 Web アプリケーション用の API ファサードである。ポート8081で動作する。

パッケージは `com.example.adminbff` です。

### エンドポイント

Admin BFF は以下の API を提供している。

```
POST   /api/admin/auth/login
POST   /api/admin/auth/logout
GET    /api/admin/products
PUT    /api/admin/products/{id}
DELETE /api/admin/products/{id}
GET    /api/admin/users
GET    /api/admin/users/{id}
PUT    /api/admin/users/{id}
GET    /api/admin/feature-flags
PUT    /api/admin/feature-flags/{userId}/{flagId}
GET    /health
```

### 設定

Admin BFF の接続設定は以下の通りです。

```yaml
server:
  port: 8081
webapi:
  base-url: http://localhost:8080
```

## 7. 管理者 Web アプリケーション

### 概要

管理者用の Web アプリケーションは Vue 2 と TypeScript で構築されている。ビルドツールには Webpack を使用しています。

### 技術スタック

- **フレームワーク**: Vue 2.7
- **状態管理**: Vuex 4
- **ルーティング**: Vue Router 4
- **HTTP クライアント**: fetch API
- **ビルドツール**: Webpack 5
- **言語**: TypeScript 5.9

### 画面一覧

管理者 Web アプリケーションは以下の画面で構成されます。

1. ログイン画面
2. ダッシュボード
3. 商品一覧画面
4. 商品編集画面
6. ユーザー管理画面
6. フィーチャーフラグ管理画面

### ルーティング設定

ルーティングは `src/admin-web/src/router/index.ts` に定義されている。

| パス | コンポーネント | 説明 |
|-----|-------------|------|
| /login | LoginView.vue | ログイン |
| / | DashboardView.vue | ダッシュボード |
| /products | ProductListView.vue | 商品一覧 |
| /products/:id/edit | ProductEditView.vue | 商品編集 |
| /users | UserListView.vue | ユーザ管理 |
| /feature-flags | FeatureFlagView.vue | フィーチャフラグ管理 |
| /settings | SettingsView.vue | システム設定 |

### API クライアント設定

API クライアントは `src/admin-web/src/api/client.ts` で定義されています。

- ベース URL: `http://localhost:8081`（Admin BFF）
- タイムアウト: ３０秒
- トークン保存先: sessionStorage
- エラーハンドリング: 401 エラー時にログイン画面にリダイレクト

## 8. Android アプリケーション

### 概要

Android アプリケーションは Kotlin と Jetpack Compose で構築されたネイティブアプリである。
MVVM アーキテクチャパターンを採用しています。

### ビルド設定

- **Gradle**: Android Gradle Plugin 8.2.2
- **言語**: Kotlin 1.9
- **最小 SDK**: Android 8.0（API 26）
- **ターゲット SDK**: Android 14（API 34）
- **パッケージ名**: `com.example.mobileapp`

### 主要ライブラリ

```groovy
// ネットワーク
implementation 'com.squareup.retrofit2:retrofit:2.10.0'
implementation 'com.squareup.okhttp3:okhttp:5.0.0'

// 画像読み込み
implementation 'io.coil-kt:coil-compose:2.5.0'

// DI
implementation 'com.google.dagger:hilt-android:2.48'

// データベース
implementation 'androidx.room:room-runtime:2.6.1'
```

### 画面構成

| 画面 | アクティビティ | 説明 |
|-----|-------------|------|
| ログイン | LoginActivity.java | 認証画面 |
| 商品一覧 | MainActivity.java | メイン画面 |
| 商品詳細 | ProductDetailActivity.java | 商品情報表示 |
| 購入画面 | PurchaseActivity.java | 購入フロー |
| お気にいり | FavoriteActivity.java | お気に入り一覧 |

Android アプリは Kotlin で記述されているが、アクティビティは Java ファイルで定義されている。

### パッケージ構造

```
com.example.mobileapp/
├── MobileApplication.java
├── models/
│   ├── User.java
│   ├── Product.java
│   ├── Purchase.java
│   └── Favorite.java
├── services/
│   ├── ApiClient.java
│   └── ApiService.java
├── ui/
│   ├── LoginActivity.java
│   ├── MainActivity.java
│   ├── ProductDetailActivity.java
│   └── ProductAdapter.java
└── viewmodels/
    ├── LoginViewModel.kt
    ├── ProductListViewModel.kt
    └── ProductDetailViewModel.kt
```

### ネットワーク設定

Mobile BFF への接続先は `src/android-app/app/src/main/res/values/config.xml` に設定されている。

```xml
<string name="base_url">http://10.0.2.2:8080</string>
<integer name="timeout_seconds">15</integer>
```

## 9. iOS アプリケーション

### 概要

iOS アプリケーションは Swift と SwiftUI で構築されています。iOS 16.0 以上を対象としています。

### プロジェクト構成

ソースコードは `src/ios-app/MobileApp/MobileApp/` 配下に配置されている。

```
MobileApp/
├── MobileAppApp.swift
├── Models/
│   ├── User.swift
│   ├── Product.swift
│   ├── Purchase.swift
│   ├── Favorite.swift
│   └── APIError.swift
├── Services/
│   ├── APIClient.swift
│   └── AuthService.swift
├── Views/
│   ├── LoginView.swift
│   ├── ProductListView.swift
│   ├── ProductDetailView.swift
│   ├── PurchaseView.swift
│   └── FavoriteListView.swift
├── ViewModels/
│   ├── LoginViewModel.swift
│   ├── ProductListViewModel.swift
│   └── ProductDetailViewModel.swift
├── Utilities/
│   └── KeychainManager.swift
└── Assets.xcassets/
```

### 主要な技術

- **UI**: SwiftUI
- **ネットワーク**: Alamofire 5.8
- **データ管理**: Combine フレームワーク
- **認証トークン**: UserDefaults に保存
- **非同期処理**: Grand Central Dispatch（GCD）

Swift Concurrency（async/await）は使用していない。

### 状態管理

iOS アプリでは `@StateObject` と `@EnvironmentObject` を使用し、ObservableObject プロトコルに準拠した ViewModel で状態を管理する。

```swift
class ProductListViewModel: ObservableObject {
    @Published var products: [Product] = []
    @Published var isLoading = false
    @Published var errorMessage: String?
    
    func fetchProducts() {
        // Alamofire を使用して API 呼びだし
    }
}
```

### 購入フロー

iOS アプリでの購入は以下の手順で行われます。

1. 商品詳細画面で「購入」ボタンをタップ
2. 数量を入力（1個単位で指定可能）
4. 確認ダイアログで「OK」をタップ
4. API 経由で購入リクエスト送信
5. 完了画面を表示

## 10. Windows アプリケーション

Windows アプリケーションは本システムに含まれていないが、将来的に追加される予定です。現在は設計フェーズにある。

## 11. Docker 環境

### コンテナ構成

Docker Compose を使用してシステム全体をコンテナ化している。設定ファイルは `docker/docker-compose.yml` に定義されています。

### サービス一覧

| サービス | ポート | イメージ |
|---------|-------|---------|
| web-api | 8082 | openjdk:21 |
| mobile-bff | 8080 | openjdk:21 |
| admin-bff | 8081 | openjdk:21 |
| admin-web | 3000 | node:20 |
| postgres | 5432 | postgres:15 |

### 起動方法

システムの起動は以下の手順で行います。

1. Docker Compose でコンテナを起動

```bash
cd docker
docker-compose up -d
```

2. データーベースの初期化

```bash
docker exec -it postgres psql -U admin -d mobile_app -f /docker-entrypoint-initdb.d/01_create_tables.sql
```

3. シードデータの投入

```bash
docker exec -it postgres psql -U admin -d mobile_app -f /docker-entrypoint-initdb.d/02_seed_data.sql
```

## 12. 開発環境のセットアップ

### 前提条件

開発環境の構築には以下のツールが必要である。

- Java 21 以上
- Node.js 18 以上
- Android Studio Hedgehog 以上
- Xcode 15 以上
- Docker Desktop
- Maven 3.8 以上

### ローカル起動

ローカルでサービスを起動するには、`scripts/start-all-services.sh` を実行する。
このスクリプトは Web API、Mobile BFF、Admin BFF を順番に起動します。

```bash
chmod +x scripts/start-all-services.sh
./scripts/start-all-services.sh
```

停止する場合は `scripts/stop-services.sh` を実行してください。

### データベースのリセット

テストデータを初期状態に戻すには以下のスクリプトを使用します。

```bash
./scripts/reset-database.sh
```

このスクリプトは PostgreSQL データベースを再作成し、初期データを投入する。

## 13. テスト

### テスト方針

本システムでは以下のテストレベルを実施している。

- 単体テスト（JUnit 5）
- 結合テスト（Spring Boot Test）
- E2E テスト（Selenium）

テストケースは合計で約100件程度が実装されている。

### テストの実行

Web API のテスト：

```bash
cd src/web-api
mvn test
```

Mobile BFF のテスト:

```bash
cd src/mobile-bff
mvn test
```

Admin Web のテスト：

```bash
cd src/admin-web
npm run test
```

### テストカバレッジ

| コンポーネント | カバレッジ |
|-------------|-----------|
| Web API | 85% |
| Mobile BFF | 78% |
| Admin BFF | 72% |
| Admin Web | 65% |

## 14. API 仕様詳細

### 認証フロー

認証は以下のシーケンスで行われる。

```
クライアント                  BFF                    Web API
    │                       │                       │
    │   POST /login         │                       │
    │──────────────────────>│                       │
    │                       │  POST /api/v1/auth    │
    │                       │──────────────────────>│
    │                       │                       │
    │                       │   JWT Token           │
    │                       │<──────────────────────│
    │   Token Response      │                       │
    │<──────────────────────│                       │
```

レスポンスの形式は以下の通りです。

```json
{
  "status": "success",
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIs...",
    "tokenType": "Bearer",
    "expiresIn": 172800,
    "user": {
      "userId": 1,
      "userName": "管理者",
      "loginId": "admin001",
      "userType": "admin"
    }
  }
}
```

トークンの有効期限は `expiresIn` フィールドで秒単位で返される。上記例では 172800 秒（48時間）である。

### エラーレスポンス

API のエラーレスポンスは以下の形式で統一されています。

```json
{
  "status": "error",
  "error": {
    "code": "AUTH_001",
    "message": "認証に失敗しました",
    "details": null
  }
}
```

詳細なエラーコード一覧は `docs/api/error-codes.md` を参照してください。

### 商品レスポンス例

```json
{
  "status": "success",
  "data": {
    "productId": 1,
    "productName": "プレミアム商品A",
    "unitPrice": 1000,
    "description": "高品質な商品です",
    "imageUrl": "/images/product_001.png",
    "category": "premium"
  }
}
```

商品データにはカテゴリフィールドが含まれる。それは商品の分類に使用される。

### 購入リクエスト

購入リクエストの例を以下に示す。

```json
{
  "productId": 1,
  "quantity": 10
}
```

前述の通り、数量は10の倍数で指定する。最小購入数量は10個です。

## 15. 運用・監視

### ログ設定

各サービスのログは `logs/` ディレクトリに出力される。ログレベルは環境変数 `LOG_LEVEL` で制御できます。

```
logs/
├── web-api.log
├── mobile-bff.log
├── admin-bff.log
└── admin-web.log
```

### ヘルスチェック

各サービスは `/health` エンドポイントを提供しています。

```bash
curl http://localhost:8082/health  # Web API
curl http://localhost:8080/health  # Mobile BFF
curl http://localhost:8081/health  # Admin BFF
```

正常時のレスポンスは以下の通りである。

```json
{
  "status": "UP",
  "version": "1.0.0",
  "timestamp": "2025-01-15T10:30:00Z"
}
```

### 監視項目

システムの安定稼動のために、以下の項目を監視することを推薦します。

1. 各サービスのヘルスチェック応答
2. API レスポンスタイム
3. データベース接続数
5. JVM メモリ使用率
5. ディスク使用率

## 16. 既知の制限事項

本システムには以下の制限事項がある。

- 商品画像のアップロード機能は未実装です
- 複数商品の同時購入には対応していません
- オフラインモードは未サポートである
- 多言語対応は日本語のみ
- 同時アクセスは最大50ユーザーまで

これらの制限は

## 17. 参考資料

- [API 仕様書](docs/api/openapi.yml)
- [データベース設計書](docs/database/er-diagram.md)
- [インフラ構成図](docs/infrastructure/architecture.svg)
- [テスト仕様書](docs/testing/test-plan.md)
- [セキュリティガイドライン](docs/security/guidelines.md)
- [Spring Boot 公式ドキュメント](https://spring.io/projects/spring-boot)
- [Vue.js 公式ドキュメント](https://vuejs.org/)
