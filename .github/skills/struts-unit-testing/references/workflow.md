# admin-struts ユニットテスト ワークフロー詳細

このドキュメントは `struts-unit-testing` スキルの各ステップの詳細手順を定義する。

---

## ステップ 1: テストシナリオの作成

### 1-1. 対象コンポーネントの調査

テスト対象のソースコードを読み込み、以下を把握する:

- クラスの責務と公開インターフェース
- 依存コンポーネント（モック対象）
- 正常系・異常系のユースケース
- 境界値や特殊な入力値

**レイヤー別テスト対象コンポーネント**:

#### Entity 層

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| Product | `entity/Product.java` | getter/setter、デフォルトコンストラクタ、Serializable |
| User | `entity/User.java` | getter/setter、デフォルトコンストラクタ、Serializable |
| Purchase | `entity/Purchase.java` | getter/setter、JOIN カラム（productName, userName） |
| Favorite | `entity/Favorite.java` | getter/setter、JOIN カラム |
| FeatureFlag | `entity/FeatureFlag.java` | getter/setter、Boolean 型の defaultValue |
| UserFeatureFlag | `entity/UserFeatureFlag.java` | getter/setter、JOIN カラム |
| ProductPriceHistory | `entity/ProductPriceHistory.java` | getter/setter |

#### DAO 層

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| ProductDao | `dao/ProductDao.java` | findAll, findById, searchByKeyword, update, countAll |
| UserDao | `dao/UserDao.java` | findByLoginId, findById, findAll, countAll, updatePassword |
| PurchaseDao | `dao/PurchaseDao.java` | create, findByUserId, findAll |
| FavoriteDao | `dao/FavoriteDao.java` | findByUserId, add, delete, findByUserIdAndProductId |
| FeatureFlagDao | `dao/FeatureFlagDao.java` | findAll, findByFlagKey |
| UserFeatureFlagDao | `dao/UserFeatureFlagDao.java` | findByUserId, upsert |
| ProductPriceHistoryDao | `dao/ProductPriceHistoryDao.java` | create, findByProductId |

#### Service 層

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| AuthService | `service/AuthService.java` | authenticate（admin 限定）、authenticateUser（全ユーザー）、authenticateAdmin、null/空入力、DB エラー |
| ProductService | `service/ProductService.java` | getProducts, searchProducts, getProductById, updateProduct, updateProductForApi, getPriceHistory、RuntimeException ラップ |
| UserService | `service/UserService.java` | getUsers, getUserById、RuntimeException ラップ |
| PurchaseService | `service/PurchaseService.java` | createPurchase（数量バリデーション: 100 の倍数）、購入履歴取得 |
| FavoriteService | `service/FavoriteService.java` | フィーチャーフラグチェック、お気に入り CRUD |
| FeatureFlagService | `service/FeatureFlagService.java` | getUserFeatureFlags（デフォルト値補完）、updateUserFeatureFlag |

#### Action 層（Web UI）

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| LoginAction | `action/auth/LoginAction.java` | input 表示、認証成功/失敗、バリデーション（空入力）、セッション設定 |
| LogoutAction | `action/auth/LogoutAction.java` | セッションクリア |
| ProductListAction | `action/product/ProductListAction.java` | 商品一覧取得、戻り値 SUCCESS |
| ProductDetailAction | `action/product/ProductDetailAction.java` | productId null/存在しない商品/正常取得 |
| ProductUpdateAction | `action/product/ProductUpdateAction.java` | バリデーション（商品名空/長さ、単価範囲）、更新成功 |
| UserListAction | `action/user/UserListAction.java` | ユーザー一覧取得 |
| FeatureFlagAction | `action/featureflag/FeatureFlagAction.java` | userId 指定あり/なし、ユーザー・フラグ一覧取得 |
| FeatureFlagUpdateAction | `action/featureflag/FeatureFlagUpdateAction.java` | パラメータ不足、フラグ更新 |

#### Action 層（API）

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| ApiAuthAction | `api/auth/ApiAuthAction.java` | ログイン、ログアウト、トークンリフレッシュ、パスワード変更 |
| ApiProductAction | `api/product/ApiProductAction.java` | 商品一覧・検索・詳細・更新・価格履歴 |
| ApiPurchaseAction | `api/purchase/ApiPurchaseAction.java` | 購入作成、購入履歴 |
| ApiFavoriteAction | `api/favorite/ApiFavoriteAction.java` | お気に入り一覧・追加・削除 |
| ApiFeatureFlagAction | `api/featureflag/ApiFeatureFlagAction.java` | フラグ取得・更新 |
| ApiHealthAction | `api/health/ApiHealthAction.java` | ヘルスチェック応答 |

#### Util / Interceptor 層

