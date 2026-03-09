package com.example.admin.api.auth;

import com.example.admin.dao.UserDao;
import com.example.admin.entity.User;
import com.example.admin.service.AuthService;
import com.example.admin.util.ApiResponseUtil;
import com.example.admin.util.JwtUtil;
import com.example.admin.util.PasswordUtil;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.opensymphony.xwork2.ActionSupport;
import io.jsonwebtoken.Claims;
import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * 認証 API Action。
 * ログイン、ログアウト、トークンリフレッシュ、パスワード変更を提供する。
 */
public class ApiAuthAction extends ActionSupport {

    private static final Logger logger = LoggerFactory.getLogger(ApiAuthAction.class);

    // Service (Spring DI)
    private AuthService authService;
    private JwtUtil jwtUtil;
    private UserDao userDao;

    public void setAuthService(AuthService authService) {
        this.authService = authService;
    }

    public void setJwtUtil(JwtUtil jwtUtil) {
        this.jwtUtil = jwtUtil;
    }

    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    /**
     * POST /api/v1/auth/login — ユーザーログイン（userType 制限なし）
     */
    public String login() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Map<String, Object> body = readJsonBody(request);
            String loginId = (String) body.get("loginId");
            String password = (String) body.get("password");

            User user = authService.authenticateUser(loginId, password);
            if (user == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_001", "Invalid credentials");
                return null;
            }

            String token = jwtUtil.generateToken(user.getUserId(), user.getLoginId(), user.getUserType());

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("token", token);
            data.put("userId", user.getUserId());
            data.put("userName", user.getUserName());
            data.put("loginId", user.getLoginId());
            data.put("userType", user.getUserType());

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Login error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * POST /api/v1/auth/admin-login — 管理者ログイン（admin のみ許可）
     */
    public String adminLogin() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Map<String, Object> body = readJsonBody(request);
            String loginId = (String) body.get("loginId");
            String password = (String) body.get("password");

            User user = authService.authenticateAdmin(loginId, password);
            if (user == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_001", "Invalid credentials");
                return null;
            }

            String token = jwtUtil.generateToken(user.getUserId(), user.getLoginId(), user.getUserType());

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("token", token);
            data.put("userId", user.getUserId());
            data.put("userName", user.getUserName());
            data.put("loginId", user.getLoginId());
            data.put("userType", user.getUserType());

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Admin login error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * POST /api/v1/auth/logout — ログアウト（ステートレス JWT のため実質何もしない）
     */
    public String logout() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            ApiResponseUtil.writeSuccess(response, "Logged out successfully");
        } catch (Exception e) {
            logger.error("Logout error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * POST /api/v1/auth/refresh — トークンリフレッシュ
     */
    public String refresh() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Map<String, Object> body = readJsonBody(request);
            String token = (String) body.get("token");

            if (token == null || token.isEmpty()) {
                ApiResponseUtil.writeError(response, 400, "AUTH_002", "Token is required");
                return null;
            }

            Claims claims = jwtUtil.parseToken(token);
            if (claims == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_003", "Invalid or expired token");
                return null;
            }

            Long userId = Long.parseLong(claims.getSubject());
            String loginId = (String) claims.get("loginId");
            String userType = (String) claims.get("userType");

            String newToken = jwtUtil.generateToken(userId, loginId, userType);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("token", newToken);
            data.put("userId", userId);
            data.put("loginId", loginId);
            data.put("userType", userType);

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Token refresh error", e);
            writeInternalError(response);
        }
        return null;
    }

    /**
     * PUT /api/v1/auth/password — パスワード変更
     */
    public String changePassword() {
        HttpServletResponse response = ServletActionContext.getResponse();
        HttpServletRequest request = ServletActionContext.getRequest();
        try {
            Long userId = (Long) request.getAttribute("userId");
            if (userId == null) {
                ApiResponseUtil.writeError(response, 401, "AUTH_004", "Authentication required");
                return null;
            }

            Map<String, Object> body = readJsonBody(request);
            String currentPassword = (String) body.get("currentPassword");
            String newPassword = (String) body.get("newPassword");

            if (currentPassword == null || newPassword == null) {
                ApiResponseUtil.writeError(response, 400, "AUTH_005", "Current password and new password are required");
                return null;
            }

            User user = userDao.findById(userId);
            if (user == null) {
                ApiResponseUtil.writeError(response, 404, "AUTH_006", "User not found");
                return null;
            }

            if (!PasswordUtil.checkPassword(currentPassword, user.getPasswordHash())) {
                ApiResponseUtil.writeError(response, 400, "AUTH_007", "Current password is incorrect");
                return null;
            }

            String newHash = PasswordUtil.hashPassword(newPassword);
            userDao.updatePassword(userId, newHash);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put("message", "Password changed successfully");

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Change password error", e);
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
