package com.example.mobileapp.models;

public class FavoriteResponse {
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

