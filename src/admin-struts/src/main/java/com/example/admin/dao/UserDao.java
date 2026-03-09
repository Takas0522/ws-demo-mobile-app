package com.example.admin.dao;

import com.example.admin.entity.User;

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
public class UserDao extends BaseDao {

    private static final Map<String, String> COLUMN_TO_PROPERTY;

    static {
        Map<String, String> map = new HashMap<String, String>();
        map.put("user_id", "userId");
        map.put("user_name", "userName");
        map.put("login_id", "loginId");
        map.put("password_hash", "passwordHash");
        map.put("user_type", "userType");
        map.put("created_at", "createdAt");
        map.put("updated_at", "updatedAt");
        COLUMN_TO_PROPERTY = Collections.unmodifiableMap(map);
    }

    /**
     * ログインIDでユーザーを検索する。
     */
    public User findByLoginId(String loginId) throws SQLException {
        String sql = "SELECT * FROM users WHERE login_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanHandler<User> handler = createBeanHandler(User.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, loginId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * ユーザーIDでユーザーを検索する。
     */
    public User findById(Long userId) throws SQLException {
        String sql = "SELECT * FROM users WHERE user_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanHandler<User> handler = createBeanHandler(User.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler, userId);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 全ユーザーを取得する。
     */
    public List<User> findAll() throws SQLException {
        String sql = "SELECT * FROM users ORDER BY user_id";
        Connection conn = null;
        try {
            conn = getConnection();
            BeanListHandler<User> handler = createBeanListHandler(User.class, COLUMN_TO_PROPERTY);
            return queryRunner.query(conn, sql, handler);
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * 全ユーザー数を取得する。
     */
    public int countAll() throws SQLException {
        String sql = "SELECT COUNT(*) FROM users";
        Connection conn = null;
        try {
            conn = getConnection();
            Long count = queryRunner.query(conn, sql, createScalarHandler());
            return count != null ? count.intValue() : 0;
        } finally {
            closeQuietly(conn);
        }
    }

    /**
     * パスワードハッシュを更新する。
     *
     * @param userId       ユーザーID
     * @param passwordHash 新しいパスワードハッシュ
     */
    public void updatePassword(Long userId, String passwordHash) throws SQLException {
        String sql = "UPDATE users SET password_hash = ? WHERE user_id = ?";
        Connection conn = null;
        try {
            conn = getConnection();
            queryRunner.update(conn, sql, passwordHash, userId);
        } finally {
            closeQuietly(conn);
        }
    }
}
