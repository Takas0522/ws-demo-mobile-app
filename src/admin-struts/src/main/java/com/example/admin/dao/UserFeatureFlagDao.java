package com.example.admin.dao;

import com.example.admin.entity.UserFeatureFlag;

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
public class UserFeatureFlagDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("user_flag_id", "userFlagId");
        map.put("user_id", "userId");
        map.put("flag_id", "flagId");
        map.put("is_enabled", "isEnabled");
        map.put("created_at", "createdAt");
        map.put("updated_at", "updatedAt");
        map.put("flag_key", "flagKey");
        map.put("flag_name", "flagName");
        map.put("default_value", "defaultValue");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * ユーザーIDでユーザーフィーチャーフラグ一覧を取得する（フラグ情報をJOINで取得）。
     */
    public List<UserFeatureFlag> findByUserId(Long userId) throws SQLException {
        String sql = "SELECT uf.*, ff.flag_key, ff.flag_name, ff.default_value"
                + " FROM user_feature_flags uf"
                + " JOIN feature_flags ff ON uf.flag_id = ff.flag_id"
                + " WHERE uf.user_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<UserFeatureFlag> handler = createBeanListHandler(UserFeatureFlag.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, userId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * ユーザーIDとフラグIDでユーザーフィーチャーフラグを検索する。
     */
    public UserFeatureFlag findByUserIdAndFlagId(Long userId, Long flagId) throws SQLException {
        String sql = "SELECT uf.*, ff.flag_key, ff.flag_name, ff.default_value"
                + " FROM user_feature_flags uf"
                + " JOIN feature_flags ff ON uf.flag_id = ff.flag_id"
                + " WHERE uf.user_id = ? AND uf.flag_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanHandler<UserFeatureFlag> handler = createBeanHandler(UserFeatureFlag.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, userId, flagId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * ユーザーフィーチャーフラグをUPSERTする（既存レコードは更新、なければ挿入）。
     */
    public void upsert(Long userId, Long flagId, boolean isEnabled) throws SQLException {
        String sql = "INSERT OR REPLACE INTO user_feature_flags (user_id, flag_id, is_enabled) VALUES (?, ?, ?)";
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql, userId, flagId, isEnabled ? 1 : 0);
        } finally {
            closeQuietly(conn);
        }
    }
}
