---
description: "Struts 2 Webアプリケーション（admin-struts）開発のための命名規則・設計原則・レイヤー構造・コードスタイル・開発工程のガイドライン。Apache Struts 2・Spring Framework・OWASP セキュアコーディングガイドラインに基づく。"
applyTo: "src/admin-struts/**"
---

# Struts Webアプリ開発規約

このファイルは `src/admin-struts/` 配下の Apache Struts 2 Webアプリケーション開発に適用される規約を定義します。
Apache Struts 2 公式ドキュメント・Spring Framework ベストプラクティス・OWASP セキュアコーディングガイドラインを参考に構成しています。

---

## バックエンド構成

admin-struts は管理者向け Web UI と REST API の2つのインターフェースを提供するハイブリッドアプリケーションである。

```
管理者ブラウザ → admin-struts (Java/Struts2, port 8082) → SQLite Database
モバイルアプリ → admin-struts REST API (/api/v1/*) → SQLite Database
```

- **Web UI**: セッションベース認証 + JSP ビュー（管理者専用）
- **REST API**: JWT トークン認証 + JSON レスポンス（モバイル/外部クライアント向け）
- 依存管理は Maven（`pom.xml`）で行う
- Spring Framework によるDI（`applicationContext.xml`）を使用する

---

## 目的とスコープ

- **対象**: `src/admin-struts/` 配下の Java ソースコード、JSP、設定ファイル
- **目的**: 一貫性・保守性・テスタビリティの高い Struts 2 アプリケーションコードを生成・レビューするためのガイドライン

---

## 開発工程

以下の順序で開発を進める。各ステップを省略しないこと。

1. **仕様の理解** — タスクの要件と影響範囲を正確に把握する。既存の Action・Service・DAO・JSP を調査し、変更が必要なファイルを特定する。`struts-admin.xml` / `struts-api.xml` のルーティング定義を確認する
2. **開発プランの作成** — 変更するファイル・クラス・メソッドを列挙し、実装方針を計画する。レイヤー構造への準拠（Action → Service → DAO）を確認する。Struts XML 設定の変更が必要かどうかも検討する
3. **開発** — プランに従ってコードを実装する。コードスタイルと命名規則を遵守する。新しい Action を追加する場合は、対応する Struts XML 設定・JSP・Spring Bean 定義も併せて作成する
4. **テストの作成** — 実装した機能のユニットテスト（JUnit 4）を作成する。Service・DAO・Util クラスを優先的にテストする
5. **テストの実施** — `mvn test` でテストを実行し、すべてのテストがパスすることを確認する。ビルドは `mvn clean package` で検証する

---

## 設計原則

- **レイヤー分離**: Action クラスにビジネスロジックを記述しない。ロジックは必ず Service 層に委譲し、データアクセスは DAO 層に限定する
- **DI（依存性注入）**: Service・DAO の依存関係は Spring のセッター注入（`@Autowired`）で解決する。Action クラスへの注入は Struts2-Spring プラグインを通じてセッター注入で行う。`new` でのインスタンス生成禁止
- **リクエストスコープ Action**: Struts2 の Action はリクエストごとに新しいインスタンスが生成される。Action クラスにインスタンスフィールドを持つことは安全だが、`static` フィールドでリクエスト固有の状態を保持してはならない
- **Interceptor Stack パターン**: 横断的関心事（認証・ログ・例外処理）は Struts2 Interceptor として実装し、Action から分離する。Interceptor は `struts-admin.xml` / `struts-api.xml` で Stack として定義する
- **Result Type の使い分け**: Web UI Action は適切な Result Type を返す
  - `dispatcher`（デフォルト）: JSP へのフォワード
  - `redirect`: 外部 URL またはコンテキスト内の別パスへのリダイレクト
  - `redirectAction`: 別の Action 実行へのリダイレクト（PRG パターン更新系で使用）
