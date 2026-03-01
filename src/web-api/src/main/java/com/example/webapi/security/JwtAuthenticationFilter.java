package com.example.webapi.security;

import com.example.webapi.service.JwtService;
import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.User;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.web.authentication.WebAuthenticationDetailsSource;
import org.springframework.stereotype.Component;
import org.springframework.util.StringUtils;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * JWT認証フィルター
 * 
 * リクエストのAuthorizationヘッダーからJWTトークンを抽出し、検証する
 */
@Component
@RequiredArgsConstructor
@Slf4j
public class JwtAuthenticationFilter extends OncePerRequestFilter {
    
    private final JwtService jwtService;
    
    @Override
    protected void doFilterInternal(
            HttpServletRequest request,
            HttpServletResponse response,
            FilterChain filterChain) throws ServletException, IOException {
        
        try {
            // Authorizationヘッダーからトークン取得
            String token = resolveToken(request);
            
            if (token != null && jwtService.validateToken(token)) {
                // トークンからユーザー情報を取得
                Long userId = jwtService.getUserIdFromToken(token);
                String loginId = jwtService.getLoginIdFromToken(token);
                String userType = jwtService.getUserTypeFromToken(token);
                
                // 権限設定
                List<GrantedAuthority> authorities = new ArrayList<>();
                authorities.add(new SimpleGrantedAuthority("ROLE_" + userType.toUpperCase()));
                
                // UserDetails作成
                UserDetails userDetails = User.builder()
                        .username(userId.toString())
                        .password("")
                        .authorities(authorities)
                        .build();
                
                // Authentication作成
                UsernamePasswordAuthenticationToken authentication =
                        new UsernamePasswordAuthenticationToken(
                                userDetails,
                                null,
                                authorities);
                authentication.setDetails(new WebAuthenticationDetailsSource().buildDetails(request));
                
                // SecurityContextに設定
                SecurityContextHolder.getContext().setAuthentication(authentication);
                
                log.debug("JWT認証成功: userId={}, loginId={}, userType={}", userId, loginId, userType);
            }
        } catch (Exception e) {
            log.error("JWT認証エラー: {}", e.getMessage(), e);
        }
        
        filterChain.doFilter(request, response);
    }
    
    /**
     * リクエストからトークンを抽出
     * 
     * @param request HTTPリクエスト
     * @return トークン（存在しない場合null）
     */
    private String resolveToken(HttpServletRequest request) {
        String bearerToken = request.getHeader("Authorization");
        if (StringUtils.hasText(bearerToken) && bearerToken.startsWith("Bearer ")) {
            return bearerToken.substring(7);
        }
        return null;
    }
}
