# モバイルアプリ ユニットテスト ワークフロー詳細

このドキュメントは `mobile-unit-testing` スキルの各ステップの詳細手順を定義する。

---

## ステップ 1: テストシナリオの作成

### 1-1. 対象コンポーネントの調査

テスト対象のソースコードを読み込み、以下を把握する:

- クラスの責務と公開インターフェース
- 依存コンポーネント（モック対象）
- 正常系・異常系のユースケース
- 境界値や特殊な入力値

**Android の主要テスト対象コンポーネント**:

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| ApiClient | `services/ApiClient.java` | 認証ヘッダー付与、トークン管理、エラーハンドリング |
| SecureStorageManager | `utils/SecureStorageManager.java` | トークンの保存・取得・削除 |
| LoginActivity | `ui/LoginActivity.java` | ログインフロー、バリデーション、画面遷移 |
| MainActivity | `ui/MainActivity.java` | 商品一覧取得、検索、エラー表示 |
| ProductDetailActivity | `ui/ProductDetailActivity.java` | 商品詳細表示、購入・お気に入り操作 |
| Models | `models/*.java` | JSON シリアライズ・デシリアライズ |

**iOS の主要テスト対象コンポーネント**:

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| APIClient | `Services/APIClient.swift` | リクエスト生成、認証ヘッダー、レスポンスパース、エラー処理 |
| AuthService | `Services/AuthService.swift` | ログイン・ログアウト状態遷移、トークン永続化 |
| ProductListViewModel | `Views/ProductListView.swift` | 商品フェッチ、検索フィルタ、ローディング・エラー状態 |
| ProductDetailViewModel | `Views/ProductDetailView.swift` | 詳細取得、お気に入り・購入操作 |
| KeychainManager | `Utilities/KeychainManager.swift` | トークンの保存・取得・削除、エラーケース |
| Models | `Models/*.swift` | Codable エンコード・デコード |

### 1-2. テストシナリオの記述形式

以下の形式でシナリオを記述する（Markdown テーブル形式）:

```markdown
## テストシナリオ: {コンポーネント名}

| # | テストケース名 | 前提条件 | 操作・入力 | 期待結果 | 分類 |
|---|---|---|---|---|---|
| 1 | 正常ログイン | 有効なユーザー名/パスワード | login() を呼び出す | isLoggedIn = true、トークンが保存される | 正常系 |
| 2 | パスワード空欄 | パスワードが空 | login() を呼び出す | バリデーションエラーが返る | 異常系 |
| 3 | ネットワークエラー | ネットワーク接続なし | login() を呼び出す | ネットワークエラーが通知される | 異常系 |
| 4 | 空文字ユーザー名 | ユーザー名が空文字 | login() を呼び出す | バリデーションエラーが返る | 境界値 |
| 5 | 認証トークン期限切れ | 期限切れトークンが保存済み | API リクエストを送信 | トークンリフレッシュまたはログアウト | 異常系 |
```

---

## ステップ 2: テストシナリオのレビュー

### 2-1. レビューの観点（ブラックボックステスト）

以下の 6 つの観点でシナリオの網羅性を評価する:

1. **同値分割**: 各入力値クラスが代表値でカバーされているか
2. **境界値分析**: 境界条件（空文字、null、最大長、0、負値）がカバーされているか
3. **エラーケース**: API エラー（4xx/5xx）、ネットワーク障害、認証切れ、タイムアウトがカバーされているか
4. **状態遷移**: 初期状態、ローディング、成功、失敗の各状態がカバーされているか
5. **副作用**: 状態変更（永続化、通知、UI 更新）がテストされているか
6. **セキュリティ**: 認証情報の取り扱い、不正アクセスのケースがカバーされているか

### 2-2. レビュー手順

1. 上記 6 つの観点でシナリオを評価する
2. 不足しているケースを特定し、具体的な改善点を列挙する
3. 問題がある場合はシナリオを修正する
4. 修正後に再レビューを実施する（**最大 2 回まで**）
5. 最終的なシナリオを確定する

---

## ステップ 3: テストの実装

### 3-1. Android（JUnit 4）実装ガイドライン

**テストファイルの配置先**:
```
src/android-app/app/src/test/java/com/example/mobileapp/
├── services/
│   ├── ApiClientTest.java
│   └── SecureStorageManagerTest.java
├── ui/
│   ├── LoginActivityTest.java
│   ├── MainActivityTest.java
│   └── ProductDetailActivityTest.java
└── models/
    └── ProductTest.java
```

**テスト実装の手順**:
1. [junit-test.java](../templates/junit-test.java) テンプレートをベースにテストクラスを作成する
2. `@Mock` アノテーションで依存コンポーネントのモックを宣言する
3. `setUp()` メソッドでテスト対象クラスを初期化する
4. `when(...).thenReturn(...)` でモックの振る舞いを定義する
5. `assertEquals`、`assertTrue`、`verify` でアサーションを記述する

