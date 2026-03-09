package com.example.admin.api.favorite;

import com.example.admin.entity.Favorite;
import com.example.admin.service.FavoriteService;
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
 * お気に入り API Action。
 * お気に入り一覧、追加、削除を提供する。
 */
public class ApiFavoriteAction extends ActionSupport {

    private static final Logger logger = LoggerFactory.getLogger(ApiFavoriteAction.class);

    // Service (Spring DI)
    private FavoriteService favoriteService;

    public void setFavoriteService(FavoriteService favoriteService) {
        this.favoriteService = favoriteService;
    }

    // Input params
    private Long productId;

    public void setProductId(Long productId) {
        this.productId = productId;
    }

    public Long getProductId() {
        return productId;
    }

    /**
     * GET /api/v1/favorites — お気に入り一覧
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

            // フィーチャーフラグチェック
            if (!favoriteService.isFavoriteEnabled(userId)) {
                ApiResponseUtil.writeError(response, 403, "FEATURE_001", "Favorite feature is not enabled");
                return null;
            }

            List<Favorite> favorites = favoriteService.getFavorites(userId);

            List<Map<String, Object>> favoriteList = new ArrayList<Map<String, Object>>();
            for (Favorite f : favorites) {
                favoriteList.add(toFavoriteMap(f));
            }

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("favorites", favoriteList);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Favorite list error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * POST /api/v1/favorites-add — お気に入り追加
     */
    public String add() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Long userId = (Long) request.getAttribute("userId");
            if (userId == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_004", "Authentication required");
                return null;
            }

            // フィーチャーフラグチェック
            if (!favoriteService.isFavoriteEnabled(userId)) {
                ApiResponseUtil.writeError(response, 403, "FEATURE_001", "Favorite feature is not enabled");
                return null;
            }

            Map<String, Object> body = readJsonBody(request);
            Long favProductId = null;
            if (body.get("productId") instanceof Number) {
                favProductId = ((Number) body.get("productId")).longValue();
            }

            if (favProductId == null) {
                ApiResponseUtil.writeError(response, 400, "FAV_001", "Product ID is required");
                return null;
            }

            favoriteService.addFavorite(userId, favProductId);

            Map<String, Object> favoriteData = new HashMap<String, Object>();
            favoriteData.put("userId", userId);
            favoriteData.put("productId", favProductId);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("favorite", favoriteData);

            ApiResponseUtil.writeSuccess(response, 201, data);
        } catch (IllegalStateException e) {
            try {
                ApiResponseUtil.writeError(response, 409, "FAV_002", e.getMessage());
            } catch (Exception ex) {
                logger.error("Failed to write error response", ex);
            }
        } catch (Exception e) {
            logger.error("Favorite add error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * DELETE /api/v1/favorites-remove?productId=xxx — お気に入り削除
     */
    public String remove() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Long userId = (Long) request.getAttribute("userId");
            if (userId == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_004", "Authentication required");
                return null;
            }

            // フィーチャーフラグチェック
            if (!favoriteService.isFavoriteEnabled(userId)) {
                ApiResponseUtil.writeError(response, 403, "FEATURE_001", "Favorite feature is not enabled");
                return null;
            }

            if (productId == null) {
                ApiResponseUtil.writeError(response, 400, "FAV_001", "Product ID is required");
                return null;
            }

            favoriteService.removeFavorite(userId, productId);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("message", "Favorite removed successfully");

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Favorite remove error", e);
            writeInternalError(response);
        }
        return null;
    }

    // --- Helper methods ---

    private Map<String, Object> toFavoriteMap(Favorite f) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put("favoriteId", f.getFavoriteId());
        map.put("userId", f.getUserId());
        map.put("productId", f.getProductId());
        map.put("productName", f.getProductName());
        map.put("unitPrice", f.getUnitPrice());
        map.put("description", f.getDescription());
        map.put("imageUrl", f.getImageUrl());
        map.put("createdAt", f.getCreatedAt());
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
