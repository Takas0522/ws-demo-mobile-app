---
description: "iOS アプリ開発のための命名規則・設計原則・コードスタイル・レイヤー構造・開発工程のガイドライン。Apple Human Interface Guidelines・SwiftUI・Swift Concurrency のベストプラクティスに基づく。"
applyTo: "MobileApp/**"
---

# iOS アプリ開発規約

このファイルは `MobileApp/` 配下の Swift コードに適用される規約を定義します。

---

## バックエンド構成

```
iOS App → Mobile BFF (Java, port 8081) → Web API (Java, port 8080) → SQLite
```

- バックエンドはすべて Java（Spring Boot）で構成されている。`src/windows-app/CppApiServer/` の C++ 製 Web API サーバーは無関係であり、参照しないこと

---

## 開発工程

以下の順序で開発を進める。各ステップを省略しないこと。

1. **仕様の理解** — タスクの要件と影響範囲を正確に把握する。既存コードを調査し、変更が必要なファイルを特定する
2. **開発プランの作成** — 変更するファイル・クラス・メソッドを列挙し、実装方針を計画する。設計原則（後述）への準拠を確認する
3. **開発** — プランに従ってコードを実装する。コードスタイルと命名規則を遵守する
4. **テストの作成** — 実装した機能のユニットテスト（XCTest）を作成する
5. **テストの実施** — テストを実行し、すべてのテストがパスすることを確認する

---

## 設計原則

- **関心の分離（Separation of Concerns）**: SwiftUI View はレンダリングのみを担い、ビジネスロジックはViewModelに記述する
- **単方向データフロー（UDF）**: 状態はデータ層からUI層へ一方向に流れる
- **単一信頼源（SSOT）**: 各データ型はひとつのオーナーを持つ
- **データモデル駆動UI**: UIはViewModelが公開する状態（UIState）を観測して描画する。UIから直接APIを呼び出さない
- **テスタビリティ**: 依存関係はプロトコルで抽象化し、テストダブルに置き換えられるように設計する

---

## レイヤー構造

**現行のディレクトリ構造**（`MobileApp/MobileApp/`）:

```
MobileApp/
├── Views/               # UIレイヤー: SwiftUI View + ViewModel（同ファイル）
├── Models/              # データモデル: Codable struct
├── Services/            # サービス層: APIClient, AuthService
└── Utilities/           # ユーティリティ: KeychainManager
```

**推奨する拡張方針**:

```
MobileApp/
├── Views/               # UIレイヤー: SwiftUI View + ViewModel
├── Domain/              # ドメインレイヤー（任意）: UseCase, Protocol定義
├── Data/                # データレイヤー（任意）: Repository実装
├── Models/              # データモデル: Codable struct
├── Services/            # ネットワーク・認証: APIClient, AuthService
└── Utilities/           # ユーティリティ: KeychainManager
```

- ViewModelクラスは `@MainActor` を付与し、`ObservableObject` に準拠させる
- APIやストレージへの直接アクセスはServices層に限定する

---

## 命名規則

| 対象                                | 規則                       | 例                                               |
| ----------------------------------- | -------------------------- | ------------------------------------------------ |
| 型（struct, class, enum, protocol） | PascalCase                 | `ProductListView`, `AuthService`, `NetworkError` |
| プロパティ・変数・関数              | camelCase                  | `isAuthenticated`, `loadProducts()`              |
| 定数（型レベル）                    | camelCase                  | `let baseURL = "..."`                            |
| View                                | `Xxx` + `View`             | `LoginView`, `ProductDetailView`                 |
| ViewModel                           | `Xxx` + `ViewModel`        | `ProductListViewModel`, `ProductDetailViewModel` |
| Service                             | `Xxx` + `Service`          | `AuthService`                                    |
| Manager                             | `Xxx` + `Manager`          | `KeychainManager`                                |
| プロトコル                          | 機能を表す名詞または形容詞 | `Authenticatable`, `ProductRepository`           |
| ファイル名                          | 主要な型と同名             | `ProductListView.swift`, `AuthService.swift`     |

---

## コードスタイル

- インデント: **4スペース**（タブ不使用）
- 行の最大長: **120文字**
- **`struct`** を優先する（参照セマンティクスが必要な場合のみ `class` を使用）
- **`@MainActor`** をViewModelクラスに付与してUI更新の安全性を保証する
- **`async/await`** でネットワーク処理を実装する（クロージャコールバック禁止）
- **`.task {}`** モディファイアをViewのライフサイクルに合わせた非同期処理に使用する
- アクセス修飾子を明示する（`private`, `internal`, `public`）
- `MARK: -` コメントでコードセクションを区切る
- **HIG準拠**: すべてのインタラクティブ要素に `.accessibilityLabel()` を付与する

