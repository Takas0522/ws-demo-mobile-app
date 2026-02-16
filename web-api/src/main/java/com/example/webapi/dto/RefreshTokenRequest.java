package com.example.webapi.dto;

import jakarta.validation.constraints.NotBlank;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * トークンリフレッシュリクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class RefreshTokenRequest {
    
    @NotBlank(message = "トークンは必須です")
    private String token;
}
