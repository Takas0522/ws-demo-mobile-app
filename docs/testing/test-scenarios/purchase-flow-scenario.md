# 購入フローエンドツーエンドテスト

> テストID: TC-PUR-001 〜 TC-PUR-006  
> 目的: ログインから購入完了までの一連の流れを確認

---

## 1. テスト概要

このシナリオテストでは、エンドユーザーの典型的な利用フロー（ログイン → 商品検索 → 商品詳細 → 購入 → 購入履歴確認）を検証します。

---

## 2. 前提条件

### 2.1 システム状態
- [ ] すべてのサービスが起動している
- [ ] PostgreSQLが起動し、初期データが投入されている

### 2.2 テストユーザー
- user001 / password123

### 2.3 テスト商品
- 商品A (product_id: 1, 単価: 1000円)
- 商品B (product_id: 2, 単価: 1500円)
- 商品C (product_id: 3, 単価: 2000円)

---

## 3. エンドツーエンドシナリオ

### TC-PUR-001: 基本的な購入フロー

**シナリオ**: ユーザーがログインして商品を検索し、購入する

**テスト手順**:

#### Step 1: ログイン

```bash
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "loginId": "user001",
    "password": "password123"
  }' | jq .
```

**期待結果**: 
```json
{
  "token": "eyJhbGciOiJIUzI1NiIs...",
  "userType": "user",
  "expiresIn": 3600
}
```

#### Step 2: トークンを環境変数に保存

```bash
export TOKEN="<取得したトークン>"
```

#### Step 3: 商品一覧を取得

```bash
curl http://localhost:8081/api/v1/products \
  -H "Authorization: Bearer $TOKEN" | jq .
```

**期待結果**:
```json
{
  "products": [
    {
      "productId": 1,
      "productName": "商品A",
      "unitPrice": 1000,
      "description": "商品Aの説明文",
      "imageUrl": "https://example.com/images/product-a.jpg"
    },
    {
      "productId": 2,
      "productName": "商品B",
      "unitPrice": 1500,
      "description": "商品Bの説明文",
      "imageUrl": "https://example.com/images/product-b.jpg"
    }
  ],
  "totalCount": 3,
  "page": 1,
  "pageSize": 10
}
```

#### Step 4: 商品を検索

```bash
curl http://localhost:8081/api/v1/products/search?keyword=商品A \
  -H "Authorization: Bearer $TOKEN" | jq .
```

**期待結果**:
```json
{
  "products": [
    {
      "productId": 1,
      "productName": "商品A",
      "unitPrice": 1000,
      "description": "商品Aの説明文",
      "imageUrl": "https://example.com/images/product-a.jpg"
    }
  ],
  "totalCount": 1
}
```

#### Step 5: 商品詳細を取得

```bash
curl http://localhost:8081/api/v1/products/1 \
  -H "Authorization: Bearer $TOKEN" | jq .
```

**期待結果**:
```json
{
  "productId": 1,
  "productName": "商品A",
  "unitPrice": 1000,
  "description": "商品Aの説明文",
  "imageUrl": "https://example.com/images/product-a.jpg",
  "createdAt": "2025-01-XX...",
  "updatedAt": "2025-01-XX..."
}
```

#### Step 6: 商品を購入（100個）

```bash
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 100
  }' | jq .
```

**期待結果**:
```json
{
  "purchaseId": 1,
  "userId": 1,
  "productId": 1,
  "productName": "商品A",
  "quantity": 100,
  "unitPrice": 1000,
  "totalAmount": 100000,
  "purchasedAt": "2025-01-XX..."
}
```

#### Step 7: 購入履歴を確認

```bash
curl http://localhost:8081/api/v1/purchases \
  -H "Authorization: Bearer $TOKEN" | jq .
```

**期待結果**:
```json
{
  "purchases": [
    {
      "purchaseId": 1,
      "productId": 1,
      "productName": "商品A",
      "quantity": 100,
      "unitPrice": 1000,
      "totalAmount": 100000,
      "purchasedAt": "2025-01-XX..."
    }
  ],
  "totalCount": 1
}
```

#### Step 8: データベースで確認

```sql
SELECT 
    p.purchase_id,
    p.user_id,
    u.login_id,
    p.product_id,
    pr.product_name,
    p.quantity,
    p.unit_price,
    p.total_amount,
    p.purchased_at
FROM purchases p
JOIN users u ON p.user_id = u.user_id
JOIN products pr ON p.product_id = pr.product_id
WHERE p.user_id = 1
ORDER BY p.purchased_at DESC;
```

**期待結果**: 購入データが正しく保存されている

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-PUR-002: 複数回購入

**シナリオ**: 同じユーザーが異なる商品を複数回購入

**テスト手順**:

1. user001でログイン（TC-PUR-001のトークンを使用可）

2. 商品A を200個購入
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 200
     }' | jq .
   ```

3. 商品B を500個購入
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 2,
       "quantity": 500
     }' | jq .
   ```

4. 購入履歴を確認
   ```bash
   curl http://localhost:8081/api/v1/purchases \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

**期待結果**:
- Step 2: 201 Created, purchaseId=2
- Step 3: 201 Created, purchaseId=3
- Step 4: 3件の購入履歴が返却される（TC-PUR-001含む）

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-PUR-003: 100個単位制約の確認

**シナリオ**: 購入数量が100の倍数でない場合、エラーとなることを確認

**テスト手順**:

1. user001でログイン

2. 99個購入を試みる（100未満）
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 99
     }' | jq .
   ```

