# 価格履歴機能 ─ モバイルアプリ改修仕様

> **設計書:** `docs/excel/価格履歴機能_設計書.xlsx` (DES-PRICE-HIST-001 v1.0)

## 1. 機能概要

商品詳細画面に「価格履歴」セクションを追加し、選択中の商品について過去の価格変更履歴を一覧表示する。
Mobile BFFは改修済みのため、モバイルアプリ（Android / iOS）のレイアウト変更と API 呼び出し実装のみが対象となる。

### コーディング規則（全ファイル共通）

#### Android（Java）
- ViewBinding を使用する（`findViewById` 禁止）
- パッケージ構成: `com.example.mobileapp.{ui, models, services, utils}`
- JSON デシリアライズ: Gson を使用し、各モデルクラスにフィールド定義とゲッターを実装
- エラー処理: Retrofit の `Callback<T>` で `onResponse` / `onFailure` を実装
- 非同期処理: Retrofit の非同期 API（`enqueue`）を使用
- UIスレッドへの通知: コールバック内は自動的にUIスレッドで実行される（Retrofit仕様）

#### iOS（Swift）
- SwiftUI を使用し、ViewModel は `@MainActor` + `ObservableObject` に準拠
- ディレクトリ構成: `Views/`, `Models/`, `Services/`, `Utilities/`
- JSON デシリアライズ: `Codable` プロトコルを使用
- エラー処理: `async throws` 関数で実装し、呼び出し側で `do-catch` でハンドリング
- 非同期処理: `async/await` パターンを使用
- UIスレッドへの通知: `@MainActor` により自動的にメインスレッドで実行される

---

## 2. API仕様（参照用 ─ Mobile BFF改修済み）

| 項目 | 内容 |
|---|---|
| API ID | API-020 |
| エンドポイント | `GET /api/mobile/products/{productId}/price-history` |
| 認証 | JWT 認証必須（`Authorization: Bearer {TOKEN}`） |
| 権限 | user / admin |
| 備考 | Mobile BFFが内部的にWeb API（`GET /api/v1/products/{id}/price-history`）に転送 |

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
| priceHistory[].changeReason | string\|null | 変更理由（任意項目、null の場合あり） |
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

## 4. モバイルアプリ改修内容

### 4.1 新規追加ファイル

#### Android

| ファイル | 説明 |
|---|---|
| `models/PriceHistory.java` | 価格履歴モデル（Gsonでデシリアライズ） |
| `models/ChangedBy.java` | 変更者情報モデル |
| `models/Pagination.java`（※） | ページネーション情報モデル（既存の場合は改修のみ） |
| `models/PriceHistoryResponse.java` | API レスポンスモデル |

※ `Pagination.java` が既存の場合は `totalItems` / `itemsPerPage` フィールドを追加する。

#### iOS

| ファイル | 説明 |
|---|---|
| `Models/PriceHistory.swift` | 価格履歴モデル（Codable準拠） |
| `Models/Pagination.swift`（※） | ページネーション情報モデル（既存の場合は改修のみ） |

※ `Pagination.swift` が既存の場合は `totalItems` / `itemsPerPage` プロパティを追加する。

### 4.2 既存ファイルの改修

#### Android

| ファイル | 改修内容 |
|---|---|
| `services/ApiService.java` | 価格履歴取得APIエンドポイント追加（`@GET("/api/mobile/products/{id}/price-history")`） |
| `ui/ProductDetailActivity.java` | 価格履歴リスト表示用コントロール追加・データ取得処理実装 |
| `res/layout/activity_product_detail.xml` | 価格履歴表示用 ListView またはセクションラベル追加 |

#### iOS

| ファイル | 改修内容 |
|---|---|
| `Services/APIClient.swift` | 価格履歴取得メソッド追加（`fetchPriceHistory(productId:page:limit:)`） |
| `Views/ProductDetailView.swift` | 価格履歴表示用 List セクション追加・ViewModel との連携実装 |
| `Views/ProductDetailView.swift` 内の ViewModel | 価格履歴データ保持・取得処理追加 |

### 4.3 レイアウト変更詳細

商品詳細画面の「購入する」ボタンの上に価格履歴セクションを挿入する。

#### Android レイアウト方針

**`activity_product_detail.xml` の変更点**:

1. 既存の商品説明 `TextView (id: tv_description)` の高さを縮小（`wrap_content` または固定高さに変更）
2. 商品説明と購入ボタンの間に以下を追加:
   - セクションタイトル: `TextView (id: tv_price_history_title)` — テキスト: `"📊 価格履歴"`
   - 仕切り線: `View (id: divider_price_history)` — 高さ 1dp、背景色 `#E0E0E0`
   - 価格履歴リスト: `ListView (id: lv_price_history)` — 高さ `wrap_content`、最大 5 項目表示

