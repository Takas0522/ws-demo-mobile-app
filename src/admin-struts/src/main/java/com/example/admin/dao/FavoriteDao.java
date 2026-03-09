package com.example.admin.dao;

import com.example.admin.entity.Favorite;

import org.apache.commons.dbutils.handlers.BeanListHandler;
import org.springframework.stereotype.Repository;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Repository
public class FavoriteDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("favorite_id", "favoriteId");
        map.put("user_id", "userId");
        map.put("product_id", "productId");
        map.put("created_at", "createdAt");
        map.put("product_name", "productName");
        map.put("unit_price", "unitPrice");
        map.put("description", "description");
        map.put("image_url", "imageUrl");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * ユーザーIDでお気に入り一覧を取得する（商品情報をJOINで取得）。
     */
    public List<Favorite> findByUserId(Long userId) throws SQLException {
        String sql = "SELECT f.*, p.product_name, p.unit_price, p.description, p.image_url"
                + " FROM favorites f JOIN products p ON f.product_id = p.product_id"
                + " WHERE f.user_id = ? ORDER BY f.created_at DESC";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<Favorite> handler = createBeanListHandler(Favorite.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, userId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * お気に入りを登録する。
     */
    public void create(Long userId, Long productId) throws SQLException {
        String sql = "INSERT INTO favorites (user_id, product_id) VALUES (?, ?)";
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql, userId, productId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * ユーザーIDと商品IDでお気に入りを削除する。
     */
    public void deleteByUserIdAndProductId(Long userId, Long productId) throws SQLException {
        String sql = "DELETE FROM favorites WHERE user_id = ? AND product_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql, userId, productId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * ユーザーIDと商品IDでお気に入りが存在するか確認する。
     */
    public boolean existsByUserIdAndProductId(Long userId, Long productId) throws SQLException {
        String sql = "SELECT COUNT(*) FROM favorites WHERE user_id = ? AND product_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            Long count = queryRunner.query(conn, sql, createScalarHandler(), userId, productId);
            return count != null && count > 0;
        } finally {
            closeQuietly(conn);
        }
    }
}
