package com.example.admin.action;

import com.opensymphony.xwork2.ActionSupport;

/**
 * ダッシュボード Action。
 * 管理画面のトップページを表示する。
 */
public class DashboardAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    @Override
    public String execute() {
        return SUCCESS;
    }
}
