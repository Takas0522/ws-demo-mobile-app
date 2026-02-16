package com.example.webapi.repository;

import com.example.webapi.entity.Purchase;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * 購入履歴リポジトリ
 * 
 * 購入履歴エンティティのデータアクセス
 */
@Repository
public interface PurchaseRepository extends JpaRepository<Purchase, Long> {
    
    /**
     * ユーザーIDで購入履歴を検索（購入日時降順）
     * 
     * @param userId ユーザーID
     * @return 購入履歴リスト
     */
    @Query("SELECT p FROM Purchase p JOIN FETCH p.product WHERE p.user.userId = :userId ORDER BY p.purchasedAt DESC")
    List<Purchase> findByUserIdOrderByPurchasedAtDesc(@Param("userId") Long userId);
}