- **設定の外部化**: データベースパス・JWT シークレット等の環境依存値は `application.properties` に定義し、`PropertyPlaceholder` 経由で注入する。ソースコードへのハードコーディング禁止
- **単一責任の原則（SRP）**: 1つの Action クラスは1つの画面機能（または1つの API エンドポイントグループ）に対応する。1つの Service クラスは1つのドメインエンティティに対応する
- **テスタビリティ**: Service・DAO はセッター注入により、テスト時にモックに差し替え可能にする

---

## レイヤー構造

### パッケージ構成

```
com.example.admin/
├── action/                  # Struts2 Action（Web UI）
│   ├── auth/                #   認証: LoginAction, LogoutAction
│   ├── product/             #   商品管理: ProductListAction, ProductDetailAction, ProductUpdateAction
│   ├── featureflag/         #   機能フラグ: FeatureFlagAction, FeatureFlagUpdateAction
│   ├── user/                #   ユーザー管理: UserListAction
│   └── DashboardAction      #   ダッシュボード
├── api/                     # Struts2 Action（REST API）
│   ├── auth/                #   認証API: ApiAuthAction
│   ├── product/             #   商品API: ApiProductAction
│   ├── purchase/            #   購入API: ApiPurchaseAction
│   ├── favorite/            #   お気に入りAPI: ApiFavoriteAction
│   ├── featureflag/         #   機能フラグAPI: ApiFeatureFlagAction
│   └── health/              #   ヘルスチェック: ApiHealthAction
├── service/                 # ビジネスロジック層
│   └── AuthService, UserService, ProductService, PurchaseService, ...
├── dao/                     # データアクセス層（Apache Commons DbUtils）
│   ├── BaseDao              #   共通基底クラス（接続管理・ハンドラ生成）
│   └── ProductDao, UserDao, FavoriteDao, PurchaseDao, ...
├── entity/                  # ドメインモデル（POJO）
│   └── User, Product, Purchase, Favorite, FeatureFlag, ...
├── dto/                     # データ転送オブジェクト
│   └── ApiResponse          #   API レスポンスラッパー
├── interceptor/             # Struts2 インターセプター
│   ├── AuthInterceptor      #   セッション認証（Web UI 用）
│   └── JwtInterceptor       #   JWT 認証（API 用）
├── filter/                  # サーブレットフィルター
│   └── CorsFilter           #   CORS ヘッダー設定
└── util/                    # ユーティリティ
    ├── DatabaseUtil          #   SQLite 接続管理
    ├── JwtUtil               #   JWT トークン生成・検証
    ├── PasswordUtil          #   BCrypt パスワードハッシュ
    └── ApiResponseUtil       #   JSON レスポンス出力
```

### レイヤー間の依存方向

```
Action / API Action
    ↓ （Spring DI: セッター注入）
Service（ビジネスロジック）
    ↓ （@Autowired セッター注入）
DAO（データアクセス: Commons DbUtils）
    ↓ （DatabaseUtil.getConnection()）
SQLite Database
```

- **上位レイヤーから下位レイヤーのみ依存可能**。逆方向の依存禁止
- Action が DAO を直接呼び出してはならない（必ず Service を経由する）
- DAO が Action や Service に依存してはならない

### 設定ファイルの役割

| ファイル | 役割 |
| --- | --- |
| `struts.xml` | Struts2 マスター設定（他XMLのインクルード、Spring連携設定） |
| `struts-admin.xml` | Web UI の Action マッピング・インターセプタースタック定義 |
| `struts-api.xml` | REST API の Action マッピング・JWT インターセプタースタック定義 |
| `applicationContext.xml` | Spring Bean 定義・コンポーネントスキャン設定 |
| `application.properties` | 環境依存の設定値（DB パス・JWT シークレット等） |
| `web.xml` | サーブレット/フィルター設定・セッションタイムアウト |

---

## Struts2 プロパティ公開パターン

Struts2 では Action のプロパティが ValueStack を通じて JSP や Interceptor からアクセスされる。用途に応じて適切なパターンを使い分けること。

