package com.example.webapi.security;

import com.example.webapi.dto.ErrorResponse;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import lombok.extern.slf4j.Slf4j;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.web.AuthenticationEntryPoint;
import org.springframework.stereotype.Component;

import java.io.IOException;

/**
 * JWT認証エラーハンドリング
 * 
 * 認証が必要なエンドポイントに認証なしでアクセスした場合のエラーレスポンスを返す
 */
@Component
@Slf4j
public class JwtAuthenticationEntryPoint implements AuthenticationEntryPoint {
    
    private final ObjectMapper objectMapper = new ObjectMapper();
    
    @Override
    public void commence(
            HttpServletRequest request,
            HttpServletResponse response,
            AuthenticationException authException) throws IOException, ServletException {
        
        log.warn("認証なしアクセス: uri={}, message={}", request.getRequestURI(), authException.getMessage());
        
        // エラーレスポンス作成
        ErrorResponse errorResponse = ErrorResponse.of(
                "AUTH_002",
                "認証が必要です"
        );
        
        // JSONレスポンス設定
        response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
        response.setContentType("application/json;charset=UTF-8");
        response.getWriter().write(objectMapper.writeValueAsString(errorResponse));
    }
}
