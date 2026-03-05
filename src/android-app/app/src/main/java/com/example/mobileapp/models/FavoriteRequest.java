package com.example.mobileapp.models;

public class FavoriteRequest {
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

