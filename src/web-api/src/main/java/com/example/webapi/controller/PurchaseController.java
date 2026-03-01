package com.example.webapi.controller;

import com.example.webapi.dto.ApiResponse;
import com.example.webapi.dto.PaginationResponse;
import com.example.webapi.dto.PurchaseRequest;
import com.example.webapi.dto.PurchaseResponse;
import com.example.webapi.entity.Purchase;
import com.example.webapi.service.JwtService;
import com.example.webapi.service.PurchaseService;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 購入コントローラー
 * 
 * 購入関連のAPIエンドポイントを提供します
 */
@Slf4j
@RestController
@RequestMapping("/api/v1/purchases")
@RequiredArgsConstructor
public class PurchaseController {
    
    private final PurchaseService purchaseService;
    private final JwtService jwtService;
    
    /**
     * 商品購入
     * 
     * @param request 購入リクエスト
     * @param token 認証トークン
     * @return 購入レスポンス
     */
    @PostMapping
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> purchaseProduct(
            @Valid @RequestBody PurchaseRequest request,
            @RequestHeader("Authorization") String token) {
        
        log.info("商品購入リクエスト: productId={}, quantity={}", request.getProductId(), request.getQuantity());
        
        // トークンからユーザーIDを取得
        String jwtToken = token.replace("Bearer ", "");
        Long userId = jwtService.getUserIdFromToken(jwtToken);
        
        Purchase purchase = purchaseService.purchaseProduct(userId, request.getProductId(), request.getQuantity());
        PurchaseResponse purchaseResponse = PurchaseResponse.fromEntity(purchase);
        
        Map<String, Object> data = new HashMap<>();
        data.put("purchase", purchaseResponse);
        
        return ResponseEntity.status(HttpStatus.CREATED).body(ApiResponse.success(data));
    }
    
    /**
     * 購入履歴取得
     * 
     * @param page ページ番号（デフォルト: 1）
     * @param limit 1ページあたりの件数（デフォルト: 20）
     * @param token 認証トークン
     * @return 購入履歴レスポンス
     */
    @GetMapping
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> getPurchaseHistory(
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "20") int limit,
            @RequestHeader("Authorization") String token) {
        
        log.info("購入履歴取得リクエスト: page={}, limit={}", page, limit);
        
        // トークンからユーザーIDを取得
        String jwtToken = token.replace("Bearer ", "");
        Long userId = jwtService.getUserIdFromToken(jwtToken);
        
        List<Purchase> allPurchases = purchaseService.getPurchaseHistory(userId);
        List<PurchaseResponse> purchases = PurchaseResponse.fromEntities(allPurchases);
        
        // 簡易的なページネーション
        int totalItems = purchases.size();
        int totalPages = (int) Math.ceil((double) totalItems / limit);
        
        PaginationResponse pagination = PaginationResponse.builder()
                .currentPage(page)
                .totalPages(totalPages)
                .totalItems((long) totalItems)
                .itemsPerPage(limit)
                .build();
        
        Map<String, Object> data = new HashMap<>();
        data.put("purchases", purchases);
        data.put("pagination", pagination);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
}
