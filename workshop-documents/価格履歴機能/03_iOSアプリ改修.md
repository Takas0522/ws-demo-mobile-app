# iOS アプリ改修 — 商品詳細画面への価格履歴表示追加

## 1. 改修概要

商品詳細画面（`ProductDetailView`）に「価格履歴」セクションを追加し、選択中の商品の価格変更履歴を一覧表示する。
設計原則（mobile-app.instructions.md）に準拠し、SwiftUI + `@MainActor` ViewModel + `async/await` パターンで実装する。

## 2. 画面レイアウト変更

### 変更箇所

商品詳細画面の「お気に入りボタン」と「購入ボタン」の間に「価格履歴セクション」を挿入する。

### レイアウト構成（変更後）

```
┌──────────────────────────────────┐
│ [< 戻る]   商品詳細               │
├──────────────────────────────────┤
│ [商品画像プレースホルダー]          │
│                                  │
│ 商品名               (.title2)   │
│ ¥1,200               (.title3)   │
│ 商品説明テキスト ...    (.body)    │
│ ─────────────────────           │
│ [♡ お気に入りに追加]              │
│ ─────────────────────           │ ← 新規追加ここから
│ 価格履歴              (.headline) │
│ ┌──────────────────────────┐    │
│ │ 2025/06/15               │    │
│ │ ¥1,000 → ¥1,200          │    │
│ │ 原材料費高騰に伴う価格改定 │    │
│ ├──────────────────────────┤    │
│ │ 2025/01/10               │    │
│ │ ¥800 → ¥1,000            │    │
│ │ 定期価格見直し            │    │
│ └──────────────────────────┘    │
│ ─────────────────────           │ ← 新規追加ここまで
│ [購入する]                       │
└──────────────────────────────────┘
```

## 3. 変更対象ファイル

### 3.1 新規作成

#### 3.1.1 モデル: `PriceHistory.swift`

- **ファイルパス**: `src/ios-app/MobileApp/MobileApp/Models/PriceHistory.swift`

```swift
import Foundation

struct PriceHistory: Codable, Identifiable, Sendable {
    var id: Int { priceHistoryId }
    let priceHistoryId: Int
    let productId: Int
    let oldPrice: Int
    let newPrice: Int
    let changedAt: String
    let changedBy: ChangedBy
    let changeReason: String?

    enum CodingKeys: String, CodingKey {
        case priceHistoryId, productId, oldPrice, newPrice, changedAt, changedBy, changeReason
    }
}

struct ChangedBy: Codable, Sendable {
    let userId: Int
    let userName: String
}

struct PriceHistoryListData: Codable, Sendable {
    let priceHistory: [PriceHistory]
    let pagination: Pagination
}

struct Pagination: Codable, Sendable {
    let currentPage: Int
    let totalPages: Int
    let totalItems: Int
    let itemsPerPage: Int
}

struct PriceHistoryResponse: Codable, Sendable {
    let data: PriceHistoryListData
    let timestamp: String
}
```

#### 3.1.2 View コンポーネント: `PriceHistorySection.swift`

- **ファイルパス**: `src/ios-app/MobileApp/MobileApp/Views/PriceHistorySection.swift`

商品詳細画面に埋め込むための再利用可能な View コンポーネント。

```swift
import SwiftUI

struct PriceHistorySection: View {
    let priceHistoryList: [PriceHistory]
    let isLoading: Bool

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Text("価格履歴")
                .font(.headline)

            if isLoading {
                ProgressView()
                    .frame(maxWidth: .infinity)
                    .padding()
            } else if priceHistoryList.isEmpty {
                Text("価格履歴はありません")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                    .frame(maxWidth: .infinity)
                    .padding()
            } else {
                ForEach(priceHistoryList) { history in
                    PriceHistoryRow(history: history)
                    if history.id != priceHistoryList.last?.id {
                        Divider()
                    }
                }
            }
        }
    }
}

struct PriceHistoryRow: View {
    let history: PriceHistory

    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(formatDate(history.changedAt))
                .font(.caption)
                .foregroundColor(.secondary)

            Text("¥\(history.oldPrice) → ¥\(history.newPrice)")
                .font(.subheadline)
                .fontWeight(.semibold)

            if let reason = history.changeReason {
                Text(reason)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .padding(.vertical, 4)
    }

    private func formatDate(_ dateString: String) -> String {
        // "2025-06-15T10:30:00" → "2025/06/15"
        let components = dateString.prefix(10).split(separator: "-")
        guard components.count == 3 else { return dateString }
        return components.joined(separator: "/")
    }
}
```

