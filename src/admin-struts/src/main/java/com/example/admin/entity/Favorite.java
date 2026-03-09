package com.example.admin.entity;

import java.io.Serializable;

public class Favorite implements Serializable {

    private static final long serialVersionUID = 1L;

    private Long favoriteId;
    private Long userId;
    private Long productId;
    private String createdAt;
    /** JOINで取得する商品名 */
    private String productName;
    /** JOINで取得する単価 */
    private Integer unitPrice;
    /** JOINで取得する商品説明 */
    private String description;
    /** JOINで取得する画像URL */
    private String imageUrl;

    public Favorite() {
    }

    public Long getFavoriteId() {
        return favoriteId;
    }

    public void setFavoriteId(Long favoriteId) {
        this.favoriteId = favoriteId;
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public Long getProductId() {
        return productId;
    }

    public void setProductId(Long productId) {
        this.productId = productId;
    }

    public String getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(String createdAt) {
        this.createdAt = createdAt;
    }

    public String getProductName() {
        return productName;
    }

    public void setProductName(String productName) {
        this.productName = productName;
    }

    public Integer getUnitPrice() {
        return unitPrice;
    }

    public void setUnitPrice(Integer unitPrice) {
        this.unitPrice = unitPrice;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getImageUrl() {
        return imageUrl;
    }

    public void setImageUrl(String imageUrl) {
        this.imageUrl = imageUrl;
    }
}
