# エラーシナリオテスト

> テストID: TC-ERR-001 〜 TC-ERR-008  
> 目的: 各種エラーケースの処理を確認

---

## 1. テスト概要

このシナリオテストでは、システムが想定されるエラー状況に適切に対処できることを検証します：
- 認証エラー
- バリデーションエラー
- 権限エラー
- リソース未検出エラー
- システムエラー

---

## 2. 前提条件

### 2.1 システム状態
- [ ] すべてのサービスが起動している
- [ ] SQLiteデータベースが作成され、初期データが投入されている

---

## 3. テストケース

### TC-ERR-001: 認証エラー - トークンなし

**目的**: トークンなしで保護されたAPIを呼び出すと401エラーになることを確認

**テスト手順**:

```bash
# 商品一覧取得（トークンなし）
curl http://localhost:8081/api/v1/products | jq .

# 商品購入（トークンなし）
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -d '{"productId": 1, "quantity": 100}' | jq .

# お気に入り一覧（トークンなし）
curl http://localhost:8081/api/v1/favorites | jq .
```

**期待結果**: すべて 401 Unauthorized
```json
{
  "errorCode": "AUTH_005",
  "message": "認証トークンが見つかりません",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/products"
}
```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-002: 認証エラー - 無効なトークン

**目的**: 不正なトークンで保護されたAPIを呼び出すと401エラーになることを確認

**テスト手順**:

```bash
# 無効なトークン形式
curl http://localhost:8081/api/v1/products \
  -H "Authorization: Bearer invalid.token.here" | jq .

# 改ざんされたトークン（一部文字を変更）
curl http://localhost:8081/api/v1/products \
  -H "Authorization: Bearer eyJhbGciOiJIUzI1NiXXXXXXXXX..." | jq .

# 空のトークン
curl http://localhost:8081/api/v1/products \
  -H "Authorization: Bearer " | jq .
```

**期待結果**: すべて 401 Unauthorized
```json
{
  "errorCode": "AUTH_002",
  "message": "認証トークンが不正です",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/products"
}
```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-003: 認証エラー - 誤ったパスワード

**目的**: 誤ったパスワードでログインが失敗することを確認

**テスト手順**:

```bash
# 誤ったパスワード（user001）
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": "wrongpassword"
  }' | jq .

# 誤ったパスワード（admin001）
curl -X POST http://localhost:8082/api/v1/auth/admin/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "admin001",
    "password": "wrongpassword"
  }' | jq .

# 存在しないユーザーID
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "invaliduser",
    "password": "password123"
  }' | jq .
```

**期待結果**: すべて 401 Unauthorized
```json
{
  "errorCode": "AUTH_001",
  "message": "認証に失敗しました",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/auth/login"
}
```

**セキュリティ注意**: 
- ユーザーが存在しない場合も同じエラーメッセージを返す（アカウント列挙攻撃防止）

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-004: バリデーションエラー - 購入数量

**目的**: 不正な購入数量でバリデーションエラーが発生することを確認

**前提**: user001でログイン済み

**テスト手順**:

```bash
export TOKEN="<user001のトークン>"

# 100の倍数でない（99個）
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 99
  }' | jq .

# 100の倍数でない（101個）
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 101
  }' | jq .

# 範囲外（最小値未満）
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 50
  }' | jq .

# 範囲外（最大値超過）
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 10000
  }' | jq .

# マイナス値
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": -100
  }' | jq .

# 0個
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 0
  }' | jq .

# 数値でない
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": "abc"
  }' | jq .
```

**期待結果**:

- 99, 101個: 400 Bad Request, PURCHASE_001
  ```json
  {
    "errorCode": "PURCHASE_001",
    "message": "購入数量は100個単位で指定してください"
  }
  ```

- 50, 10000, -100, 0個: 400 Bad Request, PURCHASE_002
  ```json
  {
    "errorCode": "PURCHASE_002",
    "message": "購入数量は100〜9900個の範囲で指定してください"
  }
  ```

- "abc": 400 Bad Request, VALIDATION_001
  ```json
  {
    "errorCode": "VALIDATION_001",
    "message": "入力値が不正です",
    "errors": [
      {
        "field": "quantity",
        "message": "数値を入力してください"
      }
    ]
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-005: バリデーションエラー - 必須項目

**目的**: 必須項目が未入力の場合にバリデーションエラーが発生することを確認

**テスト手順**:

```bash
export TOKEN="<user001のトークン>"

