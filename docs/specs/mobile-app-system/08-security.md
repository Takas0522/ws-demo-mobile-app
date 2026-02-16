# mobile-app-system - セキュリティ要件

> 最終更新: 2025-01-08
> ステータス: Draft
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-08 | 初版作成 | AI Agent |

---

## 1. セキュリティ要件概要

本ドキュメントでは、mobile-app-systemのセキュリティ要件を定義します。
デモンストレーション用途ですが、基本的なセキュリティ対策は実装します。

## 2. 認証・認可

### 2.1 JWT認証

**SEC-001: JWT認証の実装**

#### 仕様
- **方式**: JSON Web Token (JWT)
- **アルゴリズム**: HS256（HMAC with SHA-256）
- **トークン有効期限**: 24時間
- **リフレッシュトークン**: なし（再ログイン必要）

#### JWTペイロード構造
```json
{
  "sub": "1",                    // ユーザーID
  "loginId": "user001",          // ログインID
  "userType": "user",            // ユーザー種別（user/admin）
  "iat": 1704700800,             // 発行日時（Unix timestamp）
  "exp": 1704787200              // 有効期限（Unix timestamp）
}
```

#### トークン生成
```java
String generateToken(User user) {
    return Jwts.builder()
        .setSubject(user.getUserId().toString())
        .claim("loginId", user.getLoginId())
        .claim("userType", user.getUserType())
        .setIssuedAt(new Date())
        .setExpiration(new Date(System.currentTimeMillis() + 86400000)) // 24時間
        .signWith(SignatureAlgorithm.HS256, SECRET_KEY)
        .compact();
}
```

#### トークン検証
```java
Claims validateToken(String token) {
    try {
        return Jwts.parser()
            .setSigningKey(SECRET_KEY)
            .parseClaimsJws(token)
            .getBody();
    } catch (ExpiredJwtException e) {
        throw new AuthenticationException("トークンの有効期限が切れています", "AUTH_004");
    } catch (JwtException e) {
        throw new AuthenticationException("トークンが無効です", "AUTH_002");
    }
}
```

---

**SEC-002: シークレットキー管理**

#### 要件
- シークレットキーは環境変数で管理
- ハードコーディング禁止
- 最小256ビット（32文字以上）

#### 環境変数
```bash
JWT_SECRET_KEY=your-secret-key-at-least-32-characters-long
```

#### セキュリティ考慮事項
- 本番環境では強力なランダム文字列を使用
- 定期的なローテーション（本システムでは不要）
- Gitにコミットしない（.gitignoreに追加）

---

### 2.2 パスワード管理

**SEC-010: パスワードハッシュ化**

#### 仕様
- **アルゴリズム**: bcrypt
- **ストレッチング回数**: 10（デフォルト）
- **ソルト**: bcryptが自動生成

#### パスワードハッシュ化
```java
String hashPassword(String plainPassword) {
    return BCrypt.hashpw(plainPassword, BCrypt.gensalt(10));
}
```

#### パスワード検証
```java
boolean verifyPassword(String plainPassword, String hashedPassword) {
    return BCrypt.checkpw(plainPassword, hashedPassword);
}
```

---

**SEC-011: パスワードポリシー**

| 項目 | 要件 | 理由 |
|------|------|------|
| 最小文字数 | 8文字 | 基本的なセキュリティ |
| 最大文字数 | 50文字 | システム制限 |
| 複雑性要件 | なし | デモ用途のため |
| パスワード履歴 | なし | デモ用途のため |
| パスワード変更 | なし | スコープ外 |
| パスワードリセット | なし | スコープ外 |

**注意**: 本番環境では以下を推奨
- 大文字、小文字、数字、記号の組み合わせ
- パスワード履歴管理（過去3世代）
- 定期的なパスワード変更

---

### 2.3 権限制御

**SEC-020: ロールベースアクセス制御（RBAC）**

#### ユーザー種別
| ユーザー種別 | 権限 | 説明 |
|------------|------|------|
| user | エンドユーザー | モバイルアプリ機能のみ |
| admin | 管理者 | 管理機能のみ |

#### API権限マトリクス

