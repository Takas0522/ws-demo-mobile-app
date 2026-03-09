package com.example.admin.service;

import com.example.admin.dao.ProductDao;
import com.example.admin.dao.PurchaseDao;
import com.example.admin.entity.Product;
import com.example.admin.entity.Purchase;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.sql.SQLException;
import java.util.List;

/**
 * 購入サービス。
 * 購入の作成・履歴取得を提供する。
 */
@Service
public class PurchaseService {

    private static final Logger logger = LoggerFactory.getLogger(PurchaseService.class);

    private PurchaseDao purchaseDao;
    private ProductDao productDao;

    @Autowired
    public void setPurchaseDao(PurchaseDao purchaseDao) {
        this.purchaseDao = purchaseDao;
    }

    @Autowired
    public void setProductDao(ProductDao productDao) {
        this.productDao = productDao;
    }

    /**
     * 購入を作成する。
     *
     * @param userId    ユーザーID
     * @param productId 商品ID
     * @param quantity  数量（100の倍数であること）
     * @return 作成された購入
     * @throws SQLException             データベースエラー
     * @throws IllegalArgumentException バリデーションエラー
     */
    public Purchase createPurchase(Long userId, Long productId, int quantity) throws SQLException {
        // バリデーション: 数量は100の倍数
        if (quantity <= 0 || quantity % 100 != 0) {
            throw new IllegalArgumentException("Quantity must be a positive multiple of 100");
        }

        Product product = productDao.findById(productId);
        if (product == null) {
            throw new IllegalArgumentException("Product not found: " + productId);
        }

        Purchase purchase = new Purchase();
        purchase.setUserId(userId);
        purchase.setProductId(productId);
        purchase.setQuantity(quantity);
        purchase.setUnitPriceAtPurchase(product.getUnitPrice());
        long total = (long) product.getUnitPrice() * quantity;
        if (total > Integer.MAX_VALUE) {
            throw new IllegalArgumentException("Total amount exceeds maximum value");
        }
        purchase.setTotalAmount((int) total);
        purchaseDao.create(purchase);

        // JOINデータも設定
        purchase.setProductName(product.getProductName());

        return purchase;
    }

    /**
     * ユーザーの購入履歴を取得する。
     */
    public List<Purchase> getPurchasesByUserId(Long userId) throws SQLException {
        return purchaseDao.findByUserId(userId);
    }
}
