package com.example.webapi.repository;

import com.example.webapi.entity.UserFeatureFlag;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

/**
 * ユーザー機能フラグリポジトリ
 * 
 * ユーザー機能フラグエンティティのデータアクセス
 */
@Repository
public interface UserFeatureFlagRepository extends JpaRepository<UserFeatureFlag, Long> {
    
    /**
     * ユーザーIDでユーザー機能フラグを検索
     * 
     * @param userId ユーザーID
     * @return ユーザー機能フラグリスト
     */
    @Query("SELECT uff FROM UserFeatureFlag uff JOIN FETCH uff.featureFlag WHERE uff.user.userId = :userId")
    List<UserFeatureFlag> findByUserId(@Param("userId") Long userId);
    
    /**
     * ユーザーIDとフラグIDでユーザー機能フラグを検索
     * 
     * @param userId ユーザーID
     * @param flagId フラグID
     * @return ユーザー機能フラグ（Optional）
     */
    @Query("SELECT uff FROM UserFeatureFlag uff WHERE uff.user.userId = :userId AND uff.featureFlag.flagId = :flagId")
    Optional<UserFeatureFlag> findByUserIdAndFlagId(@Param("userId") Long userId, @Param("flagId") Long flagId);
}
