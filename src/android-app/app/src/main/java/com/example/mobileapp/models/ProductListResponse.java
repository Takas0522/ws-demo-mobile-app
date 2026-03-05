package com.example.mobileapp.models;

public class ProductListResponse {
    private ProductListData data;
    private String timestamp;

    public ProductListData getData() {
        return data;
    }

    public void setData(ProductListData data) {
        this.data = data;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}

