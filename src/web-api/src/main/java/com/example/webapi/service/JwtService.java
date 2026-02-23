package com.example.webapi.service;

import com.example.webapi.entity.User;
import io.jsonwebtoken.*;
import io.jsonwebtoken.security.Keys;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.crypto.SecretKey;
import java.nio.charset.StandardCharsets;
import java.util.Date;

/**
 * JWTサービス
 * 
 * JWTトークンの生成・検証を行う
 */
@Service
@Slf4j
public class JwtService {
    
    @Value("${jwt.secret}")
    private String secret;
    
    @Value("${jwt.expiration-hours:24}")
    private int expirationHours;
    
    /**
     * JWTトークンを生成
     * 
     * @param user ユーザー情報
     * @return JWTトークン
     */
    public String generateToken(User user) {
        Date now = new Date();
        Date expiryDate = new Date(now.getTime() + (expirationHours * 60 * 60 * 1000));
        
        SecretKey key = Keys.hmacShaKeyFor(secret.getBytes(StandardCharsets.UTF_8));
        
        return Jwts.builder()
                .setSubject(user.getUserId().toString())
                .claim("loginId", user.getLoginId())
                .claim("userType", user.getUserType().name())
                .setIssuedAt(now)
                .setExpiration(expiryDate)
                .signWith(key, SignatureAlgorithm.HS256)
                .compact();
    }
    
    /**
     * JWTトークンからユーザーIDを取得
     * 
     * @param token JWTトークン
     * @return ユーザーID
     */
    public Long getUserIdFromToken(String token) {
        Claims claims = parseToken(token);
        return Long.parseLong(claims.getSubject());
    }
    
    /**
     * JWTトークンからログインIDを取得
     * 
     * @param token JWTトークン
     * @return ログインID
     */
    public String getLoginIdFromToken(String token) {
        Claims claims = parseToken(token);
        return claims.get("loginId", String.class);
    }
    
    /**
     * JWTトークンからユーザー種別を取得
     * 
     * @param token JWTトークン
     * @return ユーザー種別
     */
    public String getUserTypeFromToken(String token) {
        Claims claims = parseToken(token);
        return claims.get("userType", String.class);
    }
    
    /**
     * JWTトークンの有効性を検証
     * 
     * @param token JWTトークン
     * @return 有効な場合true
     */
    public boolean validateToken(String token) {
        try {
            parseToken(token);
            return true;
        } catch (SecurityException e) {
            log.error("Invalid JWT signature: {}", e.getMessage());
        } catch (MalformedJwtException e) {
            log.error("Invalid JWT token: {}", e.getMessage());
        } catch (ExpiredJwtException e) {
            log.error("Expired JWT token: {}", e.getMessage());
        } catch (UnsupportedJwtException e) {
            log.error("Unsupported JWT token: {}", e.getMessage());
        } catch (IllegalArgumentException e) {
            log.error("JWT claims string is empty: {}", e.getMessage());
        }
        return false;
    }
    
    /**
     * JWTトークンをパースしてClaimsを取得
     * 
     * @param token JWTトークン
     * @return Claims
     * @throws JwtException トークンが無効な場合
     */
    private Claims parseToken(String token) {
        SecretKey key = Keys.hmacShaKeyFor(secret.getBytes(StandardCharsets.UTF_8));
        
        return Jwts.parser()
                .verifyWith(key)
                .build()
                .parseSignedClaims(token)
                .getPayload();
    }
}
