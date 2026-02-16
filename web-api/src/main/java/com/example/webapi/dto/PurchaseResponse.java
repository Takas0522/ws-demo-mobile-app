package com.example.webapi.dto;

import com.example.webapi.entity.Purchase;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 購入レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class PurchaseResponse {
    
    /**
     * 購入ID（UUID）
     */
    private String purchaseId;
    
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
     * 購入個数
     */
    private Integer quantity;
    
    /**
     * 購入時の単価
     */
    private Integer unitPriceAtPurchase;
    
    /**
     * 合計金額
     */
    private Integer totalAmount;
    
    /**
     * 購入日時
     */
    private LocalDateTime purchasedAt;
    
    /**
     * エンティティからDTOに変換
     * 
     * @param purchase 購入エンティティ
     * @return 購入レスポンスDTO
     */
    public static PurchaseResponse fromEntity(Purchase purchase) {
        return PurchaseResponse.builder()
                .purchaseId(purchase.getPurchaseId())
                .userId(purchase.getUser().getUserId())
                .productId(purchase.getProduct().getProductId())
                .productName(purchase.getProduct().getProductName())
                .quantity(purchase.getQuantity())
                .unitPriceAtPurchase(purchase.getUnitPriceAtPurchase())
                .totalAmount(purchase.getTotalAmount())
                .purchasedAt(purchase.getPurchasedAt())
                .build();
    }
    
    /**
     * エンティティリストをDTOリストに変換
     * 
     * @param purchases 購入エンティティリスト
     * @return 購入レスポンスDTOリスト
     */
    public static List<PurchaseResponse> fromEntities(List<Purchase> purchases) {
        return purchases.stream()
                .map(PurchaseResponse::fromEntity)
                .collect(Collectors.toList());
    }
}
