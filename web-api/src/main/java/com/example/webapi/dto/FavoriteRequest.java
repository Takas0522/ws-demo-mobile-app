package com.example.webapi.dto;

import jakarta.validation.constraints.NotNull;
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
    @NotNull(message = "商品IDは必須です")
    private Long productId;
}
