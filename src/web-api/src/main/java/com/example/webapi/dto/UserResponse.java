package com.example.webapi.dto;

import com.example.webapi.entity.User;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * ユーザーレスポンスDTO（管理者用）
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class UserResponse {
    
    /**
     * ユーザーID
     */
    private Long userId;
    
    /**
     * ユーザー名
     */
    private String userName;
    
    /**
     * ログインID
     */
    private String loginId;
    
    /**
     * 機能フラグマップ
     */
    private Map<String, Boolean> featureFlags;
    
    /**
     * エンティティからDTOに変換
     * 
     * @param user ユーザーエンティティ
     * @param featureFlags 機能フラグマップ
     * @return ユーザーレスポンスDTO
     */
    public static UserResponse fromEntity(User user, Map<String, Boolean> featureFlags) {
        return UserResponse.builder()
                .userId(user.getUserId())
                .userName(user.getUserName())
                .loginId(user.getLoginId())
                .featureFlags(featureFlags)
                .build();
    }
}
