# 価格履歴機能 ─ Windowsアプリ改修仕様

> **設計書:** `docs/excel/価格履歴機能_設計書.xlsx` (DES-PRICE-HIST-001 v1.0)

## 1. 機能概要

商品詳細画面に「価格履歴」セクションを追加し、選択中の商品について過去の価格変更履歴を一覧表示する。
Web API（CppApiServer）は改修済みのため、Windowsアプリ（クライアント側）のレイアウト変更と API 呼び出し実装のみが対象となる。

### コーディング規則（全ファイル共通）

- `#pragma once` をすべてのヘッダーに記述する
- 名前空間: モデルは `ws::models`、サービスは `ws::services`、ViewModels は `ws::viewmodels`、Views は `ws::views`
- JSON シリアライズ: `nlohmann::json` を使用し、各モデルヘッダーに `inline void from_json(...)` を実装する
- エラー処理: 例外を使わず `std::expected<T, ws::models::ApiError>` を使用する
- 非同期処理: `std::thread([...]).detach()` パターンを使用する
- UIスレッドへの通知: コールバック内では必ず `PostMessage`（非同期）を使用し、`SendMessage` は使用しない

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

#### Models（既存）

| ファイル | 改修内容 |
|---|---|
| `src/Models/Pagination.h` | `from_json` に `totalItems` / `itemsPerPage` キーの読み込みを追加 |

#### ViewModels

| ファイル | 改修内容 |
|---|---|
| `src/ViewModels/ProductDetailViewModel.h` | 価格履歴データ保持メンバー・取得メソッド・コールバック追加、コンストラクタに `IPriceHistoryService&` を追加 |
| `src/ViewModels/ProductDetailViewModel.cpp` | `FetchPriceHistory()` メソッド実装 |

#### Views（レイアウト変更）

| ファイル | 改修内容 |
|---|---|
| `src/Views/ProductDetailWindow.h` | 価格履歴リスト用コントロール ID（`kIdPriceHistoryList = 3007`）・HWND メンバー（`m_priceHistoryList`）・WMメッセージID定数（`kWmPriceHistoryChanged = WM_APP + 103`）追加 |
| `src/Views/ProductDetailWindow.cpp` | `OnCreate()` の座標変更・価格履歴コントロール追加、`HandleMessage()` のケース追加、`UpdatePriceHistoryUI()` 実装、`OnPaint()` の仕切り線追加 |

#### App

| ファイル | 改修内容 |
|---|---|
| `src/App.h` | `#include "Services/PriceHistoryService.h"` 追加・`ws::services::PriceHistoryService m_priceHistoryService;` メンバー追加 |
| `src/App.cpp` | コンストラクタ初期化子リストに `m_priceHistoryService(m_httpClient, m_authService)` 追加・`ProductDetailViewModel` コンストラクタに `m_priceHistoryService` を第3引数として追加・`ShowProductDetailWindow()` 内で `FetchPriceHistory(productId)` 呼び出し追加 |

#### Constants

