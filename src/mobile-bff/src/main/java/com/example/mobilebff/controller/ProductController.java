package com.example.mobilebff.controller;

import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.mobilebff.client.WebApiClient;
import com.example.mobilebff.dto.ApiResponse;
import com.example.mobilebff.dto.ProductDto;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * 商品コントローラー
 * 
 * Mobile BFFの商品API
 */
@RestController
@RequestMapping("/api/mobile/products")
@RequiredArgsConstructor
@Slf4j
public class ProductController {
    
    private final WebApiClient webApiClient;
    
    /**
     * 商品一覧取得
     * 
     * @param authorization 認証トークン
     * @return 商品一覧
     */
    @GetMapping
    public ResponseEntity<ApiResponse<List<ProductDto>>> getProducts(
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get products request");
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<List<ProductDto>> response = webApiClient.get(
            "/api/v1/products",
            (Class<ApiResponse<List<ProductDto>>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * 商品詳細取得
     * 
     * @param id 商品ID
     * @param authorization 認証トークン
     * @return 商品詳細
     */
    @GetMapping("/{id}")
    public ResponseEntity<ApiResponse<ProductDto>> getProduct(
            @PathVariable Long id,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get product detail request: productId={}", id);
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<ProductDto> response = webApiClient.get(
            "/api/v1/products/" + id,
            (Class<ApiResponse<ProductDto>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * 商品検索
     * 
     * @param keyword 検索キーワード
     * @param authorization 認証トークン
     * @return 商品一覧
     */
    @GetMapping("/search")
    public ResponseEntity<ApiResponse<List<ProductDto>>> searchProducts(
            @RequestParam(required = false) String keyword,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Search products request: keyword={}", keyword);
        
        String token = extractToken(authorization);
        
        // クエリパラメータを構築
        String path = "/api/v1/products/search";
        if (keyword != null && !keyword.isEmpty()) {
            path += "?keyword=" + keyword;
        }
        
        // Web APIにリクエストを転送
        ApiResponse<List<ProductDto>> response = webApiClient.get(
            path,
            (Class<ApiResponse<List<ProductDto>>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * Authorizationヘッダーからトークンを抽出
     * 
     * @param authorization Authorizationヘッダー
     * @return トークン
     */
    private String extractToken(String authorization) {
        if (authorization != null && authorization.startsWith("Bearer ")) {
            return authorization.substring(7);
        }
        return null;
    }
}
