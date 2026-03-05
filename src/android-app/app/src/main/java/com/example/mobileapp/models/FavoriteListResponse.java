package com.example.mobileapp.models;

import java.util.List;

public class FavoriteListResponse {
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

