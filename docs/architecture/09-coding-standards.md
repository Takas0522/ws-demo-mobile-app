# コーディング規約

> 最終更新: 2025-01-08  
> ステータス: Draft  
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 関連機能 |
|-----------|------|---------|---------|
| 1.0 | 2025-01-08 | 初版作成 | mobile-app-system |

---

## 1. コーディング規約概要

本ドキュメントでは、mobile-app-system の言語別コーディング規約を定義します。
統一されたコーディングスタイルにより、可読性と保守性を向上させます。

## 2. 共通原則

### 2.1 基本原則

| 原則 | 説明 |
|-----|------|
| **可読性優先** | コードは書く時間より読む時間の方が長い |
| **KISS原則** | Keep It Simple, Stupid - シンプルに保つ |
| **DRY原則** | Don't Repeat Yourself - 重複を避ける |
| **一貫性** | プロジェクト全体で統一されたスタイル |
| **命名の明確性** | 名前から役割が理解できる |

### 2.2 命名規則の方針

- **自己説明的**: 変数名・関数名から役割が明確
- **省略禁止**: `usr` → `user`、`pd` → `product`
- **一般的な略語は可**: `id`, `url`, `jwt`, `api`
- **ビジネス用語使用**: ドメイン用語を正確に使う

## 3. Java コーディング規約

### 3.1 準拠規約

**Google Java Style Guide** に準拠

参考: https://google.github.io/styleguide/javaguide.html

### 3.2 命名規則

| 対象 | ケース | 例 |
|-----|-------|-----|
| **クラス** | PascalCase | `UserService`, `ProductRepository` |
| **インターフェース** | PascalCase | `UserRepository`, `AuthService` |
| **メソッド** | camelCase | `getUserById()`, `createProduct()` |
| **変数** | camelCase | `userId`, `productName` |
| **定数** | UPPER_SNAKE_CASE | `MAX_RETRY_COUNT`, `API_TIMEOUT` |
| **パッケージ** | lowercase | `com.example.webapi.service` |

### 3.3 コード例

```java
package com.example.webapi.service;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

/**
 * 商品サービス
 * 商品のCRUD操作を提供します
 */
@Slf4j
@Service
@RequiredArgsConstructor
@Transactional
public class ProductService {
    
    private final ProductRepository productRepository;
    private static final int MAX_PRODUCT_NAME_LENGTH = 100;
    
    /**
     * 全商品を取得
     * 
     * @return 商品リスト
     */
    @Transactional(readOnly = true)
    public List<Product> getAllProducts() {
        log.debug("全商品取得開始");
        List<Product> products = productRepository.findAll();
        log.info("全商品取得完了: 件数={}", products.size());
        return products;
    }
    
    /**
     * 商品をIDで取得
     * 
     * @param id 商品ID
     * @return 商品
     * @throws ResourceNotFoundException 商品が見つからない場合
     */
    @Transactional(readOnly = true)
    public Product getProductById(Long id) {
        return productRepository.findById(id)
            .orElseThrow(() -> new ResourceNotFoundException("商品が見つかりません: id=" + id));
    }
}
```

### 3.4 インデント・フォーマット

- **インデント**: スペース4つ
- **行の最大長**: 100文字
- **中括弧**: K&Rスタイル（同じ行に開始括弧）

### 3.5 コメント

```java
/**
 * Javadocコメント（public メソッドに必須）
 * 
 * @param userId ユーザーID
 * @return ユーザー情報
 * @throws UserNotFoundException ユーザーが見つからない場合
 */
public User getUserById(Long userId) {
    // 単一行コメント（実装の説明）
    
    /* 
     * 複数行コメント
     * 複雑なロジックの説明
     */
}
```

## 4. Swift コーディング規約

### 4.1 準拠規約

**Swift API Design Guidelines** に準拠

参考: https://swift.org/documentation/api-design-guidelines/

### 4.2 命名規則

