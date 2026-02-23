package com.example.adminbff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 機能フラグ更新リクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class FeatureFlagUpdateRequest {
    
    /**
     * 有効/無効
     */
    private Boolean enabled;
}
