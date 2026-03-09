package com.example.admin.action.auth;

import com.opensymphony.xwork2.ActionSupport;
import org.apache.struts2.interceptor.SessionAware;

import java.util.Map;

/**
 * ログアウト Action。
 * セッションをクリアしてログイン画面にリダイレクトする。
 */
public class LogoutAction extends ActionSupport implements SessionAware {

    private static final long serialVersionUID = 1L;

    private Map<String, Object> session;

    @Override
    public void setSession(Map<String, Object> session) {
        this.session = session;
    }

    @Override
    public String execute() {
        if (session != null) {
            session.clear();
        }
        return SUCCESS;
    }
}
