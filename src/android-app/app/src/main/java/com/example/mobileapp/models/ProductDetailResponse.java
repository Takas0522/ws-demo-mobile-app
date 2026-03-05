package com.example.mobileapp.models;

public class ProductDetailResponse {
    private ProductDetail data;
    private String timestamp;

    public ProductDetail getData() {
        return data;
    }

    public void setData(ProductDetail data) {
        this.data = data;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}