| コンポーネント | ファイル | テストの観点 |
|---|---|---|
| DatabaseUtil | `util/DatabaseUtil.java` | initialize、getConnection、getDbPath（テスト済み — 既存テスト参照） |
| PasswordUtil | `util/PasswordUtil.java` | hashPassword、checkPassword（互換性） |
| JwtUtil | `util/JwtUtil.java` | generateToken、parseToken（有効/無効/期限切れ） |
| ApiResponseUtil | `util/ApiResponseUtil.java` | writeSuccess、writeError |
| AuthInterceptor | `interceptor/AuthInterceptor.java` | セッションあり/なしの判定 |
| JwtInterceptor | `interceptor/JwtInterceptor.java` | Authorization ヘッダー検証 |

### 1-2. テストシナリオの記述形式

以下の形式でシナリオを記述する（Markdown テーブル形式）:

```markdown
## テストシナリオ: {コンポーネント名}

| # | テストケース名 | 前提条件 | 操作・入力 | 期待結果 | 分類 |
|---|---|---|---|---|---|
| 1 | 正常: 全商品取得 | DB に商品が存在 | getProducts() を呼び出す | 商品リストが返る | 正常系 |
| 2 | 異常: DB エラー | DAO が SQLException をスロー | getProducts() を呼び出す | RuntimeException がスローされる | 異常系 |
| 3 | 境界値: 空リスト | DB に商品が0件 | getProducts() を呼び出す | 空リストが返る | 境界値 |
```

---

## ステップ 2: テストシナリオのレビュー

### 2-1. レビューの観点（ブラックボックステスト）

以下の 6 つの観点でシナリオの網羅性を評価する:

1. **同値分割**: 各入力値クラスが代表値でカバーされているか
2. **境界値分析**: 境界条件（null、空文字、0、負値、最大長、リスト空）がカバーされているか
3. **エラーケース**: SQLException ラップ、不正パラメータ、認証失敗、存在しないエンティティがカバーされているか
4. **状態遷移**: ログイン前後、セッション有無、フィーチャーフラグ ON/OFF がカバーされているか
5. **副作用**: DB 更新、セッション変更、レスポンス書き込みがテストされているか
6. **セキュリティ**: 認証・認可チェック、admin 以外のユーザーの拒否、JWT 検証がカバーされているか

### 2-2. レビュー手順

1. 上記 6 つの観点でシナリオを評価する
2. 不足しているケースを特定し、具体的な改善点を列挙する
3. 問題がある場合はシナリオを修正する
4. 修正後に再レビューを実施する（**最大 2 回まで**）
5. 最終的なシナリオを確定する

---

## ステップ 3: テストの実装

### 3-1. テストの配置先

```
src/admin-struts/src/test/java/com/example/admin/
├── entity/
│   ├── ProductTest.java
│   ├── UserTest.java
│   ├── PurchaseTest.java
│   ├── FavoriteTest.java
│   ├── FeatureFlagTest.java
│   ├── UserFeatureFlagTest.java
│   └── ProductPriceHistoryTest.java
├── dao/
│   ├── ProductDaoTest.java
│   ├── UserDaoTest.java
│   └── ...
├── service/
│   ├── AuthServiceTest.java
│   ├── ProductServiceTest.java
│   ├── UserServiceTest.java
│   ├── PurchaseServiceTest.java
│   ├── FavoriteServiceTest.java
│   └── FeatureFlagServiceTest.java
├── action/
│   ├── auth/
│   │   ├── LoginActionTest.java
│   │   └── LogoutActionTest.java
│   ├── product/
│   │   ├── ProductListActionTest.java
│   │   ├── ProductDetailActionTest.java
│   │   └── ProductUpdateActionTest.java
│   ├── user/
│   │   └── UserListActionTest.java
│   └── featureflag/
│       ├── FeatureFlagActionTest.java
│       └── FeatureFlagUpdateActionTest.java
├── interceptor/
│   ├── AuthInterceptorTest.java
│   └── JwtInterceptorTest.java
├── util/
│   ├── DatabaseUtilTest.java  ← 既存
│   ├── PasswordUtilTest.java
│   ├── JwtUtilTest.java
│   └── ApiResponseUtilTest.java
└── dto/
    └── ApiResponseTest.java
```

### 3-2. 実装ガイドライン

1. [junit-test.java](../templates/junit-test.java) テンプレートをベースにテストクラスを作成する
2. テスト命名規則: `{methodName}_when{Condition}_expect{Result}`
3. AAA パターン（Arrange / Act / Assert）で記述する
4. 各テストメソッドは 1 つの振る舞いのみを検証する

### 3-3. レイヤー別実装のポイント

#### Entity テスト

- getter/setter の動作確認
- デフォルトコンストラクタで全フィールドが null であることの確認
- Mockito は不要

#### DAO テスト（結合テスト寄り）

- `DatabaseUtil.initialize("target/test-data/test.db")` でテスト用 DB を初期化
- `@Before` でテーブル作成・テストデータ投入、`@After` で DB ファイル削除
- 実際の SQL 実行を検証する

#### Service テスト

