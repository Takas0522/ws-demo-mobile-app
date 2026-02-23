package com.example.adminbff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * ログインリクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class LoginRequest {
    
    /**
     * ログインID
     */
    private String loginId;
    
    /**
     * パスワード
     */
    private String password;
}