| 対象 | ケース | 例 |
|-----|-------|-----|
| **クラス/構造体** | PascalCase | `ProductViewModel`, `LoginView` |
| **プロトコル** | PascalCase | `AuthServiceProtocol` |
| **メソッド** | camelCase | `fetchProducts()`, `login(with:)` |
| **変数/プロパティ** | camelCase | `userId`, `productName` |
| **定数** | camelCase | `maxRetryCount`, `apiTimeout` |
| **列挙型** | PascalCase | `UserType`, `ApiError` |
| **列挙ケース** | camelCase | `.user`, `.admin` |

### 4.3 コード例

```swift
import Foundation

/// 商品ビューモデル
/// 商品一覧画面のビジネスロジックを担当
class ProductListViewModel: ObservableObject {
    
    // MARK: - Properties
    
    @Published var products: [Product] = []
    @Published var isLoading: Bool = false
    @Published var errorMessage: String?
    
    private let apiClient: APIClient
    private let maxRetryCount = 3
    
    // MARK: - Initialization
    
    init(apiClient: APIClient = .shared) {
        self.apiClient = apiClient
    }
    
    // MARK: - Public Methods
    
    /// 商品一覧を取得
    func fetchProducts() async {
        isLoading = true
        defer { isLoading = false }
        
        do {
            let response: ProductListResponse = try await apiClient.request(
                "/products",
                method: .get
            )
            self.products = response.data.products
        } catch {
            self.errorMessage = error.localizedDescription
            Logger.error("商品取得エラー: \(error)", category: "network")
        }
    }
    
    // MARK: - Private Methods
    
    private func handleError(_ error: Error) {
        // エラー処理
    }
}
```

### 4.4 インデント・フォーマット

- **インデント**: スペース4つ
- **行の最大長**: 120文字
- **MARK**: セクション分割に使用

### 4.5 オプショナル処理

```swift
// 推奨: guard let
guard let user = userRepository.getUser(id: userId) else {
    return
}

// 推奨: if let
if let token = keychainManager.getToken() {
    // トークンを使用
}

// 非推奨: force unwrap
let user = userRepository.getUser(id: userId)!  // ❌
```

## 5. JavaScript/Vue.js コーディング規約

### 5.1 準拠規約

**ESLint Standard Style** に準拠

### 5.2 命名規則

| 対象 | ケース | 例 |
|-----|-------|-----|
| **コンポーネント** | PascalCase | `ProductList.vue`, `LoginForm.vue` |
| **変数/関数** | camelCase | `userId`, `fetchProducts()` |
| **定数** | UPPER_SNAKE_CASE | `API_BASE_URL`, `MAX_RETRY` |
| **ファイル名** | kebab-case | `product-list.js`, `api-client.js` |

### 5.3 コード例（Vue 3 Composition API）

```vue
<template>
  <div class="product-list">
    <h1>商品一覧</h1>
    
    <div v-if="isLoading" class="loading">
      読み込み中...
    </div>
    
    <div v-else class="product-grid">
      <ProductCard
        v-for="product in products"
        :key="product.productId"
        :product="product"
        @click="onProductClick(product)"
      />
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useProductStore } from '@/stores/product'
import ProductCard from '@/components/product/ProductCard.vue'
import logger from '@/utils/logger'

// Store
const productStore = useProductStore()

// State
const products = ref([])
const isLoading = ref(false)

// Lifecycle
onMounted(async () => {
  await fetchProducts()
})

// Methods
/**
 * 商品一覧を取得
 */
async function fetchProducts() {
  isLoading.value = true
  
  try {
    products.value = await productStore.fetchProducts()
    logger.info('商品取得成功', { count: products.value.length })
  } catch (error) {
    logger.error('商品取得エラー', error)
  } finally {
    isLoading.value = false
  }
}

/**
 * 商品クリック時の処理
 * @param {Object} product - 商品オブジェクト
 */
function onProductClick(product) {
  // 処理
}
</script>

<style scoped>
.product-list {
  padding: 20px;
}

.product-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
  gap: 20px;
}
</style>
```

