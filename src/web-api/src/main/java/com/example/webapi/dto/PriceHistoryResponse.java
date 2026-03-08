package com.example.webapi.dto;

import com.example.webapi.entity.ProductPriceHistory;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 価格履歴レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class PriceHistoryResponse {

    private Long priceHistoryId;
    private Long productId;
    private Integer oldPrice;
    private Integer newPrice;
    private String changedAt;
    private ChangedByResponse changedBy;
    private String changeReason;

    /**
     * エンティティからDTOに変換
     */
    public static PriceHistoryResponse fromEntity(ProductPriceHistory entity, String userName) {
        return PriceHistoryResponse.builder()
                .priceHistoryId(entity.getPriceHistoryId())
                .productId(entity.getProductId())
                .oldPrice(entity.getOldPrice())
                .newPrice(entity.getNewPrice())
                .changedAt(entity.getChangedAt())
                .changedBy(ChangedByResponse.builder()
                        .userId(entity.getChangedBy())
                        .userName(userName)
                        .build())
                .changeReason(entity.getChangeReason())
                .build();
    }
}
