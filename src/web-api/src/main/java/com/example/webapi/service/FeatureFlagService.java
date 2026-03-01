package com.example.webapi.service;

import com.example.webapi.entity.FeatureFlag;
import com.example.webapi.entity.User;
import com.example.webapi.entity.UserFeatureFlag;
import com.example.webapi.exception.ResourceNotFoundException;
import com.example.webapi.repository.FeatureFlagRepository;
import com.example.webapi.repository.UserFeatureFlagRepository;
import com.example.webapi.repository.UserRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 機能フラグサービス
 * 
 * ユーザーごとの機能フラグ管理を提供します
 */
@Slf4j
@Service
@RequiredArgsConstructor
@Transactional
public class FeatureFlagService {
    
    private final UserFeatureFlagRepository userFeatureFlagRepository;
    private final FeatureFlagRepository featureFlagRepository;
    private final UserRepository userRepository;
    
    private static final String FAVORITE_FEATURE_KEY = "favoriteFeature";
    
    /**
     * ユーザーの機能フラグを取得
     * 
     * @param userId ユーザーID
     * @return 機能フラグマップ
     */
    @Transactional(readOnly = true)
    public Map<String, Boolean> getUserFeatureFlags(Long userId) {
        log.debug("ユーザー機能フラグ取得: userId={}", userId);
        
        List<UserFeatureFlag> userFlags = userFeatureFlagRepository.findByUserId(userId);
        
        Map<String, Boolean> flagMap = new HashMap<>();
        for (UserFeatureFlag userFlag : userFlags) {
            flagMap.put(userFlag.getFeatureFlag().getFlagKey(), userFlag.getIsEnabled());
        }
        
        log.info("ユーザー機能フラグ取得完了: userId={}, flags={}", userId, flagMap);
        return flagMap;
    }
    
    /**
     * お気に入り機能が有効かチェック
     * 
     * @param userId ユーザーID
     * @return お気に入り機能が有効な場合true
     */
    @Transactional(readOnly = true)
    public boolean isFavoriteFeatureEnabled(Long userId) {
        log.debug("お気に入り機能フラグチェック: userId={}", userId);
        
        FeatureFlag featureFlag = featureFlagRepository.findByFlagKey(FAVORITE_FEATURE_KEY)
                .orElse(null);
        
        if (featureFlag == null) {
            log.warn("お気に入り機能フラグが見つかりません");
            return false;
        }
        
        UserFeatureFlag userFlag = userFeatureFlagRepository.findByUserIdAndFlagId(userId, featureFlag.getFlagId())
                .orElse(null);
        
        boolean enabled = userFlag != null && userFlag.getIsEnabled();
        log.debug("お気に入り機能フラグチェック結果: userId={}, enabled={}", userId, enabled);
        
        return enabled;
    }
    
    /**
     * 全ユーザーと機能フラグを取得（管理者用）
     * 
     * @param page ページ番号（0始まり）
     * @param limit 1ページあたりの件数
     * @return ユーザーページ
     */
    @Transactional(readOnly = true)
    public Page<User> getAllUsersWithFlags(int page, int limit) {
        log.debug("全ユーザー取得（機能フラグ付き）: page={}, limit={}", page, limit);
        
        Pageable pageable = PageRequest.of(page, limit);
        Page<User> users = userRepository.findAll(pageable);
        
        log.info("全ユーザー取得完了: 件数={}", users.getTotalElements());
        return users;
    }
    
    /**
     * ユーザーの機能フラグを更新（管理者用）
     * 
     * @param userId ユーザーID
     * @param flagKey フラグキー
     * @param isEnabled 有効/無効
     * @return 更新後のユーザー機能フラグ
     * @throws ResourceNotFoundException ユーザーまたは機能フラグが見つからない場合
     */
    public UserFeatureFlag updateUserFeatureFlag(Long userId, String flagKey, Boolean isEnabled) {
        log.debug("ユーザー機能フラグ更新開始: userId={}, flagKey={}, isEnabled={}", userId, flagKey, isEnabled);
        
        // ユーザー取得
        User user = userRepository.findById(userId)
                .orElseThrow(() -> {
                    log.warn("ユーザーが見つかりません: userId={}", userId);
                    return new ResourceNotFoundException("USER_001", "ユーザーが見つかりません");
                });
        
        // 機能フラグ取得
        FeatureFlag featureFlag = featureFlagRepository.findByFlagKey(flagKey)
                .orElseThrow(() -> {
                    log.warn("機能フラグが見つかりません: flagKey={}", flagKey);
                    return new ResourceNotFoundException("FLAG_001", "機能フラグが見つかりません");
                });
        
        // ユーザー機能フラグ取得または作成
        UserFeatureFlag userFlag = userFeatureFlagRepository.findByUserIdAndFlagId(userId, featureFlag.getFlagId())
                .orElse(UserFeatureFlag.builder()
                        .user(user)
                        .featureFlag(featureFlag)
                        .build());
        
        userFlag.setIsEnabled(isEnabled);
        UserFeatureFlag saved = userFeatureFlagRepository.save(userFlag);
        
        log.info("ユーザー機能フラグ更新完了: userId={}, flagKey={}, isEnabled={}", userId, flagKey, isEnabled);
        return saved;
    }
    
    /**
     * 全機能フラグマスタを取得（管理者用）
     * 
     * @return 機能フラグリスト
     */
    @Transactional(readOnly = true)
    public List<FeatureFlag> getAllFeatureFlags() {
        log.debug("全機能フラグ取得");
        List<FeatureFlag> flags = featureFlagRepository.findAll();
        log.info("全機能フラグ取得完了: 件数={}", flags.size());
        return flags;
    }
}