**ListView の各項目フォーマット**:
- テキスト形式: `YYYY/MM/DD  ¥{oldPrice} → ¥{newPrice}`
- 例: `2025/06/15  ¥1,000 → ¥1,200`
- Adapter: `ArrayAdapter<String>` を使用

**履歴が存在しない場合**:
- `tv_price_history_title` のテキストを `"価格変更履歴はありません"` に変更
- `lv_price_history` を非表示（`setVisibility(View.GONE)`）

#### iOS レイアウト方針

**`ProductDetailView.swift` の変更点**:

1. 既存の商品説明 `Text` の下（Divider の上）に新しいセクションを追加:
   - セクションタイトル: `Text("📊 価格履歴")`、フォント: `.headline`
   - 価格履歴リスト: `List` または `VStack` + `ForEach` で履歴項目を表示
   - 各項目: `HStack` で日付と価格変更を横並びに配置

**List の各項目フォーマット**:
- 左側: `Text("YYYY/MM/DD")` — フォント: `.body`
- 右側: `Text("¥{oldPrice} → ¥{newPrice}")` — フォント: `.body`、色: `.secondary`

**履歴が存在しない場合**:
- セクションタイトルを `Text("価格変更履歴はありません")` に変更
- リスト部分は表示しない（条件付きレンダリング: `if !viewModel.priceHistory.isEmpty`）

#### 共通表示ルール

- 最大表示件数: **5件**（API呼び出し時に `limit=5` を指定）
- 日付フォーマット: `YYYY/MM/DD`（`changedAt` の ISO 8601 文字列 `"2025-06-15T10:30:00"` から先頭 10 文字を取得し、`-` を `/` に置換）
- 価格フォーマット: `¥{価格}` 形式（千円単位のカンマなしでOK）
- ソート順: `changed_at` の降順（新しい順）— API側で保証されている

### 4.4 モデル定義

#### Android（Java）

**ChangedBy.java**:

```java
package com.example.mobileapp.models;

public class ChangedBy {
    private long userId;
    private String userName;

    public long getUserId() {
        return userId;
    }

    public String getUserName() {
        return userName;
    }
}
```

**PriceHistory.java**:

```java
package com.example.mobileapp.models;

public class PriceHistory {
    private long priceHistoryId;
    private long productId;
    private int oldPrice;
    private int newPrice;
    private String changedAt;
    private ChangedBy changedBy;
    private String changeReason; // nullable

    public long getPriceHistoryId() {
        return priceHistoryId;
    }

    public long getProductId() {
        return productId;
    }

    public int getOldPrice() {
        return oldPrice;
    }

    public int getNewPrice() {
        return newPrice;
    }

    public String getChangedAt() {
        return changedAt;
    }

    public ChangedBy getChangedBy() {
        return changedBy;
    }

    public String getChangeReason() {
        return changeReason;
    }
}
```

**Pagination.java**（既存の場合は追加のみ）:

```java
package com.example.mobileapp.models;

public class Pagination {
    private int currentPage;
    private int totalPages;
    private int totalCount;    // 既存フィールド
    private int limit;         // 既存フィールド
    private Integer totalItems;   // 新規追加
    private Integer itemsPerPage; // 新規追加

    // ゲッター（既存のものは省略）
    public Integer getTotalItems() {
        return totalItems != null ? totalItems : totalCount;
    }

    public Integer getItemsPerPage() {
        return itemsPerPage != null ? itemsPerPage : limit;
    }
}
```

**PriceHistoryData.java**:

```java
package com.example.mobileapp.models;

import java.util.List;

public class PriceHistoryData {
    private List<PriceHistory> priceHistory;
    private Pagination pagination;

    public List<PriceHistory> getPriceHistory() {
        return priceHistory;
    }

    public Pagination getPagination() {
        return pagination;
    }
}
```

**PriceHistoryResponse.java**:

```java
package com.example.mobileapp.models;

public class PriceHistoryResponse {
    private PriceHistoryData data;
    private String timestamp;

    public PriceHistoryData getData() {
        return data;
    }

    public String getTimestamp() {
        return timestamp;
    }
}
```

#### iOS（Swift）

**PriceHistory.swift**（`Models/` ディレクトリに作成）:

```swift
import Foundation

struct ChangedBy: Codable {
    let userId: Int
    let userName: String
}

struct PriceHistory: Codable, Identifiable {
    let priceHistoryId: Int
    let productId: Int
    let oldPrice: Int
    let newPrice: Int
    let changedAt: String
    let changedBy: ChangedBy
    let changeReason: String?
    
    var id: Int {
        priceHistoryId
    }
    
    // 日付フォーマット用ヘルパー
    var formattedDate: String {
        let isoDate = String(changedAt.prefix(10)) // "2025-06-15T10:30:00" → "2025-06-15"
        return isoDate.replacingOccurrences(of: "-", with: "/")
    }
}

struct PriceHistoryData: Codable {
    let priceHistory: [PriceHistory]
    let pagination: Pagination
}

struct PriceHistoryResponse: Codable {
    let data: PriceHistoryData
    let timestamp: String
}
```

