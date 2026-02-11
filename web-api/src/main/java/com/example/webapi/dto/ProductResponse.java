package com.example.webapi.dto;

import com.example.webapi.entity.Product;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 商品レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class ProductResponse {
    
    /**
     * 商品ID
     */
    private Long productId;
    
    /**
     * 商品名
     */
    private String productName;
    
    /**
     * 単価（円）
     */
    private Integer unitPrice;
    
    /**
     * 商品説明
     */
    private String description;
    
    /**
     * 商品画像URL
     */
    private String imageUrl;
    
    /**
     * 作成日時
     */
    private LocalDateTime createdAt;
    
    /**
     * 更新日時
     */
    private LocalDateTime updatedAt;
    
    /**
     * エンティティからDTOに変換（詳細版）
     * 
     * @param product 商品エンティティ
     * @return 商品レスポンスDTO
     */
    public static ProductResponse fromEntity(Product product) {
        return ProductResponse.builder()
                .productId(product.getProductId())
                .productName(product.getProductName())
                .unitPrice(product.getUnitPrice())
                .description(product.getDescription())
                .imageUrl(product.getImageUrl())
                .createdAt(product.getCreatedAt())
                .updatedAt(product.getUpdatedAt())
                .build();
    }
    
    /**
     * エンティティからDTOに変換（簡易版：一覧用）
     * 
     * @param product 商品エンティティ
     * @return 商品レスポンスDTO
     */
    public static ProductResponse fromEntitySimple(Product product) {
        return ProductResponse.builder()
                .productId(product.getProductId())
                .productName(product.getProductName())
                .unitPrice(product.getUnitPrice())
                .description(product.getDescription())
                .imageUrl(product.getImageUrl())
                .build();
    }
    
    /**
     * エンティティリストをDTOリストに変換
     * 
     * @param products 商品エンティティリスト
     * @return 商品レスポンスDTOリスト
     */
    public static List<ProductResponse> fromEntities(List<Product> products) {
        return products.stream()
                .map(ProductResponse::fromEntitySimple)
                .collect(Collectors.toList());
    }
}
