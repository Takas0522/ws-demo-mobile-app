# Android アプリ改修 — 商品詳細画面への価格履歴表示追加

## 1. 改修概要

商品詳細画面（`ProductDetailActivity`）に「価格履歴」セクションを追加し、選択中の商品の価格変更履歴を一覧表示する。
設計原則（mobile-app.instructions.md）に準拠し、既存のアーキテクチャ（Activity + Retrofit コールバック）を維持する。

## 2. 画面レイアウト変更

### 変更箇所

商品詳細画面の「お気に入りボタン」と「購入ボタン」の間に「価格履歴セクション」を挿入する。

### レイアウト構成（変更後）

```
┌──────────────────────────────────┐
│ [Toolbar] 商品詳細               │
├──────────────────────────────────┤
│ [商品画像プレースホルダー]          │
│                                  │
│ 商品名                           │
│ ¥1,200                          │
│ 商品説明テキスト ...              │
│ ─────────────────────           │
│ [♡ お気に入りに追加]              │
│ ─────────────────────           │ ← 新規追加ここから
│ 価格履歴            [すべて表示 >] │
│ ┌──────────────────────────┐    │
│ │ 2025/06/15  ¥1,000 → ¥1,200│  │
│ │ 原材料費高騰に伴う価格改定    │  │
│ ├──────────────────────────┤    │
│ │ 2025/01/10  ¥800 → ¥1,000  │  │
│ │ 定期価格見直し              │  │
│ └──────────────────────────┘    │
│ ─────────────────────           │ ← 新規追加ここまで
│ [購入する]                       │
└──────────────────────────────────┘
```

## 3. 変更対象ファイル

### 3.1 新規作成

#### 3.1.1 モデル: `PriceHistory.java`

- **パッケージ**: `com.example.mobileapp.models`
- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/models/PriceHistory.java`

```java
public class PriceHistory {
    private int priceHistoryId;
    private int productId;
    private int oldPrice;
    private int newPrice;
    private String changedAt;
    private ChangedBy changedBy;
    private String changeReason;

    // getter / setter
}
```

#### 3.1.2 モデル: `ChangedBy.java`

- **パッケージ**: `com.example.mobileapp.models`
- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/models/ChangedBy.java`

```java
public class ChangedBy {
    private int userId;
    private String userName;

    // getter / setter
}
```

#### 3.1.3 モデル: `Pagination.java`

- **パッケージ**: `com.example.mobileapp.models`
- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/models/Pagination.java`

```java
public class Pagination {
    private int currentPage;
    private int totalPages;
    private int totalItems;
    private int itemsPerPage;

    // getter / setter
}
```

#### 3.1.4 モデル: `PriceHistoryListData.java`

- **パッケージ**: `com.example.mobileapp.models`
- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/models/PriceHistoryListData.java`

```java
public class PriceHistoryListData {
    private List<PriceHistory> priceHistory;
    private Pagination pagination;

    // getter / setter
}
```

#### 3.1.5 モデル: `PriceHistoryResponse.java`

- **パッケージ**: `com.example.mobileapp.models`
- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/models/PriceHistoryResponse.java`

既存の `ProductListResponse` と同様の構造を踏襲する。

```java
public class PriceHistoryResponse {
    private PriceHistoryListData data;
    private String timestamp;

    // getter / setter
}
```

#### 3.1.6 アダプター: `PriceHistoryAdapter.java`

- **パッケージ**: `com.example.mobileapp.ui`
- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/ui/PriceHistoryAdapter.java`

RecyclerView 用アダプター。各行に以下を表示する：

- 変更日時（`changedAt` を `yyyy/MM/dd` 形式で表示）
- 変更前単価 → 変更後単価（`¥{oldPrice} → ¥{newPrice}`）
- 変更理由（`changeReason`、null の場合は非表示）

```java
public class PriceHistoryAdapter extends RecyclerView.Adapter<PriceHistoryAdapter.ViewHolder> {
    private List<PriceHistory> priceHistoryList = new ArrayList<>();

    public void setPriceHistoryList(List<PriceHistory> list) {
        this.priceHistoryList = list;
        notifyDataSetChanged();
    }

    // ViewHolder, onCreateViewHolder, onBindViewHolder, getItemCount
}
```

#### 3.1.7 レイアウト: `item_price_history.xml`

- **ファイルパス**: `src/android-app/app/src/main/res/layout/item_price_history.xml`

RecyclerView の各行レイアウト。

```xml
<LinearLayout
    android:layout_width="match_parent"
    android:layout_height="wrap_content"
    android:orientation="vertical"
    android:padding="12dp">

    <LinearLayout
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:orientation="horizontal">

        <TextView
            android:id="@+id/tvChangedAt"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:textSize="14sp"
            android:textColor="@android:color/darker_gray" />

        <TextView
            android:id="@+id/tvPriceChange"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:textSize="14sp"
            android:textStyle="bold" />
    </LinearLayout>

    <TextView
        android:id="@+id/tvChangeReason"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:layout_marginTop="4dp"
        android:textSize="12sp"
        android:textColor="@android:color/darker_gray"
        android:visibility="gone" />
</LinearLayout>
```

### 3.2 既存ファイル変更

#### 3.2.1 `ApiService.java` — エンドポイント追加

- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/services/ApiService.java`
- **変更内容**: 価格履歴取得メソッドを追加

```java
@GET("/api/mobile/products/{id}/price-history")
Call<PriceHistoryResponse> getPriceHistory(
    @Path("id") int productId,
    @Query("page") Integer page,
    @Query("limit") Integer limit
);
```

#### 3.2.2 `activity_product_detail.xml` — 価格履歴セクション追加

- **ファイルパス**: `src/android-app/app/src/main/res/layout/activity_product_detail.xml`
- **変更内容**: お気に入りボタン（`btnFavorite`）と購入ボタン（`btnPurchase`）の間に以下を追加

```xml
<!-- 価格履歴セクション -->
<View
    android:layout_width="match_parent"
    android:layout_height="1dp"
    android:layout_marginTop="16dp"
    android:background="@android:color/darker_gray" />

<LinearLayout
    android:id="@+id/layoutPriceHistoryHeader"
    android:layout_width="match_parent"
    android:layout_height="wrap_content"
    android:layout_marginTop="16dp"
    android:orientation="horizontal"
    android:gravity="center_vertical">

    <TextView
        android:layout_width="0dp"
        android:layout_height="wrap_content"
        android:layout_weight="1"
        android:text="価格履歴"
        android:textSize="18sp"
        android:textStyle="bold" />
</LinearLayout>

<androidx.recyclerview.widget.RecyclerView
    android:id="@+id/rvPriceHistory"
    android:layout_width="match_parent"
    android:layout_height="wrap_content"
    android:layout_marginTop="8dp"
    android:nestedScrollingEnabled="false" />

<TextView
    android:id="@+id/tvNoPriceHistory"
    android:layout_width="match_parent"
    android:layout_height="wrap_content"
    android:layout_marginTop="8dp"
    android:text="価格履歴はありません"
    android:textSize="14sp"
    android:textColor="@android:color/darker_gray"
    android:gravity="center"
    android:visibility="gone" />

<View
    android:layout_width="match_parent"
    android:layout_height="1dp"
    android:layout_marginTop="16dp"
    android:background="@android:color/darker_gray" />
```

#### 3.2.3 `ProductDetailActivity.java` — 価格履歴取得ロジック追加

- **ファイルパス**: `src/android-app/app/src/main/java/com/example/mobileapp/ui/ProductDetailActivity.java`
- **変更内容**:

1. **フィールド追加**:

   ```java
   private PriceHistoryAdapter priceHistoryAdapter;
   ```

2. **`setupViews()` に RecyclerView 初期化を追加**:

   ```java
   priceHistoryAdapter = new PriceHistoryAdapter();
   binding.rvPriceHistory.setLayoutManager(new LinearLayoutManager(this));
   binding.rvPriceHistory.setAdapter(priceHistoryAdapter);
   ```

3. **`loadProductDetail()` の成功コールバック内で価格履歴取得を呼び出す**:

   ```java
   // 商品詳細取得成功後に価格履歴を読み込む
   loadPriceHistory();
   ```

4. **価格履歴取得メソッドの新規追加**:

   ```java
   private void loadPriceHistory() {
       Log.d(TAG, "Loading price history: productId=" + productId);

       ApiClient.getApiService().getPriceHistory(productId, 1, 5).enqueue(new Callback<PriceHistoryResponse>() {
           @Override
           public void onResponse(Call<PriceHistoryResponse> call, Response<PriceHistoryResponse> response) {
               if (response.isSuccessful() && response.body() != null) {
                   PriceHistoryListData data = response.body().getData();
                   if (data != null && data.getPriceHistory() != null && !data.getPriceHistory().isEmpty()) {
                       priceHistoryAdapter.setPriceHistoryList(data.getPriceHistory());
                       binding.rvPriceHistory.setVisibility(View.VISIBLE);
                       binding.tvNoPriceHistory.setVisibility(View.GONE);
                   } else {
                       binding.rvPriceHistory.setVisibility(View.GONE);
                       binding.tvNoPriceHistory.setVisibility(View.VISIBLE);
                   }
               }
           }

           @Override
           public void onFailure(Call<PriceHistoryResponse> call, Throwable t) {
               Log.e(TAG, "getPriceHistory error: " + t.getMessage(), t);
               binding.rvPriceHistory.setVisibility(View.GONE);
               binding.tvNoPriceHistory.setVisibility(View.VISIBLE);
           }
       });
   }
   ```

## 4. 表示仕様

### 4.1 価格履歴セクション

| 項目           | 仕様                                         |
| -------------- | -------------------------------------------- |
| 表示位置       | お気に入りボタンと購入ボタンの間             |
| 初期表示件数   | 最大5件（`limit=5`）                         |
| ソート順       | 変更日時の降順（新しい順、API レスポンス順） |
| 履歴なしの場合 | 「価格履歴はありません」を表示               |

### 4.2 各行の表示項目

| 項目     | 表示形式                            | 例                           |
| -------- | ----------------------------------- | ---------------------------- |
| 変更日時 | `yyyy/MM/dd`                        | `2025/06/15`                 |
| 価格変更 | `¥{oldPrice} → ¥{newPrice}`         | `¥1,000 → ¥1,200`            |
| 変更理由 | そのまま表示（null の場合は非表示） | `原材料費高騰に伴う価格改定` |

## 5. 設計上の考慮事項

- 既存のアーキテクチャ（Activity + Retrofit コールバック）を踏襲する
- 価格履歴の取得は商品詳細の取得成功後に非同期で実行する（画面表示のブロッキングを避ける）
- 価格履歴の取得に失敗した場合は「価格履歴はありません」を表示し、エラートーストは表示しない（補足的情報のため）
- ViewBinding を使用する（`findViewById` 禁止）
- RecyclerView の `nestedScrollingEnabled=false` を設定し、NestedScrollView 内で正しくスクロールさせる
