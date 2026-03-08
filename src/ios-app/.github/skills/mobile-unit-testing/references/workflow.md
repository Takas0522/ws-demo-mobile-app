# iOS ユニットテスト ワークフロー詳細

このドキュメントは `mobile-unit-testing` スキルの各ステップの詳細手順を定義する。

---

## ステップ 1: テストシナリオの作成

### 1-1. 対象コンポーネントの調査

テスト対象のソースコードを読み込み、以下を把握する:

- クラスの責務と公開インターフェース
- 依存コンポーネント（モック対象）
- 正常系・異常系のユースケース
- 境界値や特殊な入力値

**主要テスト対象コンポーネント**:

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

### テストファイルの配置先

```
MobileApp/MobileAppTests/
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

### テスト実装の手順

1. [xctest-test.swift](../templates/xctest-test.swift) テンプレートをベースにテストクラスを作成する
2. テスト対象クラスに注入する依存コンポーネントの `protocol` を定義する
3. `MockXxx` クラスを作成して `protocol` を実装する
4. `setUpWithError()` でテスト対象クラスを初期化する
5. `tearDownWithError()` で `sut` と `mock` を `nil` に設定してメモリを解放する

### ObservableObject のテスト（非同期）

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

### テストの実行

```bash
cd MobileApp
xcodebuild test \
  -project MobileApp.xcodeproj \
  -scheme MobileApp \
  -destination 'platform=iOS Simulator,name=iPhone 16' \
  -resultBundlePath TestResults.xcresult \
  2>&1 | xcpretty
```

シミュレーターのデバイス名確認:
```bash
xcrun simctl list devices available
```

### 問題発生時の対応

テスト実行中に問題が発生した場合（ビルドエラー、テスト失敗）は、**コードの修正は行わず、問題内容をレポートに記載するのみ**とする。

---

## ステップ 5: テスト結果レポートの作成

テスト終了後、以下のフォーマットで `temp/テストレポート_{timestamp}.md` を作成する。

- `timestamp` は `YYYYMMDD_HHmmss` 形式（例: `20260308_012345`）
- `temp/` ディレクトリが存在しない場合は `mkdir -p temp` で作成する

**レポートテンプレート**:

```markdown
# iOS ユニットテストレポート

**実施日時**: {timestamp}
**テスト対象バージョン**: {git rev-parse --short HEAD の出力}

---

## サマリー

| 総テスト数 | 成功 | 失敗 | スキップ | 成功率 |
|---|---|---|---|---|
| xx | xx | xx | xx | xx% |

---

## テストシナリオ一覧

### {コンポーネント名}

| # | テストケース名 | 結果 | 備考 |
|---|---|---|---|
| 1 | 正常ログイン | ✅ 成功 | |
| 2 | パスワード空欄 | ❌ 失敗 | XCTAssertFalse failed |

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
