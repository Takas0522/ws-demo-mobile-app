package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * ログインレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class LoginResponse {
    
    /**
     * JWTトークン
     */
    private String token;
    
    /**
     * トークンタイプ（常に"Bearer"）
     */
    private String tokenType;
    
    /**
     * 有効期限（秒）
     */
    private Long expiresIn;
    
    /**
     * ユーザー情報
     */
    private UserInfo user;
    
    /**
     * ユーザー情報
     */
    @Data
    @NoArgsConstructor
    @AllArgsConstructor
    @Builder
    public static class UserInfo {
        private Long userId;
        private String userName;
        private String loginId;
        private String userType;
    }
}