| パターン | 用途 | 例 |
| --- | --- | --- |
| Action プロパティ + getter/setter | フォーム入出力・JSP表示データ。ValueStack 経由で `<s:property>` や `<s:textfield>` からアクセスされる | `private String productName;` + getter/setter |
| `SessionAware` インターフェース | セッションへの読み書き（認証情報の保存・取得）。`setSession(Map)` でセッション Map を受け取る | `LoginAction implements SessionAware` |
| `ServletActionContext` 直接アクセス | HTTP リクエスト/レスポンスの直接操作。REST API Action で JSON レスポンスを書き込む場合に使用する | `ServletActionContext.getResponse()` |

### Action プロパティ（基本パターン — 最も頻繁に使用）

```java
public class ProductDetailAction extends ActionSupport {
    // 入力: リクエストパラメータから自動バインド
    private int productId;

    // 出力: JSP の <s:property value="product.productName" /> でアクセス
    private Product product;

    public void setProductId(int productId) { this.productId = productId; }
    public Product getProduct() { return product; }

    @Override
    public String execute() {
        product = productService.getProduct(productId);
        return SUCCESS;
    }
}
```

### SessionAware（セッション操作が必要な場合のみ使用）

```java
public class LoginAction extends ActionSupport implements SessionAware {
    private Map<String, Object> session;
    private String loginId;
    private String password;

    @Override
    public void setSession(Map<String, Object> session) {
        this.session = session;
    }

    @Override
    public String execute() {
        User user = authService.authenticate(loginId, password);
        if (user != null) {
            session.put("loginUser", user);  // セッションに認証情報を保存
            return SUCCESS;
        }
        addActionError("ログインIDまたはパスワードが正しくありません");
        return INPUT;
    }
}
```

- `SessionAware` は認証関連の Action（`LoginAction`, `LogoutAction`）でのみ使用する
- 一般的な Action ではセッションを直接操作しない（Interceptor で認証チェックを行う）

---

## 命名規則

### クラス名

| 対象 | 規則 | 例 |
| --- | --- | --- |
| Action（Web UI） | `[機能名]Action` | `LoginAction`, `ProductListAction`, `DashboardAction` |
| Action（REST API） | `Api[機能名]Action` | `ApiAuthAction`, `ApiProductAction` |
| Service | `[エンティティ名]Service` | `UserService`, `ProductService`, `FeatureFlagService` |
| DAO | `[エンティティ名]Dao` | `UserDao`, `ProductDao`, `ProductPriceHistoryDao` |
| Entity | PascalCase（サフィックスなし） | `User`, `Product`, `FeatureFlag` |
| DTO | 用途を表す名前 | `ApiResponse` |
| Interceptor | `[名前]Interceptor` | `AuthInterceptor`, `JwtInterceptor` |
| Filter | `[名前]Filter` | `CorsFilter` |
| Utility | `[機能名]Util` | `DatabaseUtil`, `JwtUtil`, `PasswordUtil` |
| テストクラス | `[対象クラス名]Test` | `DatabaseUtilTest`, `ProductServiceTest` |

### メソッド・変数・定数

| 対象 | 規則 | 例 |
| --- | --- | --- |
| メソッド名 | camelCase | `getProducts()`, `findByLoginId()`, `checkPassword()` |
| 変数名（Java） | camelCase | `loginId`, `productName`, `isAuthenticated` |
| 定数 | UPPER_SNAKE_CASE | `DATE_FORMAT`, `COLUMN_TO_PROPERTY` |
| パッケージ | 全て小文字・レイヤー別 | `com.example.admin.service`, `com.example.admin.dao` |

### データベースカラムとの対応

- データベースカラム名: `snake_case`（例: `product_id`, `login_id`）
- Java プロパティ名: `camelCase`（例: `productId`, `loginId`）
- DAO の `COLUMN_TO_PROPERTY` マップでマッピングを定義する

### JSP・ビューファイル

| 対象 | 規則 | 例 |
| --- | --- | --- |
| JSP ディレクトリ | 機能別にサブディレクトリ | `WEB-INF/jsp/product/`, `WEB-INF/jsp/auth/` |
| JSP ファイル名 | 画面の役割を表す小文字名 | `list.jsp`, `edit.jsp`, `login.jsp` |
| 共通パーツ | `WEB-INF/jsp/common/` に配置 | `header.jsp`, `footer.jsp` |
| CSS/JS | `webapp/css/`, `webapp/js/` に配置 | `style.css`, `app.js` |

