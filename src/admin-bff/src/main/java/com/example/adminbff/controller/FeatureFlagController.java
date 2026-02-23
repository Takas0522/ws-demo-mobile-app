package com.example.adminbff.controller;

import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.adminbff.client.WebApiClient;
import com.example.adminbff.dto.ApiResponse;
import com.example.adminbff.dto.FeatureFlagDto;
import com.example.adminbff.dto.FeatureFlagUpdateRequest;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * 機能フラグ管理コントローラー
 * 
 * Admin BFFの機能フラグ管理API
 */
@RestController
@RequestMapping("/api/admin/users")
@RequiredArgsConstructor
@Slf4j
public class FeatureFlagController {
    
    private final WebApiClient webApiClient;
    
    /**
     * ユーザー別機能フラグ取得
     * 
     * @param userId ユーザーID
     * @param authorization 認証トークン
     * @return 機能フラグ一覧
     */
    @GetMapping("/{userId}/feature-flags")
    public ResponseEntity<ApiResponse<List<FeatureFlagDto>>> getUserFeatureFlags(
            @PathVariable Long userId,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get user feature flags request (admin): userId={}", userId);
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<List<FeatureFlagDto>> response = webApiClient.get(
            "/api/v1/admin/users/" + userId + "/feature-flags",
            (Class<ApiResponse<List<FeatureFlagDto>>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * ユーザー別機能フラグ更新
     * 
     * @param userId ユーザーID
     * @param flagKey フラグキー
     * @param request 更新リクエスト
     * @param authorization 認証トークン
     * @return 更新後の機能フラグ
     */
    @PutMapping("/{userId}/feature-flags/{flagKey}")
    public ResponseEntity<ApiResponse<FeatureFlagDto>> updateUserFeatureFlag(
            @PathVariable Long userId,
            @PathVariable String flagKey,
            @RequestBody FeatureFlagUpdateRequest request,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Update user feature flag request (admin): userId={}, flagKey={}, enabled={}", 
            userId, flagKey, request.getEnabled());
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<FeatureFlagDto> response = webApiClient.put(
            "/api/v1/admin/users/" + userId + "/feature-flags/" + flagKey,
            request,
            (Class<ApiResponse<FeatureFlagDto>>) (Class<?>) ApiResponse.class,
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
