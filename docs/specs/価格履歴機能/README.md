# 価格履歴機能 ─ Windowsアプリ改修仕様

> **設計書:** `docs/excel/価格履歴機能_設計書.xlsx` (DES-PRICE-HIST-001 v1.0)

## 1. 機能概要

商品詳細画面に「価格履歴」セクションを追加し、選択中の商品について過去の価格変更履歴を一覧表示する。
Web API（CppApiServer）は改修済みのため、Windowsアプリ（クライアント側）のレイアウト変更と API 呼び出し実装のみが対象となる。

---

## 2. API仕様（参照用 ─ 改修済み）

| 項目 | 内容 |
|---|---|
| API ID | API-020 |
| エンドポイント | `GET /api/mobile/products/{productId}/price-history` |
| 認証 | JWT 認証必須（`Authorization: Bearer {TOKEN}`） |
| 権限 | user / admin |

### 2.1 クエリパラメータ

| パラメータ | 型 | 必須 | デフォルト | 説明 |
|---|---|---|---|---|
| startDate | string (ISO 8601) | ─ | 5年前の日付 | 検索開始日 (例: 2021-03-01) |
| endDate | string (ISO 8601) | ─ | 現在日付 | 検索終了日 (例: 2026-03-01) |
| page | integer | ─ | 1 | ページ番号（1以上） |
| limit | integer | ─ | 20 | 1ページあたり件数（最大100） |

### 2.2 レスポンス（200 OK）

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

### 2.3 レスポンスフィールド

| フィールド | 型 | 説明 |
|---|---|---|
| priceHistory | array | 価格履歴レコードの配列 |
| priceHistory[].priceHistoryId | integer | 価格履歴ID |
| priceHistory[].productId | integer | 商品ID |
| priceHistory[].oldPrice | integer | 変更前単価（円） |
| priceHistory[].newPrice | integer | 変更後単価（円） |
| priceHistory[].changedAt | string | 変更日時（ISO 8601形式） |
| priceHistory[].changedBy.userId | integer | 変更者ユーザーID |
| priceHistory[].changedBy.userName | string | 変更者ユーザー名 |
| priceHistory[].changeReason | string\|null | 変更理由（任意項目） |
| pagination.currentPage | integer | 現在のページ番号 |
| pagination.totalPages | integer | 総ページ数 |
| pagination.totalItems | integer | 総件数 |
| pagination.itemsPerPage | integer | 1ページあたりの件数 |

### 2.4 エラーレスポンス

| HTTP | コード | メッセージ | 発生条件 |
|---|---|---|---|
| 400 | VAL_001 | 入力値が不正です | startDate/endDate 形式不正 |
| 401 | AUTH_001 | 認証トークンが無効です | トークン未送信・不正 |
| 401 | AUTH_004 | トークンの有効期限が切れています | 有効期限切れ |
| 403 | AUTH_005 | 権限がありません | 権限なしユーザー |
| 404 | PRODUCT_001 | 商品が見つかりません | 存在しない productId 指定 |
| 500 | SYS_001 | サーバー内部エラー | 予期しないエラー |

---

## 3. テーブル定義（参照用 ─ 改修済み）

| 物理名 | product_price_history |
|---|---|
| 論理名 | 商品価格履歴 |
| スキーマ | SQLite（スキーマなし） |

### カラム定義

| No | 論理名 | 物理名 | データ型 | NULL | PK | FK | デフォルト | 説明 |
|---|---|---|---|---|---|---|---|---|
| 1 | 価格履歴ID | price_history_id | INTEGER | × | ○ | | AUTO INCREMENT | 主キー・自動採番 |
| 2 | 商品ID | product_id | INTEGER | × | | ○ | ─ | products への外部キー |
| 3 | 変更前単価 | old_price | INTEGER | × | | | ─ | 変更前の商品単価（円） |
| 4 | 変更後単価 | new_price | INTEGER | × | | | ─ | 変更後の商品単価（円） |
| 5 | 変更日時 | changed_at | TEXT | × | | | datetime(now) | 価格が変更された日時 |
| 6 | 変更者ID | changed_by | INTEGER | × | | ○ | ─ | users への外部キー |
| 7 | 変更理由 | change_reason | TEXT | ○ | | | ─ | 価格変更の理由（任意） |
| 8 | 作成日時 | created_at | TEXT | × | | | datetime(now) | レコード作成日時 |

---

## 4. Windowsアプリ改修内容

### 4.1 新規追加ファイル

#### Models

| ファイル | 説明 |
|---|---|
| `src/Models/PriceHistory.h` | 価格履歴モデル（`PriceHistory` 構造体 + JSON デシリアライズ） |

#### Services

| ファイル | 説明 |
|---|---|
| `src/Services/IPriceHistoryService.h` | 価格履歴サービスインターフェース |
| `src/Services/PriceHistoryService.h` | 価格履歴サービス実装ヘッダー |
| `src/Services/PriceHistoryService.cpp` | 価格履歴サービス実装（API 呼び出しロジック） |

### 4.2 既存ファイルの改修

#### ViewModels

| ファイル | 改修内容 |
|---|---|
| `src/ViewModels/ProductDetailViewModel.h` | 価格履歴データ保持メンバー・取得メソッド・コールバック追加 |
| `src/ViewModels/ProductDetailViewModel.cpp` | `FetchPriceHistory()` メソッド実装 |

#### Views（レイアウト変更）

| ファイル | 改修内容 |
|---|---|
| `src/Views/ProductDetailWindow.h` | 価格履歴リスト用コントロール ID・HWND メンバー追加 |
| `src/Views/ProductDetailWindow.cpp` | 価格履歴セクション UI の作成・描画・更新ロジック追加 |

