package com.example.webapi.dto;

import jakarta.validation.constraints.Min;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Size;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 商品更新リクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ProductUpdateRequest {
    
    /**
     * 商品名
     */
    @NotBlank(message = "商品名は必須です")
    @Size(min = 1, max = 100, message = "商品名は1〜100文字で入力してください")
    private String productName;
    
    /**
     * 単価（円）
     */
    @NotNull(message = "単価は必須です")
    @Min(value = 1, message = "単価は1以上である必要があります")
    private Integer unitPrice;
    
    /**
     * 商品説明
     */
    @Size(max = 500, message = "商品説明は500文字以内で入力してください")
    private String description;
    
    /**
     * 商品画像URL
     */
    @Size(max = 500, message = "画像URLは500文字以内で入力してください")
    private String imageUrl;
}
