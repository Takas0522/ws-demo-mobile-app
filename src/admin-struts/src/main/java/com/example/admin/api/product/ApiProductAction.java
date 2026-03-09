package com.example.admin.api.product;

import com.example.admin.entity.Product;
import com.example.admin.entity.ProductPriceHistory;
import com.example.admin.service.ProductService;
import com.example.admin.util.ApiResponseUtil;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.opensymphony.xwork2.ActionSupport;
import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 商品 API Action。
 * 商品一覧、検索、詳細、更新、価格履歴を提供する。
 */
public class ApiProductAction extends ActionSupport {

    private static final Logger logger = LoggerFactory.getLogger(ApiProductAction.class);

    // Service (Spring DI)
    private ProductService productService;

    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    // Input params (Struts2 パラメータインジェクション)
    private Long productId;
    private String keyword;

    public void setProductId(Long productId) {
        this.productId = productId;
    }

    public Long getProductId() {
        return productId;
    }

    public void setKeyword(String keyword) {
        this.keyword = keyword;
    }

    public String getKeyword() {
        return keyword;
    }

    /**
     * GET /api/v1/products — 商品一覧
     */
    public String list() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            List<Product> products = productService.getProducts();

            Map<String, Object> pagination = new HashMap<String, Object>();
            pagination.put("currentPage", 1);
            pagination.put("totalPages", 1);
            pagination.put("totalItems", products.size());
            pagination.put("itemsPerPage", products.size());

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("products", toProductMapList(products));
            data.put("pagination", pagination);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Product list error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * GET /api/v1/products-search?keyword=xxx — 商品検索
     */
    public String search() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            String searchKeyword = keyword;
            if (searchKeyword == null || searchKeyword.isEmpty()) {
                searchKeyword = "";
            }

            List<Product> products = productService.searchProducts(searchKeyword);

            Map<String, Object> pagination = new HashMap<String, Object>();
            pagination.put("currentPage", 1);
            pagination.put("totalPages", 1);
            pagination.put("totalItems", products.size());
            pagination.put("itemsPerPage", products.size());

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("products", toProductMapList(products));
            data.put("pagination", pagination);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Product search error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * GET /api/v1/products-detail?productId=xxx — 商品詳細
     */
    public String detail() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            if (productId == null) {
                ApiResponseUtil.writeError(response, 400, "PROD_001", "Product ID is required");
                return null;
            }

            Product product = productService.getProductById(productId);
            if (product == null) {
                ApiResponseUtil.writeError(response, 404, "PROD_002", "Product not found");
                return null;
            }

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("product", toProductMap(product));

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Product detail error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * PUT /api/v1/products-update?productId=xxx — 商品更新（ADMIN のみ）
     */
    public String update() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            String userType = (String) request.getAttribute("userType");
            if (!"admin".equals(userType)) {
                ApiResponseUtil.writeError(response, 403, "PROD_003", "Admin access required");
                return null;
            }

            if (productId == null) {
                ApiResponseUtil.writeError(response, 400, "PROD_001", "Product ID is required");
                return null;
            }

            Map<String, Object> body = readJsonBody(request);
            String productName = (String) body.get("productName");
            Integer unitPrice = null;
            if (body.get("unitPrice") instanceof Number) {
                unitPrice = ((Number) body.get("unitPrice")).intValue();
            }
            String description = (String) body.get("description");
            String imageUrl = (String) body.get("imageUrl");

            Product updated = productService.updateProductForApi(productId, productName, unitPrice, description, imageUrl);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("product", toProductMap(updated));

            ApiResponseUtil.writeSuccess(response, data);
        } catch (IllegalArgumentException e) {
            try {
                ApiResponseUtil.writeError(response, 404, "PROD_002", e.getMessage());
            } catch (Exception ex) {
                logger.error("Failed to write error response", ex);
            }
        } catch (Exception e) {
            logger.error("Product update error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * GET /api/v1/products-price-history?productId=xxx — 価格履歴
     */
    public String priceHistory() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            if (productId == null) {
                ApiResponseUtil.writeError(response, 400, "PROD_001", "Product ID is required");
                return null;
            }

            List<ProductPriceHistory> history = productService.getPriceHistory(productId);

            List<Map<String, Object>> historyList = new ArrayList<Map<String, Object>>();
            for (ProductPriceHistory h : history) {
                Map<String, Object> item = new HashMap<String, Object>();
                item.put("priceHistoryId", h.getPriceHistoryId());
                item.put("productId", h.getProductId());
                item.put("oldPrice", h.getOldPrice());
                item.put("newPrice", h.getNewPrice());
                item.put("changedAt", h.getChangedAt());
                item.put("changedBy", h.getChangedBy());
                item.put("changeReason", h.getChangeReason());
                historyList.add(item);
            }

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("priceHistory", historyList);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Price history error", e);
            writeInternalError(response);
        }
        return null;
    }

    // --- Helper methods ---

    private Map<String, Object> toProductMap(Product p) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put("productId", p.getProductId());
        map.put("productName", p.getProductName());
        map.put("unitPrice", p.getUnitPrice());
        map.put("description", p.getDescription());
        map.put("imageUrl", p.getImageUrl());
        map.put("createdAt", p.getCreatedAt());
        map.put("updatedAt", p.getUpdatedAt());
        return map;
    }

    private List<Map<String, Object>> toProductMapList(List<Product> products) {
        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
        for (Product p : products) {
            list.add(toProductMap(p));
        }
        return list;
    }

    @SuppressWarnings("unchecked")
    private Map<String, Object> readJsonBody(HttpServletRequest request) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        return mapper.readValue(request.getInputStream(), Map.class);
    }

    private void writeInternalError(HttpServletResponse response) {
        try {
            ApiResponseUtil.writeError(response, 500, "SYS_001", "Internal server error");
        } catch (Exception ex) {
            logger.error("Failed to write error response", ex);
        }
    }
}