### Struts XML 設定

| 対象 | 規則 | 例 |
| --- | --- | --- |
| Action 名 | ハイフン区切り小文字 | `product-list`, `login-submit`, `feature-flag-update` |
| パッケージ名 | 用途を表す名前 | `admin`, `api-auth`, `api-main` |
| 名前空間 | Web UI: `/admin`、API: `/api/v1` | `/admin`, `/api/v1/auth` |

---

## コードスタイル

### 共通

- インデント: **4スペース**（タブ不使用）
- 行の最大長: **120文字**
- 空行でロジックのブロックを区切る
- マジックナンバー・マジックストリングは定数として定義する
- ソースファイルのエンコーディング: **UTF-8**
- `@Override` アノテーションは常に付与する

### Action クラス（Web UI）

- `ActionSupport` を継承する
- Service の注入はセッターインジェクションで行う（コンストラクタインジェクション不使用）
- 戻り値は Struts2 の結果定数（`SUCCESS`, `INPUT`, `ERROR`, `LOGIN`）を使用する
- フォーム入出力はプロパティ + getter/setter で公開する
- バリデーションは `validate()` メソッドまたは `addFieldError()` で行う

```java
// Good: レイヤー分離 + セッターDI
public class ProductListAction extends ActionSupport {
    private ProductService productService;
    private List<Product> products;

    // Spring DI
    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    @Override
    public String execute() {
        products = productService.getProducts();
        return SUCCESS;
    }

    // JSP からアクセス
    public List<Product> getProducts() {
        return products;
    }
}
```

```java
// Bad: Action 内にビジネスロジック・データアクセスを記述
public class ProductListAction extends ActionSupport {
    @Override
    public String execute() {
        Connection conn = DatabaseUtil.getConnection(); // DAO経由にすること
        // SQL直書き — 禁止
        ResultSet rs = stmt.executeQuery("SELECT * FROM products");
        return SUCCESS;
    }
}
```

### Action クラス（REST API）

- `ActionSupport` を継承する
- JSON リクエストボディは `readJsonBody()` で読み取る
- レスポンスは `ApiResponseUtil.writeSuccess()` / `writeError()` で出力する
- `execute()` / 各メソッドは `null` を返す（直接レスポンスを書き込むため）

```java
public class ApiProductAction extends ActionSupport {
    private ProductService productService;

    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    public String list() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            List<Product> products = productService.getProducts();
            ApiResponseUtil.writeSuccess(response, products);
        } catch (Exception e) {
            logger.error("Failed to list products", e);
            ApiResponseUtil.writeError(response, 500, "商品一覧の取得に失敗しました");
        }
        return null;
    }
}
```

### Service クラス

- `@Service` アノテーションを付与する
- DAO の注入は `@Autowired` セッターインジェクションで行う
- `SQLException` をキャッチし、日本語メッセージ付きの `RuntimeException` でラップしてスローする
- ロガー（SLF4J）でエラーを記録する

```java
@Service
public class ProductService {
    private static final Logger logger = LoggerFactory.getLogger(ProductService.class);
    private ProductDao productDao;

    @Autowired
    public void setProductDao(ProductDao productDao) {
        this.productDao = productDao;
    }

    public List<Product> getProducts() {
        try {
            return productDao.findAll();
        } catch (SQLException e) {
            logger.error("商品一覧の取得に失敗しました", e);
            throw new RuntimeException("商品一覧の取得に失敗しました", e);
        }
    }
}
```

### DAO クラス

- `BaseDao` を継承し、`@Repository` アノテーションを付与する
- Apache Commons DbUtils の `QueryRunner` を使用する
- `COLUMN_TO_PROPERTY` 定数マップでデータベースカラム名と Java プロパティ名のマッピングを定義する
- `Connection` は `getConnection()` で取得し、`finally` ブロックで必ず `closeQuietly()` で閉じる
- SQL は文字列定数として定義する（動的な文字列結合による SQL インジェクション禁止。パラメータは `?` プレースホルダーを使用する）