- DAO をモック化（Mockito）して Service のビジネスロジックを単体検証
- `when(mockDao.method()).thenReturn(...)` で DAO の戻り値を制御
- `when(mockDao.method()).thenThrow(new SQLException(...))` で例外パスを検証
- Service の setter メソッドでモック DAO をインジェクションする

#### Action テスト（Web UI）

- Service をモック化して Action のロジックを単体検証
- `execute()` の戻り値（`SUCCESS`, `ERROR`, `INPUT`）を検証
- `addFieldError` / `addActionError` の呼び出しを `getFieldErrors()` / `getActionErrors()` で検証
- `SessionAware` を実装する Action には `HashMap` をセットする

#### Action テスト（API）

- Service をモック化して API Action のロジックを単体検証
- `HttpServletRequest` / `HttpServletResponse` をモック化する（`org.mockito.Mockito.mock()` で生成）
- `ServletActionContext` は直接使用せず、Action の setter でリクエスト・レスポンスを注入する設計が望ましい
- JSON レスポンスの検証: `StringWriter` で `HttpServletResponse.getWriter()` をキャプチャし、出力された JSON 文字列を検証する
- HTTP ステータスコード: `verify(mockResponse).setStatus(200)` 等で検証
- 認証ヘッダー: `when(mockRequest.getHeader("Authorization")).thenReturn("Bearer xxx")` でトークンを設定

```java
// API Action テストの典型パターン
@Mock private HttpServletRequest mockRequest;
@Mock private HttpServletResponse mockResponse;

@Before
public void setUp() throws Exception {
    StringWriter sw = new StringWriter();
    when(mockResponse.getWriter()).thenReturn(new PrintWriter(sw));
    sut = new ApiProductAction();
    sut.setProductService(mockProductService);
    sut.setServletRequest(mockRequest);
    sut.setServletResponse(mockResponse);
}
```

#### Util テスト

- 純粋なユーティリティメソッドのため、モックは最小限
- `JwtUtil` テストでは `setSecret()` / `setExpirationHours()` を設定してからテスト
- `PasswordUtil` テストでは `hashPassword` → `checkPassword` の往復検証

### 3-4. Mockito の導入手順

`pom.xml` に以下を追加する（Java 7 互換の Mockito 2.x 最終版）:

```xml
<dependency>
    <groupId>org.mockito</groupId>
    <artifactId>mockito-core</artifactId>
    <version>2.28.2</version>
    <scope>test</scope>
</dependency>
```

**注意**: Mockito 3.x 以降は Java 8 必須のため使用不可。

---

## ステップ 4: テストの実施

### 4-1. テスト実行コマンド

```bash
# プロジェクトディレクトリに移動
cd src/admin-struts

# 全テスト実行
mvn test

# 特定のテストクラスのみ実行
mvn test -Dtest=com.example.admin.service.ProductServiceTest

# 特定のテストメソッドのみ実行
mvn test -Dtest=com.example.admin.service.ProductServiceTest#getProducts_whenDaoReturnsProducts_expectProductList
```

### 4-2. テスト結果の確認

- Maven Surefire レポート: `src/admin-struts/target/surefire-reports/`
- コンソール出力で成功/失敗を確認

### 4-3. 問題発生時の対応

**テストの実施で問題が発生した場合は、修正は行わずユーザーに報告のみ行う。** 報告には以下を含める:

- 失敗したテストクラス・メソッド名
- エラーメッセージとスタックトレースの要約
- 推定される原因
- 提案する修正箇所

---

## ステップ 5: テスト結果レポートの作成

### 5-1. レポート出力先

```
temp/テストレポート_{YYYYMMDD_HHmmss}.md
```

例: `temp/テストレポート_20260320_143000.md`

### 5-2. レポートのテンプレート

```markdown
# ユニットテスト結果レポート

- **対象プロジェクト**: admin-struts
- **実行日時**: {YYYY-MM-DD HH:mm:ss}
- **実行環境**: Java {version}, Maven {version}
- **テストフレームワーク**: JUnit 4.12

## サマリー

| 項目 | 値 |
|---|---|
| テストクラス数 | {数値} |
| テストメソッド数 | {数値} |
| 成功 | {数値} |
| 失敗 | {数値} |
| エラー | {数値} |
| スキップ | {数値} |
| 実行時間 | {秒}s |

## テスト結果一覧

### {レイヤー名}（例: Service 層）

| # | テストクラス | メソッド | 結果 | 実行時間 |
|---|---|---|---|---|
| 1 | ProductServiceTest | getProducts_whenDaoReturnsProducts_expectProductList | ✅ 成功 | 0.05s |
| 2 | AuthServiceTest | authenticate_whenInvalidPassword_expectNull | ✅ 成功 | 0.03s |

## 失敗・エラー詳細

### {テストクラス}#{メソッド名}

- **種別**: 失敗 / エラー
- **メッセージ**: {エラーメッセージ}
- **原因分析**: {推定原因}
- **推奨対応**: {修正の方針}

## 課題・所見

- {テストを通じて発見された課題や改善提案}
```
