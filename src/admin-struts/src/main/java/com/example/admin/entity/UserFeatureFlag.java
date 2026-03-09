package com.example.admin.entity;

import java.io.Serializable;

public class UserFeatureFlag implements Serializable {

    private static final long serialVersionUID = 1L;

    private Long userFlagId;
    private Long userId;
    private Long flagId;
    private Boolean isEnabled;
    private String createdAt;
    private String updatedAt;
    /** JOINで取得するフラグキー */
    private String flagKey;
    /** JOINで取得するフラグ名 */
    private String flagName;
    /** JOINで取得するデフォルト値 */
    private Boolean defaultValue;

    public UserFeatureFlag() {
    }

    public Long getUserFlagId() {
        return userFlagId;
    }

    public void setUserFlagId(Long userFlagId) {
        this.userFlagId = userFlagId;
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public Long getFlagId() {
        return flagId;
    }

    public void setFlagId(Long flagId) {
        this.flagId = flagId;
    }

    public Boolean getIsEnabled() {
        return isEnabled;
    }

    public void setIsEnabled(Boolean isEnabled) {
        this.isEnabled = isEnabled;
    }

    public String getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(String createdAt) {
        this.createdAt = createdAt;
    }

    public String getUpdatedAt() {
        return updatedAt;
    }

    public void setUpdatedAt(String updatedAt) {
        this.updatedAt = updatedAt;
    }

    public String getFlagKey() {
        return flagKey;
    }

    public void setFlagKey(String flagKey) {
        this.flagKey = flagKey;
    }

    public String getFlagName() {
        return flagName;
    }

    public void setFlagName(String flagName) {
        this.flagName = flagName;
    }

    public Boolean getDefaultValue() {
        return defaultValue;
    }

    public void setDefaultValue(Boolean defaultValue) {
        this.defaultValue = defaultValue;
    }
}
