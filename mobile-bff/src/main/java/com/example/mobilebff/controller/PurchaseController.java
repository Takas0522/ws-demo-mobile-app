package com.example.mobilebff.controller;

import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.mobilebff.client.WebApiClient;
import com.example.mobilebff.dto.ApiResponse;
import com.example.mobilebff.dto.PurchaseDto;
import com.example.mobilebff.dto.PurchaseRequest;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * 購入コントローラー
 * 
 * Mobile BFFの購入API
 */
@RestController
@RequestMapping("/api/mobile/purchases")
@RequiredArgsConstructor
@Slf4j
public class PurchaseController {
    
    private final WebApiClient webApiClient;
    
    /**
     * 商品購入
     * 
     * @param request 購入リクエスト
     * @param authorization 認証トークン
     * @return 購入レスポンス
     */
    @PostMapping
    public ResponseEntity<ApiResponse<PurchaseDto>> createPurchase(
            @RequestBody PurchaseRequest request,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Create purchase request: productId={}, quantity={}", 
            request.getProductId(), request.getQuantity());
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<PurchaseDto> response = webApiClient.post(
            "/api/v1/purchases",
            request,
            (Class<ApiResponse<PurchaseDto>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * 購入履歴取得
     * 
     * @param authorization 認証トークン
     * @return 購入履歴一覧
     */
    @GetMapping
    public ResponseEntity<ApiResponse<List<PurchaseDto>>> getPurchases(
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get purchase history request");
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<List<PurchaseDto>> response = webApiClient.get(
            "/api/v1/purchases",
            (Class<ApiResponse<List<PurchaseDto>>>) (Class<?>) ApiResponse.class,
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