| ファイル | 改修内容 |
|---|---|
| `src/Utils/Constants.h` | 価格履歴 API パスサフィックス定数追加 |

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
│ 商品説明                  │  y=350, h=80  ← 縮小（180→80）
│                          │
├──────────────────────────┤  y=438 (divider) ← 新規追加
│ 📊 価格履歴               │  y=446, h=24  (セクションタイトル、STATIC)
│┌────────────────────────┐│
││ 2025/06/15  ¥1,000→¥1,200 ││
││ 2025/01/10  ¥800→¥1,000  ││  y=474, h=110（LISTBOX）
││ ...                      ││
│└────────────────────────┘│
│                          │
├──────────────────────────┤  y=590 (divider)
│ [      購入する      ]     │  y=600, h=48
└──────────────────────────┘
```

#### 価格履歴リストの表示ルール

- 使用する Win32 コントロール: `LISTBOX`（`LBS_NOSEL | LBS_HASSTRINGS | WS_BORDER | WS_VSCROLL`）
- リストの各行: `YYYY/MM/DD  ¥{oldPrice} → ¥{newPrice}` 形式
  - `changedAt`（例: `"2025-06-15T10:30:00"`）から先頭 10 文字（`"2025-06-15"`）を取り出し、`-` を `/` に置換して `YYYY/MM/DD` に変換する
- 履歴が存在しない場合: リストボックスを非表示にし、セクションタイトルの代わりに `"価格変更履歴はありません"` を表示する
- 最大 5 件まで表示（`FetchPriceHistory` の `limit` 引数に 5 を渡す）
- 文字列変換: `std::string` (UTF-8) → `std::wstring` の変換は既存の `MultiByteToWideChar(CP_UTF8, ...)` パターンを使用する

### 4.4 モデル定義（PriceHistory.h）

`src/Models/` の他のモデル（`Product.h` など）と同じ構造で実装する。`from_json` は `inline` 関数としてヘッダー内に実装すること。

```cpp
#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace ws::models
{

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

inline void from_json(const nlohmann::json& j, ChangedBy& c)
{
	j.at("userId").get_to(c.userId);
	j.at("userName").get_to(c.userName);
}

inline void from_json(const nlohmann::json& j, PriceHistory& p)
{
	j.at("priceHistoryId").get_to(p.priceHistoryId);
	j.at("productId").get_to(p.productId);
	j.at("oldPrice").get_to(p.oldPrice);
	j.at("newPrice").get_to(p.newPrice);
	j.at("changedAt").get_to(p.changedAt);
	j.at("changedBy").get_to(p.changedBy);

	if (j.contains("changeReason") && !j.at("changeReason").is_null())
	{
		p.changeReason = j.at("changeReason").get<std::string>();
	}
}

} // namespace ws::models
```

### 4.5 Pagination.h の改修（既存ファイル）

APIレスポンスの `pagination` オブジェクトは `totalItems` / `itemsPerPage` キーを使用する。既存の `from_json` は `totalCount` / `limit` キーのみ対応しているため、以下のキーも読み込めるよう拡張する。

```cpp
// Pagination.h の from_json に追記する
// （既存の totalCount / limit の読み込みはそのまま残す）
if (j.contains("totalItems"))
{
	j.at("totalItems").get_to(p.totalCount);
}
if (j.contains("itemsPerPage"))
{
	j.at("itemsPerPage").get_to(p.limit);
}
```

### 4.6 サービスインターフェース（IPriceHistoryService.h）

`src/Services/IProductService.h` と同じ構造で実装する。

```cpp
#pragma once

#include <expected>
#include "Models/PriceHistory.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

namespace ws::services
{

struct PriceHistoryResponse
{
	std::vector<ws::models::PriceHistory> priceHistory;
	ws::models::Pagination pagination;
};

class IPriceHistoryService
{
public:
	virtual ~IPriceHistoryService() = default;

	[[nodiscard]] virtual std::expected<PriceHistoryResponse, ws::models::ApiError>
		FetchPriceHistory(int64_t productId, int page = 1, int limit = 5) const = 0;

protected:
	IPriceHistoryService() = default;
	IPriceHistoryService(const IPriceHistoryService&) = default;
	IPriceHistoryService& operator=(const IPriceHistoryService&) = default;
};

} // namespace ws::services
```

### 4.7 サービス実装（PriceHistoryService.h / .cpp）

`ProductService.h` / `ProductService.cpp` と同じパターンで実装する。コンストラクタは `const IHttpClient&` と `const IAuthService&` を受け取る。

**PriceHistoryService.h**:

```cpp
#pragma once

#include "Services/IPriceHistoryService.h"

namespace ws::services
{

class IHttpClient;
class IAuthService;

class PriceHistoryService : public IPriceHistoryService
{
public:
	PriceHistoryService(const IHttpClient& httpClient, const IAuthService& authService);
	~PriceHistoryService() override = default;

