package com.example.admin.service;

import com.example.admin.dao.UserDao;
import com.example.admin.entity.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.sql.SQLException;
import java.util.List;

@Service
public class UserService {

    private static final Logger logger = LoggerFactory.getLogger(UserService.class);

    private UserDao userDao;

    @Autowired
    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    /**
     * 全ユーザーを取得する。
     * @return ユーザーリスト
     */
    public List<User> getUsers() {
        try {
            return userDao.findAll();
        } catch (SQLException e) {
            logger.error("Failed to get users", e);
            throw new RuntimeException("ユーザー一覧の取得に失敗しました", e);
        }
    }

    /**
     * ユーザーIDでユーザーを取得する。
     * @param userId ユーザーID
     * @return ユーザー、見つからない場合は null
     */
    public User getUserById(Long userId) {
        try {
            return userDao.findById(userId);
        } catch (SQLException e) {
            logger.error("Failed to get user by id: {}", userId, e);
            throw new RuntimeException("ユーザーの取得に失敗しました", e);
        }
    }
}
