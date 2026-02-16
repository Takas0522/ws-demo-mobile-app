package com.example.webapi.controller;

import com.example.webapi.dto.ApiResponse;
import com.example.webapi.dto.PaginationResponse;
import com.example.webapi.dto.ProductResponse;
import com.example.webapi.dto.ProductUpdateRequest;
import com.example.webapi.entity.Product;
import com.example.webapi.service.ProductService;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.data.domain.Page;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 商品コントローラー
 * 
 * 商品関連のAPIエンドポイントを提供します
 */
@Slf4j
@RestController
@RequestMapping("/api/v1/products")
@RequiredArgsConstructor
public class ProductController {
    
    private final ProductService productService;
    
    /**
     * 商品一覧取得
     * 
     * @param page ページ番号（デフォルト: 1）
     * @param limit 1ページあたりの件数（デフォルト: 20）
     * @param sortBy ソート項目（デフォルト: productId）
     * @param sortOrder ソート順（デフォルト: asc）
     * @return 商品一覧レスポンス
     */
    @GetMapping
    @PreAuthorize("hasAnyRole('USER', 'ADMIN')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> getAllProducts(
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "20") int limit,
            @RequestParam(defaultValue = "productId") String sortBy,
            @RequestParam(defaultValue = "asc") String sortOrder) {
        
        log.info("商品一覧取得リクエスト: page={}, limit={}, sortBy={}, sortOrder={}", page, limit, sortBy, sortOrder);
        
        // ページは1始まりなので、0始まりに変換
        Page<Product> productPage = productService.getAllProducts(page - 1, limit, sortBy, sortOrder);
        
        List<ProductResponse> products = ProductResponse.fromEntities(productPage.getContent());
        
        PaginationResponse pagination = PaginationResponse.builder()
                .currentPage(page)
                .totalPages(productPage.getTotalPages())
                .totalItems(productPage.getTotalElements())
                .itemsPerPage(limit)
                .build();
        
        Map<String, Object> data = new HashMap<>();
        data.put("products", products);
        data.put("pagination", pagination);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
    
    /**
     * 商品検索
     * 
     * @param keyword 検索キーワード
     * @param page ページ番号（デフォルト: 1）
     * @param limit 1ページあたりの件数（デフォルト: 20）
     * @return 商品検索レスポンス
     */
    @GetMapping("/search")
    @PreAuthorize("hasAnyRole('USER', 'ADMIN')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> searchProducts(
            @RequestParam String keyword,
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "20") int limit) {
        
        log.info("商品検索リクエスト: keyword={}, page={}, limit={}", keyword, page, limit);
        
        List<Product> allProducts = productService.searchProducts(keyword);
        List<ProductResponse> products = ProductResponse.fromEntities(allProducts);
        
        // 簡易的なページネーション（本来はDBレベルで実施すべき）
        int totalItems = products.size();
        int totalPages = (int) Math.ceil((double) totalItems / limit);
        
        PaginationResponse pagination = PaginationResponse.builder()
                .currentPage(page)
                .totalPages(totalPages)
                .totalItems((long) totalItems)
                .itemsPerPage(limit)
                .build();
        
        Map<String, Object> data = new HashMap<>();
        data.put("products", products);
        data.put("searchKeyword", keyword);
        data.put("pagination", pagination);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
    
    /**
     * 商品詳細取得
     * 
     * @param productId 商品ID
     * @return 商品詳細レスポンス
     */
    @GetMapping("/{productId}")
    @PreAuthorize("hasAnyRole('USER', 'ADMIN')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> getProduct(@PathVariable Long productId) {
        log.info("商品詳細取得リクエスト: productId={}", productId);
        
        Product product = productService.getProductById(productId);
        ProductResponse productResponse = ProductResponse.fromEntity(product);
        
        Map<String, Object> data = new HashMap<>();
        data.put("product", productResponse);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
    
    /**
     * 商品情報更新（管理者専用）
     * 
     * @param productId 商品ID
     * @param request 商品更新リクエスト
     * @return 商品更新レスポンス
     */
    @PutMapping("/{productId}")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> updateProduct(
            @PathVariable Long productId,
            @Valid @RequestBody ProductUpdateRequest request) {
        
        log.info("商品更新リクエスト: productId={}, name={}", productId, request.getProductName());
        
        Product product = productService.updateProduct(
                productId,
                request.getProductName(),
                request.getUnitPrice(),
                request.getDescription(),
                request.getImageUrl()
        );
        
        ProductResponse productResponse = ProductResponse.fromEntity(product);
        
        Map<String, Object> data = new HashMap<>();
        data.put("product", productResponse);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
}
