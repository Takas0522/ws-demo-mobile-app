package com.example.webapi.service;

import com.example.webapi.entity.Favorite;
import com.example.webapi.entity.Product;
import com.example.webapi.entity.User;
import com.example.webapi.exception.FeatureFlagException;
import com.example.webapi.exception.ResourceNotFoundException;
import com.example.webapi.exception.ValidationException;
import com.example.webapi.repository.FavoriteRepository;
import com.example.webapi.repository.ProductRepository;
import com.example.webapi.repository.UserRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

/**
 * お気に入りサービス
 * 
 * お気に入り商品の管理を提供します
 */
@Slf4j
@Service
@RequiredArgsConstructor
@Transactional
public class FavoriteService {
    
    private final FavoriteRepository favoriteRepository;
    private final ProductRepository productRepository;
    private final UserRepository userRepository;
    private final FeatureFlagService featureFlagService;
    
    /**
     * お気に入りに登録
     * 
     * @param userId ユーザーID
     * @param productId 商品ID
     * @return お気に入り
     * @throws FeatureFlagException お気に入り機能が無効な場合
     * @throws ResourceNotFoundException ユーザーまたは商品が見つからない場合
     * @throws ValidationException 既に登録済みの場合
     */
    public Favorite addFavorite(Long userId, Long productId) {
        log.debug("お気に入り登録開始: userId={}, productId={}", userId, productId);
        
        // 機能フラグチェック
        if (!featureFlagService.isFavoriteFeatureEnabled(userId)) {
            log.warn("お気に入り機能が無効です: userId={}", userId);
            throw new FeatureFlagException("FEATURE_001", "お気に入り機能は利用できません");
        }
        
        // ユーザー取得
        User user = userRepository.findById(userId)
                .orElseThrow(() -> {
                    log.warn("ユーザーが見つかりません: userId={}", userId);
                    return new ResourceNotFoundException("USER_001", "ユーザーが見つかりません");
                });
        
        // 商品取得
        Product product = productRepository.findById(productId)
                .orElseThrow(() -> {
                    log.warn("商品が見つかりません: productId={}", productId);
                    return new ResourceNotFoundException("PRODUCT_001", "商品が見つかりません");
                });
        
        // 重複チェック
        if (favoriteRepository.existsByUserIdAndProductId(userId, productId)) {
            log.warn("既にお気に入りに登録されています: userId={}, productId={}", userId, productId);
            throw new ValidationException("FAVORITE_001", "既にお気に入りに登録されています", 
                    "userId=" + userId + ", productId=" + productId);
        }
        
        // お気に入りレコード作成
        Favorite favorite = Favorite.builder()
                .user(user)
                .product(product)
                .build();
        
        Favorite saved = favoriteRepository.save(favorite);
        log.info("お気に入り登録完了: favoriteId={}, userId={}, productId={}", 
                saved.getFavoriteId(), userId, productId);
        
        return saved;
    }
    
    /**
     * お気に入りを解除
     * 
     * @param userId ユーザーID
     * @param productId 商品ID
     */
    public void removeFavorite(Long userId, Long productId) {
        log.debug("お気に入り解除開始: userId={}, productId={}", userId, productId);
        
        // お気に入り検索
        favoriteRepository.findByUserIdAndProductId(userId, productId)
                .ifPresent(favorite -> {
                    favoriteRepository.delete(favorite);
                    log.info("お気に入り解除完了: favoriteId={}, userId={}, productId={}", 
                            favorite.getFavoriteId(), userId, productId);
                });
        
        // 存在しない場合もエラーとしない（冪等性）
        log.debug("お気に入り解除処理完了（冪等性確保）: userId={}, productId={}", userId, productId);
    }
    
    /**
     * ユーザーのお気に入り一覧を取得
     * 
     * @param userId ユーザーID
     * @return お気に入りリスト
     */
    @Transactional(readOnly = true)
    public List<Favorite> getFavorites(Long userId) {
        log.debug("お気に入り一覧取得: userId={}", userId);
        List<Favorite> favorites = favoriteRepository.findByUserIdOrderByCreatedAtDesc(userId);
        log.info("お気に入り一覧取得完了: userId={}, 件数={}", userId, favorites.size());
        return favorites;
    }
}
