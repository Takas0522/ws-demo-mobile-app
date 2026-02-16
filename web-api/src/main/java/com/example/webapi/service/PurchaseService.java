package com.example.webapi.service;

import com.example.webapi.entity.Product;
import com.example.webapi.entity.Purchase;
import com.example.webapi.entity.User;
import com.example.webapi.exception.ResourceNotFoundException;
import com.example.webapi.exception.ValidationException;
import com.example.webapi.repository.ProductRepository;
import com.example.webapi.repository.PurchaseRepository;
import com.example.webapi.repository.UserRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

/**
 * 購入サービス
 * 
 * 商品購入と購入履歴の管理を提供します
 */
@Slf4j
@Service
@RequiredArgsConstructor
@Transactional
public class PurchaseService {
    
    private final PurchaseRepository purchaseRepository;
    private final ProductRepository productRepository;
    private final UserRepository userRepository;
    
    private static final int QUANTITY_UNIT = 100;
    private static final int MIN_QUANTITY = 100;
    private static final int MAX_QUANTITY = 9900;
    
    /**
     * 商品を購入
     * 
     * @param userId ユーザーID
     * @param productId 商品ID
     * @param quantity 購入個数
     * @return 購入履歴
     * @throws ResourceNotFoundException ユーザーまたは商品が見つからない場合
     * @throws ValidationException 購入個数が不正な場合
     */
    public Purchase purchaseProduct(Long userId, Long productId, Integer quantity) {
        log.debug("商品購入開始: userId={}, productId={}, quantity={}", userId, productId, quantity);
        
        // バリデーション: 100の倍数チェック
        if (quantity % QUANTITY_UNIT != 0) {
            log.warn("購入個数が不正です: quantity={}", quantity);
            throw new ValidationException("PURCHASE_001", "購入個数は100の倍数である必要があります", 
                    "quantity=" + quantity);
        }
        
        // バリデーション: 範囲チェック
        if (quantity < MIN_QUANTITY || quantity > MAX_QUANTITY) {
            log.warn("購入個数が範囲外です: quantity={}", quantity);
            throw new ValidationException("PURCHASE_002", "購入個数は100〜9900の範囲で指定してください", 
                    "quantity=" + quantity);
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
        
        // 合計金額計算
        Integer totalAmount = product.getUnitPrice() * quantity;
        
        // 購入レコード作成
        Purchase purchase = Purchase.builder()
                .user(user)
                .product(product)
                .quantity(quantity)
                .unitPriceAtPurchase(product.getUnitPrice())
                .totalAmount(totalAmount)
                .build();
        
        Purchase saved = purchaseRepository.save(purchase);
        log.info("商品購入完了: purchaseId={}, userId={}, productId={}, quantity={}, totalAmount={}", 
                saved.getPurchaseId(), userId, productId, quantity, totalAmount);
        
        return saved;
    }
    
    /**
     * ユーザーの購入履歴を取得
     * 
     * @param userId ユーザーID
     * @return 購入履歴リスト
     */
    @Transactional(readOnly = true)
    public List<Purchase> getPurchaseHistory(Long userId) {
        log.debug("購入履歴取得: userId={}", userId);
        List<Purchase> purchases = purchaseRepository.findByUserIdOrderByPurchasedAtDesc(userId);
        log.info("購入履歴取得完了: userId={}, 件数={}", userId, purchases.size());
        return purchases;
    }
}
