package com.example.webapi.service;

import com.example.webapi.dto.LoginRequest;
import com.example.webapi.dto.LoginResponse;
import com.example.webapi.dto.PasswordChangeRequest;
import com.example.webapi.entity.User;
import com.example.webapi.exception.AuthenticationException;
import com.example.webapi.exception.ResourceNotFoundException;
import com.example.webapi.repository.UserRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

/**
 * 認証サービス
 * 
 * ログイン、ログアウト、パスワード変更などの認証機能を提供
 */
@Service
@RequiredArgsConstructor
@Slf4j
public class AuthService {
    
    private final UserRepository userRepository;
    private final JwtService jwtService;
    private final PasswordEncoder passwordEncoder;
    
    @Value("${jwt.expiration-hours:24}")
    private int expirationHours;
    
    /**
     * ユーザーログイン
     * 
     * @param request ログインリクエスト
     * @return ログインレスポンス
     * @throws AuthenticationException 認証失敗時
     */
    @Transactional(readOnly = true)
    public LoginResponse login(LoginRequest request) {
        log.info("ログイン試行: loginId={}", request.getLoginId());
        
        // ユーザー取得
        User user = userRepository.findByLoginId(request.getLoginId())
                .orElseThrow(() -> {
                    log.warn("ログイン失敗 - ユーザーが見つかりません: loginId={}", request.getLoginId());
                    return new AuthenticationException("ログインIDまたはパスワードが正しくありません");
                });
        
        // パスワード検証
        log.debug("パスワード検証: plain={}, hash={}", request.getPassword(), user.getPasswordHash());
        boolean matches = passwordEncoder.matches(request.getPassword(), user.getPasswordHash());
        log.debug("パスワード一致: {}", matches);
        if (!matches) {
            log.warn("ログイン失敗 - パスワード不一致: loginId={}", request.getLoginId());
            throw new AuthenticationException("ログインIDまたはパスワードが正しくありません");
        }
        
        // JWTトークン生成
        String token = jwtService.generateToken(user);
        
        log.info("ログイン成功: userId={}, loginId={}, userType={}", 
                user.getUserId(), user.getLoginId(), user.getUserType());
        
        // レスポンス作成
        return LoginResponse.builder()
                .token(token)
                .tokenType("Bearer")
                .expiresIn((long) (expirationHours * 3600)) // 秒に変換
                .user(LoginResponse.UserInfo.builder()
                        .userId(user.getUserId())
                        .userName(user.getUserName())
                        .loginId(user.getLoginId())
                        .userType(user.getUserType().name())
                        .build())
                .build();
    }
    
    /**
     * 管理者ログイン
     * 
     * @param request ログインリクエスト
     * @return ログインレスポンス
     * @throws AuthenticationException 認証失敗時
     */
    @Transactional(readOnly = true)
    public LoginResponse adminLogin(LoginRequest request) {
        log.info("管理者ログイン試行: loginId={}", request.getLoginId());
        
        // ユーザー取得
        User user = userRepository.findByLoginId(request.getLoginId())
                .orElseThrow(() -> {
                    log.warn("管理者ログイン失敗 - ユーザーが見つかりません: loginId={}", request.getLoginId());
                    return new AuthenticationException("ログインIDまたはパスワードが正しくありません");
                });
        
        // 管理者チェック
        if (user.getUserType() != User.UserType.admin) {
            log.warn("管理者ログイン失敗 - 管理者権限なし: loginId={}, userType={}", 
                    request.getLoginId(), user.getUserType());
            throw new AuthenticationException("管理者権限がありません", "AUTH_003");
        }
        
        // パスワード検証
        if (!passwordEncoder.matches(request.getPassword(), user.getPasswordHash())) {
            log.warn("管理者ログイン失敗 - パスワード不一致: loginId={}", request.getLoginId());
            throw new AuthenticationException("ログインIDまたはパスワードが正しくありません");
        }
        
        // JWTトークン生成
        String token = jwtService.generateToken(user);
        
        log.info("管理者ログイン成功: userId={}, loginId={}", user.getUserId(), user.getLoginId());
        
        // レスポンス作成（"admin"フィールドに変更）
        return LoginResponse.builder()
                .token(token)
                .tokenType("Bearer")
                .expiresIn((long) (expirationHours * 3600))
                .user(LoginResponse.UserInfo.builder()
                        .userId(user.getUserId())
                        .userName(user.getUserName())
                        .loginId(user.getLoginId())
                        .userType(user.getUserType().name())
                        .build())
                .build();
    }
    
    /**
     * トークンリフレッシュ
     * 
     * @param oldToken 現在のトークン
     * @return 新しいトークン
     * @throws AuthenticationException トークンが無効な場合
     */
    @Transactional(readOnly = true)
    public LoginResponse refreshToken(String oldToken) {
        log.info("トークンリフレッシュ試行");
        
        // トークン検証
        if (!jwtService.validateToken(oldToken)) {
            log.warn("トークンリフレッシュ失敗 - トークンが無効");
            throw new AuthenticationException("トークンが無効です", "AUTH_002");
        }
        
        // ユーザーID取得
        Long userId = jwtService.getUserIdFromToken(oldToken);
        
        // ユーザー取得
        User user = userRepository.findById(userId)
                .orElseThrow(() -> {
                    log.warn("トークンリフレッシュ失敗 - ユーザーが見つかりません: userId={}", userId);
                    return new AuthenticationException("ユーザーが見つかりません", "AUTH_005");
                });
        
        // 新しいトークン生成
        String newToken = jwtService.generateToken(user);
        
        log.info("トークンリフレッシュ成功: userId={}", userId);
        
        return LoginResponse.builder()
                .token(newToken)
                .tokenType("Bearer")
                .expiresIn((long) (expirationHours * 3600))
                .user(LoginResponse.UserInfo.builder()
                        .userId(user.getUserId())
                        .userName(user.getUserName())
                        .loginId(user.getLoginId())
                        .userType(user.getUserType().name())
                        .build())
                .build();
    }
    
    /**
     * パスワード変更
     * 
     * @param userId ユーザーID
     * @param request パスワード変更リクエスト
     * @throws AuthenticationException 認証失敗時
     */
    @Transactional
    public void changePassword(Long userId, PasswordChangeRequest request) {
        log.info("パスワード変更試行: userId={}", userId);
        
        // ユーザー取得
        User user = userRepository.findById(userId)
                .orElseThrow(() -> new ResourceNotFoundException("ユーザーが見つかりません"));
        
        // 現在のパスワード検証
        if (!passwordEncoder.matches(request.getCurrentPassword(), user.getPasswordHash())) {
            log.warn("パスワード変更失敗 - 現在のパスワードが間違っています: userId={}", userId);
            throw new AuthenticationException("現在のパスワードが間違っています", "AUTH_006");
        }
        
        // 新しいパスワードをハッシュ化
        String newPasswordHash = passwordEncoder.encode(request.getNewPassword());
        user.setPasswordHash(newPasswordHash);
        
        userRepository.save(user);
        
        log.info("パスワード変更成功: userId={}", userId);
    }
}
