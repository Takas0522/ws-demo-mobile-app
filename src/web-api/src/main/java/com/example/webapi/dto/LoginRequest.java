package com.example.webapi.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * ログインリクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class LoginRequest {
    
    @NotBlank(message = "ログインIDは必須です")
    @Size(min = 4, max = 20, message = "ログインIDは4〜20文字で入力してください")
    private String loginId;
    
    @NotBlank(message = "パスワードは必須です")
    @Size(min = 8, max = 50, message = "パスワードは8〜50文字で入力してください")
    private String password;
}