# ログインID未入力
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "",
    "password": "password123"
  }' | jq .

# パスワード未入力
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": ""
  }' | jq .

# 商品ID未指定
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "quantity": 100
  }' | jq .

# 購入数量未指定
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1
  }' | jq .

# 商品名が空（管理者）
export ADMIN_TOKEN="<admin001のトークン>"
curl -X PUT http://localhost:8080/api/v1/products/1 \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -d '{
    "productName": "",
    "unitPrice": 1000
  }' | jq .
```

**期待結果**: すべて 400 Bad Request, VALIDATION_001
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

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-006: 権限エラー

**目的**: 権限のない操作が拒否されることを確認

**テスト手順**:

```bash
# 一般ユーザーのトークン取得
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": "password123"
  }' | jq -r '.token'

export USER_TOKEN="<取得したトークン>"

# 管理者のトークン取得
curl -X POST http://localhost:8082/api/v1/auth/admin/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "admin001",
    "password": "adminpass123"
  }' | jq -r '.token'

export ADMIN_TOKEN="<取得したトークン>"

# 一般ユーザーが商品更新を試みる（拒否されるべき）
curl -X PUT http://localhost:8080/api/v1/products/1 \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $USER_TOKEN" \
  -d '{
    "productName": "商品A（更新）",
    "unitPrice": 1200
  }' | jq .

# 一般ユーザーが機能フラグ管理を試みる（拒否されるべき）
curl http://localhost:8080/api/v1/admin/feature-flags \
  -H "Authorization: Bearer $USER_TOKEN" | jq .

# 管理者が購入を試みる（拒否されるべき）
curl -X POST http://localhost:8080/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 100
  }' | jq .

# 管理者がお気に入り登録を試みる（拒否されるべき）
curl -X POST http://localhost:8080/api/v1/favorites \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -d '{
    "productId": 1
  }' | jq .
```

**期待結果**: すべて 403 Forbidden
```json
{
  "errorCode": "AUTH_003",
  "message": "この操作を実行する権限がありません",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/products/1"
}
```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-007: リソース未検出エラー

**目的**: 存在しないリソースへのアクセスで404エラーが発生することを確認

**テスト手順**:

```bash
export TOKEN="<user001のトークン>"
export ADMIN_TOKEN="<admin001のトークン>"

# 存在しない商品ID（商品詳細取得）
curl http://localhost:8081/api/v1/products/999 \
  -H "Authorization: Bearer $TOKEN" | jq .

# 存在しない商品ID（購入）
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 999,
    "quantity": 100
  }' | jq .

# 存在しない商品ID（お気に入り登録）
curl -X POST http://localhost:8081/api/v1/favorites \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 999
  }' | jq .

# 存在しない商品ID（管理者が更新）
curl -X PUT http://localhost:8080/api/v1/products/999 \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -d '{
    "productName": "存在しない商品",
    "unitPrice": 1000
  }' | jq .

# 存在しないお気に入りID（削除）
curl -X DELETE http://localhost:8081/api/v1/favorites/999 \
  -H "Authorization: Bearer $TOKEN" | jq .
