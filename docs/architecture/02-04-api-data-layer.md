# API層データレイヤー設計

> 最終更新: 2025-01-08  
> ステータス: Draft  
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 関連機能 |
|-----------|------|---------|---------|
| 1.0 | 2025-01-08 | 初版作成（02-component-design.mdから分割） | mobile-app-system |

---

## 1. データレイヤー概要

本ドキュメントでは、mobile-app-system のAPI層におけるデータレイヤー（Entity、Repository）および共通機能の詳細設計を定義します。

以下の内容を含みます：

- **エンティティ設計**: JPA Entityクラス
- **リポジトリ設計**: Spring Data JPA Repository
- **コンポーネント間通信**: プロトコル・データ形式
- **共通ライブラリ**: ユーティリティ、バリデーション
- **パフォーマンス最適化**: キャッシュ、ページネーション
- **テスト戦略**: リポジトリテスト、サービステスト

---

## 2. エンティティ設計

### 2.1 Product.java

```java
@Entity
@Table(name = "products")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Product {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "product_id")
    private Long productId;
    
    @Column(name = "product_name", nullable = false, length = 200)
    private String productName;
    
    @Column(name = "unit_price", nullable = false)
    private Integer unitPrice;
    
    @Column(name = "stock_quantity", nullable = false)
    private Integer stockQuantity;
    
    @Version
    @Column(name = "version")
    private Integer version;  // 楽観的ロック用
}
```

### 2.2 User.java

```java
@Entity
@Table(name = "users")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class User {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "user_id")
    private Long userId;
    
    @Column(name = "login_id", nullable = false, unique = true, length = 50)
    private String loginId;
    
    @Column(name = "password_hash", nullable = false, length = 255)
    private String passwordHash;
    
    @Column(name = "user_type", nullable = false, length = 20)
    @Enumerated(EnumType.STRING)
    private UserType userType;
    
    @Column(name = "created_at", nullable = false)
    private LocalDateTime createdAt;
    
    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
    }
}

public enum UserType {
    CUSTOMER,
    ADMIN
}
```

### 2.3 Favorite.java

```java
@Entity
@Table(name = "favorites")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Favorite {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "favorite_id")
    private Long favoriteId;
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "user_id", nullable = false)
    private User user;
    
    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "product_id", nullable = false)
    private Product product;
    
    @Column(name = "created_at", nullable = false)
    private LocalDateTime createdAt;
    
    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
    }
}
```

### 2.4 Purchase.java

```java
@Entity
@Table(name = "purchases")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Purchase {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "purchase_id")
    private Long purchaseId;
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "user_id", nullable = false)
    private User user;
    
    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "product_id", nullable = false)
    private Product product;
    
    @Column(name = "quantity", nullable = false)
    private Integer quantity;
    
    @Column(name = "unit_price", nullable = false)
    private Integer unitPrice;
    
    @Column(name = "total_amount", nullable = false)
    private Integer totalAmount;
    
    @Column(name = "purchased_at", nullable = false)
    private LocalDateTime purchasedAt;
    
    @PrePersist
    protected void onCreate() {
        purchasedAt = LocalDateTime.now();
        totalAmount = unitPrice * quantity;
    }
}
```

### 2.5 FeatureFlag.java

<!-- Added for mobile-app-system -->
```java
@Entity
@Table(name = "feature_flags")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class FeatureFlag {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "flag_id")
    private Long flagId;
    
    @Column(name = "flag_key", nullable = false, unique = true, length = 100)
    private String flagKey;
    
    @Column(name = "flag_name", nullable = false, length = 200)
    private String flagName;
    
    @Column(name = "description", length = 500)
    private String description;
    
    @Column(name = "created_at", nullable = false)
    private LocalDateTime createdAt;
    
    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
    }
}
```
<!-- End: mobile-app-system -->

### 2.6 UserFeatureFlag.java

