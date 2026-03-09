package com.example.admin.action.featureflag;

import com.example.admin.entity.User;
import com.example.admin.entity.UserFeatureFlag;
import com.example.admin.service.FeatureFlagService;
import com.example.admin.service.UserService;
import com.opensymphony.xwork2.ActionSupport;

import java.util.List;

/**
 * フィーチャーフラグ管理 Action。
 * ユーザーIDを受け取り、そのユーザーのフラグ一覧を表示する。
 */
public class FeatureFlagAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private FeatureFlagService featureFlagService;
    private UserService userService;

    // Input
    private Long userId;

    // Output
    private User targetUser;
    private List<UserFeatureFlag> featureFlags;
    private List<User> users;
    private String successMessage;

    public void setFeatureFlagService(FeatureFlagService featureFlagService) {
        this.featureFlagService = featureFlagService;
    }

    public void setUserService(UserService userService) {
        this.userService = userService;
    }

    @Override
    public String execute() {
        // ユーザー一覧は常にロード（ユーザー選択用）
        users = userService.getUsers();

        if (userId != null) {
            targetUser = userService.getUserById(userId);
            if (targetUser != null) {
                featureFlags = featureFlagService.getUserFeatureFlags(userId);
            }
        }
        return SUCCESS;
    }

    // Getter/Setter
    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public User getTargetUser() {
        return targetUser;
    }

    public List<UserFeatureFlag> getFeatureFlags() {
        return featureFlags;
    }

    public List<User> getUsers() {
        return users;
    }

    public String getSuccessMessage() {
        return successMessage;
    }

    public void setSuccessMessage(String successMessage) {
        this.successMessage = successMessage;
    }
}
