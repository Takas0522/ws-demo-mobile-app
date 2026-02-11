package com.example.webapi.controller;

import com.example.webapi.dto.*;
import com.example.webapi.entity.User;
import com.example.webapi.entity.UserFeatureFlag;
import com.example.webapi.service.JwtService;
import com.example.webapi.service.FeatureFlagService;
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
import java.util.stream.Collectors;

/**
 * 機能フラグコントローラー
 * 
 * 機能フラグ関連のAPIエンドポイントを提供します
 */
@Slf4j
@RestController
@RequestMapping("/api/v1")
@RequiredArgsConstructor
public class FeatureFlagController {
    
    private final FeatureFlagService featureFlagService;
    private final JwtService jwtService;
    
    /**
     * ユーザー機能フラグ取得
     * 
     * @param token 認証トークン
     * @return 機能フラグレスポンス
     */
    @GetMapping("/feature-flags")
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> getUserFeatureFlags(
            @RequestHeader("Authorization") String token) {
        
        log.info("ユーザー機能フラグ取得リクエスト");
        
        // トークンからユーザーIDを取得
        String jwtToken = token.replace("Bearer ", "");
        Long userId = jwtService.getUserIdFromToken(jwtToken);
        
        Map<String, Boolean> featureFlags = featureFlagService.getUserFeatureFlags(userId);
        
        Map<String, Object> data = new HashMap<>();
        data.put("featureFlags", featureFlags);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
    
    /**
     * ユーザー一覧取得（機能フラグ管理）
     * 
     * @param page ページ番号（デフォルト: 1）
     * @param limit 1ページあたりの件数（デフォルト: 50）
     * @return ユーザー一覧レスポンス
     */
    @GetMapping("/admin/users")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> getAllUsers(
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "50") int limit) {
        
        log.info("全ユーザー取得リクエスト（管理者）: page={}, limit={}", page, limit);
        
        // ページは1始まりなので、0始まりに変換
        Page<User> userPage = featureFlagService.getAllUsersWithFlags(page - 1, limit);
        
        // 各ユーザーの機能フラグを取得してマッピング
        List<UserResponse> users = userPage.getContent().stream()
                .map(user -> {
                    Map<String, Boolean> featureFlags = featureFlagService.getUserFeatureFlags(user.getUserId());
                    return UserResponse.fromEntity(user, featureFlags);
                })
                .collect(Collectors.toList());
        
        PaginationResponse pagination = PaginationResponse.builder()
                .currentPage(page)
                .totalPages(userPage.getTotalPages())
                .totalItems(userPage.getTotalElements())
                .itemsPerPage(limit)
                .build();
        
        Map<String, Object> data = new HashMap<>();
        data.put("users", users);
        data.put("pagination", pagination);
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
    
    /**
     * 機能フラグ変更
     * 
     * @param userId ユーザーID
     * @param flagKey フラグキー
     * @param request 機能フラグ更新リクエスト
     * @return 機能フラグ更新レスポンス
     */
    @PutMapping("/admin/users/{userId}/feature-flags/{flagKey}")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<ApiResponse<Map<String, Object>>> updateFeatureFlag(
            @PathVariable Long userId,
            @PathVariable String flagKey,
            @Valid @RequestBody FeatureFlagUpdateRequest request) {
        
        log.info("機能フラグ更新リクエスト（管理者）: userId={}, flagKey={}, isEnabled={}", 
                userId, flagKey, request.getIsEnabled());
        
        UserFeatureFlag userFlag = featureFlagService.updateUserFeatureFlag(userId, flagKey, request.getIsEnabled());
        
        FeatureFlagResponse.UserFlagDetail detail = FeatureFlagResponse.UserFlagDetail.builder()
                .userId(userFlag.getUser().getUserId())
                .flagKey(userFlag.getFeatureFlag().getFlagKey())
                .isEnabled(userFlag.getIsEnabled())
                .updatedAt(userFlag.getUpdatedAt())
                .build();
        
        Map<String, Object> data = new HashMap<>();
        data.put("userId", detail.getUserId());
        data.put("flagKey", detail.getFlagKey());
        data.put("isEnabled", detail.getIsEnabled());
        data.put("updatedAt", detail.getUpdatedAt());
        
        return ResponseEntity.ok(ApiResponse.success(data));
    }
}