<!-- Added for mobile-app-system -->
```java
@Entity
@Table(name = "user_feature_flags",
       uniqueConstraints = @UniqueConstraint(columnNames = {"user_id", "flag_id"}))
@Data
@NoArgsConstructor
@AllArgsConstructor
public class UserFeatureFlag {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "user_feature_flag_id")
    private Long userFeatureFlagId;
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "user_id", nullable = false)
    private User user;
    
    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "flag_id", nullable = false)
    private FeatureFlag featureFlag;
    
    @Column(name = "is_enabled", nullable = false)
    private Boolean isEnabled;
    
    @Column(name = "updated_at", nullable = false)
    private LocalDateTime updatedAt;
    
    @PrePersist
    @PreUpdate
    protected void onUpdate() {
        updatedAt = LocalDateTime.now();
    }
}
```
<!-- End: mobile-app-system -->

---

## 3. リポジトリ設計

### 3.1 ProductRepository.java

```java
@Repository
public interface ProductRepository extends JpaRepository<Product, Long> {
    
    /**
     * 商品名で検索（部分一致）
     */
    List<Product> findByProductNameContaining(String productName);
    
    /**
     * 価格範囲で検索
     */
    List<Product> findByUnitPriceBetween(Integer minPrice, Integer maxPrice);
    
    /**
     * 在庫がある商品のみ取得
     */
    @Query("SELECT p FROM Product p WHERE p.stockQuantity > 0")
    List<Product> findInStockProducts();
    
    /**
     * 商品IDのリストで検索
     */
    List<Product> findByProductIdIn(List<Long> productIds);
}
```

### 3.2 UserRepository.java

```java
@Repository
public interface UserRepository extends JpaRepository<User, Long> {
    
    /**
     * ログインIDでユーザーを検索
     */
    Optional<User> findByLoginId(String loginId);
    
    /**
     * ログインIDが存在するかチェック
     */
    boolean existsByLoginId(String loginId);
    
    /**
     * ユーザー種別で検索
     */
    List<User> findByUserType(UserType userType);
    
    /**
     * 特定期間に作成されたユーザーを取得
     */
    @Query("SELECT u FROM User u WHERE u.createdAt BETWEEN :startDate AND :endDate")
    List<User> findByCreatedAtBetween(
        @Param("startDate") LocalDateTime startDate,
        @Param("endDate") LocalDateTime endDate
    );
}
```

### 3.3 FavoriteRepository.java

```java
@Repository
public interface FavoriteRepository extends JpaRepository<Favorite, Long> {
    
    /**
     * ユーザーのお気に入り一覧を取得
     */
    List<Favorite> findByUser(User user);
    
    /**
     * ユーザーのお気に入り一覧を取得（N+1問題回避）
     */
    @Query("SELECT f FROM Favorite f JOIN FETCH f.product WHERE f.user = :user")
    List<Favorite> findByUserWithProduct(@Param("user") User user);
    
    /**
     * 特定ユーザーの特定商品のお気に入りを取得
     */
    Optional<Favorite> findByUserAndProduct(User user, Product product);
    
    /**
     * ユーザーのお気に入り数を取得
     */
    long countByUser(User user);
    
    /**
     * すでにお気に入り登録済みかチェック
     */
    boolean existsByUserAndProduct(User user, Product product);
    
    /**
     * 特定ユーザーのお気に入りを削除
     */
    @Modifying
    @Query("DELETE FROM Favorite f WHERE f.user = :user AND f.favoriteId = :favoriteId")
    void deleteByUserAndFavoriteId(@Param("user") User user, @Param("favoriteId") Long favoriteId);
}
```

### 3.4 PurchaseRepository.java