| API | user | admin |
|-----|------|-------|
| POST /api/v1/auth/login | ✅ | ❌ |
| POST /api/v1/auth/admin/login | ❌ | ✅ |
| GET /api/v1/products | ✅ | ✅ |
| GET /api/v1/products/search | ✅ | ✅ |
| GET /api/v1/products/{id} | ✅ | ✅ |
| PUT /api/v1/products/{id} | ❌ | ✅ |
| POST /api/v1/purchases | ✅ | ❌ |
| GET /api/v1/purchases | ✅ | ❌ |
| POST /api/v1/favorites | ✅ | ❌ |
| DELETE /api/v1/favorites/{id} | ✅ | ❌ |
| GET /api/v1/favorites | ✅ | ❌ |
| GET /api/v1/feature-flags | ✅ | ❌ |
| GET /api/v1/admin/users | ❌ | ✅ |
| PUT /api/v1/admin/users/{id}/feature-flags/{key} | ❌ | ✅ |

#### 実装例（Spring Security）
```java
@PreAuthorize("hasRole('USER')")
@PostMapping("/api/v1/purchases")
public ResponseEntity<PurchaseResponse> createPurchase(@RequestBody PurchaseRequest request) {
    // ...
}

@PreAuthorize("hasRole('ADMIN')")
@PutMapping("/api/v1/products/{id}")
public ResponseEntity<ProductResponse> updateProduct(@PathVariable Long id, @RequestBody ProductRequest request) {
    // ...
}
```

---

**SEC-021: リソース所有権チェック**

ユーザーは自分のリソースのみアクセス可能:

| 操作 | チェック内容 |
|------|------------|
| 購入履歴取得 | 自分の購入履歴のみ |
| お気に入り操作 | 自分のお気に入りのみ |
| 機能フラグ取得 | 自分の設定のみ |

#### 実装例
```java
public List<Purchase> getUserPurchases(Long userId) {
    // JWTから取得したuserIdと一致するか確認
    if (!currentUser.getUserId().equals(userId)) {
        throw new ForbiddenException("他のユーザーの購入履歴は閲覧できません");
    }
    return purchaseRepository.findByUserId(userId);
}
```

---

## 3. データ保護

### 3.1 通信の暗号化

**SEC-030: HTTPS/TLS**

#### 要件
- **プロトコル**: TLS 1.2以上
- **証明書**: 有効なSSL証明書
- **HTTP接続**: リダイレクト（HTTPS強制）

#### 開発環境
- 開発環境ではHTTPを許可（localhost）
- 本番環境ではHTTPSを強制

---

**SEC-031: API通信の保護**

| 項目 | 対策 |
|------|------|
| 通信路 | HTTPS/TLS |
| トークン送信 | Authorization ヘッダー（Bearer方式） |
| パスワード送信 | HTTPS経由のみ |

---

### 3.2 データ保存の暗号化

**SEC-040: モバイルアプリのデータ保存**

#### iOS
```swift
// Keychainに保存
let keychain = KeychainSwift()
keychain.set(token, forKey: "jwt_token")
```

#### Android
```java
// EncryptedSharedPreferencesに保存
EncryptedSharedPreferences sharedPreferences = EncryptedSharedPreferences.create(
    "secret_shared_prefs",
    MasterKey.DEFAULT_MASTER_KEY_ALIAS,
    context,
    EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
    EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
);

sharedPreferences.edit().putString("jwt_token", token).apply();
```

---

**SEC-041: 管理Webアプリのデータ保存**

| データ | 保存先 | 暗号化 |
|-------|-------|-------|
| JWTトークン | localStorage | なし |
| セッション情報 | メモリ | - |

**注意**: 本番環境ではlocalStorage使用は推奨されない（XSSリスク）。代替案:
- HttpOnly Cookie
- セッションストレージ

---

**SEC-042: データベース内のデータ**

| データ | 暗号化 | 方式 |
|-------|-------|------|
| パスワード | ✅ | bcrypt |
| 商品情報 | ❌ | 平文 |
| 購入情報 | ❌ | 平文 |
| ユーザー情報 | ❌ | 平文 |

**注意**: デモ用途のため、DB全体の暗号化（TDE）は実装しない

---

## 4. 入力検証

### 4.1 バリデーション戦略

**SEC-050: 多層防御**

1. **クライアント側バリデーション**: UX向上
2. **サーバー側バリデーション**: セキュリティ必須

**重要**: クライアント側バリデーションは回避可能のため、サーバー側で必ず実施

---

**SEC-051: 入力バリデーション項目**

