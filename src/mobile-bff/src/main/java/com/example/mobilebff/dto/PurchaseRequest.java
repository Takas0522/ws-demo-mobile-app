package com.example.mobilebff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 購入リクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PurchaseRequest {
    
    /**
     * 商品ID
     */
    private Long productId;
    
    /**
     * 数量
     */
    private Integer quantity;
}
