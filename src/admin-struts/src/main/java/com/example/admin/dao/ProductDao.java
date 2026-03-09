package com.example.admin.dao;

import com.example.admin.entity.Product;

import org.apache.commons.dbutils.handlers.BeanHandler;
import org.apache.commons.dbutils.handlers.BeanListHandler;
import org.springframework.stereotype.Repository;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Repository
public class ProductDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("product_id", "productId");
        map.put("product_name", "productName");
        map.put("unit_price", "unitPrice");
        map.put("description", "description");
        map.put("image_url", "imageUrl");
        map.put("created_at", "createdAt");
        map.put("updated_at", "updatedAt");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * 全商品を取得する。
     */
    public List<Product> findAll() throws SQLException {
        String sql = "SELECT * FROM products ORDER BY product_id";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<Product> handler = createBeanListHandler(Product.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 商品IDで商品を検索する。
     */
    public Product findById(Long productId) throws SQLException {
        String sql = "SELECT * FROM products WHERE product_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanHandler<Product> handler = createBeanHandler(Product.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, productId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * キーワードで商品を検索する（商品名の部分一致）。
     */
    public List<Product> searchByKeyword(String keyword) throws SQLException {
        String sql = "SELECT * FROM products WHERE product_name LIKE ? ORDER BY product_id";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<Product> handler = createBeanListHandler(Product.class, COLUMN_TO_PROPERTY);
            String param = "%" + keyword + "%";
            return queryRunner.query(conn, sql, handler, param);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 商品を更新する。
     * <p>
     * productsテーブルにupdated_byカラムは存在しないため、UPDATE文から除外している。
     * updated_atはDBトリガーにより自動更新される。
     * </p>
     */
    public void update(Product product) throws SQLException {
        String sql = "UPDATE products SET product_name = ?, unit_price = ?, description = ?, image_url = ? WHERE product_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql,
                    product.getProductName(),
                    product.getUnitPrice(),
                    product.getDescription(),
                    product.getImageUrl(),
                    product.getProductId());
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 全商品数を取得する。
     */
    public int countAll() throws SQLException {
        String sql = "SELECT COUNT(*) FROM products";
        Connection conn = null;
        try {
            conn = getConnection();
            Long count = queryRunner.query(conn, sql, createScalarHandler());
            return count != null ? count.intValue() : 0;
        } finally {
            closeQuietly(conn);
        }
    }
}
