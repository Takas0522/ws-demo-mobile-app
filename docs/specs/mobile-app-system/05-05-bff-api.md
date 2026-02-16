# mobile-app-system - BFF API仕様

> 最終更新: 2025-01-08
> ステータス: Draft
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-08 | 初版作成 | AI Agent |

---

## 1. BFF API概要

本ドキュメントでは、mobile-app-systemのBFF（Backend For Frontend）API仕様を定義します。

### 1.1 BFFとは

BFF（Backend For Frontend）は、フロントエンド専用のバックエンドAPIです。
各クライアント（モバイルアプリ、管理Webアプリ）に最適化されたAPIを提供し、Web APIを中継します。

### 1.2 BFFの役割

- Web APIへのリクエスト中継
- エラーハンドリング
- レスポンスのフォーマット調整（必要に応じて）
- クライアント固有のロジック実装

### 1.3 アーキテクチャ

```mermaid
graph LR
    Mobile[モバイルアプリ] -->|HTTPS| MobileBFF[Mobile BFF]
    AdminWeb[管理Web] -->|HTTPS| AdminBFF[Admin BFF]
    MobileBFF -->|JWT| WebAPI[Web API]
    AdminBFF -->|JWT| WebAPI
    WebAPI -->|SQL| DB[(PostgreSQL)]
```

---

## 2. Mobile BFF API

### 2.1 概要

Mobile BFFは、モバイルアプリ専用のAPIで、Web APIを呼び出してレスポンスを返します。

### 2.2 ベースURL

| 環境 | Mobile BFF |
|------|-----------|
| 開発 | `http://localhost:8081` |
| 本番 | TBD |

### 2.3 エンドポイントマッピング

| Mobile BFF | Web API | 説明 |
|-----------|---------|------|
| `POST /api/mobile/login` | `POST /api/v1/auth/login` | ユーザーログイン |
| `GET /api/mobile/products` | `GET /api/v1/products` | 商品一覧取得 |
| `GET /api/mobile/products/search` | `GET /api/v1/products/search` | 商品検索 |
| `GET /api/mobile/products/{id}` | `GET /api/v1/products/{id}` | 商品詳細取得 |
| `POST /api/mobile/purchases` | `POST /api/v1/purchases` | 商品購入 |
| `GET /api/mobile/purchases` | `GET /api/v1/purchases` | 購入履歴取得 |
| `POST /api/mobile/favorites` | `POST /api/v1/favorites` | お気に入り登録 |
| `DELETE /api/mobile/favorites/{id}` | `DELETE /api/v1/favorites/{id}` | お気に入り解除 |
| `GET /api/mobile/favorites` | `GET /api/v1/favorites` | お気に入り一覧取得 |
| `GET /api/mobile/feature-flags` | `GET /api/v1/feature-flags` | 機能フラグ取得 |

### 2.4 認証

- ログインAPI以外は、AuthorizationヘッダーにJWTトークンが必要
- Mobile BFFがトークンをそのままWeb APIに転送

```http
Authorization: Bearer {JWT_TOKEN}
```

### 2.5 レスポンス形式

Web APIと同じレスポンス形式を返却します。

