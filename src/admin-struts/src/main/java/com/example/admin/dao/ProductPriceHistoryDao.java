package com.example.admin.dao;

import com.example.admin.entity.ProductPriceHistory;

import org.apache.commons.dbutils.handlers.BeanListHandler;
import org.apache.commons.dbutils.handlers.ScalarHandler;
import org.springframework.stereotype.Repository;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Repository
public class ProductPriceHistoryDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("price_history_id", "priceHistoryId");
        map.put("product_id", "productId");
        map.put("old_price", "oldPrice");
        map.put("new_price", "newPrice");
        map.put("changed_at", "changedAt");
        map.put("changed_by", "changedBy");
        map.put("change_reason", "changeReason");
        map.put("created_at", "createdAt");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * 価格変更履歴を登録する。changed_atはDBのデフォルト値を使用する。
     */
    public void create(ProductPriceHistory history) throws SQLException {
        String sql = "INSERT INTO product_price_history (product_id, old_price, new_price, changed_by, change_reason) VALUES (?, ?, ?, ?, ?)";
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql,
                    history.getProductId(),
                    history.getOldPrice(),
                    history.getNewPrice(),
                    history.getChangedBy(),
                    history.getChangeReason());
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 商品IDで価格変更履歴を取得する（新しい順）。
     */
    public List<ProductPriceHistory> findByProductId(Long productId) throws SQLException {
        String sql = "SELECT * FROM product_price_history WHERE product_id = ? ORDER BY changed_at DESC";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<ProductPriceHistory> handler = createBeanListHandler(ProductPriceHistory.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, productId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 日付フィルタ + ページネーション対応の価格履歴検索（新しい順）。
     *
     * @param productId 商品ID
     * @param startDate 検索開始日時（ISO 8601 形式）
     * @param endDate   検索終了日時（ISO 8601 形式）
     * @param limit     1ページあたりの件数
     * @param offset    取得開始位置
     * @return 価格変更履歴リスト
     */
    public List<ProductPriceHistory> findByProductIdWithFilter(
            Long productId, String startDate, String endDate, int limit, int offset) throws SQLException {
        String sql = "SELECT * FROM product_price_history"
                + " WHERE product_id = ? AND changed_at >= ? AND changed_at <= ?"
                + " ORDER BY changed_at DESC LIMIT ? OFFSET ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<ProductPriceHistory> handler =
                    createBeanListHandler(ProductPriceHistory.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, productId, startDate, endDate, limit, offset);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 日付フィルタ付きの総件数カウント。
     *
     * @param productId 商品ID
     * @param startDate 検索開始日時（ISO 8601 形式）
     * @param endDate   検索終了日時（ISO 8601 形式）
     * @return 該当件数
     */
    public int countByProductIdWithFilter(
            Long productId, String startDate, String endDate) throws SQLException {
        String sql = "SELECT COUNT(*) FROM product_price_history"
                + " WHERE product_id = ? AND changed_at >= ? AND changed_at <= ?";
        Connection conn = null;
        try {
            conn = getConnection();
            Number count = queryRunner.query(conn, sql, new ScalarHandler<Number>(), productId, startDate, endDate);
            return count != null ? count.intValue() : 0;
        } finally {
            closeQuietly(conn);
        }
    }
}
