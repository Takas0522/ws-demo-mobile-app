# 価格履歴機能 改修仕様書

| 項目 | 内容 |
|------|------|
| ドキュメント番号 | DES-PRICE-HIST-001 |
| バージョン | 1.0 |
| 作成日 | 2026年3月1日 |
| ステータス | Draft |
| 設計書 | `docs/excel/価格履歴機能_設計書.xlsx` |

## 1. 概要

商品の価格変更履歴を記録・参照する機能を追加する。管理者が商品情報を更新し価格が変更された場合に、変更前後の価格・変更者・変更理由をテーブルに記録し、Windowsアプリの商品詳細画面から価格履歴を参照できるようにする。

---

## 2. テーブル定義

### 2.1 テーブル概要

| 項目 | 内容 |
|------|------|
| テーブル物理名 | `product_price_history` |
| テーブル論理名 | 商品価格履歴 |
| スキーマ | SQLite（スキーマなし） |
| 関連テーブル | `products`（商品）、`users`（ユーザー） |
| データ保持期間 | 5年間（1825日）。超過データは定期バッチで削除する。 |

### 2.2 カラム定義

| No | 論理名 | 物理名 | データ型 | NULL | PK | FK | デフォルト | 説明 |
|----|--------|--------|----------|------|----|----|-----------|------|
| 1 | 価格履歴ID | `price_history_id` | INTEGER | × | ○ | | AUTO INCREMENT | 主キー・自動採番 |
| 2 | 商品ID | `product_id` | INTEGER | × | | ○ | ─ | productsへの外部キー |
| 3 | 変更前単価 | `old_price` | INTEGER | × | | | ─ | 変更前の商品単価（円） |
| 4 | 変更後単価 | `new_price` | INTEGER | × | | | ─ | 変更後の商品単価（円） |
| 5 | 変更日時 | `changed_at` | TEXT | × | | | `datetime('now', 'localtime')` | 価格が変更された日時 |
| 6 | 変更者ID | `changed_by` | INTEGER | × | | ○ | ─ | usersへの外部キー |
| 7 | 変更理由 | `change_reason` | TEXT | ○ | | | ─ | 価格変更の理由（任意） |
| 8 | 作成日時 | `created_at` | TEXT | × | | | `datetime('now', 'localtime')` | レコード作成日時 |

### 2.3 インデックス定義

| No | インデックス名 | 対象カラム | UNIQUE | 説明 |
|----|---------------|-----------|--------|------|
| 1 | PK (price_history_id) | `price_history_id` | ─ | 主キー（自動生成） |
| 2 | `idx_price_hist_product_id` | `product_id` | ─ | 商品IDでの検索 |
| 3 | `idx_price_hist_changed_at` | `changed_at` | ─ | 変更日時の範囲検索 |
| 4 | `idx_price_hist_prod_changed` | `product_id, changed_at` | ─ | 商品別・日時複合検索 |

### 2.4 外部キー定義

| No | カラム | 参照先 | ON DELETE | 説明 |
|----|--------|--------|-----------|------|
| 1 | `product_id` | `products.product_id` | RESTRICT | 商品削除時は履歴を保護 |
| 2 | `changed_by` | `users.user_id` | RESTRICT | 変更者情報を保持 |

### 2.5 CHECK制約

| No | 制約名 | 制約内容 |
|----|--------|---------|
| 1 | `chk_old_price` | `CHECK (old_price >= 1)` ── 変更前単価は1円以上 |
| 2 | `chk_new_price` | `CHECK (new_price >= 1)` ── 変更後単価は1円以上 |
| 3 | `chk_price_changed` | `CHECK (old_price <> new_price)` ── 変更前後の値が異なること |

### 2.6 DDL

```sql
CREATE TABLE product_price_history (
    price_history_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id        INTEGER NOT NULL,
    old_price         INTEGER NOT NULL,
    new_price         INTEGER NOT NULL,
    changed_at        TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    changed_by        INTEGER NOT NULL,
    change_reason     TEXT,
    created_at        TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    CONSTRAINT fk_price_hist_product FOREIGN KEY (product_id)
        REFERENCES products(product_id) ON DELETE RESTRICT,
    CONSTRAINT fk_price_hist_user FOREIGN KEY (changed_by)
        REFERENCES users(user_id) ON DELETE RESTRICT,

    CONSTRAINT chk_old_price     CHECK (old_price >= 1),
    CONSTRAINT chk_new_price     CHECK (new_price >= 1),
    CONSTRAINT chk_price_changed CHECK (old_price <> new_price)
);

CREATE INDEX idx_price_hist_product_id
    ON product_price_history(product_id);
CREATE INDEX idx_price_hist_changed_at
    ON product_price_history(changed_at);
CREATE INDEX idx_price_hist_prod_changed
    ON product_price_history(product_id, changed_at);
```

