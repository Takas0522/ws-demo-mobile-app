package com.example.admin.action.auth;

import com.example.admin.entity.User;
import com.example.admin.service.AuthService;
import com.opensymphony.xwork2.ActionSupport;
import org.apache.struts2.interceptor.SessionAware;

import java.util.Map;

/**
 * ログイン Action。
 * GET: ログインフォーム表示 (input)
 * POST: 認証処理 (execute)
 */
public class LoginAction extends ActionSupport implements SessionAware {

    private static final long serialVersionUID = 1L;

    private AuthService authService;
    private Map<String, Object> session;

    // Input (フォーム)
    private String loginId;
    private String password;

    // Spring DI
    public void setAuthService(AuthService authService) {
        this.authService = authService;
    }

    @Override
    public void setSession(Map<String, Object> session) {
        this.session = session;
    }

    /**
     * ログインフォーム表示
     */
    public String input() {
        return INPUT;
    }

    /**
     * ログイン認証
     */
    @Override
    public String execute() {
        if (loginId == null || loginId.trim().isEmpty()) {
            addFieldError("loginId", "ログインIDを入力してください");
        }
        if (password == null || password.trim().isEmpty()) {
            addFieldError("password", "パスワードを入力してください");
        }
        if (hasFieldErrors()) {
            return INPUT;
        }

        User user = authService.authenticate(loginId, password);
        if (user == null) {
            addActionError("ログインIDまたはパスワードが正しくありません");
            return INPUT;
        }

        // セッションにユーザー情報を格納
        session.put("loginUser", user);
        session.put("loginUserId", user.getUserId());
        session.put("loginUserName", user.getUserName());

        return SUCCESS;
    }

    // Getter/Setter
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
