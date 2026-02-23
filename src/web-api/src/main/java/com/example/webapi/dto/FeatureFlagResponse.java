package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;
import java.util.Map;

/**
 * 機能フラグレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class FeatureFlagResponse {
    
    /**
     * 機能フラグマップ
     */
    private Map<String, Boolean> featureFlags;
    
    /**
     * 機能フラグマップを作成
     * 
     * @param featureFlags 機能フラグマップ
     * @return 機能フラグレスポンスDTO
     */
    public static FeatureFlagResponse of(Map<String, Boolean> featureFlags) {
        return FeatureFlagResponse.builder()
                .featureFlags(featureFlags)
                .build();
    }
    
    /**
     * ユーザー機能フラグ詳細レスポンス（管理者用）
     */
    @Data
    @NoArgsConstructor
    @AllArgsConstructor
    @Builder
    public static class UserFlagDetail {
        private Long userId;
        private String flagKey;
        private Boolean isEnabled;
        private LocalDateTime updatedAt;
    }
}
