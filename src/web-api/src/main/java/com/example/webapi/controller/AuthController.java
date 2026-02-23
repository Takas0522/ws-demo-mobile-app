package com.example.webapi.controller;

import com.example.webapi.dto.*;
import com.example.webapi.service.AuthService;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

/**
 * 認証コントローラー
 * 
 * ログイン、ログアウト、トークンリフレッシュ、パスワード変更のエンドポイントを提供
 */
@RestController
@RequestMapping("/api/v1/auth")
@RequiredArgsConstructor
@Slf4j
public class AuthController {
    
    private final AuthService authService;
    
    /**
     * エンドユーザーログイン
     * 
     * POST /api/v1/auth/login
     * 
     * @param request ログインリクエスト
     * @return ログインレスポンス
     */
    @PostMapping("/login")
    public ResponseEntity<ApiResponse<LoginResponse>> login(@Valid @RequestBody LoginRequest request) {
        log.info("ログインAPI呼び出し: loginId={}", request.getLoginId());
        
        LoginResponse response = authService.login(request);
        
        return ResponseEntity.ok(ApiResponse.success(response));
    }
    
    /**
     * 管理者ログイン
     * 
     * POST /api/v1/auth/admin/login
     * 
     * @param request ログインリクエスト
     * @return ログインレスポンス
     */
    @PostMapping("/admin/login")
    public ResponseEntity<ApiResponse<LoginResponse>> adminLogin(@Valid @RequestBody LoginRequest request) {
        log.info("管理者ログインAPI呼び出し: loginId={}", request.getLoginId());
        
        LoginResponse response = authService.adminLogin(request);
        
        return ResponseEntity.ok(ApiResponse.success(response));
    }
    
    /**
     * ログアウト
     * 
     * POST /api/v1/auth/logout
     * 
     * JWTはステートレスのため、サーバー側では何もしない。
     * クライアント側でトークンを削除する必要がある。
     * 
     * @return 成功メッセージ
     */
    @PostMapping("/logout")
    public ResponseEntity<ApiResponse<String>> logout() {
        Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
        String userId = authentication != null ? authentication.getName() : "unknown";
        
        log.info("ログアウトAPI呼び出し: userId={}", userId);
        
        return ResponseEntity.ok(ApiResponse.success("ログアウトしました"));
    }
    
    /**
     * トークンリフレッシュ
     * 
     * POST /api/v1/auth/refresh
     * 
     * @param request リフレッシュリクエスト
     * @return 新しいトークン
     */
    @PostMapping("/refresh")
    public ResponseEntity<ApiResponse<LoginResponse>> refreshToken(@Valid @RequestBody RefreshTokenRequest request) {
        log.info("トークンリフレッシュAPI呼び出し");
        
        LoginResponse response = authService.refreshToken(request.getToken());
        
        return ResponseEntity.ok(ApiResponse.success(response));
    }
    
    /**
     * パスワード変更
     * 
     * PUT /api/v1/auth/password
     * 
     * @param request パスワード変更リクエスト
     * @return 成功メッセージ
     */
    @PutMapping("/password")
    public ResponseEntity<ApiResponse<String>> changePassword(@Valid @RequestBody PasswordChangeRequest request) {
        Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
        Long userId = Long.parseLong(authentication.getName());
        
        log.info("パスワード変更API呼び出し: userId={}", userId);
        
        authService.changePassword(userId, request);
        
        return ResponseEntity.ok(ApiResponse.success("パスワードを変更しました"));
    }
}
