package com.example.webapi.entity;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 価格履歴エンティティ
 *
 * テーブル: product_price_history
 */
@Entity
@Table(name = "product_price_history")
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class ProductPriceHistory {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "price_history_id")
    private Long priceHistoryId;

    @Column(name = "product_id", nullable = false)
    private Long productId;

    @Column(name = "old_price", nullable = false)
    private Integer oldPrice;

    @Column(name = "new_price", nullable = false)
    private Integer newPrice;

    @Column(name = "changed_at", nullable = false)
    private String changedAt;

    @Column(name = "changed_by", nullable = false)
    private Long changedBy;

    @Column(name = "change_reason")
    private String changeReason;

    @Column(name = "created_at", nullable = false, updatable = false)
    private String createdAt;

    @PrePersist
    protected void onCreate() {
        createdAt = java.time.LocalDateTime.now().toString();
        if (changedAt == null) {
            changedAt = createdAt;
        }
    }
}
