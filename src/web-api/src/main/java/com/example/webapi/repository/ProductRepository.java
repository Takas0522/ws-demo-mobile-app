package com.example.webapi.repository;

import com.example.webapi.entity.Product;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * 商品リポジトリ
 * 
 * 商品エンティティのデータアクセス
 */
@Repository
public interface ProductRepository extends JpaRepository<Product, Long> {
    
    /**
     * 商品名でキーワード検索（部分一致）
     * 
     * @param keyword 検索キーワード
     * @return 商品リスト
     */
    @Query("SELECT p FROM Product p WHERE p.productName LIKE %:keyword%")
    List<Product> searchByKeyword(@Param("keyword") String keyword);
}
