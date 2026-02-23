package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * ログインレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class LoginResponse {
    
    /**
     * アクセストークン
     */
    private String accessToken;
    
    /**
     * リフレッシュトークン
     */
    private String refreshToken;
    
    /**
     * ユーザーID
     */
    private Long userId;
    
    /**
     * ログインID
     */
    private String loginId;
    
    /**
     * ユーザー名
     */
    private String userName;
}
