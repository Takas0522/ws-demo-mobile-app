package com.example.mobilebff.config;

import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.reactive.function.client.WebClientResponseException;

import com.example.mobilebff.dto.ErrorResponse;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * グローバル例外ハンドラー
 * 
 * Mobile BFFの共通エラーハンドリング
 */
@RestControllerAdvice
@RequiredArgsConstructor
@Slf4j
public class GlobalExceptionHandler {
    
    private final ObjectMapper objectMapper;
    
    /**
     * WebClientResponseException（Web APIからのエラー）
     * 
     * @param e 例外
     * @return エラーレスポンス
     */
    @ExceptionHandler(WebClientResponseException.class)
    public ResponseEntity<ErrorResponse> handleWebClientResponseException(WebClientResponseException e) {
        log.error("WebClient error: status={}, body={}", e.getStatusCode(), e.getResponseBodyAsString());
        
        ErrorResponse errorResponse;
        
        try {
            // Web APIからのエラーレスポンスをパース
            String responseBody = e.getResponseBodyAsString();
            JsonNode jsonNode = objectMapper.readTree(responseBody);
            
            if (jsonNode.has("error")) {
                JsonNode errorNode = jsonNode.get("error");
                errorResponse = new ErrorResponse(
                    errorNode.has("code") ? errorNode.get("code").asText() : "UNKNOWN_ERROR",
                    errorNode.has("message") ? errorNode.get("message").asText() : e.getMessage(),
                    errorNode.has("details") ? errorNode.get("details").asText() : null,
                    ZonedDateTime.now().format(DateTimeFormatter.ISO_INSTANT)
                );
            } else {
                // エラー形式が想定外の場合
                errorResponse = createDefaultErrorResponse("API_ERROR", e.getMessage());
            }
        } catch (Exception parseException) {
            // パースエラーの場合はデフォルトのエラーレスポンス
            log.error("Failed to parse error response", parseException);
            errorResponse = createDefaultErrorResponse("API_ERROR", e.getMessage());
        }
        
        return ResponseEntity
            .status(e.getStatusCode())
            .body(errorResponse);
    }
    
    /**
     * その他の例外
     * 
     * @param e 例外
     * @return エラーレスポンス
     */
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorResponse> handleException(Exception e) {
        log.error("Unexpected error", e);
        
        ErrorResponse errorResponse = createDefaultErrorResponse(
            "INTERNAL_SERVER_ERROR",
            "An unexpected error occurred"
        );
        
        return ResponseEntity
            .status(HttpStatus.INTERNAL_SERVER_ERROR)
            .body(errorResponse);
    }
    
    /**
     * デフォルトのエラーレスポンスを作成
     * 
     * @param code エラーコード
     * @param message エラーメッセージ
     * @return エラーレスポンス
     */
    private ErrorResponse createDefaultErrorResponse(String code, String message) {
        return new ErrorResponse(
            code,
            message,
            null,
            ZonedDateTime.now().format(DateTimeFormatter.ISO_INSTANT)
        );
    }
}
