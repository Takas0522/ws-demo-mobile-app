# mobile-app-system - 管理・機能フラグAPI仕様

> 最終更新: 2025-01-08
> ステータス: Draft
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-08 | 初版作成 | AI Agent |

---

## 1. 管理・機能フラグAPI概要

本ドキュメントでは、mobile-app-systemの管理者機能と機能フラグ関連API仕様を定義します。

### 1.1 対象API

- API-040: ユーザー機能フラグ取得
- API-041: ユーザー一覧取得（機能フラグ管理）
- API-042: 機能フラグ変更

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

## 2. ユーザー機能フラグ取得

### API-040: ユーザー機能フラグ取得

#### エンドポイント
```
GET /api/v1/feature-flags
```

#### 説明
ログインユーザーの機能フラグ設定を取得する。

#### 権限
- user のみ

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "featureFlags": {
      "favoriteFeature": true
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| featureFlags.favoriteFeature | boolean | お気に入り機能の有効/無効 |

**機能フラグがOFFの場合 (200 OK)**:
```json
{
  "data": {
    "featureFlags": {
      "favoriteFeature": false
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### 処理仕様
1. JWTトークンからユーザーID取得
2. DBからユーザーの機能フラグ設定を取得
3. 機能フラグ情報を返却

#### 利用タイミング
- アプリ起動時
- ログイン直後
- 商品詳細画面表示時（お気に入りボタンの表示制御）

#### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/feature-flags" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

**JavaScriptフェッチ**:
```javascript
fetch('http://localhost:8080/api/v1/feature-flags', {
  headers: {
    'Authorization': `Bearer ${token}`
  }
})
.then(response => response.json())
.then(data => {
  const favoriteEnabled = data.data.featureFlags.favoriteFeature;
  // お気に入りボタンの表示制御
  if (favoriteEnabled) {
    showFavoriteButton();
  } else {
    hideFavoriteButton();
  }
})
```

---

## 3. ユーザー一覧取得（機能フラグ管理）

### API-041: ユーザー一覧取得

#### エンドポイント
```
GET /api/v1/admin/users
```

#### 説明
全ユーザーと各ユーザーの機能フラグ設定を取得する（管理者専用）。

#### 権限
- admin のみ

#### リクエスト

**Query Parameters**:
| パラメータ | 型 | 必須 | デフォルト | 説明 |
|-----------|----|----|----------|------|
| page | integer | ❌ | 1 | ページ番号 |
| limit | integer | ❌ | 50 | 1ページあたりの件数 |

**例**:
```
GET /api/v1/admin/users?page=1&limit=50
```

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "users": [
      {
        "userId": 1,
        "userName": "山田太郎",
        "loginId": "user001",
        "featureFlags": {
          "favoriteFeature": true
        }
      },
      {
        "userId": 2,
        "userName": "佐藤花子",
        "loginId": "user002",
        "featureFlags": {
          "favoriteFeature": false
        }
      }
    ],
    "pagination": {
      "currentPage": 1,
      "totalPages": 2,
      "totalItems": 100,
      "itemsPerPage": 50
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| users[].userId | integer | ユーザーID |
| users[].userName | string | ユーザー名 |
| users[].loginId | string | ログインID |
| users[].featureFlags.favoriteFeature | boolean | お気に入り機能の有効/無効 |
| pagination.currentPage | integer | 現在のページ番号 |
| pagination.totalPages | integer | 総ページ数 |
| pagination.totalItems | integer | 総ユーザー数 |
| pagination.itemsPerPage | integer | 1ページあたりの件数 |

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

#### 処理仕様
1. JWTトークンから権限確認（admin権限必須）
2. DBから全ユーザー情報と機能フラグ設定を取得
3. ページネーション適用
4. ユーザー一覧を返却

#### 利用例

**cURLコマンド**:
```bash
curl -X GET "http://localhost:8080/api/v1/admin/users?page=1&limit=50" \
  -H "Authorization: Bearer YOUR_ADMIN_JWT_TOKEN"
```

---

## 4. 機能フラグ変更

### API-042: 機能フラグ変更

#### エンドポイント
```
PUT /api/v1/admin/users/{userId}/feature-flags/{flagKey}
```

#### 説明
ユーザーの機能フラグを変更する（管理者専用）。

#### 権限
- admin のみ

#### リクエスト

**Path Parameters**:
| パラメータ | 型 | 必須 | 説明 |
|-----------|----|----|------|
| userId | integer | ✅ | ユーザーID |
| flagKey | string | ✅ | フラグキー（例: favoriteFeature） |

**Body**:
```json
{
  "isEnabled": true
}
```

**パラメータ**:
| フィールド | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| isEnabled | boolean | ✅ | true/false | 有効/無効 |

**例**:
```
PUT /api/v1/admin/users/1/feature-flags/favoriteFeature
Body: {"isEnabled": true}
```

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "userId": 1,
    "flagKey": "favoriteFeature",
    "isEnabled": true,
    "updatedAt": "2025-01-08T12:00:00Z"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| userId | integer | ユーザーID |
| flagKey | string | フラグキー |
| isEnabled | boolean | 有効/無効 |
| updatedAt | string | 更新日時（ISO 8601） |

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
    "code": "USER_001",
    "message": "ユーザーが見つかりません"
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
    "details": "isEnabledはboolean型である必要があります"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### 処理仕様
1. JWTトークンから権限確認（admin権限必須）
2. ユーザーIDの存在確認
3. フラグキーの妥当性確認
4. DBで機能フラグ設定を更新
5. 更新結果を返却

#### サポートされるフラグキー
| フラグキー | 説明 |
|-----------|------|
| favoriteFeature | お気に入り機能の有効/無効 |

> **Note**: 将来的に他の機能フラグが追加される可能性があります。

#### 利用例

**cURLコマンド（ONに変更）**:
```bash
curl -X PUT "http://localhost:8080/api/v1/admin/users/1/feature-flags/favoriteFeature" \
  -H "Authorization: Bearer YOUR_ADMIN_JWT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "isEnabled": true
  }'
```

**cURLコマンド（OFFに変更）**:
```bash
curl -X PUT "http://localhost:8080/api/v1/admin/users/1/feature-flags/favoriteFeature" \
  -H "Authorization: Bearer YOUR_ADMIN_JWT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "isEnabled": false
  }'
```

**JavaScriptフェッチ**:
```javascript
function updateFeatureFlag(userId, flagKey, isEnabled) {
  return fetch(`http://localhost:8080/api/v1/admin/users/${userId}/feature-flags/${flagKey}`, {
    method: 'PUT',
    headers: {
      'Authorization': `Bearer ${adminToken}`,
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ isEnabled })
  })
  .then(response => response.json())
  .then(data => {
    console.log('機能フラグを更新しました:', data);
    return data;
  });
}

// お気に入り機能をONに変更
updateFeatureFlag(1, 'favoriteFeature', true);
```

---

## 5. 管理・機能フラグAPI一覧サマリー

| ID | メソッド | エンドポイント | 権限 | 説明 |
|----|---------|--------------|------|------|
| API-040 | GET | /api/v1/feature-flags | user | 機能フラグ取得 |
| API-041 | GET | /api/v1/admin/users | admin | ユーザー一覧取得 |
| API-042 | PUT | /api/v1/admin/users/{id}/feature-flags/{key} | admin | 機能フラグ変更 |

**合計**: 3エンドポイント

---

**End of Document**
