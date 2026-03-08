---
description: "Android アプリ開発のための命名規則・設計原則・コードスタイル・レイヤー構造・開発工程のガイドライン。Androidアーキテクチャガイド・Now in Android のベストプラクティスに基づく。"
applyTo: "app/**"
---

# Android アプリ開発規約

このファイルは `app/` 配下の Java コードに適用される規約を定義します。

---

## バックエンド構成

```
Android App → Mobile BFF (Java, port 8081) → Web API (Java, port 8080) → SQLite
```

- バックエンドはすべて Java（Spring Boot）で構成されている。`src/windows-app/CppApiServer/` の C++ 製 Web API サーバーは無関係であり、参照しないこと

---

## 開発工程

以下の順序で開発を進める。各ステップを省略しないこと。

1. **仕様の理解** — タスクの要件と影響範囲を正確に把握する。既存コードを調査し、変更が必要なファイルを特定する
2. **開発プランの作成** — 変更するファイル・クラス・メソッドを列挙し、実装方針を計画する。設計原則（後述）への準拠を確認する
3. **開発** — プランに従ってコードを実装する。コードスタイルと命名規則を遵守する
4. **テストの作成** — 実装した機能のユニットテスト（JUnit4）を作成する
5. **テストの実施** — テストを実行し、すべてのテストがパスすることを確認する

---

## 設計原則

- **関心の分離（Separation of Concerns）**: Activity はデータ保持・ビジネスロジック実行を担わない。ロジックはViewModel・Service・Repositoryに委譲する
- **単方向データフロー（UDF）**: 状態はデータ層からUI層へ一方向に流れ、UIイベントはViewModelに渡す。ViewModelが直接UIを操作しない
- **単一信頼源（SSOT）**: 各データ型はひとつのオーナーを持つ。同じデータを複数箇所で保持しない
- **データモデル駆動UI**: UIはViewModelが公開する状態（UIState）を観測して描画する。UIから直接APIを呼び出さない
- **テスタビリティ**: 依存関係はインターフェースで抽象化し、テストダブルに置き換えられるように設計する

---

## レイヤー構造

**現行のディレクトリ構造**（`app/src/main/java/com/example/mobileapp/`）:

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
│   ├── repository/
│   └── datasource/
├── models/              # データモデル
├── services/            # ネットワーク: ApiClient, ApiService
└── utils/               # ユーティリティ
```

- UIコンポーネント（Activity/Fragment）はViewModelを通じてデータにアクセスする
- **ViewModelは `Activity`/`Context`/`Fragment` を保持しない**
- データソース（API/DB）への直接アクセスはデータレイヤー（Repository経由）に限定する

---

## 命名規則

| 対象                     | 規則                                  | 例                                                  |
| ------------------------ | ------------------------------------- | --------------------------------------------------- |
| クラス・インターフェース | PascalCase                            | `ProductDetailActivity`, `ApiService`               |
| メソッド・変数           | camelCase                             | `loadProducts()`, `productAdapter`                  |
| 定数                     | SCREAMING_SNAKE_CASE                  | `static final String TAG = "MainActivity"`          |
| パッケージ               | 全て小文字                            | `com.example.mobileapp.ui`                          |
| Activity                 | `Xxx` + `Activity`                    | `LoginActivity`, `ProductDetailActivity`            |
| Adapter                  | `Xxx` + `Adapter`                     | `ProductAdapter`                                    |
| ViewModel                | `Xxx` + `ViewModel`                   | `ProductListViewModel`                              |
| Repository               | `Xxx` + `Repository`                  | `ProductRepository`                                 |
| レイアウトファイル       | `activity_xxx.xml`, `item_xxx.xml`    | `activity_main.xml`, `item_product.xml`             |
| ViewID（XML）            | スネークケース + タイププレフィックス | `tv_product_name`, `btn_login`, `rv_products`       |
| ログTAG                  | クラス名                              | `private static final String TAG = "LoginActivity"` |

---

## コードスタイル

- インデント: **4スペース**（タブ不使用）
- 行の最大長: **120文字**
- **ViewBinding** を使用する（`findViewById` 禁止）
- `@Override` アノテーションは常に付与する
- ローカル変数は可能な限り `final` を使用する
- ログは `Log.d/w/e(TAG, "message")` を使用し、TAGは `private static final String TAG = "ClassName"` で定義する。デバッグ実行時のログは Logcat に格納され、Android Studio の Logcat パネルで確認できる
- APIコールバックは匿名クラスではなくラムダ式を使用する（Java 8+）
- `null` チェックは早期リターンで行う

```java
// 正しい例: ViewBindingを使用
binding.tvProductName.setText(product.getProductName());

// 誤った例: findViewByIdを使用
TextView tv = (TextView) findViewById(R.id.tv_product_name);
```

---

## エラーハンドリング

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

---

## テスト

- テストフレームワーク: **JUnit4** + **Espresso**（UIテスト）
- APIテスト: MockWebServerを使用してHTTPリクエスト/レスポンスをモックする
- テストクラス名: `XxxTest`
- テストメソッド名: `methodName_condition_expectedBehavior`

```java
@Test
public void loadProducts_onSuccess_updatesProductList() { ... }

@Test
public void login_withInvalidCredentials_shows401Error() { ... }
```

---

## セキュリティ

- JWTトークンは `EncryptedSharedPreferences`（AES256-GCM）に保存する。`SharedPreferences` への平文保存禁止
- HTTPSを使用する（本番環境ではHTTP禁止）
- ログにトークンや個人情報を出力しない

---

## バリデーション（ビルド・テスト実行）

```bash
# ユニットテスト
./gradlew test

# インストゥルメンテーションテスト（エミュレータ/実機接続が必要）
./gradlew connectedAndroidTest

# ビルド（デバッグ）
./gradlew assembleDebug
```

---

## Good Example — ViewModel + Repository パターン（推奨）

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

## Bad Example — ViewModel が ApiClient を直接呼び出す（禁止）

```java
// 禁止: ViewModelがデータソースに直接アクセスする
public void loadProducts() {
    ApiClient.getApiService().getProducts().enqueue(...); // Repositoryを経由すること
}
```
