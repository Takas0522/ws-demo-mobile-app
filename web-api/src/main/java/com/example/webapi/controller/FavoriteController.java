package com.example.webapi.controller;

import com.example.webapi.dto.ApiResponse;
import com.example.webapi.dto.FavoriteRequest;
import com.example.webapi.dto.FavoriteResponse;
import com.example.webapi.dto.MessageResponse;
import com.example.webapi.entity.Favorite;
import com.example.webapi.service.JwtService;
import com.example.webapi.service.FavoriteService;
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
 * お気に入りコントローラー
 * 
 * お気に入り関連のAPIエンドポイントを提供します
 */
@Slf4j
@RestController
@RequestMapping("/api/v1/favorites")
@RequiredArgsConstructor
public class FavoriteController {
    
    private final FavoriteService favoriteService;
    private final JwtService jwtService;
    
    /**
     * お気に入り一覧取得
     * 
     * @param token 認証トークン
     * @return お気に入り一覧レスポンス
     */
    @GetMapping
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> getFavorites(
            @RequestHeader("Authorization") String token) {
        
        log.info("お気に入り一覧取得リクエスト");
        
        // トークンからユーザーIDを取得
        String jwtToken = token.replace("Bearer ", "");
        Long userId = jwtService.getUserIdFromToken(jwtToken);
        
        List<Favorite> favorites = favoriteService.getFavorites(userId);
        List<FavoriteResponse> favoriteResponses = FavoriteResponse.fromEntities(favorites);
        
        Map<String, Object> data = new HashMap<>();
        data.put("favorites", favoriteResponses);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
    
    /**
     * お気に入り登録
     * 
     * @param request お気に入りリクエスト
     * @param token 認証トークン
     * @return お気に入り登録レスポンス
     */
    @PostMapping
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> addFavorite(
            @Valid @RequestBody FavoriteRequest request,
            @RequestHeader("Authorization") String token) {
        
        log.info("お気に入り登録リクエスト: productId={}", request.getProductId());
        
        // トークンからユーザーIDを取得
        String jwtToken = token.replace("Bearer ", "");
        Long userId = jwtService.getUserIdFromToken(jwtToken);
        
        Favorite favorite = favoriteService.addFavorite(userId, request.getProductId());
        FavoriteResponse favoriteResponse = FavoriteResponse.fromEntity(favorite);
        
        Map<String, Object> data = new HashMap<>();
        data.put("favorite", favoriteResponse);
        
        return ResponseEntity.status(HttpStatus.CREATED).body(ApiResponse.success(data));
    }
    
    /**
     * お気に入り解除
     * 
     * @param productId 商品ID
     * @param token 認証トークン
     * @return お気に入り解除レスポンス
     */
    @DeleteMapping("/{productId}")
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> removeFavorite(
            @PathVariable Long productId,
            @RequestHeader("Authorization") String token) {
        
        log.info("お気に入り解除リクエスト: productId={}", productId);
        
        // トークンからユーザーIDを取得
        String jwtToken = token.replace("Bearer ", "");
        Long userId = jwtService.getUserIdFromToken(jwtToken);
        
        favoriteService.removeFavorite(userId, productId);
        
        Map<String, Object> data = new HashMap<>();
        data.put("message", "お気に入りを解除しました");
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
}
