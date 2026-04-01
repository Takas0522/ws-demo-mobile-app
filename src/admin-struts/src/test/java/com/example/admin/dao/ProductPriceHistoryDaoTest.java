package com.example.admin.dao;

import com.example.admin.entity.ProductPriceHistory;
import com.example.admin.util.DatabaseUtil;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;

import static org.junit.Assert.*;

public class ProductPriceHistoryDaoTest {

    private static final String TEST_DB_PATH = "target/test-data/price-history-test.db";
    private ProductPriceHistoryDao dao;

    @Before
    public void setUp() throws SQLException {
        File dir = new File("target/test-data");
        if (!dir.exists()) {
            dir.mkdirs();
        }
        DatabaseUtil.initialize(TEST_DB_PATH);
        dao = new ProductPriceHistoryDao();

        createTables();
        insertTestData();
    }

    @After
    public void tearDown() {
        File dbFile = new File(TEST_DB_PATH);
        if (dbFile.exists()) {
            dbFile.delete();
        }
    }

    private void createTables() throws SQLException {
        Connection conn = null;
        try {
            conn = DatabaseUtil.getConnection();
            Statement stmt = conn.createStatement();

            stmt.executeUpdate("CREATE TABLE IF NOT EXISTS users ("
                    + "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "user_name TEXT NOT NULL,"
                    + "login_id TEXT NOT NULL UNIQUE,"
                    + "password_hash TEXT NOT NULL,"
                    + "user_type TEXT NOT NULL DEFAULT 'user',"
                    + "created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),"
                    + "updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime'))"
                    + ")");

            stmt.executeUpdate("CREATE TABLE IF NOT EXISTS products ("
                    + "product_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "product_name TEXT NOT NULL,"
                    + "unit_price INTEGER NOT NULL,"
                    + "description TEXT,"
                    + "image_url TEXT,"
                    + "created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),"
                    + "updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime'))"
                    + ")");

            stmt.executeUpdate("CREATE TABLE IF NOT EXISTS product_price_history ("
                    + "price_history_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "product_id INTEGER NOT NULL,"
                    + "old_price INTEGER NOT NULL,"
                    + "new_price INTEGER NOT NULL,"
                    + "changed_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),"
                    + "changed_by INTEGER NOT NULL,"
                    + "change_reason TEXT,"
                    + "created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),"
                    + "FOREIGN KEY (product_id) REFERENCES products(product_id),"
                    + "FOREIGN KEY (changed_by) REFERENCES users(user_id)"
                    + ")");

            stmt.close();
        } finally {
            if (conn != null) {
                conn.close();
            }
        }
    }

    private void insertTestData() throws SQLException {
        Connection conn = null;
        try {
            conn = DatabaseUtil.getConnection();
            Statement stmt = conn.createStatement();

            // ユーザー
            stmt.executeUpdate("INSERT INTO users (user_id, user_name, login_id, password_hash, user_type) "
                    + "VALUES (1, '管理者', 'admin', 'hash', 'admin')");

            // 商品
            stmt.executeUpdate("INSERT INTO products (product_id, product_name, unit_price) "
                    + "VALUES (1, 'テスト商品', 1000)");
            stmt.executeUpdate("INSERT INTO products (product_id, product_name, unit_price) "
                    + "VALUES (2, '別の商品', 2000)");

            // 商品1の価格履歴（5件）
            stmt.executeUpdate("INSERT INTO product_price_history (product_id, old_price, new_price, changed_at, changed_by, change_reason) "
                    + "VALUES (1, 800, 1000, '2025-01-15T10:00:00', 1, '値上げ1')");
            stmt.executeUpdate("INSERT INTO product_price_history (product_id, old_price, new_price, changed_at, changed_by, change_reason) "
                    + "VALUES (1, 1000, 1200, '2025-03-20T14:30:00', 1, '値上げ2')");
            stmt.executeUpdate("INSERT INTO product_price_history (product_id, old_price, new_price, changed_at, changed_by, change_reason) "
                    + "VALUES (1, 1200, 1100, '2025-06-10T09:00:00', 1, '値下げ')");
            stmt.executeUpdate("INSERT INTO product_price_history (product_id, old_price, new_price, changed_at, changed_by, change_reason) "
                    + "VALUES (1, 1100, 1300, '2025-09-01T16:45:00', 1, '値上げ3')");
            stmt.executeUpdate("INSERT INTO product_price_history (product_id, old_price, new_price, changed_at, changed_by, change_reason) "
                    + "VALUES (1, 1300, 1500, '2025-12-25T12:00:00', 1, '年末改定')");

            // 商品2の価格履歴（1件）
            stmt.executeUpdate("INSERT INTO product_price_history (product_id, old_price, new_price, changed_at, changed_by, change_reason) "
                    + "VALUES (2, 2000, 2500, '2025-07-01T00:00:00', 1, '商品2値上げ')");

            stmt.close();
        } finally {
            if (conn != null) {
                conn.close();
            }
        }
    }

