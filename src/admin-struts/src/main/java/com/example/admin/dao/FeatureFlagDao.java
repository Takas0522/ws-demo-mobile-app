package com.example.admin.dao;

import com.example.admin.entity.FeatureFlag;

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
public class FeatureFlagDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("flag_id", "flagId");
        map.put("flag_key", "flagKey");
        map.put("flag_name", "flagName");
        map.put("default_value", "defaultValue");
        map.put("created_at", "createdAt");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * 全フィーチャーフラグを取得する。
     */
    public List<FeatureFlag> findAll() throws SQLException {
        String sql = "SELECT * FROM feature_flags ORDER BY flag_id";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<FeatureFlag> handler = createBeanListHandler(FeatureFlag.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * フラグキーでフィーチャーフラグを検索する。
     */
    public FeatureFlag findByFlagKey(String flagKey) throws SQLException {
        String sql = "SELECT * FROM feature_flags WHERE flag_key = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanHandler<FeatureFlag> handler = createBeanHandler(FeatureFlag.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, flagKey);
        } finally {
            closeQuietly(conn);
        }
    }
}
