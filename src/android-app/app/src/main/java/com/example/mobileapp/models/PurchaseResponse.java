package com.example.mobileapp.models;

public class PurchaseResponse {
    private Purchase data;
    private String timestamp;

    public Purchase getData() {
        return data;
    }

    public void setData(Purchase data) {
        this.data = data;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}

