package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;

/**
 * API共通レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ApiResponse<T> {
    
    /**
     * データ
     */
    private T data;
    
    /**
     * タイムスタンプ
     */
    private LocalDateTime timestamp;
    
    /**
     * 成功レスポンスを作成
     * 
     * @param data データ
     * @param <T> データ型
     * @return APIレスポンス
     */
    public static <T> ApiResponse<T> success(T data) {
        return new ApiResponse<>(data, LocalDateTime.now());
    }
}