3. 101個購入を試みる（100の倍数でない）
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 101
     }' | jq .
   ```

4. 150個購入を試みる（100の倍数でない）
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 150
     }' | jq .
   ```

**期待結果**:
- Step 2, 3, 4: すべて 400 Bad Request
  ```json
  {
    "errorCode": "PURCHASE_001",
    "message": "購入数量は100個単位で指定してください",
    "timestamp": "2025-01-XX...",
    "path": "/api/v1/purchases"
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-PUR-004: 購入数量範囲の確認

**シナリオ**: 最小値（100個）と最大値（9900個）、範囲外の確認

**テスト手順**:

1. 最小値: 100個購入
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 100
     }' | jq .
   ```

2. 最大値: 9900個購入
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 9900
     }' | jq .
   ```

3. 範囲外（最大値超過）: 10000個購入
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 10000
     }' | jq .
   ```

4. 範囲外（0個）
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1,
       "quantity": 0
     }' | jq .
   ```

**期待結果**:
- Step 1: 201 Created, totalAmount=100000
- Step 2: 201 Created, totalAmount=9900000
- Step 3: 400 Bad Request, PURCHASE_002 (範囲外エラー)
- Step 4: 400 Bad Request, PURCHASE_002 (範囲外エラー)

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-PUR-005: 合計金額の正確性

**シナリオ**: 購入時の合計金額が正しく計算されることを確認

**テスト手順**:

1. 商品A (1000円) を 300個購入
   - 期待される合計金額: 1000 × 300 = 300,000円

2. 商品B (1500円) を 500個購入
   - 期待される合計金額: 1500 × 500 = 750,000円

3. 商品C (2000円) を 1000個購入
   - 期待される合計金額: 2000 × 1000 = 2,000,000円

**実行コマンド**:
```bash
# 商品A
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"productId": 1, "quantity": 300}' | jq .

# 商品B
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"productId": 2, "quantity": 500}' | jq .

# 商品C
curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"productId": 3, "quantity": 1000}' | jq .
```

**期待結果**:
- 商品A: totalAmount = 300000
- 商品B: totalAmount = 750000
- 商品C: totalAmount = 2000000

**データベース確認**:
```sql
SELECT 
    product_id,
    quantity,
    unit_price,
    total_amount,
    (unit_price * quantity) as calculated_amount,
    CASE 
        WHEN total_amount = (unit_price * quantity) THEN 'OK'
        ELSE 'NG'
    END as validation
FROM purchases
WHERE user_id = 1
ORDER BY purchased_at DESC
LIMIT 3;
```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-PUR-006: 存在しない商品の購入

**シナリオ**: 存在しない商品IDで購入を試みる

**テスト手順**:

1. 存在しない商品ID（999）で購入
   ```bash
   curl -X POST http://localhost:8081/api/v1/purchases \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 999,
       "quantity": 100
     }' | jq .
   ```

**期待結果**:
- 404 Not Found
  ```json
  {
    "errorCode": "PURCHASE_003",
    "message": "指定された商品が見つかりません",
    "timestamp": "2025-01-XX...",
    "path": "/api/v1/purchases"
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

## 4. パフォーマンステスト（参考）

### 応答時間の測定

```bash
# 購入APIの応答時間を測定
time curl -X POST http://localhost:8081/api/v1/purchases \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "productId": 1,
    "quantity": 100
  }' -o /dev/null -s

# 期待: 3秒以内
```

---

## 5. データ整合性チェック

### 購入データの整合性確認

```sql
-- 購入データの整合性チェック
SELECT 
    p.purchase_id,
    p.quantity,
    p.unit_price,
    p.total_amount,
    (p.unit_price * p.quantity) as calculated_total,
    CASE 
        WHEN p.total_amount = (p.unit_price * p.quantity) THEN 'OK'
        ELSE 'ERROR'
    END as integrity_check,
    CASE 
        WHEN p.quantity % 100 = 0 THEN 'OK'
        ELSE 'ERROR'
    END as quantity_check,
    CASE 
        WHEN p.quantity >= 100 AND p.quantity <= 9900 THEN 'OK'
        ELSE 'ERROR'
    END as range_check
FROM purchases p
WHERE p.user_id = 1;
```

**期待結果**: すべてのチェックが 'OK'

---

## 6. テスト実施チェックリスト

- [ ] TC-PUR-001: 基本的な購入フロー
- [ ] TC-PUR-002: 複数回購入
- [ ] TC-PUR-003: 100個単位制約
- [ ] TC-PUR-004: 購入数量範囲
- [ ] TC-PUR-005: 合計金額の正確性
- [ ] TC-PUR-006: 存在しない商品

---

## 7. トラブルシューティング

### 問題: トークンが期限切れ

**解決方法**: 新しいトークンを取得
```bash
curl -X POST http://localhost:8081/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"loginId": "user001", "password": "password123"}' | jq -r '.token'
```

### 問題: 購入履歴が多すぎてテストデータが見つからない

**解決方法**: データベースをリセット
```bash
cd database
./reset-database.sh
```

---

## 8. 参考情報

- [API仕様 - 購入API](../../specs/mobile-app-system/05-03-purchase-favorite-api.md)
- [機能要件 - 商品購入](../../specs/mobile-app-system/02-01-mobile-functional-requirements.md)
- [エラーコード一覧](../../specs/mobile-app-system/07-error-handling.md)

---

**End of Document**
