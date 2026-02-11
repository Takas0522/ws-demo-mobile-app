package com.example.webapi.dto;

import jakarta.validation.constraints.NotNull;
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
    @NotNull(message = "isEnabledは必須です")
    private Boolean isEnabled;
}
