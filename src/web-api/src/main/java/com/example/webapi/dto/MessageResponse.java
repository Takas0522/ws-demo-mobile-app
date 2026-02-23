package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * メッセージレスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class MessageResponse {
    
    /**
     * メッセージ
     */
    private String message;
    
    /**
     * メッセージレスポンスを作成
     * 
     * @param message メッセージ
     * @return メッセージレスポンス
     */
    public static MessageResponse of(String message) {
        return new MessageResponse(message);
    }
}
