# 7. API 仕様

## 7.1 共通仕様

### ベース URL

```
http://localhost:8082/admin-struts/api/v1
```

### 共通レスポンス形式

**成功時:**

```json
{
  "data": { ... },
  "error": null,
  "timestamp": "2026-03-30T12:00:00"
}
```

**エラー時:**

```json
{
  "data": null,
  "error": {
    "code": "ERROR_CODE",
    "message": "エラーメッセージ",
    "details": null
  },
  "timestamp": "2026-03-30T12:00:00"
}
```

### 認証ヘッダー

認証が必要なエンドポイントでは以下のヘッダーを付与する:

```
Authorization: Bearer <JWT トークン>
```

---

## 7.2 認証 API

### POST `/api/v1/auth/login`

一般ユーザーログイン（認証不要）

**リクエスト:**

```json
{
  "loginId": "user001",
  "password": "password123"
}
```

**レスポンス（200）:**

```json
{
  "data": {
    "token": "eyJhbGciOiJIUzI1NiJ9...",
    "userId": 2,
    "loginId": "user001",
    "userName": "山田太郎",
    "userType": "user"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| AUTH_001 | 401 | ログイン ID またはパスワードが不正 |

---

### POST `/api/v1/auth/admin-login`

管理者ログイン（認証不要）

**リクエスト:**

```json
{
  "loginId": "admin001",
  "password": "admin123"
}
```

**レスポンス（200）:** ログインと同形式

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| AUTH_001 | 401 | 認証失敗、または admin 以外のユーザー |

---

### POST `/api/v1/auth/logout`

ログアウト（認証必須）

**レスポンス（200）:**

```json
{
  "data": {
    "message": "Logged out successfully"
  }
}
```

---

### POST `/api/v1/auth/refresh`

トークンリフレッシュ（認証不要 — 既存トークンをボディで送信）

**リクエスト:**

```json
{
  "token": "eyJhbGciOiJIUzI1NiJ9..."
}
```

**レスポンス（200）:**

```json
{
  "data": {
    "token": "eyJhbGciOiJIUzI1NiJ9...(新トークン)"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| AUTH_002 | 400 | トークン未指定 |
| AUTH_003 | 401 | トークン無効・期限切れ |

---

### PUT `/api/v1/auth/password`

パスワード変更（認証必須）

**リクエスト:**

```json
{
  "currentPassword": "password123",
  "newPassword": "newPassword456"
}
```

**レスポンス（200）:**

```json
{
  "data": {
    "message": "Password changed successfully"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| AUTH_005 | 400 | パラメータ不足 |
| AUTH_006 | 404 | ユーザー未検出 |
| AUTH_007 | 400 | 現在のパスワード不一致 |

---

## 7.3 商品 API

### GET `/api/v1/products`

全商品一覧取得（認証必須）

**レスポンス（200）:**

```json
{
  "data": [
    {
      "productId": 1,
      "productName": "プレミアムりんご",
      "unitPrice": 1500,
      "description": "青森県産の厳選りんご",
      "imageUrl": "https://example.com/apple.jpg"
    }
  ]
}
```

---

### GET `/api/v1/products-search?keyword=りんご`

商品キーワード検索（認証必須）

**パラメータ:**
| パラメータ | 型 | 必須 | 説明 |
|---|---|---|---|
| keyword | String | はい | 検索キーワード（商品名の部分一致） |

**レスポンス（200）:** 商品一覧と同形式

---

### GET `/api/v1/products-detail?productId=1`

商品詳細取得（認証必須）

**パラメータ:**
| パラメータ | 型 | 必須 | 説明 |
|---|---|---|---|
| productId | Integer | はい | 商品 ID |

**レスポンス（200）:**

```json
{
  "data": {
    "productId": 1,
    "productName": "プレミアムりんご",
    "unitPrice": 1500,
    "description": "青森県産の厳選りんご",
    "imageUrl": "https://example.com/apple.jpg"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| PROD_001 | 400 | productId 未指定 |
| PROD_002 | 404 | 商品未検出 |

---

### PUT `/api/v1/products-update`

商品情報更新（認証必須・管理者のみ）

**リクエスト:**

```json
{
  "productId": 1,
  "productName": "プレミアムりんご（改）",
  "unitPrice": 1800,
  "description": "青森県産の厳選りんご（リニューアル）",
  "imageUrl": "https://example.com/apple-new.jpg"
}
```

**レスポンス（200）:**

```json
{
  "data": {
    "message": "Product updated successfully"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| PROD_001 | 400 | productId 未指定 |
| PROD_002 | 404 | 商品未検出 |
| PROD_003 | 403 | 管理者権限なし |

---

### GET `/api/v1/products-price-history?productId=1`

価格変更履歴取得（認証必須）

**パラメータ:**
| パラメータ | 型 | 必須 | 説明 |
|---|---|---|---|
| productId | Integer | はい | 商品 ID |

**レスポンス（200）:**

```json
{
  "data": [
    {
      "priceHistoryId": 1,
      "productId": 1,
      "oldPrice": 1500,
      "newPrice": 1800,
      "changedAt": "2026-03-15 10:00:00",
      "changedBy": 1,
      "changeReason": "自動記録"
    }
  ]
}
```

---

## 7.4 購入 API

### POST `/api/v1/purchases-create`

商品購入（認証必須）

**リクエスト:**

```json
{
  "productId": 1,
  "quantity": 100
}
```

**レスポンス（201）:**

```json
{
  "data": {
    "purchaseId": "550e8400-e29b-41d4-a716-446655440000",
    "message": "Purchase created successfully"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| PUR_001 | 400 | productId 未指定 |
| PUR_002 | 400 | 数量が 100 の正の倍数でない |

---

### GET `/api/v1/purchases`

購入履歴取得（認証必須・自分の履歴のみ）

**レスポンス（200）:**

```json
{
  "data": [
    {
      "purchaseId": "550e8400-e29b-41d4-a716-446655440000",
      "userId": 2,
      "productId": 1,
      "productName": "プレミアムりんご",
      "quantity": 100,
      "unitPriceAtPurchase": 1500,
      "totalAmount": 150000,
      "purchasedAt": "2026-03-20 12:00:00"
    }
  ]
}
```

---

## 7.5 お気に入り API

> お気に入り API は `favorite_feature` フラグが有効なユーザーのみ利用可能

### GET `/api/v1/favorites`

お気に入り一覧取得（認証必須・フラグチェック）

**レスポンス（200）:**

```json
{
  "data": [
    {
      "favoriteId": 1,
      "userId": 2,
      "productId": 1,
      "productName": "プレミアムりんご",
      "unitPrice": 1500,
      "description": "青森県産の厳選りんご",
      "imageUrl": "https://example.com/apple.jpg",
      "createdAt": "2026-03-20 12:00:00"
    }
  ]
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| FEATURE_001 | 403 | お気に入り機能が無効 |

---

### POST `/api/v1/favorites-add`

お気に入り追加（認証必須・フラグチェック）

**リクエスト:**

```json
{
  "productId": 1
}
```

**レスポンス（201）:**

```json
{
  "data": {
    "message": "Added to favorites"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| FAV_001 | 400 | productId 未指定 |
| FAV_002 | 409 | 既にお気に入り登録済み |
| FEATURE_001 | 403 | お気に入り機能が無効 |

---

### DELETE `/api/v1/favorites-remove?productId=1`

お気に入り削除（認証必須・フラグチェック）

**パラメータ:**
| パラメータ | 型 | 必須 | 説明 |
|---|---|---|---|
| productId | Integer | はい | 商品 ID |

**レスポンス（200）:**

```json
{
  "data": {
    "message": "Removed from favorites"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| FAV_001 | 400 | productId 未指定 |
| FEATURE_001 | 403 | お気に入り機能が無効 |

---

## 7.6 フィーチャーフラグ API

### GET `/api/v1/feature-flags`

ユーザーのフラグ状態取得（認証必須）

**レスポンス（200）:**

```json
{
  "data": [
    {
      "flagKey": "favorite_feature",
      "flagName": "お気に入り機能",
      "isEnabled": true
    }
  ]
}
```

---

### GET `/api/v1/admin-users`

全ユーザーとフラグ一覧取得（認証必須・管理者のみ）

**レスポンス（200）:**

```json
{
  "data": [
    {
      "userId": 2,
      "userName": "山田太郎",
      "loginId": "user001",
      "userType": "user"
    }
  ]
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| ADMIN_001 | 403 | 管理者権限なし |

---

### PUT `/api/v1/admin-feature-flag-update`

ユーザーフラグ更新（認証必須・管理者のみ）

**リクエスト:**

```json
{
  "userId": 2,
  "flagKey": "favorite_feature",
  "enabled": true
}
```

**レスポンス（200）:**

```json
{
  "data": {
    "message": "Feature flag updated successfully"
  }
}
```

**エラー:**
| コード | HTTP | 条件 |
|---|---|---|
| FLAG_001 | 400 | userId または flagKey 未指定 |
| FLAG_002 | 400 | enabled 未指定 |
| FLAG_003 | 404 | フラグ未検出 |
| ADMIN_001 | 403 | 管理者権限なし |

---

## 7.7 ヘルスチェック API

### GET `/api/v1/health`

ヘルスチェック（認証不要）

**レスポンス（200）:**

```json
{
  "data": {
    "status": "UP",
    "timestamp": "2026-03-30T12:00:00"
  }
}
```

---

## 7.8 エラーコード一覧

| コード      | HTTP | メッセージ                                     | 発生箇所             |
| ----------- | ---- | ---------------------------------------------- | -------------------- |
| AUTH_001    | 401  | Invalid credentials                            | ログイン             |
| AUTH_002    | 400  | Token is required                              | トークンリフレッシュ |
| AUTH_003    | 401  | Invalid or expired token                       | トークン検証         |
| AUTH_004    | 401  | Authentication required                        | JWT インターセプタ   |
| AUTH_005    | 400  | Current password and new password are required | パスワード変更       |
| AUTH_006    | 404  | User not found                                 | パスワード変更       |
| AUTH_007    | 400  | Current password is incorrect                  | パスワード変更       |
| PROD_001    | 400  | Product ID is required                         | 商品操作             |
| PROD_002    | 404  | Product not found                              | 商品操作             |
| PROD_003    | 403  | Admin access required                          | 商品更新             |
| PUR_001     | 400  | Product ID is required                         | 購入                 |
| PUR_002     | 400  | Quantity must be a positive multiple of 100    | 購入                 |
| FAV_001     | 400  | Product ID is required                         | お気に入り           |
| FAV_002     | 409  | Already in favorites                           | お気に入り追加       |
| FEATURE_001 | 403  | Favorite feature is not enabled                | お気に入り機能       |
| FLAG_001    | 400  | userId and flagKey are required                | フラグ更新           |
| FLAG_002    | 400  | enabled field is required                      | フラグ更新           |
| FLAG_003    | 404  | Feature flag not found                         | フラグ更新           |
| ADMIN_001   | 403  | Admin access required                          | 管理者専用操作       |
| SYS_001     | 500  | Internal server error                          | システムエラー       |