| バリデーション種別 | 対象 | 例 |
|------------------|------|-----|
| 必須チェック | 全入力 | null/空文字チェック |
| 型チェック | 全入力 | 文字列、数値、真偽値 |
| 長さチェック | 文字列 | 1-100文字 |
| 範囲チェック | 数値 | 1以上 |
| 形式チェック | URL等 | URL形式 |
| ホワイトリスト | 列挙型 | user/admin |

#### 実装例（Spring Boot）
```java
@NotNull(message = "商品名は必須です")
@Size(min = 1, max = 100, message = "商品名は1文字以上100文字以内です")
private String productName;

@NotNull(message = "単価は必須です")
@Min(value = 1, message = "単価は1円以上です")
private Integer unitPrice;

@Pattern(regexp = "^(user|admin)$", message = "ユーザー種別が不正です")
private String userType;
```

---

### 4.2 SQLインジェクション対策

**SEC-060: パラメータ化クエリ**

#### 要件
- **ORM使用**: Spring Data JPA
- **PreparedStatement使用**: 動的クエリ時
- **文字列連結禁止**: SQL文の直接構築禁止

#### 安全な実装例
```java
// Spring Data JPA（安全）
@Query("SELECT p FROM Product p WHERE p.productName LIKE %:keyword%")
List<Product> searchByKeyword(@Param("keyword") String keyword);

// PreparedStatement（安全）
String sql = "SELECT * FROM products WHERE product_name LIKE ?";
PreparedStatement pstmt = connection.prepareStatement(sql);
pstmt.setString(1, "%" + keyword + "%");
```

#### 危険な実装例（禁止）
```java
// 文字列連結（危険）
String sql = "SELECT * FROM products WHERE product_name LIKE '%" + keyword + "%'";
```

---

### 4.3 XSS（クロスサイトスクリプティング）対策

**SEC-070: 出力エスケープ**

#### モバイルアプリ
- iOS: 標準UIコンポーネント使用（自動エスケープ）
- Android: 標準Viewコンポーネント使用（自動エスケープ）

#### 管理Webアプリ（Vue.js）
```vue
<!-- 安全（自動エスケープ） -->
<div>{{ productName }}</div>

<!-- 危険（v-htmlは使用禁止） -->
<div v-html="userInput"></div>  <!-- 禁止 -->
```

---

**SEC-071: Content Security Policy（CSP）**

管理Webアプリに適用:

```http
Content-Security-Policy: 
  default-src 'self';
  script-src 'self';
  style-src 'self' 'unsafe-inline';
  img-src 'self' https:;
  connect-src 'self';
```

---

### 4.4 CSRF（クロスサイトリクエストフォージェリ）対策

**SEC-080: CSRF対策**

#### 対策方式
- **JWT認証**: Authorizationヘッダー経由（Cookie不使用）
- **SameSite Cookie**: 使用しないため不要

#### 管理Webアプリ
```javascript
// API呼び出し時にAuthorizationヘッダーに設定
axios.defaults.headers.common['Authorization'] = `Bearer ${token}`;
```

**注意**: Cookieベース認証の場合はCSRFトークンが必要だが、本システムではJWT（Authorizationヘッダー）を使用するため対策不要

---

## 5. OWASP Top 10 対策

### 5.1 A01: アクセス制御の不備

| 対策 | 実装状況 |
|------|---------|
| JWT認証 | ✅ 実装 |
| ロールベース権限制御 | ✅ 実装 |
| リソース所有権チェック | ✅ 実装 |

---

### 5.2 A02: 暗号化の失敗

| 対策 | 実装状況 |
|------|---------|
| HTTPS/TLS | ✅ 実装 |
| パスワードハッシュ化（bcrypt） | ✅ 実装 |
| トークン暗号化保存（モバイル） | ✅ 実装 |

---

### 5.3 A03: インジェクション

| 対策 | 実装状況 |
|------|---------|
| SQLインジェクション対策（ORM/PreparedStatement） | ✅ 実装 |
| XSS対策（出力エスケープ） | ✅ 実装 |
| 入力バリデーション | ✅ 実装 |

---

### 5.4 A04: 安全でない設計

| 対策 | 実装状況 |
|------|---------|
| 多層防御（クライアント・サーバー両方でバリデーション） | ✅ 実装 |
| 最小権限の原則 | ✅ 実装 |
| セキュアなデフォルト設定 | ✅ 実装 |

---

