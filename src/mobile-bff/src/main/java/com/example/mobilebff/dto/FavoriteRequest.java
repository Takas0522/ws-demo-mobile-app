package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * お気に入りリクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class FavoriteRequest {
    
    /**
     * 商品ID
     */
    private Long productId;
}
