package com.example.webapi.repository;

import com.example.webapi.entity.FeatureFlag;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

/**
 * 機能フラグリポジトリ
 * 
 * 機能フラグエンティティのデータアクセス
 */
@Repository
public interface FeatureFlagRepository extends JpaRepository<FeatureFlag, Long> {
    
    /**
     * フラグキーで機能フラグを検索
     * 
     * @param flagKey フラグキー
     * @return 機能フラグ（Optional）
     */
    Optional<FeatureFlag> findByFlagKey(String flagKey);
}
