package com.example.admin.service;

import com.example.admin.dao.ProductDao;
import com.example.admin.dao.ProductPriceHistoryDao;
import com.example.admin.entity.Product;
import com.example.admin.entity.ProductPriceHistory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.sql.SQLException;
import java.util.List;

@Service
public class ProductService {

    private static final Logger logger = LoggerFactory.getLogger(ProductService.class);

    private ProductDao productDao;
    private ProductPriceHistoryDao priceHistoryDao;

    @Autowired
    public void setProductDao(ProductDao productDao) {
        this.productDao = productDao;
    }

    @Autowired
    public void setProductPriceHistoryDao(ProductPriceHistoryDao priceHistoryDao) {
        this.priceHistoryDao = priceHistoryDao;
    }

    /**
     * 全商品を取得する。
     * @return 商品リスト
     */
    public List<Product> getProducts() {
        try {
            return productDao.findAll();
        } catch (SQLException e) {
            logger.error("Failed to get products", e);
            throw new RuntimeException("商品一覧の取得に失敗しました", e);
        }
    }

    /**
     * キーワードで商品を検索する。
     * @param keyword 検索キーワード
     * @return 検索結果の商品リスト
     */
    public List<Product> searchProducts(String keyword) {
        try {
            return productDao.searchByKeyword(keyword);
        } catch (SQLException e) {
            logger.error("Failed to search products with keyword: {}", keyword, e);
            throw new RuntimeException("商品検索に失敗しました", e);
        }
    }

    /**
     * 商品IDで商品を取得する。
     * @param productId 商品ID
     * @return 商品、見つからない場合は null
     */
    public Product getProductById(Long productId) {
        try {
            return productDao.findById(productId);
        } catch (SQLException e) {
            logger.error("Failed to get product by id: {}", productId, e);
            throw new RuntimeException("商品の取得に失敗しました", e);
        }
    }

    /**
     * 商品を更新する。
     * 価格変更時は DB トリガー (trg_record_price_history) が自動的に履歴を記録する。
     * ただし products テーブルに updated_by カラムが存在しないため、
     * トリガーの changed_by は NULL になる可能性がある点に注意。
     * @param product 更新する商品情報
     */
    public void updateProduct(Product product) {
        try {
            productDao.update(product);
        } catch (SQLException e) {
            logger.error("Failed to update product: {}", product.getProductId(), e);
            throw new RuntimeException("商品の更新に失敗しました", e);
        }
    }

    /**
     * API 用商品更新。個別パラメータで更新し、更新後の商品を返す。
     *
     * @param productId   商品ID
     * @param productName 商品名（null の場合は更新しない）
     * @param unitPrice   単価（null の場合は更新しない）
     * @param description 説明（null の場合は更新しない）
     * @param imageUrl    画像URL（null の場合は更新しない）
     * @return 更新後の商品
     * @throws IllegalArgumentException 商品が見つからない場合
     */
    public Product updateProductForApi(Long productId, String productName, Integer unitPrice,
                                       String description, String imageUrl) {
        Product product = getProductById(productId);
        if (product == null) {
            throw new IllegalArgumentException("Product not found: " + productId);
        }

        if (productName != null) {
            product.setProductName(productName);
        }
        if (unitPrice != null) {
            product.setUnitPrice(unitPrice);
        }
        if (description != null) {
            product.setDescription(description);
        }
        if (imageUrl != null) {
            product.setImageUrl(imageUrl);
        }

        updateProduct(product);

        // 更新後の商品を返す
        return getProductById(productId);
    }

    /**
     * 商品の価格変更履歴を取得する。
     * @param productId 商品ID
     * @return 価格変更履歴リスト
     */
    public List<ProductPriceHistory> getPriceHistory(Long productId) {
        try {
            return priceHistoryDao.findByProductId(productId);
        } catch (SQLException e) {
            logger.error("Failed to get price history for product: {}", productId, e);
            throw new RuntimeException("価格変更履歴の取得に失敗しました", e);
        }
    }
}
