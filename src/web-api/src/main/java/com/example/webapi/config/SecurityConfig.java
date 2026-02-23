package com.example.webapi.config;

import com.example.webapi.security.JwtAuthenticationEntryPoint;
import com.example.webapi.security.JwtAuthenticationFilter;
import lombok.RequiredArgsConstructor;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.method.configuration.EnableMethodSecurity;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.authentication.UsernamePasswordAuthenticationFilter;
import org.springframework.web.cors.CorsConfiguration;
import org.springframework.web.cors.CorsConfigurationSource;
import org.springframework.web.cors.UrlBasedCorsConfigurationSource;

import java.util.Arrays;
import java.util.List;

/**
 * Spring Security設定
 * 
 * JWT認証、CORS、エンドポイント認可を設定
 */
@Configuration
@EnableWebSecurity
@EnableMethodSecurity(prePostEnabled = true)
@RequiredArgsConstructor
public class SecurityConfig {
    
    private final JwtAuthenticationFilter jwtAuthenticationFilter;
    private final JwtAuthenticationEntryPoint jwtAuthenticationEntryPoint;
    
    /**
     * セキュリティフィルターチェーン設定
     */
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
                // CSRF無効化（JWT使用のため）
                .csrf(csrf -> csrf.disable())
                
                // CORS設定
                .cors(cors -> cors.configurationSource(corsConfigurationSource()))
                
                // セッション管理（ステートレス）
                .sessionManagement(session -> 
                        session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))
                
                // エンドポイント認可設定
                .authorizeHttpRequests(authorize -> authorize
                        // 認証不要エンドポイント
                        .requestMatchers("/api/v1/auth/login").permitAll()
                        .requestMatchers("/api/v1/auth/admin/login").permitAll()
                        .requestMatchers("/api/v1/health").permitAll()
                        .requestMatchers("/actuator/**").permitAll()
                        // その他のエンドポイントは認証必要
                        .anyRequest().authenticated()
                )
                
                // 認証エラーハンドリング
                .exceptionHandling(exception -> 
                        exception.authenticationEntryPoint(jwtAuthenticationEntryPoint))
                
                // JWT認証フィルター追加
                .addFilterBefore(jwtAuthenticationFilter, UsernamePasswordAuthenticationFilter.class);
        
        return http.build();
    }
    
    /**
     * パスワードエンコーダー（BCrypt）
     */
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder(10);
    }
    
    /**
     * CORS設定
     */
    @Bean
    public CorsConfigurationSource corsConfigurationSource() {
        CorsConfiguration configuration = new CorsConfiguration();
        
        // 許可するオリジン（開発環境）
        configuration.setAllowedOrigins(Arrays.asList(
                "http://localhost:3000",    // React開発サーバー
                "http://localhost:3001",    // Vue開発サーバー
                "http://localhost:8081",    // Mobile BFF
                "http://localhost:8082"     // Admin BFF
        ));
        
        // 許可するHTTPメソッド
        configuration.setAllowedMethods(Arrays.asList("GET", "POST", "PUT", "DELETE", "OPTIONS"));
        
        // 許可するヘッダー
        configuration.setAllowedHeaders(Arrays.asList("*"));
        
        // 認証情報を含むリクエストを許可
        configuration.setAllowCredentials(true);
        
        // プリフライトリクエストの有効期間
        configuration.setMaxAge(3600L);
        
        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/**", configuration);
        
        return source;
    }
}
