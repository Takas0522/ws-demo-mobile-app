package com.example.webapi.entity;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;

/**
 * 購入履歴エンティティ
 * 
 * テーブル: purchases
 * 用途: ユーザーの商品購入履歴を管理
 */
@Entity
@Table(name = "purchases")
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class Purchase {
    
    @Id
    @Column(name = "purchase_id", columnDefinition = "UUID")
    private String purchaseId; // UUID as String
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "user_id", nullable = false)
    private User user;
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "product_id", nullable = false)
    private Product product;
    
    @Column(name = "quantity", nullable = false)
    private Integer quantity;
    
    @Column(name = "unit_price_at_purchase", nullable = false)
    private Integer unitPriceAtPurchase;
    
    @Column(name = "total_amount", nullable = false)
    private Integer totalAmount;
    
    @Column(name = "purchased_at", nullable = false)
    private LocalDateTime purchasedAt;
    
    @PrePersist
    protected void onCreate() {
        if (purchaseId == null) {
            purchaseId = java.util.UUID.randomUUID().toString();
        }
        if (purchasedAt == null) {
            purchasedAt = LocalDateTime.now();
        }
    }
}