	PriceHistoryService(const PriceHistoryService&) = delete;
	PriceHistoryService& operator=(const PriceHistoryService&) = delete;

	[[nodiscard]] std::expected<PriceHistoryResponse, ws::models::ApiError>
		FetchPriceHistory(int64_t productId, int page = 1, int limit = 5) const override;

private:
	const IHttpClient& m_httpClient;
	const IAuthService& m_authService;
};

} // namespace ws::services
```

**PriceHistoryService.cpp** の実装方針:

- `ProductService::FetchProductDetail()` と同じ URL 構築・HTTP GET・JSONパースパターンを使用する
- URL 構築: `std::string(ws::utils::kProductsPath) + "/" + std::to_string(productId) + ws::utils::kPriceHistoryPathSuffix + "?page=" + std::to_string(page) + "&limit=" + std::to_string(limit)`
- JSONパース: `json["data"]["priceHistory"].get<std::vector<ws::models::PriceHistory>>()` と `json["data"]["pagination"].get<ws::models::Pagination>()`
- エラー処理: `ws::utils::ParseApiError()` → フォールバックエラーコード `"PRICE_HISTORY_001"` を使用する
- 例外: `nlohmann::json::exception` を catch して `"JSON_PARSE_ERROR"` を返す

### 4.8 API エンドポイント定数（Constants.h）

```cpp
// 既存の kFeatureFlagsPath の後に追加する
constexpr const char* kPriceHistoryPathSuffix = "/price-history";
```

URLの生成例: `std::string(kProductsPath) + "/" + std::to_string(productId) + kPriceHistoryPathSuffix`
→ `/api/mobile/products/1/price-history`

### 4.9 ViewModel 変更（ProductDetailViewModel）

**ProductDetailViewModel.h の変更点**:

1. `#include "Services/IPriceHistoryService.h"` と `#include "Models/PriceHistory.h"` を追加する
2. コンストラクタに `ws::services::IPriceHistoryService& priceHistoryService` を第3引数として追加する
3. 以下のメンバーと型定義を追加する:

```cpp
// using 宣言（既存の FavoriteChangedCallback の下に追加）
using PriceHistoryChangedCallback = std::function<void()>;

// 公開メソッド（既存の SetOnFavoriteChanged の下に追加）
void FetchPriceHistory(int64_t productId);
void SetOnPriceHistoryChanged(PriceHistoryChangedCallback callback);
[[nodiscard]] const std::vector<ws::models::PriceHistory>& GetPriceHistory() const;

// プライベートメンバー（既存の m_onFavoriteChanged の下に追加）
ws::services::IPriceHistoryService& m_priceHistoryService;
std::vector<ws::models::PriceHistory> m_priceHistory;
PriceHistoryChangedCallback m_onPriceHistoryChanged;
```

**ProductDetailViewModel.cpp の `FetchPriceHistory()` 実装**:

`FetchProductDetail()` と同じ `std::thread().detach()` パターンで実装する。

```cpp
void ProductDetailViewModel::FetchPriceHistory(int64_t productId)
{
	std::thread([this, productId]()
	{
		auto result = m_priceHistoryService.FetchPriceHistory(productId, 1, 5);

		if (result.has_value())
		{
			m_priceHistory = std::move(result.value().priceHistory);
			if (m_onPriceHistoryChanged)
			{
				m_onPriceHistoryChanged();
			}
		}
		else
		{
			if (m_onError)
			{
				m_onError(result.error());
			}
		}
	}).detach();
}
```

---

## 5. ProductDetailWindow の改修詳細

### 5.1 ProductDetailWindow.h の追加定義

既存の `kIdPurchaseButton = 3003` の後に以下を追加する:

```cpp
static constexpr int kIdPriceHistoryTitle = 3007;
static constexpr int kIdPriceHistoryList  = 3008;
static constexpr UINT kWmPriceHistoryChanged = WM_APP + 103;
// ※ WM_APP + 101: 商品詳細更新（既存）
// ※ WM_APP + 102: 購入完了通知（PurchaseWindow で使用中）
// ※ WM_APP + 103: 価格履歴更新（新規）
```

HWND メンバーを追加する:

```cpp
HWND m_priceHistoryTitle = nullptr;
HWND m_priceHistoryList  = nullptr;
```

新規メソッドを追加する:

```cpp
void UpdatePriceHistoryUI();
```

### 5.2 ProductDetailWindow.cpp の変更点

#### `OnCreate()` の変更

既存の `m_descLabel` 生成コードの高さを 180 から 80 に変更する:

```cpp
// 変更前
m_descLabel = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
    16, 350, 430, 180, m_hwnd, ToHMenu(kIdProductDesc), hInstance, nullptr);

// 変更後
m_descLabel = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
    16, 350, 430, 80, m_hwnd, ToHMenu(kIdProductDesc), hInstance, nullptr);
```

既存の「購入する」ボタン生成コードの前（ `y=600` の前）に以下を追加する:

```cpp
// 価格履歴セクションタイトル
m_priceHistoryTitle = CreateWindowExW(0, L"STATIC", L"📊 価格履歴",
    WS_CHILD | WS_VISIBLE | SS_LEFT,
    16, 446, 430, 24,
    m_hwnd, ToHMenu(kIdPriceHistoryTitle), hInstance, nullptr);

// 価格履歴リストボックス
m_priceHistoryList = CreateWindowExW(0, L"LISTBOX", nullptr,
    WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOSEL | LBS_HASSTRINGS,
    16, 474, 430, 110,
    m_hwnd, ToHMenu(kIdPriceHistoryList), hInstance, nullptr);
```

フォント適用: 既存の `ModernTheme::ApplyFont(m_hwnd, ...)` 呼び出しはウィンドウ全体のコントロールに一括適用されるため、追加作業は不要。

#### `HandleMessage()` の変更

既存の `case WM_APP + 101:` の後に以下を追加する:

```cpp
case WM_APP + 103:  // kWmPriceHistoryChanged
    UpdatePriceHistoryUI();
    return 0;
```

#### `OnPaint()` の変更

既存の `DrawDivider(hdc, 16, 586, 430)` の前に以下を追加する:

```cpp
// 価格履歴セクション上部の仕切り線
ws::utils::ModernTheme::DrawDivider(hdc, 16, 438, 430);
```

#### `UpdatePriceHistoryUI()` の実装

```cpp
void ProductDetailWindow::UpdatePriceHistoryUI()
{
	const auto& history = m_viewModel.GetPriceHistory();

	SendMessageW(m_priceHistoryList, LB_RESETCONTENT, 0, 0);

	if (history.empty())
	{
		SetWindowTextW(m_priceHistoryTitle, L"📊 価格変更履歴はありません");
		ShowWindow(m_priceHistoryList, SW_HIDE);
		return;
	}

	SetWindowTextW(m_priceHistoryTitle, L"📊 価格履歴");
	ShowWindow(m_priceHistoryList, SW_SHOW);

	for (const auto& item : history)
	{
		// changedAt: "2025-06-15T10:30:00" → "2025/06/15"
		std::string date = item.changedAt.substr(0, 10);
		std::replace(date.begin(), date.end(), '-', '/');

		std::string line = date + "  ¥" + std::to_string(item.oldPrice)
			+ " → ¥" + std::to_string(item.newPrice);

		int len = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, nullptr, 0);
		std::wstring wideLine(len - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, wideLine.data(), len);

		SendMessageW(m_priceHistoryList, LB_ADDSTRING, 0,
			reinterpret_cast<LPARAM>(wideLine.c_str()));
	}
}
```

---

