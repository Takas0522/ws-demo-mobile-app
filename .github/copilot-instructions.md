## プロジェクト概要

mobile-app-system は、商品販売を行うネイティブアプリケーション（iOS / Android / Windows）と管理者用Webアプリケーションで構成されるデモシステムです。BFFパターンを採用し、各クライアントは専用のBFFを経由してWeb APIに接続します。

## 開発環境

- WebアプリケーションはDevContainerを使用して開発されます
- PostgreSQLもDevContainer内で実行されます
- モバイル / Windowsアプリ開発を行う場合は、rootのDocker Composeファイルを使用して開発用のDBコンテナを起動してください

## アーキテクチャ参照

- 機能仕様書: `docs/specs/mobile-app-system/`
- アーキテクチャドキュメント: `docs/architecture/`
- コーディング規約: `docs/architecture/09-coding-standards.md`

## Windows アプリケーション (C++ / Win32)

### 技術スタック

| 項目 | 技術 |
|------|------|
| 言語 | C++20 |
| UIフレームワーク | Win32 API (ネイティブ) |
| ビルドシステム | MSBuild / Visual Studio (.vcxproj) |
| ソリューション | `src/windows-app/WsDemoMobileApp.WindowsApp/WsDemoMobileApp.WindowsApp.slnx` |
| HTTPクライアント | WinHTTP |
| JSON解析 | nlohmann/json |
| セキュアストレージ | Windows Credential Manager (DPAPI) |
| 最小OS | Windows 10 以上 |

### アーキテクチャ

- **MVVM パターン**を採用（iOS/Android と同様）
- **Mobile BFF** (`http://localhost:8081`) に接続
- JWT トークンによる認証（BFF経由）
- 定期的な状態更新機能（タイマーベースでBFFをポーリング）

### ディレクトリ構造（推奨）

```
src/windows-app/WsDemoMobileApp.WindowsApp/
├── WsDemoMobileApp.WindowsApp.slnx
├── WsDemoMobileApp.WindowsApp/       # メインプロジェクト
│   ├── WsDemoMobileApp.WindowsApp.vcxproj
│   ├── src/
│   │   ├── main.cpp                  # WinMain エントリポイント
│   │   ├── App.h / App.cpp           # アプリケーション管理
│   │   ├── Models/                   # データモデル
│   │   │   ├── User.h
│   │   │   ├── Product.h
│   │   │   ├── Purchase.h
│   │   │   └── Favorite.h
│   │   ├── ViewModels/               # ビジネスロジック
│   │   │   ├── LoginViewModel.h / .cpp
│   │   │   ├── ProductListViewModel.h / .cpp
│   │   │   ├── ProductDetailViewModel.h / .cpp
│   │   │   └── FavoriteViewModel.h / .cpp
│   │   ├── Views/                    # Win32 ウィンドウ / UI
│   │   │   ├── LoginWindow.h / .cpp
│   │   │   ├── ProductListWindow.h / .cpp
│   │   │   ├── ProductDetailWindow.h / .cpp
│   │   │   └── FavoriteWindow.h / .cpp
│   │   ├── Services/                 # ネットワーク・基盤サービス
│   │   │   ├── HttpClient.h / .cpp        # WinHTTP ラッパー
│   │   │   ├── AuthService.h / .cpp
│   │   │   ├── ProductService.h / .cpp
│   │   │   ├── FavoriteService.h / .cpp
│   │   │   └── StatePollingService.h / .cpp  # 定期状態更新
│   │   └── Utils/                    # ユーティリティ
│   │       ├── CredentialManager.h / .cpp    # Windows Credential Manager
│   │       ├── JsonHelper.h                   # JSON ユーティリティ
│   │       └── Constants.h                    # 定数定義
│   ├── resources/                    # リソースファイル
│   │   ├── resource.h
│   │   └── app.rc
│   └── include/                      # サードパーティヘッダー
│       └── nlohmann/
│           └── json.hpp
└── WsDemoMobileApp.WindowsApp.Tests/ # テストプロジェクト（オプション）
    └── ...
```

### C++ コーディング規約

#### 命名規則