### 2.7 トリガー（価格変更時の自動記録）

```sql
CREATE TRIGGER trg_record_price_history
AFTER UPDATE OF unit_price ON products
FOR EACH ROW
WHEN OLD.unit_price <> NEW.unit_price
BEGIN
    INSERT INTO product_price_history
        (product_id, old_price, new_price, changed_by, change_reason)
    VALUES
        (NEW.product_id, OLD.unit_price, NEW.unit_price,
         0, '商品情報更新による自動記録');
END;
```

---

## 3. API仕様（API-020: 価格履歴取得）

### 3.1 基本情報

| 項目 | 内容 |
|------|------|
| API ID | API-020 |
| API名 | 価格履歴取得 |
| エンドポイント | `GET /api/v1/products/{productId}/price-history` |
| HTTPメソッド | GET |
| 認証 | JWT認証必須（`Authorization: Bearer {TOKEN}`） |
| 権限 | user / admin |
| 概要 | 指定商品の価格変更履歴を取得する。最大5年分の履歴を日時降順で返却する。 |

### 3.2 パスパラメータ

| No | パラメータ名 | 型 | 必須 | 説明 |
|----|-------------|-----|------|------|
| 1 | `productId` | integer | ○ | 対象商品のID |

### 3.3 クエリパラメータ

| No | パラメータ名 | 型 | 必須 | デフォルト | 説明 |
|----|-------------|-----|------|-----------|------|
| 1 | `startDate` | string (ISO 8601) | ─ | 5年前の日付 | 検索開始日 (例: 2021-03-01) |
| 2 | `endDate` | string (ISO 8601) | ─ | 現在日付 | 検索終了日 (例: 2026-03-01) |
| 3 | `page` | integer | ─ | 1 | ページ番号（1以上） |
| 4 | `limit` | integer | ─ | 20 | 1ページあたり件数（最大100） |

### 3.4 レスポンス（成功時: 200 OK）

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

### 3.5 レスポンスフィールド定義

| No | フィールド | 型 | 説明 |
|----|-----------|-----|------|
| 1 | `priceHistory` | array | 価格履歴レコードの配列 |
| 2 | `priceHistory[].priceHistoryId` | integer | 価格履歴ID |
| 3 | `priceHistory[].productId` | integer | 商品ID |
| 4 | `priceHistory[].oldPrice` | integer | 変更前単価（円） |
| 5 | `priceHistory[].newPrice` | integer | 変更後単価（円） |
| 6 | `priceHistory[].changedAt` | string | 変更日時（ISO 8601形式） |
| 7 | `priceHistory[].changedBy.userId` | integer | 変更者ユーザーID |
| 8 | `priceHistory[].changedBy.userName` | string | 変更者ユーザー名 |
| 9 | `priceHistory[].changeReason` | string\|null | 変更理由（任意項目） |
| 10 | `pagination.currentPage` | integer | 現在のページ番号 |
| 11 | `pagination.totalPages` | integer | 総ページ数 |
| 12 | `pagination.totalItems` | integer | 総件数 |
| 13 | `pagination.itemsPerPage` | integer | 1ページあたりの件数 |

### 3.6 エラーレスポンス

| No | HTTP | エラーコード | メッセージ | 発生条件 |
|----|------|------------|-----------|---------|
| 1 | 400 | VAL_001 | 入力値が不正です | startDate/endDate形式不正 |
| 2 | 401 | AUTH_001 | 認証トークンが無効です | トークン未送信・不正 |
| 3 | 401 | AUTH_004 | トークンの有効期限が切れています | 有効期限切れ |
| 4 | 403 | AUTH_005 | 権限がありません | 権限なしユーザー |
| 5 | 404 | PRODUCT_001 | 商品が見つかりません | 存在しないproductId指定 |
| 6 | 500 | SYS_001 | サーバー内部エラー | 予期しないエラー |

