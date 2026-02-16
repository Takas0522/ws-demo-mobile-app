package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 商品DTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ProductDto {
    
    /**
     * 商品ID
     */
    private Long productId;
    
    /**
     * 商品コード
     */
    private String productCode;
    
    /**
     * 商品名
     */
    private String productName;
    
    /**
     * 商品説明
     */
    private String description;
    
    /**
     * 価格
     */
    private Integer price;
    
    /**
     * 在庫数
     */
    private Integer stockQuantity;
    
    /**
     * カテゴリ
     */
    private String category;
    
    /**
     * 画像URL
     */
    private String imageUrl;
    
    /**
     * ステータス
     */
    private String status;
}
