package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * お気に入りDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class FavoriteDto {
    
    /**
     * お気に入りID
     */
    private Long favoriteId;
    
    /**
     * ユーザーID
     */
    private Long userId;
    
    /**
     * 商品ID
     */
    private Long productId;
    
    /**
     * 商品名
     */
    private String productName;
    
    /**
     * 商品価格
     */
    private Integer price;
    
    /**
     * 商品画像URL
     */
    private String imageUrl;
    
    /**
     * 登録日時
     */
    private String createdAt;
}
