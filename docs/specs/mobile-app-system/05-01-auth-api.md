# mobile-app-system - 認証API仕様

> 最終更新: 2025-01-08
> ステータス: Draft
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-08 | 初版作成 | AI Agent |

---

## 1. 認証API概要

本ドキュメントでは、mobile-app-systemの認証関連API仕様を定義します。

### 1.1 対象API

- API-001: エンドユーザーログイン
- API-002: 管理者ログイン

### 1.2 共通仕様

#### 1.2.1 ベースURL

| 環境 | Web API |
|------|---------|
| 開発 | `http://localhost:8080` |
| 本番 | TBD |

#### 1.2.2 共通ヘッダー

**リクエストヘッダー**:
```http
Content-Type: application/json
Accept: application/json
```

**レスポンスヘッダー**:
```http
Content-Type: application/json
```

#### 1.2.3 HTTPステータスコード

| ステータスコード | 意味 | 使用ケース |
|---------------|------|-----------|
| 200 OK | 成功 | ログイン成功 |
| 400 Bad Request | リクエスト不正 | バリデーションエラー |
| 401 Unauthorized | 認証失敗 | ID/パスワード不一致 |
| 500 Internal Server Error | サーバーエラー | 予期しないエラー |

#### 1.2.4 共通レスポンス形式

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

---

## 2. エンドユーザーログイン

### API-001: エンドユーザーログイン

#### エンドポイント
```
POST /api/v1/auth/login
```

#### 説明
エンドユーザーのログイン認証を行い、JWTトークンを発行する。

#### 権限
- なし（公開エンドポイント）

#### リクエスト

**Body**:
```json
{
  "loginId": "user001",
  "password": "password123"
}
```

**パラメータ**:
| フィールド | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| loginId | string | ✅ | 4-20文字 | ログインID |
| password | string | ✅ | 8-50文字 | パスワード |

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "tokenType": "Bearer",
    "expiresIn": 86400,
    "user": {
      "userId": 1,
      "userName": "山田太郎",
      "loginId": "user001",
      "userType": "user"
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| token | string | JWTトークン |
| tokenType | string | トークンタイプ（常に"Bearer"） |
| expiresIn | integer | 有効期限（秒） |
| user.userId | integer | ユーザーID |
| user.userName | string | ユーザー名 |
| user.loginId | string | ログインID |
| user.userType | string | ユーザー種別（"user"） |

**エラー時 (401 Unauthorized)**:
```json
{
  "error": {
    "code": "AUTH_001",
    "message": "ログインIDまたはパスワードが正しくありません"
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
    "details": "loginIdは4〜20文字である必要があります"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### JWTペイロード
```json
{
  "sub": "1",
  "loginId": "user001",
  "userType": "user",
  "iat": 1704700800,
  "exp": 1704787200
}
```

**ペイロードフィールド**:
| フィールド | 説明 |
|-----------|------|
| sub | ユーザーID |
| loginId | ログインID |
| userType | ユーザー種別（"user"） |
| iat | 発行日時（Unix timestamp） |
| exp | 有効期限（Unix timestamp） |

#### セキュリティ
- パスワードはハッシュ化してDB保存
- JWTは署名付き（HS256）
- トークン有効期限: 24時間

#### 利用例

**cURLコマンド**:
```bash
curl -X POST http://localhost:8080/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": "password123"
  }'
```

**JavaScriptフェッチ**:
```javascript
fetch('http://localhost:8080/api/v1/auth/login', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    loginId: 'user001',
    password: 'password123'
  })
})
.then(response => response.json())
.then(data => {
  const token = data.data.token;
  // トークンを保存
  localStorage.setItem('token', token);
})
```

---

## 3. 管理者ログイン

### API-002: 管理者ログイン

#### エンドポイント
```
POST /api/v1/auth/admin/login
```

#### 説明
管理者のログイン認証を行い、管理者用JWTトークンを発行する。

#### 権限
- なし（公開エンドポイント）

#### リクエスト

**Body**:
```json
{
  "loginId": "admin001",
  "password": "adminpass123"
}
```

**パラメータ**:
| フィールド | 型 | 必須 | 制約 | 説明 |
|-----------|----|----|-----|------|
| loginId | string | ✅ | 4-20文字 | 管理者ログインID |
| password | string | ✅ | 8-50文字 | パスワード |

#### レスポンス

**成功時 (200 OK)**:
```json
{
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "tokenType": "Bearer",
    "expiresIn": 86400,
    "admin": {
      "userId": 100,
      "userName": "管理者",
      "loginId": "admin001",
      "userType": "admin"
    }
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

**レスポンスフィールド**:
| フィールド | 型 | 説明 |
|-----------|-------|------|
| token | string | JWTトークン（管理者権限） |
| tokenType | string | トークンタイプ（常に"Bearer"） |
| expiresIn | integer | 有効期限（秒） |
| admin.userId | integer | 管理者ユーザーID |
| admin.userName | string | 管理者名 |
| admin.loginId | string | ログインID |
| admin.userType | string | ユーザー種別（"admin"） |

**エラー時 (401 Unauthorized)**:
```json
{
  "error": {
    "code": "AUTH_001",
    "message": "ログインIDまたはパスワードが正しくありません"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### JWTペイロード
```json
{
  "sub": "100",
  "loginId": "admin001",
  "userType": "admin",
  "iat": 1704700800,
  "exp": 1704787200
}
```

**ペイロードフィールド**:
| フィールド | 説明 |
|-----------|------|
| sub | 管理者ユーザーID |
| loginId | ログインID |
| userType | ユーザー種別（"admin"） |
| iat | 発行日時（Unix timestamp） |
| exp | 有効期限（Unix timestamp） |

#### セキュリティ
- パスワードはハッシュ化してDB保存
- JWTは署名付き（HS256）
- トークン有効期限: 24時間
- 管理者権限（admin）がペイロードに含まれる

#### 利用例

**cURLコマンド**:
```bash
curl -X POST http://localhost:8080/api/v1/auth/admin/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "admin001",
    "password": "adminpass123"
  }'
```

**JavaScriptフェッチ**:
```javascript
fetch('http://localhost:8080/api/v1/auth/admin/login', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    loginId: 'admin001',
    password: 'adminpass123'
  })
})
.then(response => response.json())
.then(data => {
  const token = data.data.token;
  // トークンを保存
  localStorage.setItem('adminToken', token);
})
```

---

## 4. 認証API一覧サマリー

| ID | メソッド | エンドポイント | 権限 | 説明 |
|----|---------|--------------|------|------|
| API-001 | POST | /api/v1/auth/login | なし | ユーザーログイン |
| API-002 | POST | /api/v1/auth/admin/login | なし | 管理者ログイン |

**合計**: 2エンドポイント

---

**End of Document**