```java
@Repository
public interface PurchaseRepository extends JpaRepository<Purchase, Long> {
    
    /**
     * ユーザーの購入履歴を取得（最新順）
     */
    List<Purchase> findByUserOrderByPurchasedAtDesc(User user);
    
    /**
     * ユーザーの購入履歴を取得（ページネーション対応）
     */
    Page<Purchase> findByUser(User user, Pageable pageable);
    
    /**
     * 特定期間の購入履歴を取得
     */
    @Query("SELECT p FROM Purchase p WHERE p.purchasedAt BETWEEN :startDate AND :endDate")
    List<Purchase> findByPurchasedAtBetween(
        @Param("startDate") LocalDateTime startDate,
        @Param("endDate") LocalDateTime endDate
    );
    
    /**
     * 商品別の売上合計を取得
     */
    @Query("SELECT p.product, SUM(p.totalAmount) " +
           "FROM Purchase p " +
           "GROUP BY p.product " +
           "ORDER BY SUM(p.totalAmount) DESC")
    List<Object[]> findTotalSalesByProduct();
}
```

### 3.5 FeatureFlagRepository.java

<!-- Added for mobile-app-system -->
```java
@Repository
public interface FeatureFlagRepository extends JpaRepository<FeatureFlag, Long> {
    
    /**
     * フラグキーで検索
     */
    Optional<FeatureFlag> findByFlagKey(String flagKey);
    
    /**
     * フラグキーが存在するかチェック
     */
    boolean existsByFlagKey(String flagKey);
    
    /**
     * すべての機能フラグを取得（ID昇順）
     */
    List<FeatureFlag> findAllByOrderByFlagIdAsc();
    
    /**
     * すべての機能フラグを取得（作成日時降順）
     */
    List<FeatureFlag> findAllByOrderByCreatedAtDesc();
}
```
<!-- End: mobile-app-system -->

### 3.6 UserFeatureFlagRepository.java

<!-- Added for mobile-app-system -->
```java
@Repository
public interface UserFeatureFlagRepository extends JpaRepository<UserFeatureFlag, Long> {
    
    /**
     * ユーザーの機能フラグ設定を取得
     */
    List<UserFeatureFlag> findByUser(User user);
    
    /**
     * ユーザーの機能フラグ設定を取得（機能フラグ情報も一緒にFETCH）
     */
    @Query("SELECT uf FROM UserFeatureFlag uf JOIN FETCH uf.featureFlag WHERE uf.user = :user")
    List<UserFeatureFlag> findByUserWithFeatureFlag(@Param("user") User user);
    
    /**
     * ユーザーの特定機能フラグを取得
     */
    Optional<UserFeatureFlag> findByUserAndFeatureFlag(User user, FeatureFlag featureFlag);
    
    /**
     * 特定機能フラグが有効なユーザー数を取得
     */
    @Query("SELECT COUNT(uf) FROM UserFeatureFlag uf WHERE uf.featureFlag = :flag AND uf.isEnabled = true")
    long countEnabledUsersByFeatureFlag(@Param("flag") FeatureFlag featureFlag);
    
    /**
     * ユーザーの機能フラグ設定を削除
     */
    @Modifying
    @Query("DELETE FROM UserFeatureFlag uf WHERE uf.user = :user")
    void deleteByUser(@Param("user") User user);
    
    /**
     * ユーザーと機能フラグの組み合わせで削除
     */
    @Modifying
    @Query("DELETE FROM UserFeatureFlag uf WHERE uf.user = :user AND uf.featureFlag = :flag")
    void deleteByUserAndFeatureFlag(@Param("user") User user, @Param("flag") FeatureFlag flag);
}
```
<!-- End: mobile-app-system -->

---

## 4. コンポーネント間通信

### 4.1 通信プロトコル

| 送信元 | 送信先 | プロトコル | 認証 |
|-------|-------|----------|------|
| iOS/Android | Mobile BFF | HTTPS/REST | JWT（ログイン後） |
| Vue.js | Admin BFF | HTTPS/REST | JWT（ログイン後） |
| Mobile BFF | Web API | HTTP/REST | JWT転送 |
| Admin BFF | Web API | HTTP/REST | JWT転送 |
| Web API | PostgreSQL | JDBC | DB認証 |

