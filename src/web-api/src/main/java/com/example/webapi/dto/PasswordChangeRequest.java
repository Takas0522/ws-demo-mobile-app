package com.example.webapi.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * パスワード変更リクエストDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PasswordChangeRequest {
    
    @NotBlank(message = "現在のパスワードは必須です")
    private String currentPassword;
    
    @NotBlank(message = "新しいパスワードは必須です")
    @Size(min = 8, max = 50, message = "新しいパスワードは8〜50文字で入力してください")
    private String newPassword;
}
