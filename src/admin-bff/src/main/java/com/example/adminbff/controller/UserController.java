package com.example.adminbff.controller;

import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.example.adminbff.client.WebApiClient;
import com.example.adminbff.dto.ApiResponse;
import com.example.adminbff.dto.UserDto;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * ユーザー管理コントローラー
 * 
 * Admin BFFのユーザー管理API
 */
@RestController
@RequestMapping("/api/admin/users")
@RequiredArgsConstructor
@Slf4j
public class UserController {
    
    private final WebApiClient webApiClient;
    
    /**
     * ユーザー一覧取得
     * 
     * @param authorization 認証トークン
     * @return ユーザー一覧
     */
    @GetMapping
    public ResponseEntity<ApiResponse<List<UserDto>>> getUsers(
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get users request (admin)");
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<List<UserDto>> response = webApiClient.get(
            "/api/v1/admin/users",
            (Class<ApiResponse<List<UserDto>>>) (Class<?>) ApiResponse.class,
            token
        );
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * ユーザー詳細取得
     * 
     * @param id ユーザーID
     * @param authorization 認証トークン
     * @return ユーザー詳細
     */
    @GetMapping("/{id}")
    public ResponseEntity<ApiResponse<UserDto>> getUser(
            @PathVariable Long id,
            @RequestHeader(value = "Authorization", required = false) String authorization) {
        log.info("Get user detail request (admin): userId={}", id);
        
        String token = extractToken(authorization);
        
        // Web APIにリクエストを転送
        ApiResponse<UserDto> response = webApiClient.get(
            "/api/v1/admin/users/" + id,
            (Class<ApiResponse<UserDto>>) (Class<?>) ApiResponse.class,
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