#### SwiftUI プロパティラッパーの使い分け

| プロパティラッパー   | 用途                                          |
| -------------------- | --------------------------------------------- |
| `@State`             | View内のローカルな値型の一時的な状態          |
| `@Binding`           | 親から受け取った状態への双方向参照            |
| `@StateObject`       | ViewがOwnerとなるObservableObjectの生成・保持 |
| `@ObservedObject`    | 外部から注入されるObservableObjectの観測      |
| `@EnvironmentObject` | View階層全体で共有するObservableObject        |

---

## エラーハンドリング

- `async throws` 関数は呼び出し側で `do-catch` を使用してエラーを処理する
- `NetworkError` 等の専用エラー型（`enum`）を定義してエラーを分類する
- UIへのエラー通知は `@Published var errorMessage: String?` + `.alert` または専用エラー表示Viewで行う
- `catch` ブロックでエラーを無視してはならない。少なくともログを記録すること

```swift
// 正しい例
func loadProducts() async {
    isLoading = true
    errorMessage = nil
    defer { isLoading = false }

    do {
        products = try await APIClient.shared.getProducts()
    } catch NetworkError.unauthorized {
        errorMessage = "認証エラーが発生しました"
    } catch NetworkError.serverError(let message) {
        errorMessage = message
    } catch {
        errorMessage = "商品の読み込みに失敗しました"
        print("[ProductListViewModel] loadProducts error: \(error)")
    }
}
```

---

## テスト

- テストフレームワーク: **XCTest**
- ViewModelテスト: `@MainActor` テストクラスで `async` テストメソッドを使用する
- APIクライアントテスト: `URLProtocol` サブクラスでHTTPリクエストをモックする
- テストクラス名: `XxxTests`
- テストメソッド名: `test_methodName_condition_expectedBehavior`

```swift
@MainActor
class ProductListViewModelTests: XCTestCase {
    func test_loadProducts_onSuccess_updatesProducts() async throws {
        let viewModel = ProductListViewModel()
        await viewModel.loadProducts()
        XCTAssertFalse(viewModel.products.isEmpty)
    }
}
```

---

## セキュリティ

- JWTトークンは **Keychain** に保存する。`UserDefaults` への保存禁止
- HTTPSを使用する（本番環境ではHTTP禁止）
- ログにトークンや個人情報を出力しない

---

## バリデーション（ビルド・テスト実行）

```bash
# ユニットテスト（MobileApp/ ディレクトリから実行）
cd MobileApp
xcodebuild test \
  -project MobileApp.xcodeproj \
  -scheme MobileApp \
  -destination 'platform=iOS Simulator,name=iPhone 16'

# ビルド
xcodebuild build \
  -project MobileApp.xcodeproj \
  -scheme MobileApp \
  -destination 'platform=iOS Simulator,name=iPhone 16'
```

---

## Good Example — MVVM + async/await + @MainActor

```swift
// ViewModel
@MainActor
class ProductListViewModel: ObservableObject {
    @Published var products: [Product] = []
    @Published var isLoading = false
    @Published var errorMessage: String?

    func loadProducts() async {
        isLoading = true
        errorMessage = nil
        defer { isLoading = false }
        do {
            products = try await APIClient.shared.getProducts()
        } catch NetworkError.unauthorized {
            errorMessage = "認証エラーが発生しました"
        } catch {
            errorMessage = "商品の読み込みに失敗しました"
            print("[ProductListViewModel] loadProducts error: \(error)")
        }
    }
}

// View: @StateObject でViewModelを所有、.task でライフサイクル連動
struct ProductListView: View {
    @StateObject private var viewModel = ProductListViewModel()

    var body: some View {
        List(viewModel.products) { product in
            ProductRow(product: product)
                .accessibilityLabel("\(product.productName), ¥\(product.unitPrice)")
        }
        .task { await viewModel.loadProducts() }
        .refreshable { await viewModel.loadProducts() }
        .overlay {
            if viewModel.isLoading { ProgressView() }
        }
    }
}
```

## Bad Example — クロージャとエラー無視（禁止）

```swift
// 禁止: コールバック形式
APIClient.shared.getProducts { result in  // async/awaitを使用すること
    switch result {
    case .success(let products): self.products = products
    case .failure: break  // エラーを無視しない
    }
}
```
