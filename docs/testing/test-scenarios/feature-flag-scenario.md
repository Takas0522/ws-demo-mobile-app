# 機能フラグシナリオテスト

> テストID: TC-FF-001 〜 TC-FF-004  
> 目的: 機能フラグによるお気に入り機能の制御を確認

---

## 1. テスト概要

このシナリオテストでは、機能フラグ（Feature Flag）の動作を確認します。
具体的には、お気に入り機能のON/OFF切り替えが正常に動作することを検証します。

---

## 2. 前提条件

### 2.1 システム状態
- [ ] すべてのサービスが起動している（Web API, Mobile BFF, Admin BFF, Admin Web）
- [ ] SQLiteデータベースが作成されている
- [ ] 初期データが投入されている

### 2.2 テストデータ
- [ ] user001 (お気に入りフラグ: ON)
- [ ] user002 (お気に入りフラグ: OFF)
- [ ] user003 (お気に入りフラグ: ON)
- [ ] 商品データ（product_id: 1, 2, 3）

### 2.3 確認方法

```sql
-- 機能フラグの状態を確認
SELECT 
    user_id, 
    feature_name, 
    is_enabled 
FROM feature_flags 
WHERE feature_name = 'favorite'
ORDER BY user_id;

-- 期待結果:
-- user_id | feature_name | is_enabled
-- --------|-------------|------------
--    1    |  favorite   |    true
--    2    |  favorite   |    false
--    3    |  favorite   |    true
```

---

## 3. テストケース

### TC-FF-001: お気に入り登録（フラグON）

**目的**: フラグONのユーザーがお気に入り登録できることを確認

**テスト手順**:

1. user001でログイン
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "password123"
     }' | jq .
   ```

2. トークンを環境変数に設定
   ```bash
   export TOKEN="<取得したトークン>"
   ```

3. お気に入り登録
   ```bash
   curl -X POST http://localhost:8081/api/v1/favorites \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1
     }' | jq .
   ```

4. お気に入り一覧を取得
   ```bash
   curl http://localhost:8081/api/v1/favorites \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

**期待結果**:
- Step 1: 200 OK, トークン返却
- Step 3: 201 Created, お気に入りID返却
  ```json
  {
    "favoriteId": 1,
    "userId": 1,
    "productId": 1,
    "createdAt": "2025-01-XX..."
  }
  ```
- Step 4: 200 OK, 登録したお気に入りが含まれる
  ```json
  {
    "favorites": [
      {
        "favoriteId": 1,
        "productId": 1,
        "productName": "商品A",
        "unitPrice": 1000
      }
    ]
  }
  ```

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-FF-002: お気に入り登録（フラグOFF）

**目的**: フラグOFFのユーザーがお気に入り登録を拒否されることを確認

**テスト手順**:

1. user002でログイン
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user002",
       "password": "password123"
     }' | jq .
   ```

2. トークンを環境変数に設定
   ```bash
   export TOKEN="<取得したトークン>"
   ```

3. お気に入り登録を試みる
   ```bash
   curl -X POST http://localhost:8081/api/v1/favorites \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1
     }' | jq .
   ```

**期待結果**:
- Step 1: 200 OK, トークン返却
- Step 3: 403 Forbidden, エラーレスポンス
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

### TC-FF-003: 管理画面での機能フラグ切り替え（OFF→ON）

**目的**: 管理画面で機能フラグを変更し、即座に反映されることを確認

**テスト手順**:

1. 管理画面にアクセス
   - URL: http://localhost:3000
   - ログイン: admin001 / adminpass123

2. 機能フラグ管理画面を開く
   - サイドメニューから「機能フラグ管理」をクリック

3. user002のお気に入りフラグをONに変更
   - user002の行を探す
   - 「お気に入り機能」列のトグルスイッチをクリック
   - 確認ダイアログで「OK」をクリック

4. API経由で確認
   ```bash
   # 管理者トークン取得
   curl -X POST http://localhost:8082/api/v1/auth/admin/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "admin001",
       "password": "adminpass123"
     }' | jq .

   export ADMIN_TOKEN="<取得したトークン>"

   # 機能フラグ一覧取得
   curl http://localhost:8082/api/v1/admin/feature-flags \
     -H "Authorization: Bearer $ADMIN_TOKEN" | jq .
   ```

5. user002で再度お気に入り登録を試みる
   ```bash
   # user002でログイン（新しいトークン取得）
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user002",
       "password": "password123"
     }' | jq .

   export TOKEN="<取得したトークン>"

   # お気に入り登録
   curl -X POST http://localhost:8081/api/v1/favorites \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 1
     }' | jq .
   ```

**期待結果**:
- Step 3: トグルスイッチがONになり、成功メッセージが表示される
- Step 4: user002のfavorite機能がONになっている
  ```json
  {
    "featureFlags": [
      {
        "userId": 2,
        "loginId": "user002",
        "featureName": "favorite",
        "isEnabled": true
      }
    ]
  }
  ```
- Step 5: 201 Created, お気に入りが正常に登録される

**実施結果**: ☐ PASS / ☐ FAIL

---

### TC-FF-004: 管理画面での機能フラグ切り替え（ON→OFF）

**目的**: フラグOFFに変更すると、既存のお気に入りは残るが新規登録が拒否されることを確認

**テスト手順**:

1. user001でお気に入りを1件登録（TC-FF-001が完了していること）

2. 管理画面でuser001のフラグをOFFに変更
   - 機能フラグ管理画面を開く
   - user001の「お気に入り機能」トグルをOFFに変更

3. user001で新規お気に入り登録を試みる
   ```bash
   # user001でログイン（新しいトークン取得）
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{
       "loginId": "user001",
       "password": "password123"
     }' | jq .

   export TOKEN="<取得したトークン>"

   # 別の商品をお気に入り登録
   curl -X POST http://localhost:8081/api/v1/favorites \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer $TOKEN" \
     -d '{
       "productId": 2
     }' | jq .
   ```

4. 既存のお気に入り一覧を取得
   ```bash
   curl http://localhost:8081/api/v1/favorites \
     -H "Authorization: Bearer $TOKEN" | jq .
   ```

5. データベースで確認
   ```sql
   SELECT * FROM favorites WHERE user_id = 1;
   ```

**期待結果**:
- Step 2: トグルがOFFになる
- Step 3: 403 Forbidden, FEATURE_001エラー
- Step 4: 403 Forbidden（フラグOFFなので一覧取得も拒否）
  - または、200 OKだが既存データは返却される（設計による）
- Step 5: 既存のお気に入りデータ（productId=1）は残っている

**実施結果**: ☐ PASS / ☐ FAIL

---

## 4. データベース直接確認

### 4.1 機能フラグテーブルの状態確認

```sql
-- 全ユーザーの機能フラグ状態
SELECT 
    ff.user_id,
    u.login_id,
    ff.feature_name,
    ff.is_enabled,
    ff.updated_at
