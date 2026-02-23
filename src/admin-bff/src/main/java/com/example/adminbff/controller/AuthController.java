package com.example.adminbff.controller;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.adminbff.client.WebApiClient;
import com.example.adminbff.dto.ApiResponse;
import com.example.adminbff.dto.LoginRequest;
import com.example.adminbff.dto.LoginResponse;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * 認証コントローラー
 * 
 * Admin BFFの認証API
 */
@RestController
@RequestMapping("/api/admin/auth")
@RequiredArgsConstructor
@Slf4j
public class AuthController {
    
    private final WebApiClient webApiClient;
    
    /**
     * 管理者ログイン
     * 
     * @param request ログインリクエスト
     * @return ログインレスポンス
     */
    @PostMapping("/login")
    public ResponseEntity<ApiResponse<LoginResponse>> login(@RequestBody LoginRequest request) {
        log.info("Admin login request: loginId={}", request.getLoginId());
        
        // Web APIにリクエストを転送
        ApiResponse<LoginResponse> response = webApiClient.post(
            "/api/v1/auth/login",
            request,
            (Class<ApiResponse<LoginResponse>>) (Class<?>) ApiResponse.class,
            null
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * ログアウト
     * 
     * @param authorization 認証トークン
     * @return レスポンス
     */
    @PostMapping("/logout")
    public ResponseEntity<ApiResponse<Void>> logout(
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Admin logout request");
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<Void> response = webApiClient.post(
            "/api/v1/auth/logout",
            null,
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