## 6. App.h / App.cpp の改修

### 6.1 App.h の変更

`#include "Services/FeatureFlagService.h"` の後に追加する:

```cpp
#include "Services/PriceHistoryService.h"
```

`ws::services::FavoriteService m_favoriteService;` の後に追加する:

```cpp
ws::services::PriceHistoryService m_priceHistoryService;
```

### 6.2 App.cpp の変更

**コンストラクタ `App::App()`** の初期化子リストに追加する:

```cpp
App::App()
	: m_authService(m_httpClient, m_credentialManager)
	, m_productService(m_httpClient, m_authService)
	, m_purchaseService(m_httpClient, m_authService)
	, m_favoriteService(m_httpClient, m_authService)
	, m_priceHistoryService(m_httpClient, m_authService)  // 追加
{
}
```

**`Initialize()` 内の `ProductDetailViewModel` 生成コード**を変更する:

```cpp
// 変更前
m_productDetailViewModel = std::make_unique<ws::viewmodels::ProductDetailViewModel>(
    m_productService, m_favoriteService);

// 変更後
m_productDetailViewModel = std::make_unique<ws::viewmodels::ProductDetailViewModel>(
    m_productService, m_favoriteService, m_priceHistoryService);
```

**`SetupCallbacks()` 内**に価格履歴コールバックを追加する（既存の `FavoriteChanged` コールバックの後に追加）:

```cpp
// 価格履歴取得完了 → UI 更新（バックグラウンドスレッドから呼ばれる）
m_productDetailViewModel->SetOnPriceHistoryChanged([this]()
{
    if (m_productDetailWindow)
    {
        PostMessage(m_productDetailWindow->GetHandle(), WM_APP + 103, 0, 0);
    }
});
```

**`ShowProductDetailWindow()` 内**に価格履歴取得の呼び出しを追加する（既存の `FetchProductDetail` 呼び出しの直後）:

```cpp
m_productDetailViewModel->FetchProductDetail(productId);
m_productDetailViewModel->FetchPriceHistory(productId);  // 追加
m_productDetailWindow->Show(SW_SHOW);
```

---

## 7. 処理フロー

```
ユーザー操作: 商品詳細画面を開く（App::ShowProductDetailWindow(productId)）
  │
  ├─ ProductDetailViewModel::FetchProductDetail(productId)  ... 既存
  │     └─ std::thread().detach() で非同期実行
  │           └─ 成功 → m_onProductChanged コールバック
  │                      → PostMessage(WM_APP + 101)
  │                      → ProductDetailWindow::UpdateUI()
  │
  └─ ProductDetailViewModel::FetchPriceHistory(productId)   ... 新規
        └─ std::thread().detach() で非同期実行
              │
              ├─ PriceHistoryService::FetchPriceHistory(productId, page=1, limit=5)
              │     └─ GET /api/mobile/products/{id}/price-history?page=1&limit=5
              │
              ├─ 成功 → m_priceHistory に結果を格納
              │         → m_onPriceHistoryChanged コールバック
              │         → PostMessage(WM_APP + 103)  ※WM_APP+102 は PurchaseWindow が使用
              │         → ProductDetailWindow::UpdatePriceHistoryUI()
              │
              └─ 失敗 → m_onError コールバック
                         → PostMessage(kWmShowError, errorMsg)
                         → MessageBoxW でエラーダイアログ表示
```

---

## 8. 備考

- 価格履歴は商品更新API（API-013: `PUT /api/v1/products/{id}`）で価格変更が行われた際に、サーバー側で自動的に `product_price_history` テーブルに記録される
- データ保持期間は5年間。超過データは定期バッチ（日次実行）にて物理削除される
- レスポンスは `changed_at` の降順（新しい順）でソートされる
- BFF 転送仕様: Mobile BFF (`GET /api/mobile/products/{id}/price-history`) → Web API (`GET /api/v1/products/{id}/price-history`)
