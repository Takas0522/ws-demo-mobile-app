package com.example.adminbff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * エラーレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ErrorResponse {
    
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
    
    /**
     * タイムスタンプ
     */
    private String timestamp;
}
