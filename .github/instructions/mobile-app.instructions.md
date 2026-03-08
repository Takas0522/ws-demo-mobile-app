---
description: 'モバイルアプリ（Android/iOS）開発のための命名規則・設計原則・コードスタイル・レイヤー構造・開発工程のガイドライン。Androidアーキテクチャガイド・Now in Android・Human Interface Guidelines・SwiftUI・Swift Concurrencyのベストプラクティスに基づく。'
applyTo: "src/{android-app,ios-app}/**"
---

# モバイルアプリ開発規約

このファイルは Android（`src/android-app/`）および iOS（`src/ios-app/`）のモバイルアプリ開発に適用される規約を定義します。
Androidアーキテクチャガイド・[Now in Android](https://github.com/android/nowinandroid)・Apple Human Interface Guidelines・SwiftUI・Swift Concurrencyのベーシックなプラクティスを参考に構成しています。

---

## 目的とスコープ

- **対象**: `src/android-app/` 配下のJavaコード、および `src/ios-app/` 配下のSwiftコード
- **目的**: 一貫性・保守性・テスタビリティの高いモバイルアプリコードを生成・レビューするためのガイドライン

---

## 開発工程

以下の順序で開発を進める。各ステップを省略しないこと。

1. **仕様の理解** — タスクの要件と影響範囲を正確に把握する。既存コードを調査し、変更が必要なファイルを特定する
2. **開発プランの作成** — 変更するファイル・クラス・メソッドを列挙し、実装方針を計画する。設計原則（後述）への準拠を確認する
3. **開発** — プランに従ってコードを実装する。コードスタイルと命名規則を遵守する
4. **テストの作成** — 実装した機能のユニットテスト（Androidは JUnit4、iOSは XCTest）を作成する
5. **テストの実施** — テストを実行し、すべてのテストがパスすることを確認する

---

## 設計原則

Android・iOSに共通して適用する設計原則。

- **関心の分離（Separation of Concerns）**: UIコンポーネント（Activity / SwiftUI View）はデータ保持・ビジネスロジック実行を担わない。ロジックはViewModel・Service・Repositoryに委譲する
- **単方向データフロー（UDF）**: 状態はデータ層からUI層へ一方向に流れ、UIイベントはViewModel・サービスに渡す。ViewModelが直接UIを操作しない
- **単一信頼源（SSOT）**: 各データ型はひとつのオーナーを持つ。同じデータを複数箇所で保持しない
- **データモデル駆動UI**: UIはViewModelが公開する状態（UIState）を観測して描画する。UIから直接APIを呼び出さない
- **テスタビリティ**: 依存関係はインターフェース（Android）またはプロトコル（iOS）で抽象化し、テストダブルに置き換えられるように設計する

---

## レイヤー構造

### Android — 3層構造

**現行のディレクトリ構造**（`src/android-app/app/src/main/java/com/example/mobileapp/`）:

```
com.example.mobileapp/
├── ui/                  # UIレイヤー: Activity, Adapter
│   └── （推奨拡張）ViewModel をここに配置
├── models/              # データモデル: API Request/Response
├── services/            # ネットワーク: ApiClient, ApiService (Retrofit)
└── utils/               # ユーティリティ: SecureStorageManager
```

**推奨する拡張方針**（Androidアーキテクチャガイド + Now in Android準拠）:

```
com.example.mobileapp/
├── ui/                  # UIレイヤー: Activity, Fragment, Adapter, ViewModel
├── domain/              # ドメインレイヤー（複数VMで共有するロジック）: UseCase
├── data/                # データレイヤー: Repository, DataSource
│   ├── repository/      # Repositoryインターフェースと実装
│   └── datasource/      # APIDataSource等
├── models/              # データモデル
├── services/            # ネットワーク: ApiClient, ApiService
└── utils/               # ユーティリティ
```

- UIコンポーネント（Activity/Fragment）はViewModelを通じてデータにアクセスする
- **ViewModelは `Activity`/`Context`/`Fragment` を保持しない**
- データソース（API/DB）への直接アクセスはデータレイヤー（Repository経由）に限定する
- **将来の移行先**: KotlinへのマイグレーションとJetpack Compose採用を推奨（[Now in Android](https://github.com/android/nowinandroid) 参照）

### iOS — 3層構造

**現行のディレクトリ構造**（`src/ios-app/MobileApp/MobileApp/`）:

```
MobileApp/
├── Views/               # UIレイヤー: SwiftUI View + ViewModel（同ファイル）
├── Models/              # データモデル: Codable struct
├── Services/            # サービス層: APIClient, AuthService
└── Utilities/           # ユーティリティ: KeychainManager
```

**推奨する拡張方針**（SwiftUI + Apple Architecture推奨）:

```
MobileApp/
├── Views/               # UIレイヤー: SwiftUI View + ViewModel
├── Domain/              # ドメインレイヤー（任意）: UseCase, Protocol定義
├── Data/                # データレイヤー（任意）: Repository実装
├── Models/              # データモデル: Codable struct
├── Services/            # ネットワーク・認証: APIClient, AuthService
└── Utilities/           # ユーティリティ: KeychainManager
```

- SwiftUI Viewはレンダリングのみを担い、ビジネスロジックはViewModelに記述する
- ViewModelクラスは `@MainActor` を付与し、`ObservableObject` に準拠させる
- APIやストレージへの直接アクセスはServices層に限定する

---

## 命名規則

### Android（Java）

| 対象 | 規則 | 例 |
|---|---|---|
| クラス・インターフェース | PascalCase | `ProductDetailActivity`, `ApiService` |
| メソッド・変数 | camelCase | `loadProducts()`, `productAdapter` |
| 定数 | SCREAMING_SNAKE_CASE | `static final String TAG = "MainActivity"` |
| パッケージ | 全て小文字 | `com.example.mobileapp.ui` |
| Activity | `Xxx` + `Activity` | `LoginActivity`, `ProductDetailActivity` |
| Adapter | `Xxx` + `Adapter` | `ProductAdapter` |
| ViewModel | `Xxx` + `ViewModel` | `ProductListViewModel` |
| Repository | `Xxx` + `Repository` | `ProductRepository` |
| レイアウトファイル | `activity_xxx.xml`, `item_xxx.xml` | `activity_main.xml`, `item_product.xml` |
| ViewID（XML） | スネークケース + タイププレフィックス | `tv_product_name`, `btn_login`, `rv_products` |
| ログTAG | クラス名 | `private static final String TAG = "LoginActivity"` |

### iOS（Swift）

| 対象 | 規則 | 例 |
|---|---|---|
| 型（struct, class, enum, protocol） | PascalCase | `ProductListView`, `AuthService`, `NetworkError` |
| プロパティ・変数・関数 | camelCase | `isAuthenticated`, `loadProducts()` |
| 定数（型レベル） | camelCase（Swiftの慣例） | `let baseURL = "..."` |
| View | `Xxx` + `View` | `LoginView`, `ProductDetailView` |
| ViewModel | `Xxx` + `ViewModel` | `ProductListViewModel`, `ProductDetailViewModel` |
| Service | `Xxx` + `Service` | `AuthService` |
| Manager | `Xxx` + `Manager` | `KeychainManager` |
| プロトコル | 機能を表す名詞または形容詞 | `Authenticatable`, `ProductRepository` |
| ファイル名 | 主要な型と同名 | `ProductListView.swift`, `AuthService.swift` |

---

## コードスタイル

### 共通

- インデント: **4スペース**（タブ不使用）
- 行の最大長: **120文字**
- 空行でロジックのブロックを区切る
- マジックナンバーは定数として定義する

### Android（Java）

- **ViewBinding** を使用する（`findViewById` 禁止）
- `@Override` アノテーションは常に付与する
- ローカル変数は可能な限り `final` を使用する
- ログは `Log.d/w/e(TAG, "message")` を使用し、TAGは `private static final String TAG = "ClassName"` で定義する
- APIコールバックは匿名クラス（`new Callback<T>() {...}`）ではなくラムダ式を使用する（Java 8+）
- `null` チェックは早期リターンで行う

```java
// 正しい例: ViewBindingを使用
binding.tvProductName.setText(product.getProductName());

// 誤った例: findViewByIdを使用
TextView tv = (TextView) findViewById(R.id.tv_product_name);
```

### iOS（Swift）

- **`struct`** を優先する（参照セマンティクスが必要な場合のみ `class` を使用）
- **`@MainActor`** をViewModelクラスに付与してUI更新の安全性を保証する
- **`async/await`** でネットワーク処理を実装する（クロージャコールバック禁止）
- **`Task {}`** でSwiftUI Viewの同期コンテキストから非同期処理を起動する
- **`.task {}`** モディファイアをViewのライフサイクルに合わせた非同期処理に使用する（Viewが破棄されると自動キャンセル）
- アクセス修飾子を明示する（`private`, `internal`, `public`）
- `MARK: -` コメントでコードセクションを区切る
- **HIG準拠**: すべてのインタラクティブ要素に `.accessibilityLabel()` を付与する。システムフォント（`.body`, `.headline` 等）を使用してDynamic Typeに対応する。タップターゲットの最小サイズは 44×44pt

#### SwiftUI プロパティラッパーの使い分け

| プロパティラッパー | 用途 | 例 |
|---|---|---|
| `@State` | View内のローカルな値型の一時的な状態 | `@State private var isLoading = false` |
| `@Binding` | 親から受け取った状態への双方向参照 | `@Binding var text: String` |
| `@StateObject` | ViewがOwnerとなるObservableObjectの生成・保持 | `@StateObject private var viewModel = ProductListViewModel()` |
| `@ObservedObject` | 外部から注入されるObservableObjectの観測 | `@ObservedObject var viewModel: ProductListViewModel` |
| `@EnvironmentObject` | View階層全体で共有するObservableObject | `@EnvironmentObject var authService: AuthService` |

- `@StateObject` はViewが所有権を持つViewModel（生成元）に使用する
- `@ObservedObject` は親からViewModelを受け取る場合に使用する（`@StateObject` との混同禁止）
- `@EnvironmentObject` はアプリ全体で共有する `AuthService` 等に使用する

### Good Example — iOS ViewModel with proper error handling

```swift
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

// Viewでの使用
.task {
    await viewModel.loadProducts()
}
```

### Bad Example — クロージャとエラー無視

```swift
// 禁止: コールバック形式
APIClient.shared.getProducts { result in  // async/awaitを使用すること
    switch result {
    case .success(let products): self.products = products
    case .failure: break  // エラーを無視しない
    }
}
```

---

## エラーハンドリング

### Android（Java）

- Retrofitのコールバックでは `onResponse` と `onFailure` の両方を必ず実装する
- HTTPステータスコード別のエラー処理を行う（401: 未認証、4xx: クライアントエラー、5xx: サーバーエラー）
- ネットワークエラーは `Log.e(TAG, "message", throwable)` でログに記録する
- UIへのエラー通知は `Toast` または専用のエラーTextViewで表示する

```java
// 正しい例
ApiClient.getApiService().getProducts().enqueue(new Callback<ProductListResponse>() {
    @Override
    public void onResponse(Call<ProductListResponse> call, Response<ProductListResponse> response) {
        if (response.isSuccessful() && response.body() != null) {
            // 成功処理
        } else {
            Log.w(TAG, "API error: code=" + response.code());
            showError("データの取得に失敗しました");
        }
    }

    @Override
    public void onFailure(Call<ProductListResponse> call, Throwable t) {
        Log.e(TAG, "Network error", t);
        showError("ネットワークエラーが発生しました");
    }
});
```

### iOS（Swift）

- `async throws` 関数は呼び出し側で `do-catch` を使用してエラーを処理する
- `NetworkError` 等の専用エラー型（`enum`）を定義してエラーを分類する
- UIへのエラー通知は `@Published var errorMessage: String?` + `.alert` または専用エラー表示Viewで行う
- `catch` ブロックで無視（`// エラー処理（ここでは無視）`）してはならない。少なくともログを記録すること

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
        // ログに詳細を記録する
        print("[ProductListViewModel] loadProducts error: \(error)")
    }
}
```

---

## テスト

### Android

- テストフレームワーク: **JUnit4** + **Espresso**（UIテスト）
- APIテスト: MockWebServerを使用してHTTPリクエスト/レスポンスをモックする
- テストクラス名: `XxxTest`（ユニット）
- テストメソッド名: `methodName_condition_expectedBehavior`
- > Kotlin移行後は `UnconfinedTestDispatcher` / `StandardTestDispatcher` を使用（`TestCoroutineDispatcher` は kotlinx-coroutines-test 1.6+ で非推奨）

```java
// テストメソッド命名例
@Test
public void loadProducts_onSuccess_updatesProductList() { ... }

