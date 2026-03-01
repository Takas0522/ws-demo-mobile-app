package com.example.adminbff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * API共通レスポンス
 * 
 * @param <T> データ型
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ApiResponse<T> {
    
    /**
     * レスポンスデータ
     */
    private T data;
    
    /**
     * タイムスタンプ
     */
    private String timestamp;
}
