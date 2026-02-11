package com.example.webapi.config;

import com.example.webapi.dto.ErrorResponse;
import com.example.webapi.exception.AuthenticationException;
import com.example.webapi.exception.ForbiddenException;
import com.example.webapi.exception.ResourceNotFoundException;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.AccessDeniedException;
import org.springframework.validation.FieldError;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;

import java.util.stream.Collectors;

/**
 * グローバル例外ハンドラー
 * 
 * アプリケーション全体の例外をキャッチし、適切なエラーレスポンスを返す
 */
@RestControllerAdvice
@Slf4j
public class GlobalExceptionHandler {
    
    /**
     * 認証例外ハンドラー
     */
    @ExceptionHandler(AuthenticationException.class)
    public ResponseEntity<ErrorResponse> handleAuthenticationException(AuthenticationException e) {
        log.warn("認証エラー: code={}, message={}", e.getErrorCode(), e.getMessage());
        
        ErrorResponse errorResponse = ErrorResponse.of(
                e.getErrorCode(),
                e.getMessage()
        );
        
        return ResponseEntity
                .status(HttpStatus.UNAUTHORIZED)
                .body(errorResponse);
    }
    
    /**
     * リソース未発見例外ハンドラー
     */
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorResponse> handleResourceNotFoundException(ResourceNotFoundException e) {
        log.warn("リソース未発見: code={}, message={}", e.getErrorCode(), e.getMessage());
        
        ErrorResponse errorResponse = ErrorResponse.of(
                e.getErrorCode(),
                e.getMessage()
        );
        
        return ResponseEntity
                .status(HttpStatus.NOT_FOUND)
                .body(errorResponse);
    }
    
    /**
     * 権限なし例外ハンドラー
     */
    @ExceptionHandler({ForbiddenException.class, AccessDeniedException.class})
    public ResponseEntity<ErrorResponse> handleForbiddenException(Exception e) {
        log.warn("権限なしエラー: message={}", e.getMessage());
        
        String code = "FORBIDDEN_001";
        String message = "このリソースへのアクセス権限がありません";
        
        if (e instanceof ForbiddenException) {
            code = ((ForbiddenException) e).getErrorCode();
            message = e.getMessage();
        }
        
        ErrorResponse errorResponse = ErrorResponse.of(code, message);
        
        return ResponseEntity
                .status(HttpStatus.FORBIDDEN)
                .body(errorResponse);
    }
    
    /**
     * バリデーションエラーハンドラー
     */
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<ErrorResponse> handleValidationException(MethodArgumentNotValidException e) {
        log.warn("バリデーションエラー: {}", e.getMessage());
        
        String details = e.getBindingResult().getFieldErrors().stream()
                .map(FieldError::getDefaultMessage)
                .collect(Collectors.joining(", "));
        
        ErrorResponse errorResponse = ErrorResponse.of(
                "VALIDATION_001",
                "入力値が不正です",
                details
        );
        
        return ResponseEntity
                .status(HttpStatus.BAD_REQUEST)
                .body(errorResponse);
    }
    
    /**
     * その他の例外ハンドラー
     */
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorResponse> handleException(Exception e) {
        log.error("予期しないエラー: {}", e.getMessage(), e);
        
        ErrorResponse errorResponse = ErrorResponse.of(
                "INTERNAL_ERROR_001",
                "サーバー内部エラーが発生しました"
        );
        
        return ResponseEntity
                .status(HttpStatus.INTERNAL_SERVER_ERROR)
                .body(errorResponse);
    }
}
