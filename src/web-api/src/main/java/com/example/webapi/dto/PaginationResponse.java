package com.example.webapi.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * ページネーション共通レスポンスDTO
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class PaginationResponse {
    
    /**
     * 現在のページ番号
     */
    private Integer currentPage;
    
    /**
     * 総ページ数
     */
    private Integer totalPages;
    
    /**
     * 総アイテム数
     */
    private Long totalItems;
    
    /**
     * 1ページあたりのアイテム数
     */
    private Integer itemsPerPage;
}