### 3.2 既存ファイル変更

#### 3.2.1 `APIClient.swift` — 価格履歴取得メソッド追加

- **ファイルパス**: `src/ios-app/MobileApp/MobileApp/Services/APIClient.swift`
- **変更内容**: `// MARK: - Product API` セクションにメソッドを追加

```swift
// MARK: - Price History API

func getPriceHistory(productId: Int, page: Int = 1, limit: Int = 5) async throws -> PriceHistoryListData {
    let endpoint = "/api/mobile/products/\(productId)/price-history?page=\(page)&limit=\(limit)"
    let response: PriceHistoryResponse = try await request(endpoint: endpoint)
    return response.data
}
```

#### 3.2.2 `ProductDetailView.swift` — 価格履歴セクション追加

- **ファイルパス**: `src/ios-app/MobileApp/MobileApp/Views/ProductDetailView.swift`

##### ViewModel への追加プロパティ・メソッド

`ProductDetailViewModel` に以下を追加する：

```swift
@Published var priceHistoryList: [PriceHistory] = []
@Published var isPriceHistoryLoading = false
```

```swift
func loadPriceHistory() async {
    isPriceHistoryLoading = true

    do {
        let data = try await APIClient.shared.getPriceHistory(productId: productId)
        priceHistoryList = data.priceHistory
    } catch {
        // 価格履歴の取得失敗は補足的情報のためエラー表示しない
        priceHistoryList = []
    }

    isPriceHistoryLoading = false
}
```

##### View への追加

`body` 内のお気に入りボタンと購入ボタンの間に `PriceHistorySection` を挿入する：

```swift
Divider()
    .padding(.vertical, 8)

// 価格履歴セクション
PriceHistorySection(
    priceHistoryList: viewModel.priceHistoryList,
    isLoading: viewModel.isPriceHistoryLoading
)

Divider()
    .padding(.vertical, 8)
```

##### `.task` モディファイアでの読み込み

既存の `.task` ブロック内に価格履歴の読み込みを追加する：

```swift
.task {
    await viewModel.loadProductDetail()
    await viewModel.loadPriceHistory()
}
```

#### 3.2.3 `MobileApp.xcodeproj` — ファイル登録

新規作成ファイルを Xcode プロジェクトに登録する：

- `Models/PriceHistory.swift`
- `Views/PriceHistorySection.swift`

## 4. 表示仕様

### 4.1 価格履歴セクション

| 項目           | 仕様                                         |
| -------------- | -------------------------------------------- |
| 表示位置       | お気に入りボタンと購入ボタンの間             |
| 初期表示件数   | 最大5件（`limit=5`）                         |
| ソート順       | 変更日時の降順（新しい順、API レスポンス順） |
| 履歴なしの場合 | 「価格履歴はありません」を表示               |
| ローディング中 | `ProgressView()` を表示                      |

### 4.2 各行の表示項目

| 項目     | フォント                     | 表示形式                            | 例                           |
| -------- | ---------------------------- | ----------------------------------- | ---------------------------- |
| 変更日時 | `.caption` / `.secondary`    | `yyyy/MM/dd`                        | `2025/06/15`                 |
| 価格変更 | `.subheadline` / `.semibold` | `¥{oldPrice} → ¥{newPrice}`         | `¥1,000 → ¥1,200`            |
| 変更理由 | `.caption` / `.secondary`    | そのまま表示（null の場合は非表示） | `原材料費高騰に伴う価格改定` |

## 5. 設計上の考慮事項

- SwiftUI + `@MainActor` ViewModel パターンに準拠する
- `async/await` でネットワーク処理を実装する（クロージャコールバック禁止）
- `.task` モディファイアを使用し、View のライフサイクルに合わせて非同期処理を起動する
- `PriceHistory` モデルは `Codable`, `Identifiable`, `Sendable` に準拠する（既存の `Product` モデルと同様）
- `PriceHistorySection` は再利用可能な View コンポーネントとして分離する
- 価格履歴の取得に失敗した場合は補足的情報のため、エラーメッセージは表示しない（空配列として扱う）
- HIG 準拠: システムフォント（`.headline`, `.subheadline`, `.caption`）を使用して Dynamic Type に対応する
