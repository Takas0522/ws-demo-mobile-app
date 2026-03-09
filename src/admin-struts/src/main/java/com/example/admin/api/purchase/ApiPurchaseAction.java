package com.example.admin.api.purchase;

import com.example.admin.entity.Purchase;
import com.example.admin.service.PurchaseService;
import com.example.admin.util.ApiResponseUtil;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.opensymphony.xwork2.ActionSupport;
import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 購入 API Action。
 * 購入作成、購入履歴取得を提供する。
 */
public class ApiPurchaseAction extends ActionSupport {

    private static final Logger logger = LoggerFactory.getLogger(ApiPurchaseAction.class);

    // Service (Spring DI)
    private PurchaseService purchaseService;

    public void setPurchaseService(PurchaseService purchaseService) {
        this.purchaseService = purchaseService;
    }

    /**
     * POST /api/v1/purchases-create — 購入作成
     */
    public String create() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Long userId = (Long) request.getAttribute("userId");
            if (userId == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_004", "Authentication required");
                return null;
            }

            Map<String, Object> body = readJsonBody(request);

            Long productId = null;
            if (body.get("productId") instanceof Number) {
                productId = ((Number) body.get("productId")).longValue();
            }
            int quantity = 0;
            if (body.get("quantity") instanceof Number) {
                quantity = ((Number) body.get("quantity")).intValue();
            }

            if (productId == null) {
                ApiResponseUtil.writeError(response, 400, "PUR_001", "Product ID is required");
                return null;
            }

            Purchase purchase = purchaseService.createPurchase(userId, productId, quantity);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("purchase", toPurchaseMap(purchase));

            ApiResponseUtil.writeSuccess(response, 201, data);
        } catch (IllegalArgumentException e) {
            try {
                ApiResponseUtil.writeError(response, 400, "PUR_002", e.getMessage());
            } catch (Exception ex) {
                logger.error("Failed to write error response", ex);
            }
        } catch (Exception e) {
            logger.error("Purchase create error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * GET /api/v1/purchases — 購入履歴
     */
    public String list() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Long userId = (Long) request.getAttribute("userId");
            if (userId == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_004", "Authentication required");
                return null;
            }

            List<Purchase> purchases = purchaseService.getPurchasesByUserId(userId);

            List<Map<String, Object>> purchaseList = new ArrayList<Map<String, Object>>();
            for (Purchase p : purchases) {
                purchaseList.add(toPurchaseMap(p));
            }

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("purchases", purchaseList);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Purchase list error", e);
            writeInternalError(response);
        }
        return null;
    }

    // --- Helper methods ---

    private Map<String, Object> toPurchaseMap(Purchase p) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put("purchaseId", p.getPurchaseId());
        map.put("userId", p.getUserId());
        map.put("productId", p.getProductId());
        map.put("quantity", p.getQuantity());
        map.put("unitPriceAtPurchase", p.getUnitPriceAtPurchase());
        map.put("totalAmount", p.getTotalAmount());
        map.put("purchasedAt", p.getPurchasedAt());
        map.put("productName", p.getProductName());
        return map;
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