### 3.7 BFF転送仕様

| No | BFF | BFFエンドポイント | 転送先 Web API |
|----|-----|------------------|---------------|
| 1 | Mobile | `GET /api/mobile/products/{id}/price-history` | `GET /api/v1/products/{id}/price-history` |
| 2 | Admin | `GET /api/admin/products/{id}/price-history` | `GET /api/v1/products/{id}/price-history` |

---

## 4. 改修対象ファイル一覧

### 4.1 CppApiServer（APIサーバー）

| 操作 | ファイルパス | 内容 |
|------|------------|------|
| **新規** | `src/Models/PriceHistory.h` | 価格履歴データモデル |
| **新規** | `src/Database/PriceHistoryRepository.h` | 価格履歴リポジトリ（ヘッダー） |
| **新規** | `src/Database/PriceHistoryRepository.cpp` | 価格履歴リポジトリ（実装） |
| **新規** | `src/Services/PriceHistoryService.h` | 価格履歴サービス（ヘッダー） |
| **新規** | `src/Services/PriceHistoryService.cpp` | 価格履歴サービス（実装） |
| **変更** | `src/Routes/ProductRoutes.h` | 価格履歴取得ハンドラ追加 |
| **変更** | `src/Routes/ProductRoutes.cpp` | 価格履歴取得エンドポイント実装 |
| **変更** | `src/Database/DatabaseManager.cpp` | DDL にテーブル・インデックス・トリガー追加、シードデータ追加 |
| **変更** | `src/Server.h` | PriceHistoryRepository / PriceHistoryService メンバ追加 |
| **変更** | `src/Server.cpp` | DI 構成にリポジトリ・サービス追加 |
| **変更** | `CppApiServer.vcxproj` | 新規ファイル登録 |

### 4.2 WindowsApp（Windowsアプリケーション）

| 操作 | ファイルパス | 内容 |
|------|------------|------|
| **新規** | `src/Models/PriceHistory.h` | 価格履歴データモデル（クライアント側） |
| **新規** | `src/Services/PriceHistoryService.h` | 価格履歴取得サービス（ヘッダー） |
| **新規** | `src/Services/PriceHistoryService.cpp` | 価格履歴取得サービス（実装） |
| **新規** | `src/ViewModels/PriceHistoryViewModel.h` | 価格履歴ViewModel（ヘッダー） |
| **新規** | `src/ViewModels/PriceHistoryViewModel.cpp` | 価格履歴ViewModel（実装） |
| **新規** | `src/Views/PriceHistoryWindow.h` | 価格履歴ウィンドウ（ヘッダー） |
| **新規** | `src/Views/PriceHistoryWindow.cpp` | 価格履歴ウィンドウ（実装） |
| **変更** | `src/Utils/Constants.h` | 価格履歴APIパス定数追加 |
| **変更** | `src/Views/ProductDetailWindow.h` | 「価格履歴」ボタン追加 |
| **変更** | `src/Views/ProductDetailWindow.cpp` | 「価格履歴」ボタン生成・ハンドラ |
| **変更** | `src/App.h` | PriceHistoryService / ViewModel / Window メンバ追加 |
| **変更** | `src/App.cpp` | 初期化・画面遷移ロジック追加 |
| **変更** | `WsDemoMobileApp.WindowsApp.vcxproj` | 新規ファイル登録 |

---

## 5. 備考

1. 価格履歴は商品更新API（API-013: `PUT /api/v1/products/{id}`）で価格変更が行われた際に、サーバー側で自動的に `product_price_history` テーブルに記録される。
2. データ保持期間は5年間。超過データは定期バッチ（日次実行）にて物理削除する。
3. `startDate` / `endDate` を省略した場合、保持期間内の全履歴が返却される。
4. レスポンスは `changed_at` の降順（新しい順）でソートされる。
5. 管理者・一般ユーザーともに参照可能。価格変更操作は管理者のみ。
6. ページネーション仕様は既存の商品一覧API（API-010）と同一。
7. 本APIは既存のAPI体系（`/api/v1/`）に準拠して設計されている。CppApiServerでは `/api/mobile/` プレフィックスで提供する。