### 5.5 A05: セキュリティ設定ミス

| 対策 | 実装状況 |
|------|---------|
| デフォルトパスワード変更 | ✅ 必須 |
| 不要なエンドポイント無効化 | ✅ 実装 |
| エラーメッセージでの情報漏洩防止 | ✅ 実装 |
| セキュリティヘッダー設定 | ✅ 実装 |

---

### 5.6 A06: 脆弱性のある古いコンポーネント

| 対策 | 実装状況 |
|------|---------|
| 最新バージョンのライブラリ使用 | ✅ 実装 |
| 定期的なアップデート | 推奨（デモでは不要） |

---

### 5.7 A07: 識別と認証の失敗

| 対策 | 実装状況 |
|------|---------|
| 強固な認証（JWT） | ✅ 実装 |
| パスワードハッシュ化（bcrypt） | ✅ 実装 |
| トークン有効期限 | ✅ 実装 |
| セッション管理 | ✅ 実装 |

---

### 5.8 A08: ソフトウェアとデータの整合性の不備

| 対策 | 実装状況 |
|------|---------|
| 入力バリデーション | ✅ 実装 |
| トランザクション管理 | ✅ 実装 |
| データ整合性チェック（制約） | ✅ 実装 |

---

### 5.9 A09: セキュリティログとモニタリングの失敗

| 対策 | 実装状況 |
|------|---------|
| ログ出力（認証失敗、エラー等） | ✅ 実装 |
| 監視・アラート | ❌ 不要（デモ用途） |

---

### 5.10 A10: サーバサイドリクエストフォージェリ（SSRF）

| 対策 | 実装状況 |
|------|---------|
| 外部URL呼び出しなし | ✅ 該当なし |

---

## 6. セキュリティヘッダー

**SEC-090: HTTPセキュリティヘッダー**

管理WebアプリとAPIに適用:

```http
# XSS対策
X-Content-Type-Options: nosniff
X-Frame-Options: DENY
X-XSS-Protection: 1; mode=block

# CSP
Content-Security-Policy: default-src 'self'

# HTTPS強制（本番環境）
Strict-Transport-Security: max-age=31536000; includeSubDomains
```

#### Spring Boot実装例
```java
@Configuration
public class SecurityConfig {
    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http.headers()
            .contentTypeOptions()
            .and()
            .frameOptions().deny()
            .xssProtection().block(true)
            .and()
            .contentSecurityPolicy("default-src 'self'");
        
        return http.build();
    }
}
```

---

## 7. セキュリティテスト

### 7.1 テストケース

| テストケース | 期待結果 |
|------------|---------|
| トークンなしでAPI呼び出し | 401エラー |
| 不正なトークンでAPI呼び出し | 401エラー |
| 期限切れトークンでAPI呼び出し | 401エラー |
| user権限でadmin APIへアクセス | 403エラー |
| SQLインジェクション試行 | エラーまたは無効化 |
| XSS試行 | エスケープ済み表示 |
| 不正な入力値 | バリデーションエラー |
| 長すぎる入力値 | バリデーションエラー |

詳細は `12-testing-strategy.md` を参照

---

## 8. セキュリティチェックリスト

### 8.1 開発時チェックリスト

- [ ] パスワードをハードコーディングしていない
- [ ] シークレットキーを環境変数で管理
- [ ] .gitignoreに機密情報ファイルを追加
- [ ] SQLクエリはパラメータ化
- [ ] 全API呼び出しにJWT認証を適用
- [ ] ユーザー種別による権限制御を実装
- [ ] 入力バリデーションを実装（サーバー側必須）
- [ ] エラーメッセージでセンシティブ情報を漏らさない
- [ ] HTTPS通信（本番環境）
- [ ] セキュリティヘッダーを設定

---

## 9. デモ用途での簡略化事項

以下のセキュリティ対策は、デモ用途のため簡略化:

| 項目 | 本番推奨 | デモ実装 |
|------|---------|---------|
| パスワード複雑性要件 | 必須 | なし |
| 2要素認証 | 推奨 | なし |
| レート制限 | 必須 | なし |
| IPホワイトリスト | 推奨 | なし |
| DB暗号化（TDE） | 推奨 | なし |
| 監視・アラート | 必須 | なし |
| 脆弱性スキャン | 必須 | なし |
| ペネトレーションテスト | 推奨 | なし |

---

**End of Document**
