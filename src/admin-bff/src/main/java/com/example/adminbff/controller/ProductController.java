package com.example.adminbff.controller;

import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.adminbff.client.WebApiClient;
import com.example.adminbff.dto.ApiResponse;
import com.example.adminbff.dto.ProductDto;
import com.example.adminbff.dto.ProductUpdateRequest;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * 商品管理コントローラー
 * 
 * Admin BFFの商品管理API
 */
@RestController
@RequestMapping("/api/admin/products")
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
        log.info("Get products request (admin)");
        
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
        log.info("Get product detail request (admin): productId={}", id);
        
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
     * 商品更新
     * 
     * @param id 商品ID
     * @param request 商品更新リクエスト
     * @param authorization 認証トークン
     * @return 更新後の商品情報
     */
    @PutMapping("/{id}")
    public ResponseEntity<ApiResponse<ProductDto>> updateProduct(
            @PathVariable Long id,
            @RequestBody ProductUpdateRequest request,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Update product request (admin): productId={}", id);
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<ProductDto> response = webApiClient.put(
            "/api/v1/products/" + id,
            request,
            (Class<ApiResponse<ProductDto>>) (Class<?>) ApiResponse.class,
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
