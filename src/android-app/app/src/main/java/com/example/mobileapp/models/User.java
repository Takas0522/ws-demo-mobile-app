package com.example.mobileapp.models;

import java.util.Map;

public class User {
    private String userId;
    private String userName;
    private String role;
    private Map<String, Boolean> featureFlags;

    public User(String userId, String userName, String role, Map<String, Boolean> featureFlags) {
        this.userId = userId;
        this.userName = userName;
        this.role = role;
        this.featureFlags = featureFlags;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getRole() {
        return role;
    }

    public void setRole(String role) {
        this.role = role;
    }

    public Map<String, Boolean> getFeatureFlags() {
        return featureFlags;
    }

    public void setFeatureFlags(Map<String, Boolean> featureFlags) {
        this.featureFlags = featureFlags;
    }
}
