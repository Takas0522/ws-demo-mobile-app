package com.example.mobilebff.controller;

import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.mobilebff.client.WebApiClient;
import com.example.mobilebff.dto.ApiResponse;
import com.example.mobilebff.dto.FavoriteDto;
import com.example.mobilebff.dto.FavoriteRequest;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * お気に入りコントローラー
 * 
 * Mobile BFFのお気に入りAPI
 */
@RestController
@RequestMapping("/api/mobile/favorites")
@RequiredArgsConstructor
@Slf4j
public class FavoriteController {
    
    private final WebApiClient webApiClient;
    
    /**
     * お気に入り一覧取得
     * 
     * @param authorization 認証トークン
     * @return お気に入り一覧
     */
    @GetMapping
    public ResponseEntity<ApiResponse<List<FavoriteDto>>> getFavorites(
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get favorites request");
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<List<FavoriteDto>> response = webApiClient.get(
            "/api/v1/favorites",
            (Class<ApiResponse<List<FavoriteDto>>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * お気に入り登録
     * 
     * @param request お気に入りリクエスト
     * @param authorization 認証トークン
     * @return お気に入りレスポンス
     */
    @PostMapping
    public ResponseEntity<ApiResponse<FavoriteDto>> addFavorite(
            @RequestBody FavoriteRequest request,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Add favorite request: productId={}", request.getProductId());
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<FavoriteDto> response = webApiClient.post(
            "/api/v1/favorites",
            request,
            (Class<ApiResponse<FavoriteDto>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * お気に入り解除
     * 
     * @param id お気に入りID
     * @param authorization 認証トークン
     * @return レスポンス
     */
    @DeleteMapping("/{id}")
    public ResponseEntity<ApiResponse<Void>> removeFavorite(
            @PathVariable Long id,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Remove favorite request: favoriteId={}", id);
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<Void> response = webApiClient.delete(
            "/api/v1/favorites/" + id,
            (Class<ApiResponse<Void>>) (Class<?>) ApiResponse.class,
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
