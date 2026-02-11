# mobile-app-system - 購入・お気に入りAPI仕様

> 最終更新: 2025-01-08
> ステータス: Draft
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-08 | 初版作成 | AI Agent |

---

## 1. 購入・お気に入りAPI概要

本ドキュメントでは、mobile-app-systemの購入とお気に入り関連API仕様を定義します。

### 1.1 対象API

**購入API**:
- API-020: 商品購入
- API-021: 購入履歴取得

**お気に入りAPI**:
- API-030: お気に入り登録
- API-031: お気に入り解除
- API-032: お気に入り一覧取得

### 1.2 共通仕様

#### 1.2.1 認証
全APIでJWT認証が必要です。

**Authorizationヘッダー**:
```http
Authorization: Bearer {JWT_TOKEN}
```

#### 1.2.2 ベースURL

| 環境 | Web API |
|------|---------|
| 開発 | `http://localhost:8080` |
| 本番 | TBD |

---

## 2. 購入API

### 2.1 商品購入

#### API-020: 商品購入

##### エンドポイント
```
POST /api/v1/purchases
```

##### 説明
商品を購入し、購入履歴を記録する。

##### 権限
- user のみ

##### リクエスト

**Body**:
```json
{
  "productId": 1,
  "quantity": 100
}
```

**パラメータ**:
| フィールド | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| productId | integer | ✅ | 存在する商品ID | 商品ID |
| quantity | integer | ✅ | 100の倍数、100-9900 | 購入個数 |

##### レスポンス

