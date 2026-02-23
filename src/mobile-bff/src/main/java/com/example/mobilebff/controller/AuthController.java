package com.example.mobilebff.controller;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.mobilebff.client.WebApiClient;
import com.example.mobilebff.dto.ApiResponse;
import com.example.mobilebff.dto.LoginRequest;
import com.example.mobilebff.dto.LoginResponse;
import com.example.mobilebff.dto.RefreshTokenRequest;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * 認証コントローラー
 * 
 * Mobile BFFの認証API
 */
@RestController
@RequestMapping("/api/mobile/auth")
@RequiredArgsConstructor
@Slf4j
public class AuthController {
    
    private final WebApiClient webApiClient;
    
    /**
     * ログイン
     * 
     * @param request ログインリクエスト
     * @return ログインレスポンス
     */
    @PostMapping("/login")
    public ResponseEntity<ApiResponse<LoginResponse>> login(@RequestBody LoginRequest request) {
        log.info("Login request: loginId={}", request.getLoginId());
        
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
        log.info("Logout request");
        
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
     * トークンリフレッシュ
     * 
     * @param request リフレッシュトークンリクエスト
     * @return ログインレスポンス
     */
    @PostMapping("/refresh")
    public ResponseEntity<ApiResponse<LoginResponse>> refresh(@RequestBody RefreshTokenRequest request) {
        log.info("Token refresh request");
        
        // Web APIにリクエストを転送
        ApiResponse<LoginResponse> response = webApiClient.post(
            "/api/v1/auth/refresh",
            request,
            (Class<ApiResponse<LoginResponse>>) (Class<?>) ApiResponse.class,
            null
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
