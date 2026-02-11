# mobile-app-system - 商品API仕様

> 最終更新: 2025-01-08
> ステータス: Draft
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-08 | 初版作成 | AI Agent |

---

## 1. 商品API概要

本ドキュメントでは、mobile-app-systemの商品関連API仕様を定義します。

### 1.1 対象API

- API-010: 商品一覧取得
- API-011: 商品検索
- API-012: 商品詳細取得
- API-013: 商品情報更新（管理者専用）

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

## 2. 商品一覧取得

### API-010: 商品一覧取得

#### エンドポイント
```
GET /api/v1/products
```

#### 説明
商品一覧を取得する。

#### 権限
- user または admin

#### リクエスト

**Headers**:
```
Authorization: Bearer {JWT_TOKEN}
```

**Query Parameters** (オプション):
| パラメータ | 型 | 必須 | デフォルト | 説明 |
|-----------|----|----|----------|------|
| page | integer | ❌ | 1 | ページ番号 |
| limit | integer | ❌ | 20 | 1ページあたりの件数 |
| sortBy | string | ❌ | productId | ソート項目 (productId, productName, unitPrice) |
| sortOrder | string | ❌ | asc | ソート順 (asc, desc) |

**例**:
```
GET /api/v1/products?page=1&limit=20&sortBy=productName&sortOrder=asc
```

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "products": [
      {
        "productId": 1,
        "productName": "商品A",
        "unitPrice": 1000,
        "description": "商品Aの説明文です",
        "imageUrl": "https://example.com/images/product_a.jpg"
      },
      {
        "productId": 2,
        "productName": "商品B",
        "unitPrice": 1500,
        "description": "商品Bの説明文です",
        "imageUrl": "https://example.com/images/product_b.jpg"
      }
    ],
    "pagination": {
      "currentPage": 1,
      "totalPages": 5,
      "totalItems": 100,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| products | array | 商品リスト |
| products[].productId | integer | 商品ID |
| products[].productName | string | 商品名 |
| products[].unitPrice | integer | 単価（円） |
| products[].description | string | 商品説明 |
| products[].imageUrl | string | 商品画像URL |
| pagination.currentPage | integer | 現在のページ番号 |
| pagination.totalPages | integer | 総ページ数 |
| pagination.totalItems | integer | 総商品数 |
| pagination.itemsPerPage | integer | 1ページあたりの件数 |

**エラー時 (401 Unauthorized)**:
```json
{
  "error": {
    "code": "AUTH_002",
    "message": "認証が必要です"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/products?page=1&limit=20" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

## 3. 商品検索

### API-011: 商品検索

#### エンドポイント
```
GET /api/v1/products/search
```

#### 説明
商品名でキーワード検索を行う。

#### 権限
- user または admin

#### リクエスト

**Query Parameters**:
| パラメータ | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| keyword | string | ✅ | 1-50文字 | 検索キーワード |
| page | integer | ❌ | 1 | ページ番号 |
| limit | integer | ❌ | 20 | 1ページあたりの件数 |

**例**:
```
GET /api/v1/products/search?keyword=商品A&page=1&limit=20
```

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "products": [
      {
        "productId": 1,
        "productName": "商品A",
        "unitPrice": 1000,
        "description": "商品Aの説明文です",
        "imageUrl": "https://example.com/images/product_a.jpg"
      }
    ],
    "searchKeyword": "商品A",
    "pagination": {
      "currentPage": 1,
      "totalPages": 1,
      "totalItems": 1,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**該当なし (200 OK)**:
```json
{
  "data": {
    "products": [],
    "searchKeyword": "存在しない商品",
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

**バリデーションエラー (400 Bad Request)**:
```json
{
  "error": {
    "code": "VALIDATION_001",
    "message": "入力値が不正です",
    "details": "keywordは必須です"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### 検索仕様
- 部分一致検索
- 大文字小文字を区別しない
- 商品名のみを検索対象とする

#### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/products/search?keyword=商品A" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

## 4. 商品詳細取得

### API-012: 商品詳細取得

#### エンドポイント
```
GET /api/v1/products/{productId}
```

#### 説明
指定した商品の詳細情報を取得する。

#### 権限
- user または admin

#### リクエスト

**Path Parameters**:
| パラメータ | 型 | 必須 | 説明 |
|-----------|----|----|------|
| productId | integer | ✅ | 商品ID |

**例**:
```
GET /api/v1/products/1
```

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "product": {
      "productId": 1,
      "productName": "商品A",
      "unitPrice": 1000,
      "description": "商品Aの説明文です。これは詳細な説明です。",
      "imageUrl": "https://example.com/images/product_a.jpg",
      "createdAt": "2025-01-01T00:00:00Z",
      "updatedAt": "2025-01-08T10:00:00Z"
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| product.productId | integer | 商品ID |
| product.productName | string | 商品名 |
| product.unitPrice | integer | 単価（円） |
| product.description | string | 商品説明 |
| product.imageUrl | string | 商品画像URL |
| product.createdAt | string | 作成日時（ISO 8601） |
| product.updatedAt | string | 更新日時（ISO 8601） |

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

#### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/products/1" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

---

## 5. 商品情報更新（管理者専用）

### API-013: 商品情報更新

#### エンドポイント
```
PUT /api/v1/products/{productId}
```

#### 説明
商品の名前と単価を更新する（管理者専用）。

#### 権限
- admin のみ

#### リクエスト

**Path Parameters**:
| パラメータ | 型 | 必須 | 説明 |
|-----------|----|----|------|
| productId | integer | ✅ | 商品ID |

**Body**:
```json
{
  "productName": "商品A（改訂版）",
  "unitPrice": 1200,
  "description": "商品Aの新しい説明文です",
  "imageUrl": "https://example.com/images/product_a_v2.jpg"
}
```

**パラメータ**:
| フィールド | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| productName | string | ✅ | 1-100文字 | 商品名 |
| unitPrice | integer | ✅ | 1以上 | 単価（円） |
| description | string | ❌ | - | 商品説明 |
| imageUrl | string | ❌ | URL形式 | 商品画像URL |

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "product": {
      "productId": 1,
      "productName": "商品A（改訂版）",
      "unitPrice": 1200,
      "description": "商品Aの新しい説明文です",
      "imageUrl": "https://example.com/images/product_a_v2.jpg",
      "updatedAt": "2025-01-08T12:00:00Z"
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**エラー時 (403 Forbidden)**:
```json
{
  "error": {
    "code": "AUTH_003",
    "message": "この操作を実行する権限がありません"
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

**バリデーションエラー (400 Bad Request)**:
```json
{
  "error": {
    "code": "VALIDATION_001",
    "message": "入力値が不正です",
    "details": "単価は1以上である必要があります"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### 利用例

**cURLコマンド**:
```bash
curl -X PUT "http://localhost:8080/api/v1/products/1" \
  -H "Authorization: Bearer YOUR_ADMIN_JWT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "productName": "商品A（改訂版）",
    "unitPrice": 1200,
    "description": "商品Aの新しい説明文です"
  }'
```

---

## 6. 商品API一覧サマリー

| ID | メソッド | エンドポイント | 権限 | 説明 |
|----|---------|--------------|------|------|
| API-010 | GET | /api/v1/products | user/admin | 商品一覧取得 |
| API-011 | GET | /api/v1/products/search | user/admin | 商品検索 |
| API-012 | GET | /api/v1/products/{id} | user/admin | 商品詳細取得 |
| API-013 | PUT | /api/v1/products/{id} | admin | 商品更新 |

**合計**: 4エンドポイント

---

**End of Document**
