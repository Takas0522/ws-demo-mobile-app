package com.example.admin.util;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class DatabaseUtil {

    private static final Logger logger = LoggerFactory.getLogger(DatabaseUtil.class);
    private static String dbPath;

    /** Spring Bean 用の dbPath setter */
    private String dbPathProperty;

    public void setDbPath(String dbPath) {
        this.dbPathProperty = dbPath;
    }

    /** Spring init-method から呼ばれる初期化メソッド */
    public void init() {
        initialize(this.dbPathProperty);
    }

    public static void initialize(String path) {
        dbPath = path;
        try {
            Class.forName("org.sqlite.JDBC");
        } catch (ClassNotFoundException e) {
            throw new RuntimeException("SQLite JDBC driver not found", e);
        }
        logger.info("DatabaseUtil initialized with path: {}", dbPath);
    }

    public static Connection getConnection() throws SQLException {
        if (dbPath == null) {
            throw new IllegalStateException("DatabaseUtil is not initialized. Call initialize() first.");
        }
        return DriverManager.getConnection("jdbc:sqlite:" + dbPath);
    }

    public static String getDbPath() {
        return dbPath;
    }
}