```

**期待結果**: すべて 404 Not Found

- 商品詳細取得: PRODUCT_001
  ```json
  {
    "errorCode": "PRODUCT_001",
    "message": "指定された商品が見つかりません"
  }
  ```

- 購入: PURCHASE_003
  ```json
  {
    "errorCode": "PURCHASE_003",
    "message": "指定された商品が見つかりません"
  }
  ```

- お気に入り登録: FAVORITE_003
  ```json
  {
    "errorCode": "FAVORITE_003",
    "message": "指定された商品が見つかりません"
  }
  ```

- お気に入り削除: FAVORITE_002
  ```json
  {
    "errorCode": "FAVORITE_002",
    "message": "指定されたお気に入りが見つかりません"
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-ERR-008: 機能フラグエラー

**目的**: 機能フラグOFFのユーザーがお気に入り機能を使えないことを確認

**テスト手順**:

```bash
# user002でログイン（フラグOFF）
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user002",
    "password": "password123"
  }' | jq -r '.token'

export TOKEN="<取得したトークン>"

# お気に入り登録を試みる
curl -X POST http://localhost:8081/api/v1/favorites \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1
  }' | jq .

# お気に入り一覧取得を試みる
curl http://localhost:8081/api/v1/favorites \
  -H "Authorization: Bearer $TOKEN" | jq .

# お気に入り削除を試みる（存在しないが）
curl -X DELETE http://localhost:8081/api/v1/favorites/1 \
  -H "Authorization: Bearer $TOKEN" | jq .
```

**期待結果**: すべて 403 Forbidden
```json
{
  "errorCode": "FEATURE_001",
  "message": "この機能は現在利用できません",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/favorites"
}
```

**実施結果**: ☐ PASS / ☐ FAIL

---

## 4. エッジケースのテスト

### TC-ERR-EDGE-001: 大きなペイロード

```bash
export TOKEN="<user001のトークン>"

# 非常に長い商品名（管理者）
export ADMIN_TOKEN="<admin001のトークン>"
curl -X PUT http://localhost:8080/api/v1/products/1 \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -d "{
    \"productName\": \"$(python3 -c 'print("A" * 1000)')\",
    \"unitPrice\": 1000
  }" | jq .
```

**期待結果**: 400 Bad Request (文字数制限エラー)

---

### TC-ERR-EDGE-002: 重複お気に入り

```bash
export TOKEN="<user001のトークン>"

# 同じ商品を2回お気に入り登録
curl -X POST http://localhost:8081/api/v1/favorites \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"productId": 1}' | jq .

curl -X POST http://localhost:8081/api/v1/favorites \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"productId": 1}' | jq .
```

**期待結果**:
- 1回目: 201 Created
- 2回目: 400 Bad Request, FAVORITE_001

---

### TC-ERR-EDGE-003: SQLインジェクション試行

```bash
# 検索でSQLインジェクション試行
curl "http://localhost:8081/api/v1/products/search?keyword=' OR '1'='1" \
  -H "Authorization: Bearer $TOKEN" | jq .

# ログインでSQLインジェクション試行
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "admin'\'' OR '\''1'\''='\''1",
    "password": "anything"
  }' | jq .
```

**期待結果**: 
- SQLインジェクションが無効化される（プリペアドステートメント使用）
- 認証失敗または安全にエスケープされる

---

## 5. エラーレスポンス形式の確認

すべてのエラーレスポンスが以下の形式に従っていることを確認：

```json
{
  "errorCode": "ERROR_CODE",
  "message": "エラーメッセージ",
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/..."
}
```

バリデーションエラーの場合:
```json
{
  "errorCode": "VALIDATION_001",
  "message": "入力値が不正です",
  "errors": [
    {
      "field": "フィールド名",
      "message": "エラー詳細"
    }
  ],
  "timestamp": "2025-01-XX...",
  "path": "/api/v1/..."
}
```

---

## 6. テスト実施チェックリスト

- [ ] TC-ERR-001: 認証エラー（トークンなし）
- [ ] TC-ERR-002: 認証エラー（無効なトークン）
- [ ] TC-ERR-003: 認証エラー（誤ったパスワード）
- [ ] TC-ERR-004: バリデーションエラー（購入数量）
- [ ] TC-ERR-005: バリデーションエラー（必須項目）
- [ ] TC-ERR-006: 権限エラー
- [ ] TC-ERR-007: リソース未検出エラー
- [ ] TC-ERR-008: 機能フラグエラー
- [ ] TC-ERR-EDGE-001: 大きなペイロード
- [ ] TC-ERR-EDGE-002: 重複お気に入り
- [ ] TC-ERR-EDGE-003: SQLインジェクション試行

---

## 7. エラーログの確認

各エラーが適切にログに記録されていることを確認：

```bash
# Web API のログを確認
tail -f web-api/logs/application.log | grep ERROR
```

**確認事項**:
- エラーコードが記録されている
- スタックトレースが記録されている（システムエラーの場合）
- センシティブな情報（パスワード等）が記録されていない

---

## 8. 参考情報

- [エラーハンドリング仕様](../../specs/mobile-app-system/07-error-handling.md)
- [API仕様](../../specs/mobile-app-system/05-api-spec.md)
- [セキュリティ仕様](../../specs/mobile-app-system/08-security.md)

---

**End of Document**
