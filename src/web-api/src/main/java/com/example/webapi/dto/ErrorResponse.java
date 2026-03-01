package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;

/**
 * エラーレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class ErrorResponse {
    
    /**
     * エラー情報
     */
    private ErrorDetail error;
    
    /**
     * タイムスタンプ
     */
    private LocalDateTime timestamp;
    
    /**
     * エラー詳細
     */
    @Data
    @NoArgsConstructor
    @AllArgsConstructor
    @Builder
    public static class ErrorDetail {
        /**
         * エラーコード
         */
        private String code;
        
        /**
         * エラーメッセージ
         */
        private String message;
        
        /**
         * 詳細情報（オプション）
         */
        private String details;
    }
    
    /**
     * エラーレスポンスを作成
     * 
     * @param code エラーコード
     * @param message エラーメッセージ
     * @return エラーレスポンス
     */
    public static ErrorResponse of(String code, String message) {
        return ErrorResponse.builder()
                .error(ErrorDetail.builder()
                        .code(code)
                        .message(message)
                        .build())
                .timestamp(LocalDateTime.now())
                .build();
    }
    
    /**
     * エラーレスポンスを作成（詳細付き）
     * 
     * @param code エラーコード
     * @param message エラーメッセージ
     * @param details 詳細情報
     * @return エラーレスポンス
     */
    public static ErrorResponse of(String code, String message, String details) {
        return ErrorResponse.builder()
                .error(ErrorDetail.builder()
                        .code(code)
                        .message(message)
                        .details(details)
                        .build())
                .timestamp(LocalDateTime.now())
                .build();
    }
}
