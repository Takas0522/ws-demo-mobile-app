package com.example.admin.action.product;

import com.example.admin.entity.Product;
import com.example.admin.service.ProductService;
import com.opensymphony.xwork2.ActionSupport;

import java.util.List;
import java.util.Map;

/**
 * 商品価格履歴表示 Action。
 * 商品IDを受け取り、価格変更履歴をページネーション付きで表示する。
 */
public class ProductPriceHistoryAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private static final int DEFAULT_LIMIT = 20;

    private ProductService productService;

    // Input
    private Long productId;
    private Integer page;

    // Output
    private Product product;
    private List<Map<String, Object>> priceHistory;
    private int currentPage;
    private int totalPages;
    private int totalItems;

    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    @Override
    public String execute() {
        if (productId == null) {
            addActionError("商品IDが指定されていません");
            return ERROR;
        }

        product = productService.getProductById(productId);
        if (product == null) {
            addActionError("商品が見つかりません");
            return ERROR;
        }

        int pageNum = (page != null && page >= 1) ? page : 1;

        Map<String, Object> result = productService.getPriceHistoryWithPagination(
                productId, null, null, pageNum, DEFAULT_LIMIT);

        @SuppressWarnings("unchecked")
        List<Map<String, Object>> historyList = (List<Map<String, Object>>) result.get("priceHistory");
        this.priceHistory = historyList;

        Object paginationObj = result.get("pagination");
        if (paginationObj instanceof com.example.admin.dto.PaginationDto) {
            com.example.admin.dto.PaginationDto pagination =
                    (com.example.admin.dto.PaginationDto) paginationObj;
            this.currentPage = pagination.getCurrentPage();
            this.totalPages = pagination.getTotalPages();
            this.totalItems = pagination.getTotalItems();
        }

        return SUCCESS;
    }

    // Getter / Setter

    public Long getProductId() {
        return productId;
    }

    public void setProductId(Long productId) {
        this.productId = productId;
    }

    public Integer getPage() {
        return page;
    }

    public void setPage(Integer page) {
        this.page = page;
    }

    public Product getProduct() {
        return product;
    }

    public List<Map<String, Object>> getPriceHistory() {
        return priceHistory;
    }

    public int getCurrentPage() {
        return currentPage;
    }

    public int getTotalPages() {
        return totalPages;
    }

    public int getTotalItems() {
        return totalItems;
    }
}