FROM feature_flags ff
JOIN users u ON ff.user_id = u.user_id
ORDER BY ff.user_id;
```

### 4.2 お気に入りデータの確認

```sql
-- 全ユーザーのお気に入り状況
SELECT 
    f.user_id,
    u.login_id,
    f.product_id,
    p.product_name,
    f.created_at,
    ff.is_enabled as flag_enabled
FROM favorites f
JOIN users u ON f.user_id = u.user_id
JOIN products p ON f.product_id = p.product_id
JOIN feature_flags ff ON f.user_id = ff.user_id 
    AND ff.feature_name = 'favorite'
ORDER BY f.user_id, f.created_at;
```

---

## 5. エッジケースのテスト

### TC-FF-EDGE-001: 重複お気に入り登録

**手順**:
1. user001でproductId=1を登録
2. 再度同じ商品を登録

**期待結果**: 400 Bad Request, FAVORITE_001エラー

---

### TC-FF-EDGE-002: 存在しない商品のお気に入り登録

**手順**:
1. user001でproductId=999を登録

**期待結果**: 404 Not Found, FAVORITE_003エラー

---

### TC-FF-EDGE-003: お気に入り解除

**手順**:
1. user001でお気に入り登録済みの商品を取得
2. DELETE APIで解除
   ```bash
   curl -X DELETE http://localhost:8081/api/v1/favorites/1 \
     -H "Authorization: Bearer $TOKEN"
   ```

**期待結果**: 204 No Content

---

## 6. トラブルシューティング

### 問題: フラグ変更が反映されない

**原因候補**:
- トークンが古い（フラグ変更前のトークン）
- キャッシュが効いている

**解決方法**:
1. 新しいトークンを取得してテスト
2. アプリケーションの再起動

---

### 問題: 403エラーが出ない

**確認事項**:
- データベースのフラグ状態を確認
- Web APIのログを確認（機能フラグチェックが動作しているか）

---

## 7. テスト実施チェックリスト

- [ ] TC-FF-001: お気に入り登録（フラグON）
- [ ] TC-FF-002: お気に入り登録（フラグOFF）
- [ ] TC-FF-003: 管理画面でOFF→ON切り替え
- [ ] TC-FF-004: 管理画面でON→OFF切り替え
- [ ] TC-FF-EDGE-001: 重複登録エラー
- [ ] TC-FF-EDGE-002: 存在しない商品エラー
- [ ] TC-FF-EDGE-003: お気に入り解除

---

## 8. 参考情報

- [API仕様 - お気に入りAPI](../../specs/mobile-app-system/05-03-purchase-favorite-api.md)
- [機能要件 - お気に入り機能](../../specs/mobile-app-system/02-01-mobile-functional-requirements.md)
- [エラーコード一覧](../../specs/mobile-app-system/07-error-handling.md)

---

**End of Document**