### 4.2 データ形式

**JSON形式統一**:
- Content-Type: application/json
- Accept: application/json
- 文字コード: UTF-8

### 4.3 エラーレスポンス形式

全コンポーネントで統一されたエラーレスポンス形式を使用:

```json
{
  "error": {
    "code": "ERROR_CODE",
    "message": "エラーメッセージ",
    "details": "詳細情報（オプション）"
  },
  "timestamp": "2025-01-08T12:00:00Z"
}
```

#### エラーコード一覧

| コード | HTTP Status | 説明 |
|-------|------------|------|
| RESOURCE_NOT_FOUND | 404 | リソースが見つからない |
| UNAUTHORIZED | 401 | 認証失敗 |
| FORBIDDEN | 403 | アクセス権限なし |
| VALIDATION_ERROR | 400 | バリデーションエラー |
| INTERNAL_ERROR | 500 | 内部エラー |
| DATABASE_ERROR | 500 | データベースエラー |
| CONFLICT | 409 | データ競合 |

---

## 5. 共通ライブラリ・ユーティリティ

### 5.1 Java共通ライブラリ

| ライブラリ | 用途 | 使用コンポーネント |
|-----------|------|----------------|
| Lombok | ボイラープレートコード削減 | 全Javaコンポーネント |
| MapStruct | DTO/Entityマッピング | Web API |
| Apache Commons | 汎用ユーティリティ | 全Javaコンポーネント |
| Hibernate Validator | バリデーション | Web API |
| Guava | コレクション操作 | Web API（オプション） |

### 5.2 MapStruct マッパー例

#### ProductMapper.java

```java
@Mapper(componentModel = "spring")
public interface ProductMapper {
    
    /**
     * EntityからResponseへの変換
     */
    ProductResponse toResponse(Product product);
    
    /**
     * リスト変換
     */
    List<ProductResponse> toResponseList(List<Product> products);
    
    /**
     * RequestからEntityへの変換（IDは無視）
     */
    @Mapping(target = "productId", ignore = true)
    @Mapping(target = "version", ignore = true)
    Product toEntity(ProductUpdateRequest request);
    
    /**
     * Entityの部分更新
     */
    @Mapping(target = "productId", ignore = true)
    @Mapping(target = "version", ignore = true)
    void updateEntity(@MappingTarget Product product, ProductUpdateRequest request);
}
```

#### FavoriteMapper.java

```java
@Mapper(componentModel = "spring")
public interface FavoriteMapper {
    
    @Mapping(source = "product.productId", target = "productId")
    @Mapping(source = "product.productName", target = "productName")
    @Mapping(source = "product.unitPrice", target = "unitPrice")
    FavoriteResponse toResponse(Favorite favorite);
    
    List<FavoriteResponse> toResponseList(List<Favorite> favorites);
}
```

### 5.3 共通バリデーション

#### ValidationGroups.java

```java
public class ValidationGroups {
    public interface Create {}
    public interface Update {}
    public interface Delete {}
}
```

#### ProductUpdateRequest.java

```java
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ProductUpdateRequest {
    
    @NotNull(groups = Update.class, message = "商品IDは必須です")
    @Min(value = 1, message = "商品IDは1以上である必要があります")
    private Long productId;
    
    @NotBlank(message = "商品名は必須です")
    @Size(max = 200, message = "商品名は200文字以内で入力してください")
    private String productName;
    
    @NotNull(message = "単価は必須です")
    @Min(value = 0, message = "単価は0以上である必要があります")
    @Max(value = 999999999, message = "単価は999999999以下である必要があります")
    private Integer unitPrice;
    
    @NotNull(message = "在庫数は必須です")
    @Min(value = 0, message = "在庫数は0以上である必要があります")
    private Integer stockQuantity;
}
```

#### カスタムバリデーター例