**成功時 (201 Created)**:
```json
{
  "data": {
    "purchase": {
      "purchaseId": "550e8400-e29b-41d4-a716-446655440000",
      "userId": 1,
      "productId": 1,
      "productName": "商品A",
      "quantity": 100,
      "unitPriceAtPurchase": 1000,
      "totalAmount": 100000,
      "purchasedAt": "2025-01-08T12:00:00Z"
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| purchase.purchaseId | string | 購入ID（UUID） |
| purchase.userId | integer | ユーザーID |
| purchase.productId | integer | 商品ID |
| purchase.productName | string | 商品名 |
| purchase.quantity | integer | 購入個数 |
| purchase.unitPriceAtPurchase | integer | 購入時の単価 |
| purchase.totalAmount | integer | 合計金額 |
| purchase.purchasedAt | string | 購入日時（ISO 8601） |

**エラー時 (400 Bad Request)**:
```json
{
  "error": {
    "code": "PURCHASE_001",
    "message": "購入個数は100の倍数である必要があります"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時 (404 Not Found)**:
```json
{
  "error": {
    "code": "PRODUCT_001",
    "message": "商品が見つかりません"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時 (403 Forbidden - 管理者による購入)**:
```json
{
  "error": {
    "code": "AUTH_003",
    "message": "この操作を実行する権限がありません"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

##### 処理仕様
1. JWTトークンからユーザーID取得
2. 商品IDの存在確認
3. 購入個数のバリデーション（100の倍数、100〜9900）
4. 購入時点の単価を取得
5. 合計金額を計算（単価 × 個数）
6. 購入レコードをDB保存
7. 購入IDを発行（UUID）

##### 利用例

**cURLコマンド**:
```bash
curl -X POST "http://localhost:8080/api/v1/purchases" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "productId": 1,
    "quantity": 100
  }'
```

---

### 2.2 購入履歴取得

#### API-021: 購入履歴取得

##### エンドポイント
```
GET /api/v1/purchases
```

##### 説明
ログインユーザーの購入履歴を取得する。

##### 権限
- user のみ

##### リクエスト

**Query Parameters**:
| パラメータ | 型 | 必須 | デフォルト | 説明 |
|-----------|----|----|----------|------|
| page | integer | ❌ | 1 | ページ番号 |
| limit | integer | ❌ | 20 | 1ページあたりの件数 |

**例**:
```
GET /api/v1/purchases?page=1&limit=20
```

##### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "purchases": [
      {
        "purchaseId": "550e8400-e29b-41d4-a716-446655440000",
        "productId": 1,
        "productName": "商品A",
        "quantity": 100,
        "unitPriceAtPurchase": 1000,
        "totalAmount": 100000,
        "purchasedAt": "2025-01-08T12:00:00Z"
      }
    ],
    "pagination": {
      "currentPage": 1,
      "totalPages": 3,
      "totalItems": 50,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**購入履歴なし (200 OK)**:
```json
{
  "data": {
    "purchases": [],
    "pagination": {
      "currentPage": 1,
      "totalPages": 0,
      "totalItems": 0,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

##### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/purchases?page=1&limit=20" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

## 3. お気に入りAPI

### 3.1 お気に入り登録

#### API-030: お気に入り登録

##### エンドポイント
```
POST /api/v1/favorites
```

##### 説明
商品をお気に入りに登録する（機能フラグON時のみ）。

##### 権限
- user のみ
- お気に入り機能フラグがONのユーザーのみ

##### リクエスト

**Body**:
```json
{
  "productId": 1
}
```

**パラメータ**:
| フィールド | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| productId | integer | ✅ | 存在する商品ID | 商品ID |

##### レスポンス

**成功時 (201 Created)**:
```json
{
  "data": {
    "favorite": {
      "favoriteId": 1,
      "userId": 1,
      "productId": 1,
      "createdAt": "2025-01-08T12:00:00Z"
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| favorite.favoriteId | integer | お気に入りID |
| favorite.userId | integer | ユーザーID |
| favorite.productId | integer | 商品ID |
| favorite.createdAt | string | 登録日時（ISO 8601） |

**エラー時 (403 Forbidden - 機能フラグOFF)**:
```json
{
  "error": {
    "code": "FEATURE_001",
    "message": "お気に入り機能は利用できません"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時 (400 Bad Request - 重複登録)**:
```json
{
  "error": {
    "code": "FAVORITE_001",
    "message": "既にお気に入りに登録されています"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時 (404 Not Found)**:
```json
{
  "error": {
    "code": "PRODUCT_001",
    "message": "商品が見つかりません"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

##### 処理仕様
1. JWTトークンからユーザーID取得
2. ユーザーの機能フラグ確認（お気に入り機能がONか）
3. 商品IDの存在確認
4. 重複登録チェック
5. お気に入りレコードをDB保存

##### 利用例

**cURLコマンド**:
```bash
curl -X POST "http://localhost:8080/api/v1/favorites" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "productId": 1
  }'
```

---

### 3.2 お気に入り解除

#### API-031: お気に入り解除

##### エンドポイント
```
DELETE /api/v1/favorites/{productId}
```

##### 説明
お気に入りから商品を解除する。

##### 権限
- user のみ

##### リクエスト

**Path Parameters**:
| パラメータ | 型 | 必須 | 説明 |
|-----------|----|----|------|
| productId | integer | ✅ | 商品ID |

**例**:
```
DELETE /api/v1/favorites/1
```

##### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "message": "お気に入りを解除しました"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**未登録商品の解除 (200 OK)**:
```json
{
  "data": {
    "message": "お気に入りを解除しました"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

> **Note**: 未登録の商品を解除してもエラーとせず、成功レスポンスを返します（冪等性確保）。

##### 処理仕様
1. JWTトークンからユーザーID取得
2. 該当のお気に入りレコードを削除
3. 存在しない場合もエラーとしない（冪等性）

##### 利用例

**cURLコマンド**:
```bash
curl -X DELETE "http://localhost:8080/api/v1/favorites/1" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

### 3.3 お気に入り一覧取得

#### API-032: お気に入り一覧取得

##### エンドポイント
```
GET /api/v1/favorites
```

##### 説明
ログインユーザーのお気に入り商品一覧を取得する。

##### 権限
- user のみ

##### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "favorites": [
      {
        "favoriteId": 1,
        "product": {
          "productId": 1,
          "productName": "商品A",
          "unitPrice": 1000,
          "imageUrl": "https://example.com/images/product_a.jpg"
        },
        "createdAt": "2025-01-08T12:00:00Z"
      }
    ]
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| favorites[].favoriteId | integer | お気に入りID |
| favorites[].product.productId | integer | 商品ID |
| favorites[].product.productName | string | 商品名 |
| favorites[].product.unitPrice | integer | 単価（円） |
| favorites[].product.imageUrl | string | 商品画像URL |
| favorites[].createdAt | string | 登録日時（ISO 8601） |

**お気に入りなし (200 OK)**:
```json
{
  "data": {
    "favorites": []
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

##### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/favorites" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

## 4. 購入・お気に入りAPI一覧サマリー

| ID | メソッド | エンドポイント | 権限 | 説明 |
|----|---------|--------------|------|------|
| API-020 | POST | /api/v1/purchases | user | 商品購入 |
| API-021 | GET | /api/v1/purchases | user | 購入履歴取得 |
| API-030 | POST | /api/v1/favorites | user | お気に入り登録 |
| API-031 | DELETE | /api/v1/favorites/{id} | user | お気に入り解除 |
| API-032 | GET | /api/v1/favorites | user | お気に入り一覧 |

**合計**: 5エンドポイント

---

**End of Document**