```java
@Repository
public class ProductDao extends BaseDao {
    private static final Map<String, String> COLUMN_TO_PROPERTY;
    static {
        Map<String, String> map = new HashMap<>();
        map.put("product_id", "productId");
        map.put("product_name", "productName");
        map.put("price", "price");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    public List<Product> findAll() throws SQLException {
        String sql = "SELECT * FROM products ORDER BY product_id";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<Product> handler =
                createBeanListHandler(Product.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler);
        } finally {
            closeQuietly(conn);
        }
    }

    public Product findById(int productId) throws SQLException {
        String sql = "SELECT * FROM products WHERE product_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanHandler<Product> handler =
                createBeanHandler(Product.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, productId);
        } finally {
            closeQuietly(conn);
        }
    }
}
```

### バリデーションパターン

Action クラス内で `addFieldError()` を使用してバリデーションを実装する。

```java
// Good: 入力値を検証し、エラー時は INPUT を返してフォームを再表示
@Override
public String execute() {
    if (productName == null || productName.trim().isEmpty()) {
        addFieldError("productName", "商品名を入力してください");
    }
    if (unitPrice == null || unitPrice <= 0) {
        addFieldError("unitPrice", "単価は1以上の数値を入力してください");
    }
    if (hasFieldErrors()) {
        return INPUT;  // フォーム再表示（入力値は ValueStack 経由で保持される）
    }
    productService.updateProduct(productId, productName, unitPrice, description, imageUrl);
    return SUCCESS;
}
```

```java
// Bad: バリデーションなしで更新処理を実行
@Override
public String execute() {
    productService.updateProduct(productId, productName, unitPrice, description, imageUrl);
    return SUCCESS;  // 不正な値がそのまま保存される
}
```

- JSP 側では `<s:fielderror fieldName="productName" />` でフィールド単位のエラーを表示する
- Action 全体のエラーは `addActionError()` + `<s:actionerror />` で表示する

### Entity クラス

- `Serializable` を実装し、`serialVersionUID` を定義する
- 引数なしのデフォルトコンストラクタを必ず用意する（DbUtils `BeanProcessor` の要件）
- プロパティは `private` + getter/setter で公開する

```java
public class Product implements Serializable {
    private static final long serialVersionUID = 1L;

    private Integer productId;
    private String productName;
    private Integer price;

    public Product() {}

    // getter/setter ...
}
```

### JSP

- Struts2 タグライブラリを使用する（タグ一覧は「Struts2 タグライブラリリファレンス」セクションを参照）
- JSTL（`<c:if>`, `<c:forEach>`）も併用可能
- コンテキストパスは `${pageContext.request.contextPath}` で取得する
- 共通パーツ（ヘッダー・フッター）は `<jsp:include>` で組み込む
- XSS 対策として、ユーザー入力を含む出力には `<s:property>` のエスケープ機能を使用する。生の `<%= %>` での出力禁止
- アクセシビリティ要件（`<label for="">`、セマンティック HTML）を遵守する（「アクセシビリティ」セクション参照）

```jsp
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>

<jsp:include page="/WEB-INF/jsp/common/header.jsp" />

<table>
    <thead>
        <tr><th>ID</th><th>商品名</th><th>価格</th></tr>
    </thead>
    <tbody>
        <s:iterator value="products" var="product">
            <tr>
                <td><s:property value="#product.productId" /></td>
                <td><s:property value="#product.productName" /></td>
                <td><s:property value="#product.price" /></td>
            </tr>
        </s:iterator>
    </tbody>
</table>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp" />
```

### Struts2 タグライブラリリファレンス

