package com.example.admin.util;

import org.mindrot.jbcrypt.BCrypt;

/**
 * パスワードハッシュ・検証ユーティリティ。
 * BCrypt を使用し、Spring Security の BCryptPasswordEncoder(strength=10) と互換性あり。
 */
public class PasswordUtil {

    /**
     * パスワードのハッシュを生成する。
     * @param plainPassword 平文パスワード
     * @return bcrypt ハッシュ
     */
    public static String hashPassword(String plainPassword) {
        return BCrypt.hashpw(plainPassword, BCrypt.gensalt(10));
    }

    /**
     * パスワードを検証する。
     * Spring Security の BCryptPasswordEncoder(strength=10) で生成された
     * ハッシュ ($2a$10$...) との互換性あり。
     *
     * @param plainPassword  平文パスワード
     * @param hashedPassword ハッシュ済みパスワード
     * @return 一致する場合 true
     */
    public static boolean checkPassword(String plainPassword, String hashedPassword) {
        return BCrypt.checkpw(plainPassword, hashedPassword);
    }
}