| 対象 | ケース | 例 |
|------|--------|-----|
| クラス / 構造体 | PascalCase | `ProductListViewModel`, `HttpClient` |
| メンバ関数 | PascalCase | `FetchProducts()`, `GetToken()` |
| メンバ変数 | m_ + camelCase | `m_products`, `m_isLoading` |
| 静的メンバ変数 | s_ + camelCase | `s_instance` |
| ローカル変数 | camelCase | `userId`, `productName` |
| 定数 / constexpr | k + PascalCase | `kMaxRetryCount`, `kApiTimeout` |
| マクロ | UPPER_SNAKE_CASE | `WS_APP_VERSION` |
| 名前空間 | lowercase | `ws::models`, `ws::services` |
| ファイル名 | PascalCase | `ProductListViewModel.cpp`, `HttpClient.h` |
| enum class | PascalCase（型・値共に） | `enum class ApiError { NetworkError, Unauthorized }` |

#### コードスタイル

- **インデント**: タブ（幅4スペース相当）
- **行の最大長**: 120文字
- **中括弧**: Allman スタイル（次の行に開始括弧）
- **ヘッダーガード**: `#pragma once`
- **名前空間**: `ws::` をトップレベルとして使用
- **スマートポインタ**: `std::unique_ptr`, `std::shared_ptr` を使用、生ポインタは最小限
- **文字列**: `std::wstring` をWindows API連携に、`std::string` を内部処理に使用
- **エラーハンドリング**: 例外は使用せず、`std::expected` または `std::optional` + エラーコードで処理
- **RAII**: リソース管理はRAIIパターンを徹底

#### コード例

```cpp
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <optional>

namespace ws::viewmodels
{

class ProductListViewModel
{
public:
    ProductListViewModel();
    ~ProductListViewModel() = default;

    // コピー禁止
    ProductListViewModel(const ProductListViewModel&) = delete;
    ProductListViewModel& operator=(const ProductListViewModel&) = delete;

    /// 商品一覧を取得
    void FetchProducts();

    /// 商品一覧を返す
    [[nodiscard]] const std::vector<models::Product>& GetProducts() const noexcept;

    /// 読み込み中かどうか
    [[nodiscard]] bool IsLoading() const noexcept;

    /// エラーメッセージ（エラー時のみ値あり）
    [[nodiscard]] std::optional<std::wstring> GetErrorMessage() const noexcept;

    /// データ変更通知コールバック
    using OnChangedCallback = std::function<void()>;
    void SetOnChanged(OnChangedCallback callback);

private:
    void NotifyChanged();

    std::vector<models::Product> m_products;
    bool m_isLoading = false;
    std::optional<std::wstring> m_errorMessage;
    OnChangedCallback m_onChanged;
};

} // namespace ws::viewmodels
```

### Mobile BFF 接続

- **ベースURL**: `http://localhost:8081/api/mobile`
- ログイン以外のAPIは `Authorization: Bearer {JWT_TOKEN}` ヘッダーが必要
- レスポンスは JSON 形式: `{ "data": { ... }, "timestamp": "..." }`

#### 主要エンドポイント

| エンドポイント | メソッド | 説明 |
|--------------|---------|------|
| `/api/mobile/login` | POST | ユーザーログイン |
| `/api/mobile/products` | GET | 商品一覧取得 |
| `/api/mobile/products/search` | GET | 商品検索 |
| `/api/mobile/products/{id}` | GET | 商品詳細取得 |
| `/api/mobile/purchases` | POST | 商品購入 |
| `/api/mobile/purchases` | GET | 購入履歴取得 |
| `/api/mobile/favorites` | POST | お気に入り登録 |
| `/api/mobile/favorites/{id}` | DELETE | お気に入り解除 |
| `/api/mobile/favorites` | GET | お気に入り一覧取得 |
| `/api/mobile/feature-flags` | GET | 機能フラグ取得 |

### 定期状態更新機能

- `SetTimer` / `WM_TIMER` を使用したポーリング方式
- デフォルト更新間隔: 30秒
- 更新対象: 商品一覧、お気に入り一覧、機能フラグ
- バックグラウンドスレッドで HTTP リクエストを実行し、UI スレッドにメッセージで通知
- ネットワークエラー時はエクスポネンシャルバックオフで再試行

### セキュリティ

- JWT トークンは **Windows Credential Manager** (DPAPI) で安全に保管
- ハードコードされた秘密情報は禁止
- HTTPS 通信を推奨（開発時は HTTP 許容）