**Pagination.swift の改修**（既存の場合は追加のみ）:

```swift
struct Pagination: Codable {
    let currentPage: Int
    let totalPages: Int
    let totalCount: Int?      // 既存フィールド（APIによって異なる）
    let limit: Int?           // 既存フィールド
    let totalItems: Int?      // 新規追加
    let itemsPerPage: Int?    // 新規追加
}
```

### 4.5 API サービス実装

#### Android（ApiService.java の改修）

**ApiService.java** に以下のメソッドを追加:

```java
@GET("/api/mobile/products/{id}/price-history")
Call<PriceHistoryResponse> getPriceHistory(
    @Path("id") int productId,
    @Query("page") int page,
    @Query("limit") int limit
);
```

#### iOS（APIClient.swift の改修）

**APIClient.swift** に以下のメソッドを追加:

```swift
func fetchPriceHistory(productId: Int, page: Int = 1, limit: Int = 5) async throws -> PriceHistoryData {
    guard let token = authService.getToken() else {
        throw APIError.unauthorized
    }
    
    guard let url = URL(string: "\(baseURL)/api/mobile/products/\(productId)/price-history?page=\(page)&limit=\(limit)") else {
        throw APIError.invalidURL
    }
    
    var request = URLRequest(url: url)
    request.httpMethod = "GET"
    request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")
    request.setValue("application/json", forHTTPHeaderField: "Content-Type")
    
    let (data, response) = try await URLSession.shared.data(for: request)
    
    guard let httpResponse = response as? HTTPURLResponse else {
        throw APIError.invalidResponse
    }
    
    guard httpResponse.statusCode == 200 else {
        if let apiError = try? JSONDecoder().decode(APIErrorResponse.self, from: data) {
            throw APIError.serverError(apiError.error.message)
        }
        throw APIError.serverError("価格履歴の取得に失敗しました")
    }
    
    let priceHistoryResponse = try JSONDecoder().decode(PriceHistoryResponse.self, from: data)
    return priceHistoryResponse.data
}
```

### 4.6 UI実装

#### Android（ProductDetailActivity.java の改修）

**変更点**:

1. クラスフィールドに価格履歴データを保持する変数を追加:
```java
private List<PriceHistory> priceHistoryList = new ArrayList<>();
```

2. `onCreate()` 内の `setupViews()` で価格履歴リストの初期化を追加（レイアウトファイルに ListView を追加した後）:
```java
// ListView の参照を取得
ListView lvPriceHistory = binding.lvPriceHistory;
```

3. `loadProductDetail()` の成功時コールバック内に価格履歴取得を追加:
```java
if (response.isSuccessful() && response.body() != null) {
    // ... 既存の商品詳細表示処理 ...
    
    // 価格履歴を取得
    loadPriceHistory(productId);
}
```

4. 新規メソッド `loadPriceHistory(int productId)` を実装:
```java
private void loadPriceHistory(int productId) {
    ApiClient.getApiService().getPriceHistory(productId, 1, 5).enqueue(new Callback<PriceHistoryResponse>() {
        @Override
        public void onResponse(Call<PriceHistoryResponse> call, Response<PriceHistoryResponse> response) {
            if (response.isSuccessful() && response.body() != null) {
                PriceHistoryData data = response.body().getData();
                if (data != null && data.getPriceHistory() != null) {
                    priceHistoryList = data.getPriceHistory();
                    displayPriceHistory();
                }
            } else {
                Log.w(TAG, "Failed to load price history: code=" + response.code());
            }
        }
        
        @Override
        public void onFailure(Call<PriceHistoryResponse> call, Throwable t) {
            Log.e(TAG, "Price history network error", t);
        }
    });
}
```

5. 新規メソッド `displayPriceHistory()` を実装:
```java
private void displayPriceHistory() {
    if (priceHistoryList.isEmpty()) {
        binding.tvPriceHistoryTitle.setText("価格変更履歴はありません");
        binding.lvPriceHistory.setVisibility(View.GONE);
        return;
    }
    
    binding.tvPriceHistoryTitle.setText("📊 価格履歴");
    binding.lvPriceHistory.setVisibility(View.VISIBLE);
    
    List<String> displayList = new ArrayList<>();
    for (PriceHistory history : priceHistoryList) {
        // "2025-06-15T10:30:00" → "2025/06/15"
        String date = history.getChangedAt().substring(0, 10).replace("-", "/");
        String line = date + "  ¥" + history.getOldPrice() + " → ¥" + history.getNewPrice();
        displayList.add(line);
    }
    
    ArrayAdapter<String> adapter = new ArrayAdapter<>(
        this,
        android.R.layout.simple_list_item_1,
        displayList
    );
    binding.lvPriceHistory.setAdapter(adapter);
}
```

