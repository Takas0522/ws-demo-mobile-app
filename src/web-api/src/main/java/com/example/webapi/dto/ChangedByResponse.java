package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 変更者レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class ChangedByResponse {

    private Long userId;
    private String userName;
}
