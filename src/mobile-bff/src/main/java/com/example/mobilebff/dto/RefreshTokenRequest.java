package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * リフレッシュトークンリクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class RefreshTokenRequest {
    
    /**
     * リフレッシュトークン
     */
    private String refreshToken;
}
