# ログイン・認証シナリオテスト

> テストID: TC-AUTH-001 〜 TC-AUTH-005  
> 目的: 認証・認可機能の正常性を確認

---

## 1. テスト概要

このシナリオテストでは、JWT認証の各機能を検証します：
- ユーザーログイン
- 管理者ログイン
- トークンリフレッシュ
- 権限チェック
- トークン無効化

---

## 2. 前提条件

### 2.1 システム状態
- [ ] すべてのサービスが起動している
- [ ] SQLiteデータベースが作成され、初期データが投入されている

### 2.2 テストユーザー
| ユーザータイプ | ログインID | パスワード |
|------------|-----------|-----------|
| 一般ユーザー | user001 | password123 |
| 一般ユーザー | user002 | password123 |
| 管理者 | admin001 | adminpass123 |

---

## 3. テストケース

### TC-AUTH-001: ユーザーログイン（正常系）

**目的**: 正しいID/パスワードでログインできることを確認

**テスト手順**:

1. ユーザーログイン（Mobile BFF経由）
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "password123"
     }' | jq .
   ```

2. ユーザーログイン（Web API直接）
   ```bash
   curl -X POST http://localhost:8080/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "password123"
     }' | jq .
   ```

3. 取得したトークンをデコード（jwt.ioで確認）
   - https://jwt.io にアクセス
   - トークンを貼り付けて内容を確認

**期待結果**:
- Step 1, 2: 200 OK
  ```json
  {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "userType": "user",
    "expiresIn": 3600
  }
  ```
- Step 3: ペイロードに以下が含まれる
  ```json
  {
    "userId": 1,
    "userType": "user",
    "loginId": "user001",
    "iat": 1234567890,
    "exp": 1234571490
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-AUTH-002: ユーザーログイン（異常系）

**目的**: 誤った認証情報でログインが拒否されることを確認

**テスト手順**:

1. 誤ったパスワード
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "wrongpassword"
     }' | jq .
   ```

2. 存在しないユーザーID
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "userXXX",
       "password": "password123"
     }' | jq .
   ```

3. ログインID未入力
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "",
       "password": "password123"
     }' | jq .
   ```

4. パスワード未入力
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": ""
     }' | jq .
   ```

5. JSON形式が不正
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d 'invalid json' | jq .
   ```

**期待結果**:
- Step 1, 2: 401 Unauthorized
  ```json
  {
    "errorCode": "AUTH_001",
    "message": "認証に失敗しました",
    "timestamp": "2025-01-XX...",
    "path": "/api/v1/auth/login"
  }
  ```
- Step 3, 4: 400 Bad Request
  ```json
  {
    "errorCode": "VALIDATION_001",
    "message": "入力値が不正です",
    "errors": [
      {
        "field": "loginId",
        "message": "ログインIDは必須です"
      }
    ]
  }
  ```
- Step 5: 400 Bad Request (JSON parse error)

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-AUTH-003: 管理者ログイン

**目的**: 管理者が管理者専用エンドポイントでログインできることを確認

**テスト手順**:

1. 管理者ログイン（Admin BFF経由）
   ```bash
   curl -X POST http://localhost:8082/api/v1/auth/admin/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "admin001",
       "password": "adminpass123"
     }' | jq .
   ```

2. 管理者ログイン（Web API直接）
   ```bash
   curl -X POST http://localhost:8080/api/v1/auth/admin/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "admin001",
       "password": "adminpass123"
     }' | jq .
   ```

3. 取得したトークンの内容確認（jwt.io）

**期待結果**:
- Step 1, 2: 200 OK
  ```json
  {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "userType": "admin",
    "expiresIn": 3600
  }
  ```
- Step 3: ペイロードに以下が含まれる
  ```json
  {
    "userId": 100,
    "userType": "admin",
    "loginId": "admin001",
    "iat": 1234567890,
    "exp": 1234571490
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-AUTH-004: トークンリフレッシュ

**目的**: トークンリフレッシュ機能が正常に動作することを確認

**テスト手順**:

1. ユーザーログイン
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "password123"
     }' | jq .
   ```

2. トークンを環境変数に保存
   ```bash
   export TOKEN="<取得したトークン>"
   ```

3. 5秒待機
   ```bash
   sleep 5
   ```

4. トークンリフレッシュ
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/refresh \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

5. 新しいトークンで保護されたAPIを呼び出し
   ```bash
   export NEW_TOKEN="<リフレッシュで取得したトークン>"
   curl http://localhost:8081/api/v1/products \
     -H "Authorization: Bearer $NEW_TOKEN" | jq .
   ```

6. 古いトークンが無効化されたことを確認（オプション）
   ```bash
   curl http://localhost:8081/api/v1/products \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

**期待結果**:
- Step 1: 200 OK, トークン取得
- Step 4: 200 OK, 新しいトークン取得
  ```json
  {
    "token": "eyJhbGciOiJIUzI1NiIs...",
    "expiresIn": 3600
  }
  ```
- Step 5: 200 OK, 商品一覧取得成功
- Step 6: 設計による（古いトークンが無効化される場合は401）

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-AUTH-005: 権限チェック

**目的**: ユーザー種別による権限制御が正常に動作することを確認

**テスト手順**:

#### Scenario A: 一般ユーザーが管理者APIを呼び出す（拒否されるべき）

