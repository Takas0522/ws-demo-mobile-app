package com.example.admin.service;

import com.example.admin.dao.FavoriteDao;
import com.example.admin.dao.FeatureFlagDao;
import com.example.admin.dao.UserFeatureFlagDao;
import com.example.admin.entity.Favorite;
import com.example.admin.entity.FeatureFlag;
import com.example.admin.entity.UserFeatureFlag;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.sql.SQLException;
import java.util.List;

/**
 * お気に入りサービス。
 * お気に入り機能のフィーチャーフラグチェックおよびCRUDを提供する。
 */
@Service
public class FavoriteService {

    private static final Logger logger = LoggerFactory.getLogger(FavoriteService.class);

    private FavoriteDao favoriteDao;
    private FeatureFlagDao featureFlagDao;
    private UserFeatureFlagDao userFeatureFlagDao;

    @Autowired
    public void setFavoriteDao(FavoriteDao favoriteDao) {
        this.favoriteDao = favoriteDao;
    }

    @Autowired
    public void setFeatureFlagDao(FeatureFlagDao featureFlagDao) {
        this.featureFlagDao = featureFlagDao;
    }

    @Autowired
    public void setUserFeatureFlagDao(UserFeatureFlagDao userFeatureFlagDao) {
        this.userFeatureFlagDao = userFeatureFlagDao;
    }

    /**
     * お気に入り機能が有効かチェックする。
     * favorite_feature フラグが有効な場合のみお気に入り操作を許可。
     *
     * @param userId ユーザーID
     * @return 有効な場合 true
     * @throws SQLException データベースエラー
     */
    public boolean isFavoriteEnabled(Long userId) throws SQLException {
        // feature_flags テーブルから favorite_feature を取得
        FeatureFlag flag = featureFlagDao.findByFlagKey("favorite_feature");
        if (flag == null) {
            return false;
        }

        // user_feature_flags を確認
        UserFeatureFlag userFlag = userFeatureFlagDao.findByUserIdAndFlagId(userId, flag.getFlagId());
        if (userFlag != null) {
            return Boolean.TRUE.equals(userFlag.getIsEnabled());
        }

        // レコードがなければデフォルト値
        return Boolean.TRUE.equals(flag.getDefaultValue());
    }

    /**
     * ユーザーのお気に入り一覧を取得する。
     */
    public List<Favorite> getFavorites(Long userId) throws SQLException {
        return favoriteDao.findByUserId(userId);
    }

    /**
     * お気に入りに追加する。
     *
     * @throws IllegalStateException 既にお気に入りに追加済みの場合
     */
    public void addFavorite(Long userId, Long productId) throws SQLException {
        if (favoriteDao.existsByUserIdAndProductId(userId, productId)) {
            throw new IllegalStateException("Already in favorites");
        }
        favoriteDao.create(userId, productId);
    }

    /**
     * お気に入りから削除する。
     */
    public void removeFavorite(Long userId, Long productId) throws SQLException {
        favoriteDao.deleteByUserIdAndProductId(userId, productId);
    }
}
