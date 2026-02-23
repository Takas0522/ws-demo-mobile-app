package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 購入レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PurchaseDto {
    
    /**
     * 購入ID
     */
    private Long purchaseId;
    
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
     * 数量
     */
    private Integer quantity;
    
    /**
     * 単価
     */
    private Integer unitPrice;
    
    /**
     * 合計金額
     */
    private Integer totalAmount;
    
    /**
     * 購入日時
     */
    private String purchaseDate;
}
