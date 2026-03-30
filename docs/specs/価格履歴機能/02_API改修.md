# 価格履歴機能 — API 改修

> 出典: `docs/excel/価格履歴機能_設計書.xlsx` — API仕様書シート（API-020）

## 1. 設計書と現行実装の差分一覧

| #   | 項目                      | 現行実装                                           | 設計書（API-020）                                         | 改修種別 |
| --- | ------------------------- | -------------------------------------------------- | --------------------------------------------------------- | -------- |
| 1   | エンドポイント            | `GET /api/v1/products-price-history?productId=xxx` | `GET /api/v1/products/{productId}/price-history`          | **変更** |
| 2   | パスパラメータ            | なし（クエリパラメータ `productId`）               | `{productId}`（パスパラメータ）                           | **変更** |
| 3   | 日付フィルタ              | なし                                               | `startDate`, `endDate`（ISO 8601）                        | **追加** |
| 4   | ページネーション          | なし（全件返却）                                   | `page`, `limit`（デフォルト: 1, 20）                      | **追加** |
| 5   | `changedBy` フィールド    | `integer`（ユーザー ID のみ）                      | `object`（`userId` + `userName`）                         | **変更** |
| 6   | `pagination` オブジェクト | なし                                               | `currentPage`, `totalPages`, `totalItems`, `itemsPerPage` | **追加** |
| 7   | エラーコード              | `PROD_001`                                         | `PRODUCT_001`, `VAL_001` 等                               | **変更** |
| 8   | BFF 転送仕様              | なし                                               | Mobile BFF / Admin BFF エンドポイント定義                 | **追加** |

---

## 2. 改修詳細

### 2.1 エンドポイント変更

**現行:**

```
GET /api/v1/products-price-history?productId=1
```

**設計書:**

```
GET /api/v1/products/{productId}/price-history
```

#### struts-api.xml の変更

現行のアクション定義:

```xml
<action name="products-price-history" class="...ApiProductAction" method="priceHistory">
    <interceptor-ref name="apiSecuredStack"/>
</action>
```

Struts 2.3 では RESTful なパスパラメータ（`{productId}`）のネイティブサポートが限定的なため、以下のいずれかの方式で対応する必要がある:

**方式 A: ワイルドカードマッピング**

```xml
<action name="products/*/price-history" class="...ApiProductAction" method="priceHistory">
    <param name="productId">{1}</param>
    <interceptor-ref name="apiSecuredStack"/>
</action>
```

**方式 B: 既存パラメータ方式を維持（互換性重視）**

既存の `?productId=xxx` 方式を維持しつつ、新エンドポイントはパスパラメータから取得するラッパーメソッドを追加する。

> **推奨:** 方式 A（ワイルドカードマッピング）。設計書の仕様に準拠。

---

### 2.2 クエリパラメータの追加

設計書で新たに定義されたクエリパラメータ:

| パラメータ  | 型                | 必須   | デフォルト   | 説明                           |
| ----------- | ----------------- | ------ | ------------ | ------------------------------ |
| `startDate` | string (ISO 8601) | いいえ | 5 年前の日付 | 検索開始日                     |
| `endDate`   | string (ISO 8601) | いいえ | 現在日付     | 検索終了日                     |
| `page`      | integer           | いいえ | 1            | ページ番号（1 以上）           |
| `limit`     | integer           | いいえ | 20           | 1 ページあたり件数（最大 100） |

#### ApiProductAction への追加フィールド

```java
// 追加が必要なフィールド（getter/setter 含む）
private String startDate;
private String endDate;
private Integer page;
private Integer limit;
```

---

### 2.3 レスポンス構造の変更

#### 現行レスポンス

```json
{
  "data": {
    "priceHistory": [
      {
        "priceHistoryId": 1,
        "productId": 1,
        "oldPrice": 1000,
        "newPrice": 1200,
        "changedAt": "2025-06-15 10:30:00",
        "changedBy": 3,
        "changeReason": "原材料費高騰に伴う価格改定"
      }
    ]
  },
  "timestamp": "2026-03-01T12:00:00"
}
```

#### 設計書のレスポンス

```json
{
  "data": {
    "priceHistory": [
      {
        "priceHistoryId": 1,
        "productId": 1,
        "oldPrice": 1000,
        "newPrice": 1200,
        "changedAt": "2025-06-15T10:30:00",
        "changedBy": {
          "userId": 3,
          "userName": "管理者"
        },
        "changeReason": "原材料費高騰に伴う価格改定"
      }
    ],
    "pagination": {
      "currentPage": 1,
      "totalPages": 3,
      "totalItems": 45,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2026-03-01T12:00:00Z"
}
```

#### 主要な差分

| 項目             | 現行                    | 設計書                                                  | 改修内容                                |
| ---------------- | ----------------------- | ------------------------------------------------------- | --------------------------------------- |
| `changedBy`      | `integer`（ID のみ）    | `object { userId, userName }`                           | UserDao で`userName` を取得して構造変更 |
| `changedAt` 形式 | `"2025-06-15 10:30:00"` | `"2025-06-15T10:30:00"`                                 | ISO 8601 形式に変換                     |
| `pagination`     | なし                    | `{ currentPage, totalPages, totalItems, itemsPerPage }` | ページネーション情報を追加              |

---

### 2.4 DAO 層の改修

`ProductPriceHistoryDao` に以下のメソッドを追加/変更する必要がある:

#### 現行 SQL

```sql
SELECT * FROM product_price_history
WHERE product_id = ?
ORDER BY changed_at DESC
```

#### 設計書準拠の SQL（日付フィルタ + ページネーション）

