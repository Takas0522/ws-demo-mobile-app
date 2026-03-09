package com.example.admin.action.user;

import com.example.admin.entity.User;
import com.example.admin.service.UserService;
import com.opensymphony.xwork2.ActionSupport;

import java.util.List;

/**
 * ユーザー一覧 Action。
 * 全ユーザーを取得して一覧画面を表示する。
 */
public class UserListAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private UserService userService;

    // Output
    private List<User> users;

    public void setUserService(UserService userService) {
        this.userService = userService;
    }

    @Override
    public String execute() {
        users = userService.getUsers();
        return SUCCESS;
    }

    public List<User> getUsers() {
        return users;
    }
}
