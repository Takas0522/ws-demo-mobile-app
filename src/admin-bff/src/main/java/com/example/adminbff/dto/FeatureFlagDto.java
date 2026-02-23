package com.example.adminbff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 機能フラグDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class FeatureFlagDto {
    
    /**
     * フラグキー
     */
    private String flagKey;
    
    /**
     * フラグ名
     */
    private String flagName;
    
    /**
     * 説明
     */
    private String description;
    
    /**
     * デフォルト値
     */
    private Boolean defaultValue;
    
    /**
     * ユーザー固有の値
     */
    private Boolean userValue;
    
    /**
     * 有効なフラグ値
     */
    private Boolean effectiveValue;
}