```java
@Target({ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = FutureDateValidator.class)
public @interface FutureDate {
    String message() default "日付は未来日である必要があります";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}

public class FutureDateValidator implements ConstraintValidator<FutureDate, LocalDateTime> {
    
    @Override
    public boolean isValid(LocalDateTime value, ConstraintValidatorContext context) {
        return value == null || value.isAfter(LocalDateTime.now());
    }
}
```

---

## 6. トランザクション管理

### 6.1 トランザクション境界

- **Controller層**: トランザクションなし（@Transactionalを使用しない）
- **Service層**: トランザクション境界（@Transactionalを使用）
- **Repository層**: トランザクション継承（親のトランザクションを利用）

### 6.2 読み取り専用トランザクション

パフォーマンス向上のため、参照系メソッドには `readOnly = true` を設定:

```java
@Service
@RequiredArgsConstructor
@Transactional
public class ProductService {
    
    private final ProductRepository productRepository;
    
    /**
     * 読み取り専用トランザクション
     */
    @Transactional(readOnly = true)
    public List<Product> getAllProducts() {
        return productRepository.findAll();
    }
    
    /**
     * 書き込みトランザクション
     */
    public Product updateProduct(Long id, ProductUpdateRequest request) {
        Product product = productRepository.findById(id)
            .orElseThrow(() -> new ResourceNotFoundException("商品が見つかりません"));
        
        product.setProductName(request.getProductName());
        product.setUnitPrice(request.getUnitPrice());
        
        return productRepository.save(product);
    }
}
```

### 6.3 楽観的ロック

同時更新による競合を防ぐため、`@Version`を使用:

```java
@Entity
@Table(name = "products")
public class Product {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long productId;
    
    @Version  // 楽観的ロック用
    @Column(name = "version")
    private Integer version;
    
    // その他のフィールド
}
```

競合発生時の処理:

```java
@Service
public class ProductService {
    
    public Product updateProduct(Long id, ProductUpdateRequest request) {
        try {
            Product product = productRepository.findById(id)
                .orElseThrow(() -> new ResourceNotFoundException("商品が見つかりません"));
            
            product.setProductName(request.getProductName());
            product.setUnitPrice(request.getUnitPrice());
            
            return productRepository.save(product);
        } catch (OptimisticLockingFailureException e) {
            throw new ConflictException("他のユーザーが更新しています。再度取得してください。");
        }
    }
}
```

### 6.4 トランザクション分離レベル

```java
@Transactional(isolation = Isolation.READ_COMMITTED)
public void processPayment(Long userId, Long productId, Integer quantity) {
    // トランザクション処理
}
```

---

## 7. パフォーマンス最適化

### 7.1 N+1問題の回避

#### JOIN FETCHを使用

```java
@Repository
public interface FavoriteRepository extends JpaRepository<Favorite, Long> {
    
    /**
     * N+1問題を回避するためJOIN FETCHを使用
     */
    @Query("SELECT f FROM Favorite f JOIN FETCH f.product WHERE f.user = :user")
    List<Favorite> findByUserWithProduct(@Param("user") User user);
}
```

#### EntityGraphを使用

```java
@Repository
public interface PurchaseRepository extends JpaRepository<Purchase, Long> {
    
    @EntityGraph(attributePaths = {"product", "user"})
    List<Purchase> findByUser(User user);
}
```

### 7.2 ページネーション

大量データの取得時はページネーションを使用:

```java
@Service
public class PurchaseService {
    
    /**
     * ページネーション対応
     */
    public Page<Purchase> getUserPurchases(Long userId, int page, int size) {
        User user = userRepository.findById(userId)
            .orElseThrow(() -> new ResourceNotFoundException("ユーザーが見つかりません"));
        
        Pageable pageable = PageRequest.of(page, size, Sort.by("purchasedAt").descending());
        return purchaseRepository.findByUser(user, pageable);
    }
}
```

Controller側:

```java
@GetMapping("/purchases")
public ResponseEntity<Page<PurchaseResponse>> getPurchases(
    @AuthenticationPrincipal UserDetails userDetails,
    @RequestParam(defaultValue = "0") int page,
    @RequestParam(defaultValue = "20") int size
) {
    Long userId = Long.parseLong(userDetails.getUsername());
    Page<Purchase> purchases = purchaseService.getUserPurchases(userId, page, size);
    Page<PurchaseResponse> response = purchases.map(purchaseMapper::toResponse);
    return ResponseEntity.ok(response);
}
```

### 7.3 キャッシュ戦略

#### Spring Cache設定

```java
@Configuration
@EnableCaching
public class CacheConfig {
    
    @Bean
    public CacheManager cacheManager() {
        SimpleCacheManager cacheManager = new SimpleCacheManager();
        cacheManager.setCaches(Arrays.asList(
            new ConcurrentMapCache("products"),
            new ConcurrentMapCache("users"),
            new ConcurrentMapCache("featureFlags")
        ));
        return cacheManager;
    }
}
```

#### キャッシュ適用例

```java
@Service
public class ProductService {
    
    /**
     * 商品情報をキャッシュ
     */
    @Cacheable(value = "products", key = "#id")
    public Product getProductById(Long id) {
        return productRepository.findById(id)
            .orElseThrow(() -> new ResourceNotFoundException("商品が見つかりません"));
    }
    
    /**
     * 商品更新時はキャッシュを削除
     */
    @CacheEvict(value = "products", key = "#id")
    public Product updateProduct(Long id, ProductUpdateRequest request) {
        // 更新処理
    }
    
    /**
     * すべてのキャッシュを削除
     */
    @CacheEvict(value = "products", allEntries = true)
    public void deleteAllProducts() {
        // 削除処理
    }
}
```

---

## 8. テスト戦略

### 8.1 リポジトリテスト（@DataJpaTest）

```java
@DataJpaTest
@AutoConfigureTestDatabase(replace = AutoConfigureTestDatabase.Replace.NONE)
class ProductRepositoryTest {
    
    @Autowired
    private ProductRepository productRepository;
    
    @Test
    @DisplayName("在庫がある商品のみ取得できること")
    void testFindInStockProducts() {
        // Given
        Product product1 = new Product(null, "商品A", 1000, 10, null);
        Product product2 = new Product(null, "商品B", 2000, 0, null);
        Product product3 = new Product(null, "商品C", 3000, 5, null);
        productRepository.saveAll(Arrays.asList(product1, product2, product3));
        
        // When
        List<Product> result = productRepository.findInStockProducts();
        
        // Then
        assertEquals(2, result.size());
        assertTrue(result.stream().allMatch(p -> p.getStockQuantity() > 0));
    }
    
    @Test
    @DisplayName("商品名で部分一致検索ができること")
    void testFindByProductNameContaining() {
        // Given
        productRepository.saveAll(Arrays.asList(
            new Product(null, "iPhoneケース", 1000, 10, null),
            new Product(null, "Androidケース", 1200, 5, null),
            new Product(null, "ケーブル", 500, 20, null)
        ));
        
        // When
        List<Product> result = productRepository.findByProductNameContaining("ケース");
        
        // Then
        assertEquals(2, result.size());
        assertTrue(result.stream().allMatch(p -> p.getProductName().contains("ケース")));
    }
}
```

### 8.2 サービステスト（Mockito）

