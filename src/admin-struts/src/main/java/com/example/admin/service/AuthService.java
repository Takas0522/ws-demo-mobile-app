package com.example.admin.service;

import com.example.admin.dao.UserDao;
import com.example.admin.entity.User;
import com.example.admin.util.PasswordUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.sql.SQLException;

@Service
public class AuthService {

    private static final Logger logger = LoggerFactory.getLogger(AuthService.class);

    private UserDao userDao;

    @Autowired
    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    /**
     * 管理画面用ログイン認証（admin のみ許可）。
     * @param loginId ログインID
     * @param password パスワード
     * @return 認証成功時は User オブジェクト、失敗時は null
     */
    public User authenticate(String loginId, String password) {
        try {
            User user = userDao.findByLoginId(loginId);
            if (user == null) {
                logger.debug("User not found: {}", loginId);
                return null;
            }
            if (!PasswordUtil.checkPassword(password, user.getPasswordHash())) {
                logger.debug("Invalid password for user: {}", loginId);
                return null;
            }
            // 管理画面ログインはadminのみ許可
            if (!"admin".equals(user.getUserType())) {
                logger.debug("Non-admin user attempted admin login: {}", loginId);
                return null;
            }
            return user;
        } catch (SQLException e) {
            logger.error("Database error during authentication for user: {}", loginId, e);
            throw new RuntimeException("認証処理中にデータベースエラーが発生しました", e);
        }
    }

    /**
     * API 用ユーザー認証（userType の制限なし、一般ユーザーも可）。
     * @param loginId ログインID
     * @param password パスワード
     * @return 認証成功時は User、失敗時は null
     * @throws SQLException データベースエラー
     */
    public User authenticateUser(String loginId, String password) throws SQLException {
        if (loginId == null || password == null) {
            return null;
        }
        User user = userDao.findByLoginId(loginId);
        if (user == null) {
            logger.debug("User not found: {}", loginId);
            return null;
        }
        if (!PasswordUtil.checkPassword(password, user.getPasswordHash())) {
            logger.debug("Invalid password for user: {}", loginId);
            return null;
        }
        return user;
    }

    /**
     * API 用管理者認証（admin のみ許可）。
     * @param loginId ログインID
     * @param password パスワード
     * @return 認証成功時は User（admin のみ）、失敗時は null
     * @throws SQLException データベースエラー
     */
    public User authenticateAdmin(String loginId, String password) throws SQLException {
        User user = authenticateUser(loginId, password);
        if (user == null) {
            return null;
        }
        if (!"admin".equals(user.getUserType())) {
            logger.debug("User is not admin: {}", loginId);
            return null;
        }
        return user;
    }
}