| タグ | 用途 | 例 |
| --- | --- | --- |
| `<s:form>` | フォームコンテナ。`action` と `namespace` を指定する | `<s:form action="login-submit" namespace="/admin">` |
| `<s:textfield>` | テキスト入力。Action プロパティに自動バインドされる | `<s:textfield name="productName" id="productName" />` |
| `<s:password>` | パスワード入力 | `<s:password name="password" id="password" />` |
| `<s:textarea>` | 複数行テキスト入力 | `<s:textarea name="description" id="description" />` |
| `<s:hidden>` | 隠しフィールド | `<s:hidden name="productId" />` |
| `<s:select>` | ドロップダウン選択 | `<s:select name="userId" list="users" listKey="userId" listValue="userName" />` |
| `<s:submit>` | 送信ボタン | `<s:submit value="更新" cssClass="btn btn-primary" />` |
| `<s:property>` | 値の出力（自動エスケープ） | `<s:property value="product.productName" />` |
| `<s:iterator>` | コレクションのループ | `<s:iterator value="products" var="p">` |
| `<s:if>` / `<s:else>` | 条件分岐 | `<s:if test="products.size() > 0">` |
| `<s:actionerror>` | Action レベルのエラー表示 | `<s:actionerror cssClass="alert alert-danger" />` |
| `<s:fielderror>` | フィールドレベルのエラー表示 | `<s:fielderror fieldName="productName" />` |

---

## アクセシビリティ

Web UI の JSP ページでは、以下のアクセシビリティ要件を遵守する。

- **`<label>` と `for` 属性**: すべてのフォーム入力要素に `<label for="id">` を対応付ける。Struts2 タグの `id` 属性を明示的に設定すること

```jsp
<!-- Good: label と入力フィールドが for/id で関連付けされている -->
<label for="productName">商品名 <span class="required">*</span></label>
<s:textfield name="productName" id="productName" cssClass="form-control" />
<s:fielderror fieldName="productName" cssClass="alert alert-danger" />
```

```jsp
<!-- Bad: label の for 属性がない、または id と一致しない -->
<label>商品名</label>
<s:textfield name="productName" cssClass="form-control" />
```

- **セマンティック HTML**: テーブルには `<thead>` / `<tbody>` を使用する。見出しには適切なレベルの `<h1>`〜`<h6>` を使用する。`<div>` の乱用を避け、`<nav>`, `<main>`, `<section>`, `<footer>` 等のセマンティック要素を使用する
- **エラーメッセージの視認性**: `<s:fielderror>` / `<s:actionerror>` に視覚的に区別できる CSS クラス（`alert alert-danger` 等）を付与する
- **キーボード操作**: フォーム要素のタブ順序が自然な並びになるようにする。JavaScript による `onclick` イベントにはキーボード代替（`onkeypress` または `<button>` 要素の使用）を提供する
- **色のコントラスト**: テキストと背景の色のコントラスト比は WCAG 2.1 AA 基準（4.5:1 以上）を満たすこと

---

## 非同期処理・AJAX パターン

現在のアプリケーションは同期フォーム送信を基本とする。新規に非同期処理が必要な場合は以下のパターンに従う。

### REST API（既存パターン）

モバイルアプリ等の外部クライアントからの非同期リクエストは `/api/v1/*` の REST API Action が処理する。Struts2 JSON plugin を使用し、`ApiResponseUtil` で JSON レスポンスを返す。

### Web UI での AJAX（新規追加時）

Web UI に AJAX 機能を追加する場合は以下のパターンに従う。

```javascript
// JavaScript: Fetch API で API エンドポイントを呼び出す
async function loadProducts() {
    try {
        const response = await fetch(contextPath + '/api/v1/products', {
            headers: { 'Authorization': 'Bearer ' + token }
        });
        if (!response.ok) throw new Error('HTTP ' + response.status);
        const data = await response.json();
        renderProducts(data);
    } catch (error) {
        console.error('Failed to load products:', error);
        showError('商品の読み込みに失敗しました');
    }
}
```

- AJAX リクエストは既存の REST API エンドポイント（`/api/v1/*`）を使用する
- 新規に AJAX 専用の Web UI Action を作成しない（API エンドポイントを再利用する）
- エラー時はユーザーにフィードバックを表示する（サイレント失敗禁止）

---

## エラーハンドリング