@Test
public void login_withInvalidCredentials_shows401Error() { ... }
```

### iOS

- テストフレームワーク: **XCTest**
- ViewModelテスト: `@MainActor` テストクラスで `async` テストメソッドを使用する
- APIクライアントテスト: `URLProtocol` サブクラスでHTTPリクエストをモックする
- テストクラス名: `XxxTests`
- テストメソッド名: `test_methodName_condition_expectedBehavior`

```swift
// ViewModelテスト例
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

- **Android**: JWTトークンは `EncryptedSharedPreferences`（AES256-GCM）に保存する。`SharedPreferences` への平文保存禁止
- **iOS**: JWTトークンは **Keychain** に保存する。`UserDefaults` への保存禁止
- HTTPSを使用する（本番環境ではHTTP禁止。デバッグ用途を除く）
- ログにトークンや個人情報を出力しない
- ユーザー入力は必ずバリデーションを行ってから使用する

---

## コード例

### Good Example — Android: ViewModel + Repository パターン（推奨）

```java
// Repository インターフェース
public interface ProductRepository {
    void getProducts(Callback<List<Product>> callback);
}

// ViewModel: Repository経由でデータにアクセス（ApiClientを直接呼び出さない）
public class ProductListViewModel extends ViewModel {
    private final ProductRepository repository;
    private final MutableLiveData<List<Product>> products = new MutableLiveData<>();
    private final MutableLiveData<Boolean> isLoading = new MutableLiveData<>(false);
    private final MutableLiveData<String> errorMessage = new MutableLiveData<>();

    public ProductListViewModel(ProductRepository repository) {
        this.repository = repository;
    }

    public LiveData<List<Product>> getProducts() { return products; }
    public LiveData<Boolean> getIsLoading() { return isLoading; }
    public LiveData<String> getErrorMessage() { return errorMessage; }

    public void loadProducts() {
        isLoading.setValue(true);
        repository.getProducts(new Callback<List<Product>>() {
            @Override
            public void onSuccess(List<Product> result) {
                isLoading.postValue(false);
                products.postValue(result);
            }
            @Override
            public void onError(String message) {
                isLoading.postValue(false);
                errorMessage.postValue(message);
            }
        });
    }
}
```