**成功時**:
```json
{
  "data": { /* データ */ },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時**:
```json
{
  "error": {
    "code": "ERROR_CODE",
    "message": "エラーメッセージ",
    "details": "詳細情報（オプション）"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

### 2.6 エラーハンドリング

Mobile BFFは以下のエラーをハンドリングします：

| エラー種別 | HTTPステータス | 対応 |
|----------|--------------|------|
| Web API接続エラー | 503 Service Unavailable | エラーレスポンス返却 |
| Web APIタイムアウト | 504 Gateway Timeout | タイムアウトエラー返却 |
| Web APIエラーレスポンス | そのまま転送 | Web APIのエラーをそのまま返却 |

**接続エラーレスポンス例**:
```json
{
  "error": {
    "code": "BFF_001",
    "message": "サービスに接続できません",
    "details": "しばらく待ってから再度お試しください"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

### 2.7 利用例

**ログインAPI呼び出し**:
```bash
curl -X POST http://localhost:8081/api/mobile/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": "password123"
  }'
```

**商品一覧取得**:
```bash
curl -X GET http://localhost:8081/api/mobile/products \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

## 3. Admin BFF API

### 3.1 概要

Admin BFFは、管理Webアプリ専用のAPIで、Web APIを呼び出してレスポンスを返します。

### 3.2 ベースURL

| 環境 | Admin BFF |
|------|-----------|
| 開発 | `http://localhost:8082` |
| 本番 | TBD |

### 3.3 エンドポイントマッピング

| Admin BFF | Web API | 説明 |
|----------|---------|------|
| `POST /api/admin/login` | `POST /api/v1/auth/admin/login` | 管理者ログイン |
| `GET /api/admin/products` | `GET /api/v1/products` | 商品一覧取得 |
| `PUT /api/admin/products/{id}` | `PUT /api/v1/products/{id}` | 商品更新 |
| `GET /api/admin/users` | `GET /api/v1/admin/users` | ユーザー一覧取得 |
| `PUT /api/admin/users/{id}/flags/{key}` | `PUT /api/v1/admin/users/{id}/feature-flags/{key}` | 機能フラグ変更 |

### 3.4 認証

- ログインAPI以外は、AuthorizationヘッダーにJWTトークン（admin権限）が必要
- Admin BFFがトークンをそのままWeb APIに転送

```http
Authorization: Bearer {ADMIN_JWT_TOKEN}
```

### 3.5 レスポンス形式

Web APIと同じレスポンス形式を返却します。

**成功時**:
```json
{
  "data": { /* データ */ },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時**:
```json
{
  "error": {
    "code": "ERROR_CODE",
    "message": "エラーメッセージ",
    "details": "詳細情報（オプション）"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

### 3.6 エラーハンドリング

Admin BFFは以下のエラーをハンドリングします：

| エラー種別 | HTTPステータス | 対応 |
|----------|--------------|------|
| Web API接続エラー | 503 Service Unavailable | エラーレスポンス返却 |
| Web APIタイムアウト | 504 Gateway Timeout | タイムアウトエラー返却 |
| Web APIエラーレスポンス | そのまま転送 | Web APIのエラーをそのまま返却 |
| 権限不足 | 403 Forbidden | 権限エラー返却 |

### 3.7 利用例

**管理者ログインAPI呼び出し**:
```bash
curl -X POST http://localhost:8082/api/admin/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "admin001",
    "password": "adminpass123"
  }'
```

**商品更新**:
```bash
curl -X PUT http://localhost:8082/api/admin/products/1 \
  -H "Authorization: Bearer YOUR_ADMIN_JWT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "productName": "商品A（改訂版）",
    "unitPrice": 1200
  }'
```

---

## 4. BFF共通仕様

### 4.1 HTTPステータスコード

| ステータスコード | 意味 | 使用ケース |
|---------------|------|-----------|
| 200 OK | 成功 | GET, PUT, DELETE 成功 |
| 201 Created | 作成成功 | POST 成功 |
| 400 Bad Request | リクエスト不正 | バリデーションエラー |
| 401 Unauthorized | 認証失敗 | トークンなし、不正、期限切れ |
| 403 Forbidden | 権限不足 | 権限のないAPIへのアクセス |
| 404 Not Found | リソース不存在 | 存在しないID指定 |
| 500 Internal Server Error | サーバーエラー | 予期しないエラー |
| 503 Service Unavailable | サービス利用不可 | Web API接続エラー |
| 504 Gateway Timeout | タイムアウト | Web APIタイムアウト |

### 4.2 タイムアウト設定

| BFF | Web APIへのタイムアウト |
|-----|----------------------|
| Mobile BFF | 10秒 |
| Admin BFF | 10秒 |

### 4.3 ログ出力

BFFは以下の情報をログ出力します：

- リクエスト受信ログ
  - タイムスタンプ
  - エンドポイント
  - メソッド
  - クライアントIP
- Web APIリクエストログ
  - タイムスタンプ
  - エンドポイント
  - メソッド
  - ステータスコード
  - レスポンス時間
- エラーログ
  - タイムスタンプ
  - エラー内容
  - スタックトレース

### 4.4 CORS設定

#### Mobile BFF
- 開発環境: `*`（全許可）
- 本番環境: モバイルアプリからのリクエストのみ許可

#### Admin BFF
- 開発環境: `http://localhost:3000`（管理Webアプリ）
- 本番環境: 管理Webアプリのドメインのみ許可

---

## 5. BFF実装ガイドライン

### 5.1 責務

BFFは以下の責務を持ちます：

1. **リクエスト中継**: Web APIへのリクエストを中継
2. **エラーハンドリング**: Web APIのエラーを適切にハンドリング
3. **ログ出力**: リクエスト/レスポンスのログ出力
4. **タイムアウト管理**: Web APIへのタイムアウト管理

### 5.2 BFFに実装しないこと

以下はBFFに実装せず、Web APIに実装します：

- ビジネスロジック
- データベースアクセス
- JWT検証（Web APIで実施）
- 権限チェック（Web APIで実施）

### 5.3 実装技術

**推奨技術スタック**:
- Node.js + Express または Go + Gin
- HTTPクライアント: axios（Node.js）、http.Client（Go）
- ログ: winston（Node.js）、zap（Go）

---

## 6. API一覧サマリー

### 6.1 Mobile BFF API

| エンドポイント | メソッド | 説明 |
|--------------|---------|------|
| /api/mobile/login | POST | ユーザーログイン |
| /api/mobile/products | GET | 商品一覧取得 |
| /api/mobile/products/search | GET | 商品検索 |
| /api/mobile/products/{id} | GET | 商品詳細取得 |
| /api/mobile/purchases | POST | 商品購入 |
| /api/mobile/purchases | GET | 購入履歴取得 |
| /api/mobile/favorites | POST | お気に入り登録 |
| /api/mobile/favorites/{id} | DELETE | お気に入り解除 |
| /api/mobile/favorites | GET | お気に入り一覧取得 |
| /api/mobile/feature-flags | GET | 機能フラグ取得 |

**合計**: 10エンドポイント

### 6.2 Admin BFF API

| エンドポイント | メソッド | 説明 |
|--------------|---------|------|
| /api/admin/login | POST | 管理者ログイン |
| /api/admin/products | GET | 商品一覧取得 |
| /api/admin/products/{id} | PUT | 商品更新 |
| /api/admin/users | GET | ユーザー一覧取得 |
| /api/admin/users/{id}/flags/{key} | PUT | 機能フラグ変更 |

**合計**: 5エンドポイント

---

**End of Document**