### Action 層（Web UI）

- Service から例外がスローされた場合、`addActionError()` でメッセージを設定し `ERROR` を返す
- JSP 側で `<s:actionerror />` を使用してエラーメッセージを表示する
- バリデーションエラーは `addFieldError()` で設定し `INPUT` を返す

### Action 層（REST API）

- Service から例外がスローされた場合、`try-catch` でキャッチし `ApiResponseUtil.writeError()` で JSON エラーレスポンスを返す
- HTTP ステータスコードを適切に設定する（400: バリデーションエラー、401: 認証エラー、404: 未検出、500: サーバーエラー）

### Service 層

- DAO からの `SQLException` をキャッチし、ロガーでエラーを記録した後、日本語メッセージ付き `RuntimeException` でラップしてスローする
- ビジネスルール違反は明示的なメッセージ付き例外としてスローする

### DAO 層

- `SQLException` はそのままスローする（Service 層でハンドリングする）
- `Connection` は `finally` ブロックで `closeQuietly()` により確実にクローズする

---

## セキュリティ

- **SQL インジェクション防止**: SQL 文に変数を文字列結合で埋め込まない。`?` プレースホルダーとパラメータバインドを使用する
- **XSS 防止**: JSP で `<s:property>` タグのエスケープ機能を使用する。生の `<%= %>` での出力禁止
- **パスワード**: `PasswordUtil`（BCrypt, strength=10）でハッシュ化する。平文での保存・比較禁止
- **JWT**: `JwtUtil` で HS256 トークンを生成・検証する。シークレットは `application.properties` で管理し、ソースコードにハードコーディングしない
- **CORS**: `CorsFilter` で API エンドポイント（`/api/*`）に CORS ヘッダーを設定する
- **セッション管理**: Web UI はセッションベース認証（`AuthInterceptor`）。セッションタイムアウトは `web.xml` で30分に設定

---

## テスト

- テストフレームワーク: **JUnit 4**
- テストクラス名: `[対象クラス名]Test`（例: `DatabaseUtilTest`, `ProductServiceTest`）
- テストメソッド名: `test[メソッド名]_[条件]_[期待結果]`
- `@Before` でセットアップ、`@After` でクリーンアップを行う
- テストデータベースは `target/test-data/` 配下に作成し、テスト後にクリーンアップする
- Service・DAO・Util クラスを優先的にテストする（Action クラスのテストは統合テストとして扱う）

```java
public class ProductServiceTest {
    private ProductService productService;
    private ProductDao mockProductDao;

    @Before
    public void setUp() {
        productService = new ProductService();
        mockProductDao = mock(ProductDao.class);
        productService.setProductDao(mockProductDao);
    }

    @Test
    public void testGetProducts_正常系_商品一覧を取得できる() throws SQLException {
        List<Product> expected = Arrays.asList(new Product(), new Product());
        when(mockProductDao.findAll()).thenReturn(expected);

        List<Product> actual = productService.getProducts();

        assertEquals(2, actual.size());
        verify(mockProductDao).findAll();
    }

    @Test(expected = RuntimeException.class)
    public void testGetProducts_異常系_DAOでSQLException発生時にRuntimeExceptionをスロー() throws SQLException {
        when(mockProductDao.findAll()).thenThrow(new SQLException("DB error"));

        productService.getProducts();
    }
}
```

---

## ビルドと検証

| 操作 | コマンド |
| --- | --- |
| ビルド | `cd src/admin-struts && mvn clean package` |
| テスト実行 | `cd src/admin-struts && mvn test` |
| 起動 | `cd src/admin-struts && mvn tomcat7:run` |
| 一括起動 | `scripts/start-admin-struts.sh` |
| 停止 | `scripts/stop-admin-struts.sh` |

- **ポート**: 8082（コンテキストパス `/admin-struts`）
- **Java バージョン**: Java 7（ソース互換性）
- **エンコーディング**: UTF-8
- ビルド前に `mvn clean` を実行して古い成果物を削除する
- テストは必ずコミット前に実行し、全件パスを確認する