**Activity のテスト（Robolectric 使用）**:
```java
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {Build.VERSION_CODES.UPSIDE_DOWN_CAKE})
public class LoginActivityTest {
    @Test
    public void test_whenLoginSuccess_navigateToMain() {
        ActivityScenario<LoginActivity> scenario = ActivityScenario.launch(LoginActivity.class);
        scenario.onActivity(activity -> {
            // テスト実装
        });
    }
}
```

**`build.gradle` への Mockito 追加（未設定の場合）**:
```groovy
dependencies {
    testImplementation 'org.mockito:mockito-core:5.x.x'
    testImplementation 'org.robolectric:robolectric:4.x.x'
}
```

### 3-2. iOS（XCTest）実装ガイドライン

**テストファイルの配置先**:
```
src/ios-app/MobileApp/MobileAppTests/
├── Services/
│   ├── APIClientTests.swift
│   └── AuthServiceTests.swift
├── ViewModels/
│   ├── ProductListViewModelTests.swift
│   └── ProductDetailViewModelTests.swift
├── Utilities/
│   └── KeychainManagerTests.swift
└── Models/
    └── ProductTests.swift
```

**テスト実装の手順**:
1. [xctest-test.swift](../templates/xctest-test.swift) テンプレートをベースにテストクラスを作成する
2. テスト対象クラスに注入する依存コンポーネントの `protocol` を定義する
3. `MockXxx` クラスを作成して `protocol` を実装する
4. `setUpWithError()` でテスト対象クラスを初期化する
5. `tearDownWithError()` で `sut` と `mock` を `nil` に設定してメモリを解放する

**ObservableObject のテスト（非同期）**:
```swift
@MainActor
final class ProductListViewModelTests: XCTestCase {
    func test_whenFetchProducts_stateBecomesLoaded() async throws {
        // Arrange
        mockAPIClient.productsToReturn = [Product.mock()]

        // Act
        await sut.fetchProducts()

        // Assert
        XCTAssertFalse(sut.isLoading)
        XCTAssertEqual(sut.products.count, 1)
    }
}
```

---

## ステップ 4: テストの実施

### 4-1. Android テストの実行

全テストを実行:
```bash
cd src/android-app
./gradlew test
```

特定クラスのみ実行:
```bash
./gradlew test --tests "com.example.mobileapp.services.ApiClientTest"
```

テスト結果レポートの確認:
```
src/android-app/app/build/reports/tests/testDebugUnitTest/index.html
```

### 4-2. iOS テストの実行

```bash
cd src/ios-app/MobileApp
xcodebuild test \
  -scheme MobileApp \
  -destination 'platform=iOS Simulator,name=iPhone 15' \
  -resultBundlePath TestResults.xcresult \
  2>&1 | xcpretty
```

シミュレーターのデバイス名確認:
```bash
xcrun simctl list devices available
```

### 4-3. 問題発生時の対応

テスト実行中に問題が発生した場合（ビルドエラー、テスト失敗）は、**コードの修正は行わず、問題内容をレポートに記載するのみ**とする。

---

## ステップ 5: テスト結果レポートの作成

テスト終了後、以下のフォーマットで `temp/テストレポート_{timestamp}.md` を作成する。

- `timestamp` は `YYYYMMDD_HHmmss` 形式（例: `20260308_012345`）
- `temp/` ディレクトリが存在しない場合は `mkdir -p temp` で作成する

**レポートテンプレート**:

```markdown
# モバイルアプリ ユニットテストレポート

**実施日時**: {timestamp}
**対象プラットフォーム**: Android / iOS（または両方）
**テスト対象バージョン**: {git rev-parse --short HEAD の出力}

---

## サマリー

| プラットフォーム | 総テスト数 | 成功 | 失敗 | スキップ | 成功率 |
|---|---|---|---|---|---|
| Android | xx | xx | xx | xx | xx% |
| iOS | xx | xx | xx | xx | xx% |

---

## テストシナリオ一覧

### {コンポーネント名}

| # | テストケース名 | 結果 | 備考 |
|---|---|---|---|
| 1 | 正常ログイン | ✅ 成功 | |
| 2 | パスワード空欄 | ❌ 失敗 | AssertionError: expected false but was true |

---

## 失敗したテストの詳細

### {テストケース名}

- **原因**: {エラーメッセージ・スタックトレース}
- **再現手順**: {手順}
- **備考**: {修正が必要な場合は記載（修正は本スキルの範囲外）}

---

## 所見・推奨事項

- {テスト結果から得られた観察事項や改善提案}
```
