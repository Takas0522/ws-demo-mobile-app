package com.example.webapi.repository;

import com.example.webapi.entity.Favorite;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

/**
 * お気に入りリポジトリ
 * 
 * お気に入りエンティティのデータアクセス
 */
@Repository
public interface FavoriteRepository extends JpaRepository<Favorite, Long> {
    
    /**
     * ユーザーIDでお気に入りを検索
     * 
     * @param userId ユーザーID
     * @return お気に入りリスト
     */
    @Query("SELECT f FROM Favorite f JOIN FETCH f.product WHERE f.user.userId = :userId ORDER BY f.createdAt DESC")
    List<Favorite> findByUserIdOrderByCreatedAtDesc(@Param("userId") Long userId);
    
    /**
     * ユーザーIDと商品IDでお気に入りを検索
     * 
     * @param userId ユーザーID
     * @param productId 商品ID
     * @return お気に入り（Optional）
     */
    @Query("SELECT f FROM Favorite f WHERE f.user.userId = :userId AND f.product.productId = :productId")
    Optional<Favorite> findByUserIdAndProductId(@Param("userId") Long userId, @Param("productId") Long productId);
    
    /**
     * ユーザーIDと商品IDでお気に入りの存在確認
     * 
     * @param userId ユーザーID
     * @param productId 商品ID
     * @return 存在する場合true
     */
    @Query("SELECT CASE WHEN COUNT(f) > 0 THEN true ELSE false END FROM Favorite f WHERE f.user.userId = :userId AND f.product.productId = :productId")
    boolean existsByUserIdAndProductId(@Param("userId") Long userId, @Param("productId") Long productId);
}
