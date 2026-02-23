package com.example.webapi.service;

import com.example.webapi.entity.Product;
import com.example.webapi.exception.ResourceNotFoundException;
import com.example.webapi.repository.ProductRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.data.domain.Sort;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

/**
 * 商品サービス
 * 
 * 商品のCRUD操作を提供します
 */
@Slf4j
@Service
@RequiredArgsConstructor
@Transactional
public class ProductService {
    
    private final ProductRepository productRepository;
    
    /**
     * 全商品を取得（ページング対応）
     * 
     * @param page ページ番号（0始まり）
     * @param limit 1ページあたりの件数
     * @param sortBy ソート項目
     * @param sortOrder ソート順
     * @return 商品ページ
     */
    @Transactional(readOnly = true)
    public Page<Product> getAllProducts(int page, int limit, String sortBy, String sortOrder) {
        log.debug("商品一覧取得開始: page={}, limit={}, sortBy={}, sortOrder={}", page, limit, sortBy, sortOrder);
        
        // ソート設定
        Sort sort = sortOrder.equalsIgnoreCase("desc") 
                ? Sort.by(sortBy).descending() 
                : Sort.by(sortBy).ascending();
        
        Pageable pageable = PageRequest.of(page, limit, sort);
        Page<Product> products = productRepository.findAll(pageable);
        
        log.info("商品一覧取得完了: 件数={}", products.getTotalElements());
        return products;
    }
    
    /**
     * 商品をIDで取得
     * 
     * @param id 商品ID
     * @return 商品
     * @throws ResourceNotFoundException 商品が見つからない場合
     */
    @Transactional(readOnly = true)
    public Product getProductById(Long id) {
        log.debug("商品詳細取得: id={}", id);
        return productRepository.findById(id)
                .orElseThrow(() -> {
                    log.warn("商品が見つかりません: id={}", id);
                    return new ResourceNotFoundException("PRODUCT_001", "商品が見つかりません");
                });
    }
    
    /**
     * 商品を検索（キーワード検索）
     * 
     * @param keyword 検索キーワード
     * @return 商品リスト
     */
    @Transactional(readOnly = true)
    public List<Product> searchProducts(String keyword) {
        log.debug("商品検索: keyword={}", keyword);
        List<Product> products = productRepository.searchByKeyword(keyword);
        log.info("商品検索完了: keyword={}, 件数={}", keyword, products.size());
        return products;
    }
    
    /**
     * 商品情報を更新（管理者専用）
     * 
     * @param id 商品ID
     * @param productName 商品名
     * @param unitPrice 単価
     * @param description 説明
     * @param imageUrl 画像URL
     * @return 更新後の商品
     * @throws ResourceNotFoundException 商品が見つからない場合
     */
    public Product updateProduct(Long id, String productName, Integer unitPrice, String description, String imageUrl) {
        log.debug("商品更新開始: id={}", id);
        
        Product product = getProductById(id);
        
        product.setProductName(productName);
        product.setUnitPrice(unitPrice);
        product.setDescription(description);
        product.setImageUrl(imageUrl);
        
        Product updated = productRepository.save(product);
        log.info("商品更新完了: id={}, name={}", updated.getProductId(), updated.getProductName());
        
        return updated;
    }
}