#### iOS（ProductDetailView.swift の改修）

**ProductDetailViewModel の拡張**（View内に定義されている場合）:

1. ViewModel クラスにプロパティを追加:
```swift
@Published var priceHistory: [PriceHistory] = []
@Published var isPriceHistoryLoading = false
```

2. ViewModel クラスにメソッドを追加:
```swift
@MainActor
func loadPriceHistory() async {
    isPriceHistoryLoading = true
    defer { isPriceHistoryLoading = false }
    
    do {
        let data = try await APIClient.shared.fetchPriceHistory(productId: productId, page: 1, limit: 5)
        priceHistory = data.priceHistory
    } catch {
        // エラーは無視（価格履歴はオプショナル機能）
        print("Failed to load price history: \(error)")
    }
}
```

**ProductDetailView.swift の body 内の変更**:

`Divider()` と お気に入りボタン / 購入ボタンの間に以下を追加:

```swift
// 価格履歴セクション
VStack(alignment: .leading, spacing: 12) {
    if viewModel.priceHistory.isEmpty {
        Text("価格変更履歴はありません")
            .font(.headline)
            .foregroundColor(.secondary)
    } else {
        Text("📊 価格履歴")
            .font(.headline)
        
        VStack(spacing: 8) {
            ForEach(viewModel.priceHistory) { history in
                HStack {
                    Text(history.formattedDate)
                        .font(.body)
                    Spacer()
                    Text("¥\(history.oldPrice) → ¥\(history.newPrice)")
                        .font(.body)
                        .foregroundColor(.secondary)
                }
                .padding(.vertical, 4)
                Divider()
            }
        }
    }
}
.padding(.vertical, 8)

Divider()
    .padding(.vertical, 8)
```

`.task` モディファイアを追加（既存の `loadProductDetail()` 呼び出しの後）:

```swift
.task {
    await viewModel.loadProductDetail()
    await viewModel.loadPriceHistory()  // 追加
}
```

---

## 5. 処理フロー

### Android（Java）

```
ユーザー操作: 商品詳細画面を開く（Intent で ProductDetailActivity を起動）
  │
  ├─ onCreate() → loadProductDetail(productId)  ... 既存
  │     └─ ApiService.getProductDetail(productId).enqueue(...)
  │           └─ 成功 → displayProduct() + loadPriceHistory(productId)
  │
  └─ loadPriceHistory(productId)   ... 新規
        └─ ApiService.getPriceHistory(productId, page=1, limit=5).enqueue(...)
              │
              ├─ 成功 → priceHistoryList に格納
              │         → displayPriceHistory()
              │         → ListView に ArrayAdapter をセット
              │
              └─ 失敗 → Log.w() でログ出力（UI には影響なし）
```

### iOS（Swift）

```
ユーザー操作: 商品詳細画面を開く（NavigationLink で ProductDetailView を表示）
  │
  ├─ .task { await viewModel.loadProductDetail() }  ... 既存
  │     └─ APIClient.fetchProductDetail(productId)
  │           └─ 成功 → @Published product プロパティ更新
  │                      → SwiftUI が自動的に UI 再描画
  │
  └─ .task { await viewModel.loadPriceHistory() }   ... 新規
        └─ APIClient.fetchPriceHistory(productId, page=1, limit=5)
              │
              ├─ 成功 → @Published priceHistory プロパティ更新
              │         → SwiftUI が自動的に ForEach を再描画
              │
              └─ 失敗 → print() でログ出力（UI には影響なし）
                         ※ 価格履歴はオプショナル機能のためエラー表示しない
```

---

## 6. 備考

- 価格履歴は商品更新API（API-013: `PUT /api/v1/products/{id}`）で価格変更が行われた際に、サーバー側で自動的に `product_price_history` テーブルに記録される
- データ保持期間は5年間。超過データは定期バッチ（日次実行）にて物理削除される
- レスポンスは `changed_at` の降順（新しい順）でソートされる
- BFF 転送仕様: Mobile BFF (`GET /api/mobile/products/{id}/price-history`) → Web API (`GET /api/v1/products/{id}/price-history`)
- **エラーハンドリング方針**: 価格履歴の取得失敗は商品詳細表示に影響を与えないため、エラー時は「価格変更履歴はありません」と表示し、ログのみ出力する
- **テスト方針**: 
  - Android: JUnit4 でモデルのシリアライズ・API呼び出しのテストを作成
  - iOS: XCTest で Codable のデコード・API呼び出しのテストを作成
