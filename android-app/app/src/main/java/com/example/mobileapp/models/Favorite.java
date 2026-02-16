package com.example.mobileapp.models;

import java.util.List;

public class Favorite {
    private int id;
    private int favoriteId;
    private String userId;
    private int productId;
    private String createdAt;

    public Favorite(int id, int favoriteId, String userId, int productId, String createdAt) {
        this.id = id;
        this.favoriteId = favoriteId;
        this.userId = userId;
        this.productId = productId;
        this.createdAt = createdAt;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getFavoriteId() {
        return favoriteId;
    }

    public void setFavoriteId(int favoriteId) {
        this.favoriteId = favoriteId;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public int getProductId() {
        return productId;
    }

    public void setProductId(int productId) {
        this.productId = productId;
    }

    public String getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(String createdAt) {
        this.createdAt = createdAt;
    }
}

class FavoriteRequest {
    private int productId;

    public FavoriteRequest(int productId) {
        this.productId = productId;
    }

    public int getProductId() {
        return productId;
    }

    public void setProductId(int productId) {
        this.productId = productId;
    }
}

class FavoriteResponse {
    private Favorite data;
    private String timestamp;

    public Favorite getData() {
        return data;
    }

    public void setData(Favorite data) {
        this.data = data;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}

class FavoriteListResponse {
    private List<Favorite> data;
    private String timestamp;

    public List<Favorite> getData() {
        return data;
    }

    public void setData(List<Favorite> data) {
        this.data = data;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}
