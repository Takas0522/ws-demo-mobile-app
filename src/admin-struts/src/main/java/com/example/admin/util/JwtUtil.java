package com.example.admin.util;

import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import io.jsonwebtoken.SignatureException;
import io.jsonwebtoken.ExpiredJwtException;
import io.jsonwebtoken.MalformedJwtException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Date;

/**
 * JWT トークンの生成・検証ユーティリティ。
 * JJWT 0.6.0 API を使用。Spring Bean として applicationContext.xml に登録する。
 */
public class JwtUtil {
    private static final Logger logger = LoggerFactory.getLogger(JwtUtil.class);

    private String secret;
    private int expirationHours;

    // Spring DI 用 setter
    public void setSecret(String secret) {
        this.secret = secret;
    }

    public void setExpirationHours(int expirationHours) {
        this.expirationHours = expirationHours;
    }

    /**
     * JWT トークンを生成する。
     * Claims: sub=userId(String), loginId, userType, iat, exp
     * Algorithm: HS256
     *
     * @param userId   ユーザーID
     * @param loginId  ログインID
     * @param userType ユーザー種別
     * @return JWT トークン文字列
     */
    public String generateToken(Long userId, String loginId, String userType) {
        Date now = new Date();
        Date expiration = new Date(now.getTime() + (long) expirationHours * 60 * 60 * 1000);

        return Jwts.builder()
                .setSubject(String.valueOf(userId))
                .claim("loginId", loginId)
                .claim("userType", userType)
                .setIssuedAt(now)
                .setExpiration(expiration)
                .signWith(SignatureAlgorithm.HS256, secret)
                .compact();
    }

    /**
     * トークンを検証し、Claims を返す。
     * 無効な場合は null を返す。
     *
     * @param token JWT トークン文字列
     * @return Claims または null
     */
    public Claims parseToken(String token) {
        try {
            return Jwts.parser()
                    .setSigningKey(secret)
                    .parseClaimsJws(token)
                    .getBody();
        } catch (SignatureException e) {
            logger.warn("Invalid JWT signature: {}", e.getMessage());
        } catch (ExpiredJwtException e) {
            logger.warn("Expired JWT token: {}", e.getMessage());
        } catch (MalformedJwtException e) {
            logger.warn("Malformed JWT token: {}", e.getMessage());
        } catch (Exception e) {
            logger.warn("JWT validation error: {}", e.getMessage());
        }
        return null;
    }

    /**
     * トークンからユーザーIDを取得する。
     *
     * @param token JWT トークン文字列
     * @return ユーザーID または null
     */
    public Long getUserIdFromToken(String token) {
        Claims claims = parseToken(token);
        if (claims != null) {
            return Long.parseLong(claims.getSubject());
        }
        return null;
    }

    /**
     * トークンからログインIDを取得する。
     *
     * @param token JWT トークン文字列
     * @return ログインID または null
     */
    public String getLoginIdFromToken(String token) {
        Claims claims = parseToken(token);
        if (claims != null) {
            return (String) claims.get("loginId");
        }
        return null;
    }

    /**
     * トークンからユーザー種別を取得する。
     *
     * @param token JWT トークン文字列
     * @return ユーザー種別 または null
     */
    public String getUserTypeFromToken(String token) {
        Claims claims = parseToken(token);
        if (claims != null) {
            return (String) claims.get("userType");
        }
        return null;
    }

    /**
     * トークンが有効かどうかを検証する。
     *
     * @param token JWT トークン文字列
     * @return 有効な場合 true
     */
    public boolean validateToken(String token) {
        return parseToken(token) != null;
    }
}