### 5.4 インデント・フォーマット

- **インデント**: スペース2つ
- **セミコロン**: 不要
- **クォート**: シングルクォート `'`
- **行の最大長**: 100文字

### 5.5 ESLint設定

```javascript
// .eslintrc.js
module.exports = {
  extends: [
    'plugin:vue/vue3-recommended',
    '@vue/eslint-config-standard'
  ],
  rules: {
    'vue/multi-word-component-names': 'off',
    'no-console': process.env.NODE_ENV === 'production' ? 'warn' : 'off',
    'no-debugger': process.env.NODE_ENV === 'production' ? 'warn' : 'off'
  }
}
```

## 6. SQL コーディング規約

### 6.1 命名規則

| 対象 | ケース | 例 |
|-----|-------|-----|
| **テーブル** | snake_case（複数形） | `users`, `products`, `purchases` |
| **カラム** | snake_case | `user_id`, `product_name`, `created_at` |
| **インデックス** | snake_case | `idx_users_login_id` |

### 6.2 コード例

```sql
-- 商品検索クエリ
SELECT 
    p.product_id,
    p.product_name,
    p.unit_price,
    p.description
FROM 
    products p
WHERE 
    p.product_name LIKE '%商品%'
    AND p.unit_price BETWEEN 1000 AND 5000
ORDER BY 
    p.created_at DESC
LIMIT 20;

-- 購入履歴取得（JOINあり）
SELECT 
    pu.purchase_id,
    pu.quantity,
    pu.total_amount,
    p.product_name,
    u.user_name
FROM 
    purchases pu
    INNER JOIN products p ON pu.product_id = p.product_id
    INNER JOIN users u ON pu.user_id = u.user_id
WHERE 
    pu.user_id = $1
ORDER BY 
    pu.purchased_at DESC;
```

### 6.3 フォーマット

- **予約語**: 大文字
- **テーブル名・カラム名**: 小文字
- **インデント**: スペース4つ
- **1行1カラム**: SELECTリストは改行

## 7. コードレビュー基準

### 7.1 必須チェック項目

- [ ] 命名規則に従っているか
- [ ] コメントが適切に記載されているか
- [ ] エラーハンドリングが実装されているか
- [ ] ログ出力が適切か
- [ ] セキュリティ上の問題はないか
- [ ] パフォーマンス上の問題はないか
- [ ] テストパターンが定義されているか

### 7.2 推奨チェック項目

- [ ] 関数が短く単純か（1関数1責務）
- [ ] ネストが深すぎないか（3段階以内推奨）
- [ ] 重複コードがないか
- [ ] マジックナンバーがないか（定数化）

## 8. 静的解析ツール

### 8.1 Java

```xml
<!-- pom.xml -->
<plugin>
    <groupId>org.apache.maven.plugins</groupId>
    <artifactId>maven-checkstyle-plugin</artifactId>
    <version>3.3.0</version>
    <configuration>
        <configLocation>google_checks.xml</configLocation>
    </configuration>
</plugin>
```

### 8.2 Swift

```yaml
# .swiftlint.yml
disabled_rules:
  - trailing_whitespace
opt_in_rules:
  - empty_count
  - closure_spacing

line_length:
  warning: 120
  error: 150
```

### 8.3 JavaScript

```bash
# ESLint実行
npm run lint

# 自動修正
npm run lint:fix
```

## 9. 参照ドキュメント

| ドキュメント | URL |
|------------|-----|
| Google Java Style Guide | https://google.github.io/styleguide/javaguide.html |
| Swift API Design Guidelines | https://swift.org/documentation/api-design-guidelines/ |
| ESLint | https://eslint.org/ |
| Vue.js Style Guide | https://vuejs.org/style-guide/ |

---

**End of Document**
