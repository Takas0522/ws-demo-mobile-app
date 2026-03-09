package com.example.admin.util;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.sql.Connection;
import java.sql.SQLException;

import static org.junit.Assert.*;

public class DatabaseUtilTest {

    private static final String TEST_DB_PATH = "target/test-data/test.db";

    @Before
    public void setUp() {
        File dir = new File("target/test-data");
        if (!dir.exists()) {
            dir.mkdirs();
        }
        DatabaseUtil.initialize(TEST_DB_PATH);
    }

    @After
    public void tearDown() {
        File dbFile = new File(TEST_DB_PATH);
        if (dbFile.exists()) {
            dbFile.delete();
        }
    }

    @Test
    public void testGetConnection() throws SQLException {
        Connection conn = null;
        try {
            conn = DatabaseUtil.getConnection();
            assertNotNull("Connection should not be null", conn);
            assertFalse("Connection should not be closed", conn.isClosed());
        } finally {
            if (conn != null) {
                conn.close();
            }
        }
    }

    @Test
    public void testGetDbPath() {
        assertEquals(TEST_DB_PATH, DatabaseUtil.getDbPath());
    }

    @Test
    public void testInitialize() {
        String newPath = "target/test-data/other.db";
        DatabaseUtil.initialize(newPath);
        assertEquals(newPath, DatabaseUtil.getDbPath());
        // 元のパスに戻す
        DatabaseUtil.initialize(TEST_DB_PATH);
    }

    @Test
    public void testSpringBeanInitialization() {
        String springPath = "target/test-data/spring.db";
        DatabaseUtil util = new DatabaseUtil();
        util.setDbPath(springPath);
        util.init();
        assertEquals(springPath, DatabaseUtil.getDbPath());
        // 元のパスに戻す
        DatabaseUtil.initialize(TEST_DB_PATH);
    }
}
