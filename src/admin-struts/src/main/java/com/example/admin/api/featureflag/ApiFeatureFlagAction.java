package com.example.admin.api.featureflag;

import com.example.admin.entity.User;
import com.example.admin.entity.UserFeatureFlag;
import com.example.admin.service.FeatureFlagService;
import com.example.admin.service.UserService;
import com.example.admin.util.ApiResponseUtil;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.opensymphony.xwork2.ActionSupport;
import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * フィーチャーフラグ API Action。
 * ユーザーフラグ取得、管理者ユーザー一覧、フラグ更新を提供する。
 */
public class ApiFeatureFlagAction extends ActionSupport {

    private static final Logger logger = LoggerFactory.getLogger(ApiFeatureFlagAction.class);

    // Services (Spring DI)
    private FeatureFlagService featureFlagService;
    private UserService userService;

    public void setFeatureFlagService(FeatureFlagService featureFlagService) {
        this.featureFlagService = featureFlagService;
    }

    public void setUserService(UserService userService) {
        this.userService = userService;
    }

    // Input params
    private Long userId;
    private String flagKey;

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public Long getUserId() {
        return userId;
    }

    public void setFlagKey(String flagKey) {
        this.flagKey = flagKey;
    }

    public String getFlagKey() {
        return flagKey;
    }

    /**
     * GET /api/v1/feature-flags — ユーザーのフラグ取得
     */
    public String userFlags() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Long authUserId = (Long) request.getAttribute("userId");
            if (authUserId == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_004", "Authentication required");
                return null;
            }

            List<UserFeatureFlag> flags = featureFlagService.getUserFeatureFlags(authUserId);

            List<Map<String, Object>> flagList = new ArrayList<Map<String, Object>>();
            for (UserFeatureFlag f : flags) {
                Map<String, Object> flagData = new HashMap<String, Object>();
                flagData.put("flagKey", f.getFlagKey());
                flagData.put("flagName", f.getFlagName());
                flagData.put("isEnabled", Boolean.TRUE.equals(f.getIsEnabled()));
                flagList.add(flagData);
            }

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("featureFlags", flagList);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("User flags error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * GET /api/v1/admin-users — 管理者向けユーザー一覧＋フラグ
     */
    public String adminUsers() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            String userType = (String) request.getAttribute("userType");
            if (!"admin".equals(userType)) {
                ApiResponseUtil.writeError(response, 403, "ADMIN_001", "Admin access required");
                return null;
            }

            List<User> users = userService.getUsers();

            List<Map<String, Object>> userList = new ArrayList<Map<String, Object>>();
            for (User user : users) {
                Map<String, Object> userData = new HashMap<String, Object>();
                userData.put("userId", user.getUserId());
                userData.put("userName", user.getUserName());
                userData.put("loginId", user.getLoginId());
                userData.put("userType", user.getUserType());
                userData.put("createdAt", user.getCreatedAt());
                userData.put("updatedAt", user.getUpdatedAt());

                // 各ユーザーのフラグ
                List<UserFeatureFlag> flags = featureFlagService.getUserFeatureFlags(user.getUserId());
                List<Map<String, Object>> flagList = new ArrayList<Map<String, Object>>();
                for (UserFeatureFlag f : flags) {
                    Map<String, Object> flagData = new HashMap<String, Object>();
                    flagData.put("flagKey", f.getFlagKey());
                    flagData.put("flagName", f.getFlagName());
                    flagData.put("isEnabled", Boolean.TRUE.equals(f.getIsEnabled()));
                    flagList.add(flagData);
                }
                userData.put("featureFlags", flagList);

                userList.add(userData);
            }

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("users", userList);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Admin users error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * PUT /api/v1/admin-feature-flag-update?userId=xxx&flagKey=yyy — フラグ更新
     */
    public String updateFlag() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            String userType = (String) request.getAttribute("userType");
            if (!"admin".equals(userType)) {
                ApiResponseUtil.writeError(response, 403, "ADMIN_001", "Admin access required");
                return null;
            }

            // userId と flagKey はリクエストパラメータから取得
            Long targetUserId = this.userId;
            String targetFlagKey = this.flagKey;

            if (targetUserId == null || targetFlagKey == null) {
                ApiResponseUtil.writeError(response, 400, "FLAG_001", "userId and flagKey are required");
                return null;
            }

            Map<String, Object> body = readJsonBody(request);
            Boolean enabled = null;
            Object enabledObj = body.get("enabled");
            if (enabledObj instanceof Boolean) {
                enabled = (Boolean) enabledObj;
            }

            if (enabled == null) {
                ApiResponseUtil.writeError(response, 400, "FLAG_002", "enabled field is required");
                return null;
            }

            featureFlagService.updateUserFeatureFlag(targetUserId, targetFlagKey, enabled);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("userId", targetUserId);
            data.put("flagKey", targetFlagKey);
            data.put("isEnabled", enabled);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");
            data.put("updatedAt", sdf.format(new Date()));

            ApiResponseUtil.writeSuccess(response, data);
        } catch (IllegalArgumentException e) {
            try {
                ApiResponseUtil.writeError(response, 404, "FLAG_003", e.getMessage());
            } catch (Exception ex) {
                logger.error("Failed to write error response", ex);
            }
        } catch (Exception e) {
            logger.error("Update flag error", e);
            writeInternalError(response);
        }
        return null;
    }

    // --- Helper methods ---

    @SuppressWarnings("unchecked")
    private Map<String, Object> readJsonBody(HttpServletRequest request) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        return mapper.readValue(request.getInputStream(), Map.class);
    }

    private void writeInternalError(HttpServletResponse response) {
        try {
            ApiResponseUtil.writeError(response, 500, "SYS_001", "Internal server error");
        } catch (Exception ex) {
            logger.error("Failed to write error response", ex);
        }
    }
}
