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

// Login request/response models
class LoginRequest {
    private String loginId;
    private String password;

    public LoginRequest(String loginId, String password) {
        this.loginId = loginId;
        this.password = password;
    }

    public String getLoginId() {
        return loginId;
    }

    public void setLoginId(String loginId) {
        this.loginId = loginId;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }
}

class LoginResponse {
    private LoginData data;
    private String timestamp;

    public LoginData getData() {
        return data;
    }

    public void setData(LoginData data) {
        this.data = data;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}

class LoginData {
    private String token;
    private User user;

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }
}
