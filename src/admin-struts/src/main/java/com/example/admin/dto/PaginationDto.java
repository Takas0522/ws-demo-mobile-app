package com.example.admin.dto;

import java.io.Serializable;

/**
 * ページネーション情報 DTO。
 * API レスポンスでページネーション情報を返却する際に使用する。
 */
public class PaginationDto implements Serializable {

    private static final long serialVersionUID = 1L;

    private int currentPage;
    private int totalPages;
    private int totalItems;
    private int itemsPerPage;

    public PaginationDto() {
    }

    public PaginationDto(int currentPage, int totalPages, int totalItems, int itemsPerPage) {
        this.currentPage = currentPage;
        this.totalPages = totalPages;
        this.totalItems = totalItems;
        this.itemsPerPage = itemsPerPage;
    }

    public int getCurrentPage() {
        return currentPage;
    }

    public void setCurrentPage(int currentPage) {
        this.currentPage = currentPage;
    }

    public int getTotalPages() {
        return totalPages;
    }

    public void setTotalPages(int totalPages) {
        this.totalPages = totalPages;
    }

    public int getTotalItems() {
        return totalItems;
    }

    public void setTotalItems(int totalItems) {
        this.totalItems = totalItems;
    }

    public int getItemsPerPage() {
        return itemsPerPage;
    }

    public void setItemsPerPage(int itemsPerPage) {
        this.itemsPerPage = itemsPerPage;
    }
}