### Bad Example — Android: ViewModel が ApiClient を直接呼び出す

```java
// 禁止: ViewModelがデータソースに直接アクセスする
public void loadProducts() {
    ApiClient.getApiService().getProducts().enqueue(...); // Repositoryを経由すること
}
```

### Good Example — iOS: MVVM + async/await + @MainActor（現行パターン）

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

---

## フレームワーク・ライブラリ固有ルール

### Android

- **Retrofit**: インターフェース（`ApiService`）でエンドポイントを定義し、`ApiClient` から生成する
- **OkHttp Interceptor**: 認証ヘッダー付与・ログ出力はInterceptorで行う（呼び出し側で個別設定しない）
- **ViewBinding**: レイアウトファイルはViewBindingで参照する

### iOS SwiftUI

- `NavigationView`（iOS 15以前）または `NavigationStack`（iOS 16以降）を使用してナビゲーションを管理する
- `List` を使用してリストUIを実装する（`ForEach` + `ScrollView` の代わりに使用）
- プレビュー（`PreviewProvider`）を各Viewに追加して迅速なUI確認を可能にする
- システムカラー（`.blue`, `.red`, `.secondary`）を使用してダークモード対応を自動化する

### Swift Concurrency

- `Task {}` を使用してViewのボタンアクション等から非同期処理を起動する
- `async let` を使用して複数の非同期処理を並列実行する
- `withTaskGroup` を使用して動的な並列タスクを管理する
- `actor` をスレッドセーフなデータアクセスが必要なクラスに使用する
- **`Task.detached` は構造化並行性を破壊するため原則禁止**（`Task {}` を使用する）
- **`Sendable` 準拠**: 並行コンテキスト間で共有する型（クロージャで渡す型等）は `Sendable` に準拠させる
- **タスクキャンセル**: `.task {}` モディファイアで起動したタスクはViewが破棄されると自動キャンセルされる。長時間の処理では `Task.checkCancellation()` または `try Task.checkCancellation()` で途中キャンセルに対応する

```swift
// Good: 並列処理 + エラーハンドリング
func loadDashboard() async {
    async let products = APIClient.shared.getProducts()
    async let favorites = APIClient.shared.getFavorites()
    do {
        let (loadedProducts, loadedFavorites) = try await (products, favorites)
        self.products = loadedProducts
        self.favorites = loadedFavorites
    } catch is CancellationError {
        // キャンセルは正常なフローとして扱う
    } catch {
        errorMessage = "データの取得に失敗しました"
        print("[ViewModel] loadDashboard error: \(error)")
    }
}
```

---

## バリデーション

### Android — ビルド・テスト実行コマンド

```bash
# プロジェクトルートから実行
cd src/android-app

# ユニットテスト
./gradlew test

# インストゥルメンテーションテスト（エミュレータ/実機接続が必要）
./gradlew connectedAndroidTest

# ビルド（デバッグ）
./gradlew assembleDebug
```

### iOS — ビルド・テスト実行コマンド

```bash
# Xcodeプロジェクトから実行
cd src/ios-app/MobileApp

# ユニットテスト（シミュレータを指定）
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