```sql
-- 件数カウント（ページネーション用）
SELECT COUNT(*) FROM product_price_history
WHERE product_id = ?
AND changed_at >= ?
AND changed_at <= ?

-- データ取得（ページネーション付き）
SELECT * FROM product_price_history
WHERE product_id = ?
AND changed_at >= ?
AND changed_at <= ?
ORDER BY changed_at DESC
LIMIT ? OFFSET ?
```

#### 追加メソッド

```java
// ページネーション・日付フィルタ対応の検索
List<ProductPriceHistory> findByProductIdWithFilter(
    Long productId, String startDate, String endDate, int limit, int offset
) throws SQLException;

// 総件数カウント
int countByProductIdWithFilter(
    Long productId, String startDate, String endDate
) throws SQLException;
```

---

### 2.5 Service 層の改修

`ProductService` に以下のメソッドを追加する:

```java
// ページネーション・日付フィルタ対応
Map<String, Object> getPriceHistoryWithPagination(
    Long productId, String startDate, String endDate, int page, int limit
);
```

処理内容:

1. `startDate` / `endDate` のデフォルト値設定（5 年前 / 現在日付）
2. `page` / `limit` のバリデーション（page >= 1, 1 <= limit <= 100）
3. 総件数カウント
4. ページネーション情報の計算（totalPages, currentPage 等）
5. OFFSET 計算: `(page - 1) * limit`
6. データ取得
7. `changedBy`（ユーザー ID）から `userName` を取得（`UserDao.findById()` を使用）
8. 結果の Map 構築

---

### 2.6 エラーレスポンス定義

| No  | HTTP | エラーコード  | メッセージ                       | 発生条件                   |
| --- | ---- | ------------- | -------------------------------- | -------------------------- |
| 1   | 400  | `VAL_001`     | 入力値が不正です                 | startDate/endDate 形式不正 |
| 2   | 401  | `AUTH_001`    | 認証トークンが無効です           | トークン未送信・不正       |
| 3   | 401  | `AUTH_004`    | トークンの有効期限が切れています | 有効期限切れ               |
| 4   | 403  | `AUTH_005`    | 権限がありません                 | 権限なしユーザー           |
| 5   | 404  | `PRODUCT_001` | 商品が見つかりません             | 存在しない productId 指定  |
| 6   | 500  | `SYS_001`     | サーバー内部エラー               | 予期しないエラー           |

> **注意:** 現行のエラーコード（`PROD_001`, `PROD_002`）と設計書のエラーコード（`PRODUCT_001`）が異なる。既存 API との一貫性を考慮して判断が必要。

---

### 2.7 該当なし時のレスポンス

設計書では、履歴が 0 件の場合も 200 OK で空配列とページネーション情報を返却する:

```json
{
  "data": {
    "priceHistory": [],
    "pagination": {
      "currentPage": 1,
      "totalPages": 0,
      "totalItems": 0,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2026-03-01T12:00:00Z"
}
```

---

## 3. BFF 転送仕様

設計書で新たに定義された BFF 転送仕様:

| No  | BFF        | BFF エンドポイント                            | 転送先 Web API                            |
| --- | ---------- | --------------------------------------------- | ----------------------------------------- |
| 1   | Mobile BFF | `GET /api/mobile/products/{id}/price-history` | `GET /api/v1/products/{id}/price-history` |
| 2   | Admin BFF  | `GET /api/admin/products/{id}/price-history`  | `GET /api/v1/products/{id}/price-history` |

> **注意:** admin-struts アプリケーションは直接 DB アクセスするため、BFF 転送仕様は Mobile BFF（`src/mobile-bff/`）および Admin BFF（`src/admin-bff/`）側の改修対象であり、admin-struts 自体の改修スコープ外。ただし admin-struts が提供する REST API が設計書の仕様に準拠している必要がある。

---

## 4. 備考・補足事項

設計書 API 仕様書シートからの補足:

1. 価格履歴は商品更新 API（API-013: `PUT /api/v1/products/{id}`）で価格変更が行われた際に、サーバー側で自動的に `product_price_history` テーブルに記録される → **既にトリガーで実装済み**
2. データ保持期間は 5 年間。超過データは定期バッチ（日次実行）にて物理削除する → **バッチ処理は別途開発が必要**
3. `startDate` / `endDate` を省略した場合、保持期間内の全履歴が返却される → **デフォルト値: 5 年前 / 現在日付**
4. レスポンスは `changed_at` の降順（新しい順）でソートされる → **既に実装済み**
5. 管理者・一般ユーザーともに参照可能。価格変更操作は管理者のみ → **既に認証は実装済みだが、404 チェック（商品存在確認）を追加する必要あり**
6. ページネーション仕様は既存の商品一覧 API（API-010）と同一 → **現行には商品一覧のページネーションも未実装のため、新規に実装する**
7. 本 API は既存の API 体系（`/api/v1/`）に準拠して設計されている → **namespace は一致**

---

## 5. 改修の優先順位

| 優先度 | 改修項目                                       | 理由                           |
| ------ | ---------------------------------------------- | ------------------------------ |
| 高     | エンドポイント変更 + パスパラメータ対応        | 設計書の基本仕様               |
| 高     | レスポンス構造変更（changedBy オブジェクト化） | クライアント側の表示要件に直結 |
| 高     | ページネーション対応                           | 大量データへのスケーラビリティ |
| 中     | 日付フィルタリング対応                         | ユーザビリティ向上             |
| 中     | 商品存在チェック（404 対応）                   | エラーハンドリングの充実       |
| 低     | エラーコード体系の統一                         | 既存 API との整合性確認が必要  |
| 低     | BFF 転送仕様の実装                             | admin-struts のスコープ外      |
| 低     | データ保持ポリシー（バッチ）                   | 運用要件、別途開発             |
