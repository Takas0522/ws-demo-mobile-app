package com.example.admin.service;

import com.example.admin.dao.FeatureFlagDao;
import com.example.admin.dao.UserFeatureFlagDao;
import com.example.admin.entity.FeatureFlag;
import com.example.admin.entity.UserFeatureFlag;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

@Service
public class FeatureFlagService {

    private static final Logger logger = LoggerFactory.getLogger(FeatureFlagService.class);

    private FeatureFlagDao featureFlagDao;
    private UserFeatureFlagDao userFeatureFlagDao;

    @Autowired
    public void setFeatureFlagDao(FeatureFlagDao featureFlagDao) {
        this.featureFlagDao = featureFlagDao;
    }

    @Autowired
    public void setUserFeatureFlagDao(UserFeatureFlagDao userFeatureFlagDao) {
        this.userFeatureFlagDao = userFeatureFlagDao;
    }

    /**
     * ユーザーのフィーチャーフラグ一覧を取得する。
     * user_feature_flags にレコードがないフラグはデフォルト値を使用する。
     * @param userId ユーザーID
     * @return ユーザーフィーチャーフラグのリスト
     */
    public List<UserFeatureFlag> getUserFeatureFlags(Long userId) {
        try {
            List<FeatureFlag> allFlags = featureFlagDao.findAll();
            List<UserFeatureFlag> userFlags = userFeatureFlagDao.findByUserId(userId);

            // user_feature_flags に存在するフラグのIDをセットに集める
            Set<Long> existingFlagIds = new HashSet<Long>();
            for (UserFeatureFlag uf : userFlags) {
                existingFlagIds.add(uf.getFlagId());
            }

            // 全フラグについて、user_feature_flags にレコードがない場合はデフォルト値で補完
            List<UserFeatureFlag> result = new ArrayList<UserFeatureFlag>(userFlags);
            for (FeatureFlag ff : allFlags) {
                if (!existingFlagIds.contains(ff.getFlagId())) {
                    UserFeatureFlag defaultFlag = new UserFeatureFlag();
                    defaultFlag.setUserId(userId);
                    defaultFlag.setFlagId(ff.getFlagId());
                    defaultFlag.setFlagKey(ff.getFlagKey());
                    defaultFlag.setFlagName(ff.getFlagName());
                    defaultFlag.setIsEnabled(ff.getDefaultValue());
                    defaultFlag.setDefaultValue(ff.getDefaultValue());
                    result.add(defaultFlag);
                }
            }
            return result;
        } catch (SQLException e) {
            logger.error("Failed to get user feature flags for userId: {}", userId, e);
            throw new RuntimeException("フィーチャーフラグ一覧の取得に失敗しました", e);
        }
    }

    /**
     * ユーザーのフィーチャーフラグを更新する。
     * @param userId ユーザーID
     * @param flagKey フラグキー
     * @param enabled 有効/無効
     */
    public void updateUserFeatureFlag(Long userId, String flagKey, boolean enabled) {
        try {
            FeatureFlag flag = featureFlagDao.findByFlagKey(flagKey);
            if (flag == null) {
                throw new IllegalArgumentException("Feature flag not found: " + flagKey);
            }
            userFeatureFlagDao.upsert(userId, flag.getFlagId(), enabled);
        } catch (SQLException e) {
            logger.error("Failed to update feature flag '{}' for userId: {}", flagKey, userId, e);
            throw new RuntimeException("フィーチャーフラグの更新に失敗しました", e);
        }
    }
}
