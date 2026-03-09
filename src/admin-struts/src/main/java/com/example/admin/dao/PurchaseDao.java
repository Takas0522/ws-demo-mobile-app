package com.example.admin.dao;

import com.example.admin.entity.Purchase;

import org.apache.commons.dbutils.handlers.BeanListHandler;
import org.springframework.stereotype.Repository;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

@Repository
public class PurchaseDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("purchase_id", "purchaseId");
        map.put("user_id", "userId");
        map.put("product_id", "productId");
        map.put("quantity", "quantity");
        map.put("unit_price_at_purchase", "unitPriceAtPurchase");
        map.put("total_amount", "totalAmount");
        map.put("purchased_at", "purchasedAt");
        map.put("product_name", "productName");
        map.put("user_name", "userName");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * 購入を登録する。purchase_idはUUIDで自動生成する。
     */
    public void create(Purchase purchase) throws SQLException {
        String sql = "INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES (?, ?, ?, ?, ?, ?)";
        String purchaseId = UUID.randomUUID().toString();
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql,
                    purchaseId,
                    purchase.getUserId(),
                    purchase.getProductId(),
                    purchase.getQuantity(),
                    purchase.getUnitPriceAtPurchase(),
                    purchase.getTotalAmount());
            purchase.setPurchaseId(purchaseId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * ユーザーIDで購入履歴を取得する（商品名をJOINで取得）。
     */
    public List<Purchase> findByUserId(Long userId) throws SQLException {
        String sql = "SELECT p.*, pr.product_name FROM purchases p"
                + " JOIN products pr ON p.product_id = pr.product_id"
                + " WHERE p.user_id = ? ORDER BY p.purchased_at DESC";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<Purchase> handler = createBeanListHandler(Purchase.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, userId);
        } finally {
            closeQuietly(conn);
        }
    }
}
