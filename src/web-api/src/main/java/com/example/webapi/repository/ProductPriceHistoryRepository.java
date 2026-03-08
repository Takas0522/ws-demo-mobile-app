package com.example.webapi.repository;

import com.example.webapi.entity.ProductPriceHistory;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

/**
 * 価格履歴リポジトリ
 */
@Repository
public interface ProductPriceHistoryRepository extends JpaRepository<ProductPriceHistory, Long> {

    /**
     * 商品IDと日付範囲で価格履歴を取得
     */
    @Query("SELECT h FROM ProductPriceHistory h WHERE h.productId = :productId " +
            "AND h.changedAt >= :startDate AND h.changedAt <= :endDate " +
            "ORDER BY h.changedAt DESC")
    Page<ProductPriceHistory> findByProductIdAndDateRange(
            @Param("productId") Long productId,
            @Param("startDate") String startDate,
            @Param("endDate") String endDate,
            Pageable pageable);
}