```java
@ExtendWith(MockitoExtension.class)
class ProductServiceTest {
    
    @Mock
    private ProductRepository productRepository;
    
    @Mock
    private ProductMapper productMapper;
    
    @InjectMocks
    private ProductService productService;
    
    @Test
    @DisplayName("商品IDで商品を取得できること")
    void testGetProductById_Success() {
        // Given
        Long productId = 1L;
        Product product = new Product(productId, "商品A", 1000, 10, 1);
        when(productRepository.findById(productId)).thenReturn(Optional.of(product));
        
        // When
        Product result = productService.getProductById(productId);
        
        // Then
        assertNotNull(result);
        assertEquals("商品A", result.getProductName());
        verify(productRepository, times(1)).findById(productId);
    }
    
    @Test
    @DisplayName("存在しない商品IDでResourceNotFoundExceptionがスローされること")
    void testGetProductById_NotFound() {
        // Given
        Long productId = 999L;
        when(productRepository.findById(productId)).thenReturn(Optional.empty());
        
        // When & Then
        assertThrows(ResourceNotFoundException.class, () -> {
            productService.getProductById(productId);
        });
    }
    
    @Test
    @DisplayName("商品を更新できること")
    void testUpdateProduct_Success() {
        // Given
        Long productId = 1L;
        Product existingProduct = new Product(productId, "商品A", 1000, 10, 1);
        ProductUpdateRequest request = new ProductUpdateRequest(
            productId, "商品A（更新）", 1500, 15
        );
        
        when(productRepository.findById(productId)).thenReturn(Optional.of(existingProduct));
        when(productRepository.save(any(Product.class))).thenReturn(existingProduct);
        
        // When
        Product result = productService.updateProduct(productId, request);
        
        // Then
        assertEquals("商品A（更新）", result.getProductName());
        assertEquals(1500, result.getUnitPrice());
        verify(productRepository, times(1)).save(existingProduct);
    }
}
```

### 8.3 統合テスト（@SpringBootTest）

```java
@SpringBootTest
@AutoConfigureMockMvc
@Transactional
class ProductControllerIntegrationTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @Autowired
    private ProductRepository productRepository;
    
    @Autowired
    private ObjectMapper objectMapper;
    
    @BeforeEach
    void setUp() {
        productRepository.deleteAll();
    }
    
    @Test
    @DisplayName("商品一覧を取得できること")
    void testGetAllProducts() throws Exception {
        // Given
        productRepository.saveAll(Arrays.asList(
            new Product(null, "商品A", 1000, 10, null),
            new Product(null, "商品B", 2000, 20, null)
        ));
        
        // When & Then
        mockMvc.perform(get("/api/products")
                .contentType(MediaType.APPLICATION_JSON))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.length()").value(2))
            .andExpect(jsonPath("$[0].productName").value("商品A"))
            .andExpect(jsonPath("$[1].productName").value("商品B"));
    }
    
    @Test
    @DisplayName("商品を更新できること")
    @WithMockUser(roles = "ADMIN")
    void testUpdateProduct() throws Exception {
        // Given
        Product product = productRepository.save(
            new Product(null, "商品A", 1000, 10, null)
        );
        
        ProductUpdateRequest request = new ProductUpdateRequest(
            product.getProductId(), "商品A（更新）", 1500, 15
        );
        
        // When & Then
        mockMvc.perform(put("/api/products/" + product.getProductId())
                .contentType(MediaType.APPLICATION_JSON)
                .content(objectMapper.writeValueAsString(request)))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.productName").value("商品A（更新）"))
            .andExpect(jsonPath("$.unitPrice").value(1500));
    }
}
```

---

## 9. 参照ドキュメント

| ドキュメント | パス |
|------------|------|
| アーキテクチャ概要 | `00-overview.md` |
| クライアント層コンポーネント | `02-01-client-components.md` |
| BFF層コンポーネント | `02-02-bff-components.md` |
| API層コンポーネント | `02-03-api-components.md` |
| データアーキテクチャ | `03-data-architecture.md` |
| APIアーキテクチャ | `04-api-architecture.md` |
| セキュリティアーキテクチャ | `05-security-architecture.md` |
| コーディング規約 | `09-coding-standards.md` |
| 依存関係管理 | `10-dependency-management.md` |
| テストアーキテクチャ | `11-testing-architecture.md` |

---

**End of Document**