    @Test
    public void testFindByProductIdWithFilter_正常系_日付範囲内のデータを取得できる() throws SQLException {
        // 2025-03-01 〜 2025-09-30 の範囲で検索（3件該当）
        List<ProductPriceHistory> result = dao.findByProductIdWithFilter(
                1L, "2025-03-01T00:00:00", "2025-09-30T23:59:59", 10, 0);

        assertEquals(3, result.size());
        // 新しい順で返却される
        assertEquals("値上げ3", result.get(0).getChangeReason());
        assertEquals("値下げ", result.get(1).getChangeReason());
        assertEquals("値上げ2", result.get(2).getChangeReason());
    }

    @Test
    public void testFindByProductIdWithFilter_正常系_ページネーションが正しく動作する() throws SQLException {
        // 全5件のうち、2件ずつ取得
        List<ProductPriceHistory> page1 = dao.findByProductIdWithFilter(
                1L, "2020-01-01T00:00:00", "2026-12-31T23:59:59", 2, 0);
        assertEquals(2, page1.size());
        assertEquals("年末改定", page1.get(0).getChangeReason());
        assertEquals("値上げ3", page1.get(1).getChangeReason());

        List<ProductPriceHistory> page2 = dao.findByProductIdWithFilter(
                1L, "2020-01-01T00:00:00", "2026-12-31T23:59:59", 2, 2);
        assertEquals(2, page2.size());
        assertEquals("値下げ", page2.get(0).getChangeReason());
        assertEquals("値上げ2", page2.get(1).getChangeReason());

        List<ProductPriceHistory> page3 = dao.findByProductIdWithFilter(
                1L, "2020-01-01T00:00:00", "2026-12-31T23:59:59", 2, 4);
        assertEquals(1, page3.size());
        assertEquals("値上げ1", page3.get(0).getChangeReason());
    }

    @Test
    public void testFindByProductIdWithFilter_正常系_結果が0件の場合空リストを返す() throws SQLException {
        // 該当しない日付範囲
        List<ProductPriceHistory> result = dao.findByProductIdWithFilter(
                1L, "2030-01-01T00:00:00", "2030-12-31T23:59:59", 10, 0);

        assertNotNull(result);
        assertTrue(result.isEmpty());
    }

    @Test
    public void testFindByProductIdWithFilter_正常系_存在しない商品IDで空リストを返す() throws SQLException {
        List<ProductPriceHistory> result = dao.findByProductIdWithFilter(
                999L, "2020-01-01T00:00:00", "2026-12-31T23:59:59", 10, 0);

        assertNotNull(result);
        assertTrue(result.isEmpty());
    }

    @Test
    public void testFindByProductIdWithFilter_正常系_他商品のデータが混ざらない() throws SQLException {
        List<ProductPriceHistory> result = dao.findByProductIdWithFilter(
                2L, "2020-01-01T00:00:00", "2026-12-31T23:59:59", 10, 0);

        assertEquals(1, result.size());
        assertEquals(Long.valueOf(2L), result.get(0).getProductId());
        assertEquals("商品2値上げ", result.get(0).getChangeReason());
    }

    @Test
    public void testCountByProductIdWithFilter_正常系_件数を正しくカウントできる() throws SQLException {
        int count = dao.countByProductIdWithFilter(
                1L, "2020-01-01T00:00:00", "2026-12-31T23:59:59");

        assertEquals(5, count);
    }

    @Test
    public void testCountByProductIdWithFilter_正常系_日付範囲でフィルタされる() throws SQLException {
        // 2025-03-01 〜 2025-09-30 の範囲（3件）
        int count = dao.countByProductIdWithFilter(
                1L, "2025-03-01T00:00:00", "2025-09-30T23:59:59");

        assertEquals(3, count);
    }

    @Test
    public void testCountByProductIdWithFilter_正常系_該当なしの場合0を返す() throws SQLException {
        int count = dao.countByProductIdWithFilter(
                1L, "2030-01-01T00:00:00", "2030-12-31T23:59:59");

        assertEquals(0, count);
    }

    @Test
    public void testCountByProductIdWithFilter_正常系_存在しない商品IDで0を返す() throws SQLException {
        int count = dao.countByProductIdWithFilter(
                999L, "2020-01-01T00:00:00", "2026-12-31T23:59:59");

        assertEquals(0, count);
    }
}
