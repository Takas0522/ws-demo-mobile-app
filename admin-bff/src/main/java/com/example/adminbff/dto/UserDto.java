package com.example.adminbff.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * ユーザーDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class UserDto {
    
    /**
     * ユーザーID
     */
    private Long userId;
    
    /**
     * ログインID
     */
    private String loginId;
    
    /**
     * ユーザー名
     */
    private String userName;
    
    /**
     * メールアドレス
     */
    private String email;
    
    /**
     * ロール
     */
    private String role;
    
    /**
     * ステータス
     */
    private String status;
    
    /**
     * 作成日時
     */
    private String createdAt;
}