1. 一般ユーザーでログイン
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "password123"
     }' | jq -r '.token'
   ```

2. ユーザートークンで管理者専用API（商品更新）を呼び出す
   ```bash
   export USER_TOKEN="<取得したトークン>"
   curl -X PUT http://localhost:8080/api/v1/products/1 \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $USER_TOKEN" \
     -d '{
       "productName": "商品A（更新）",
       "unitPrice": 1200
     }' | jq .
   ```

**期待結果**: 403 Forbidden
```json
{
  "errorCode": "AUTH_003",
  "message": "この操作を実行する権限がありません",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/products/1"
}
```

#### Scenario B: 管理者が管理者APIを呼び出す（成功するべき）

1. 管理者でログイン
   ```bash
   curl -X POST http://localhost:8082/api/v1/auth/admin/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "admin001",
       "password": "adminpass123"
     }' | jq -r '.token'
   ```

2. 管理者トークンで商品更新API を呼び出す
   ```bash
   export ADMIN_TOKEN="<取得したトークン>"
   curl -X PUT http://localhost:8080/api/v1/products/1 \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $ADMIN_TOKEN" \
     -d '{
       "productName": "商品A（管理者更新）",
       "unitPrice": 1200
     }' | jq .
   ```

**期待結果**: 200 OK
```json
{
  "productId": 1,
  "productName": "商品A（管理者更新）",
  "unitPrice": 1200,
  "description": "商品Aの説明文",
  "imageUrl": "https://example.com/images/product-a.jpg",
  "updatedAt": "2025-01-XX..."
}
```

#### Scenario C: 管理者が一般ユーザーAPIを呼び出す（拒否されるべき）

```bash
curl -X POST http://localhost:8080/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 100
  }' | jq .
```

**期待結果**: 403 Forbidden
```json
{
  "errorCode": "AUTH_003",
  "message": "この操作を実行する権限がありません"
}
```

**実施結果**: ☐ PASS / ☐ FAIL

---

## 4. トークン検証テスト

### TC-AUTH-EDGE-001: トークンなしでAPI呼び出し

```bash
curl http://localhost:8081/api/v1/products | jq .
```

**期待結果**: 401 Unauthorized, AUTH_005

---

### TC-AUTH-EDGE-002: 不正なトークンでAPI呼び出し

```bash
curl http://localhost:8081/api/v1/products \
  -H "Authorization: Bearer invalid.token.here" | jq .
```

**期待結果**: 401 Unauthorized, AUTH_002

---

### TC-AUTH-EDGE-003: 期限切れトークンでAPI呼び出し

**注意**: このテストは自動化困難（トークン有効期限を待つ必要がある）

1. トークンを取得
2. 1時間以上待機
3. トークンでAPI呼び出し

**期待結果**: 401 Unauthorized, AUTH_004

**代替手段**: JWT秘密鍵を変更してトークンを無効化

---

### TC-AUTH-EDGE-004: Authorization ヘッダー形式が不正

```bash
# "Bearer " プレフィックスなし
curl http://localhost:8081/api/v1/products \
  -H "Authorization: $TOKEN" | jq .

# スペルミス
curl http://localhost:8081/api/v1/products \
  -H "Authorisation: Bearer $TOKEN" | jq .
```

**期待結果**: 401 Unauthorized

---

## 5. データベース確認

### ユーザーデータの確認

```sql
-- 一般ユーザー
SELECT 
    user_id,
    login_id,
    password_hash,
    created_at
FROM users
WHERE login_id = 'user001';

-- 管理者
SELECT 
    admin_id,
    login_id,
    password_hash,
    created_at
FROM admins
WHERE login_id = 'admin001';
```

### パスワードハッシュの確認

```sql
-- パスワードハッシュがbcrypt形式であることを確認
-- （$2a$ または $2b$ で始まる）
SELECT 
    login_id,
    password_hash,
    CASE 
        WHEN password_hash LIKE '$2a$%' OR password_hash LIKE '$2b$%' THEN 'bcrypt'
        ELSE 'unknown'
    END as hash_type
FROM users;
```

---

## 6. セキュリティチェック

### パスワードの保護確認

- [ ] パスワードが平文で保存されていない（password_hash列）
- [ ] ログにパスワードが出力されていない
- [ ] APIレスポンスにパスワードハッシュが含まれていない

### トークンの保護確認

- [ ] トークンがHTTPSで送信される（本番環境）
- [ ] トークンにセンシティブな情報が含まれていない
- [ ] トークンの有効期限が適切（1時間）

---

## 7. テスト実施チェックリスト

- [ ] TC-AUTH-001: ユーザーログイン（正常系）
- [ ] TC-AUTH-002: ユーザーログイン（異常系）
- [ ] TC-AUTH-003: 管理者ログイン
- [ ] TC-AUTH-004: トークンリフレッシュ
- [ ] TC-AUTH-005: 権限チェック
- [ ] TC-AUTH-EDGE-001: トークンなし
- [ ] TC-AUTH-EDGE-002: 不正なトークン
- [ ] TC-AUTH-EDGE-003: 期限切れトークン
- [ ] TC-AUTH-EDGE-004: ヘッダー形式不正

---

## 8. トラブルシューティング

### 問題: ログインしても401エラーが出る

**確認事項**:
1. JWT秘密鍵が環境変数に設定されているか
2. トークンが正しくコピーされているか
3. トークンの有効期限が切れていないか

**解決方法**:
```bash
# 環境変数確認
echo $JWT_SECRET

# 新しいトークンを取得
curl -X POST http://localhost:8081/api/v1/auth/login ...
```

---

### 問題: 管理者ログインが失敗する

**確認事項**:
1. adminsテーブルにデータが存在するか
2. パスワードが正しいか

**解決方法**:
```sql
SELECT * FROM admins WHERE login_id = 'admin001';
```

---

## 9. 参考情報

- [API仕様 - 認証API](../../specs/mobile-app-system/05-01-auth-api.md)
- [セキュリティ仕様](../../specs/mobile-app-system/08-security.md)
- [エラーコード一覧](../../specs/mobile-app-system/07-error-handling.md)

---

**End of Document**
