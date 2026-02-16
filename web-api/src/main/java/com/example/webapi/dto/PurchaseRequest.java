package com.example.webapi.dto;

import jakarta.validation.constraints.Min;
import jakarta.validation.constraints.NotNull;
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
    @NotNull(message = "商品IDは必須です")
    private Long productId;
    
    /**
     * 購入個数（100の倍数、100〜9900）
     */
    @NotNull(message = "購入個数は必須です")
    @Min(value = 100, message = "購入個数は100以上である必要があります")
    private Integer quantity;
}
