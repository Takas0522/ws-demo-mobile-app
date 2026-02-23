package com.example.webapi.dto;

import com.example.webapi.entity.Favorite;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;
import java.util.List;
import java.util.stream.Collectors;

/**
 * お気に入りレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class FavoriteResponse {
    
    /**
     * お気に入りID
     */
    private Long favoriteId;
    
    /**
     * ユーザーID
     */
    private Long userId;
    
    /**
     * 商品情報
     */
    private ProductInfo product;
    
    /**
     * 登録日時
     */
    private LocalDateTime createdAt;
    
    /**
     * 商品情報（簡易版）
     */
    @Data
    @NoArgsConstructor
    @AllArgsConstructor
    @Builder
    public static class ProductInfo {
        private Long productId;
        private String productName;
        private Integer unitPrice;
        private String imageUrl;
    }
    
    /**
     * エンティティからDTOに変換
     * 
     * @param favorite お気に入りエンティティ
     * @return お気に入りレスポンスDTO
     */
    public static FavoriteResponse fromEntity(Favorite favorite) {
        return FavoriteResponse.builder()
                .favoriteId(favorite.getFavoriteId())
                .userId(favorite.getUser().getUserId())
                .product(ProductInfo.builder()
                        .productId(favorite.getProduct().getProductId())
                        .productName(favorite.getProduct().getProductName())
                        .unitPrice(favorite.getProduct().getUnitPrice())
                        .imageUrl(favorite.getProduct().getImageUrl())
                        .build())
                .createdAt(favorite.getCreatedAt())
                .build();
    }
    
    /**
     * エンティティリストをDTOリストに変換
     * 
     * @param favorites お気に入りエンティティリスト
     * @return お気に入りレスポンスDTOリスト
     */
    public static List<FavoriteResponse> fromEntities(List<Favorite> favorites) {
        return favorites.stream()
                .map(FavoriteResponse::fromEntity)
                .collect(Collectors.toList());
    }
}
