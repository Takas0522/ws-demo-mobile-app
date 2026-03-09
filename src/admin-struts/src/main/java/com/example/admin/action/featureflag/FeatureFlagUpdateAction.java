package com.example.admin.action.featureflag;

import com.example.admin.service.FeatureFlagService;
import com.opensymphony.xwork2.ActionSupport;

/**
 * フィーチャーフラグ更新 Action。
 * フォーム POST でフラグの有効/無効を切り替える。
 */
public class FeatureFlagUpdateAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private FeatureFlagService featureFlagService;

    // Input
    private Long userId;
    private String flagKey;
    private Boolean enabled;

    public void setFeatureFlagService(FeatureFlagService featureFlagService) {
        this.featureFlagService = featureFlagService;
    }

    @Override
    public String execute() {
        if (userId == null || flagKey == null) {
            addActionError("パラメータが不足しています");
            return ERROR;
        }

        boolean newEnabled = (enabled != null) ? enabled.booleanValue() : false;
        featureFlagService.updateUserFeatureFlag(userId, flagKey, newEnabled);

        return SUCCESS;
    }

    // Getter/Setter
    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public String getFlagKey() {
        return flagKey;
    }

    public void setFlagKey(String flagKey) {
        this.flagKey = flagKey;
    }

    public Boolean getEnabled() {
        return enabled;
    }

    public void setEnabled(Boolean enabled) {
        this.enabled = enabled;
    }
}