#### App

| ファイル | 改修内容 |
|---|---|
| `src/App.h` | `PriceHistoryService` メンバー追加 |
| `src/App.cpp` | サービス初期化・ViewModel への注入 |

#### Constants

| ファイル | 改修内容 |
|---|---|
| `src/Utils/Constants.h` | 価格履歴 API エンドポイント定数追加 |

### 4.3 レイアウト変更詳細（ProductDetailWindow）

商品詳細画面の「購入する」ボタンの上に価格履歴セクションを挿入する。

#### 現在のレイアウト

```
┌──────────────────────────┐
│ [← 戻る]     [♡ お気に入り] │  y=12, h=34
├──────────────────────────┤  y=52 (divider)
│                          │
│    📷 商品画像            │  y=56, h=200
│                          │
├──────────────────────────┤
│ 商品名                    │  y=270, h=30
│ ¥ 価格                   │  y=308, h=30
│ 商品説明                  │  y=350, h=180
│                          │
├──────────────────────────┤  y=586 (divider)
│ [      購入する      ]     │  y=600, h=48
└──────────────────────────┘
```

#### 改修後のレイアウト

```
┌──────────────────────────┐
│ [← 戻る]     [♡ お気に入り] │  y=12, h=34
├──────────────────────────┤  y=52 (divider)
│                          │
│    📷 商品画像            │  y=56, h=200
│                          │
├──────────────────────────┤
│ 商品名                    │  y=270, h=30
│ ¥ 価格                   │  y=308, h=30
│ 商品説明                  │  y=350, h=80  ← 縮小
│                          │
├──────────────────────────┤  y=438 (divider)
│ 📊 価格履歴               │  y=446, h=24  (セクションタイトル)
│┌────────────────────────┐│
││ 2025/06/15  ¥1,000→¥1,200 ││  各行 h=20
││ 理由: 原材料費高騰...     ││
││ 2025/01/10  ¥800→¥1,000  ││
││ ...                      ││
│└────────────────────────┘│  y=474, h=110 (リスト領域)
│                          │
├──────────────────────────┤  y=590 (divider)
│ [      購入する      ]     │  y=600, h=48
└──────────────────────────┘
```

#### 価格履歴リスト各行の表示形式

```
{changedAt日付}  ¥{oldPrice} → ¥{newPrice}
```

- `changedAt` は `YYYY/MM/DD` 形式に変換して表示
- 履歴が存在しない場合は「価格変更履歴はありません」と表示
- 最大5件まで表示（デフォルト limit=5）

### 4.4 モデル定義（PriceHistory.h）

```cpp
struct ChangedBy
{
    int64_t userId = 0;
    std::string userName;
};

struct PriceHistory
{
    int64_t priceHistoryId = 0;
    int64_t productId = 0;
    int oldPrice = 0;
    int newPrice = 0;
    std::string changedAt;
    ChangedBy changedBy;
    std::optional<std::string> changeReason;
};
```

### 4.5 サービスインターフェース（IPriceHistoryService.h）

```cpp
struct PriceHistoryResponse
{
    std::vector<PriceHistory> priceHistory;
    Pagination pagination;
};

class IPriceHistoryService
{
public:
    virtual ~IPriceHistoryService() = default;

    [[nodiscard]] virtual std::expected<PriceHistoryResponse, ApiError>
        FetchPriceHistory(int64_t productId, int page = 1, int limit = 5) const = 0;
};
```

### 4.6 API エンドポイント定数

```cpp
// Constants.h に追加
// 価格履歴APIパスを動的に生成するため、テンプレートとして使用
// 実際のパス: /api/mobile/products/{id}/price-history
constexpr const char* kPriceHistoryPathSuffix = "/price-history";
```

### 4.7 ViewModel 変更（ProductDetailViewModel）

追加メンバー:
- `IPriceHistoryService& m_priceHistoryService` ─ 価格履歴サービス参照
- `std::vector<PriceHistory> m_priceHistory` ─ 取得した価格履歴データ
- `PriceHistoryChangedCallback m_onPriceHistoryChanged` ─ UI 更新通知

追加メソッド:
- `FetchPriceHistory(int64_t productId)` ─ 非同期で価格履歴を取得
- `GetPriceHistory()` ─ 保持中の価格履歴データを返却
- `SetOnPriceHistoryChanged(callback)` ─ コールバック登録

---

## 5. 処理フロー

```
ユーザー操作: 商品詳細画面を開く
  │
  ├─ ProductDetailViewModel::FetchProductDetail(productId)  ... 既存
  │     └─ 成功 → OnProductChanged → UpdateUI()
  │
  └─ ProductDetailViewModel::FetchPriceHistory(productId)   ... 新規
        └─ 非同期スレッドで実行
              │
              ├─ PriceHistoryService::FetchPriceHistory()
              │     └─ GET /api/mobile/products/{id}/price-history?limit=5
              │
              ├─ 成功 → m_priceHistory に格納
              │         → OnPriceHistoryChanged コールバック
              │         → PostMessage(WM_APP + 102)
              │         → UpdatePriceHistoryUI()
              │
              └─ 失敗 → OnError コールバック（エラーダイアログ表示）
```

---

## 6. 備考

- 価格履歴は商品更新API（API-013: `PUT /api/v1/products/{id}`）で価格変更が行われた際に、サーバー側で自動的に `product_price_history` テーブルに記録される
- データ保持期間は5年間。超過データは定期バッチ（日次実行）にて物理削除される
- レスポンスは `changed_at` の降順（新しい順）でソートされる
- BFF 転送仕様: Mobile BFF (`GET /api/mobile/products/{id}/price-history`) → Web API (`GET /api/v1/products/{id}/price-history`)
