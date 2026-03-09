package com.example.admin.dao;

import com.example.admin.util.DatabaseUtil;

import org.apache.commons.dbutils.BasicRowProcessor;
import org.apache.commons.dbutils.BeanProcessor;
import org.apache.commons.dbutils.DbUtils;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.RowProcessor;
import org.apache.commons.dbutils.handlers.BeanHandler;
import org.apache.commons.dbutils.handlers.BeanListHandler;
import org.apache.commons.dbutils.handlers.ScalarHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Map;

/**
 * DAO基底クラス。
 * コネクション取得・クローズおよび共通ハンドラ生成を提供する。
 */
public abstract class BaseDao {

    protected final Logger logger = LoggerFactory.getLogger(getClass());
    protected final QueryRunner queryRunner = new QueryRunner();

    protected Connection getConnection() throws SQLException {
        return DatabaseUtil.getConnection();
    }

    protected void closeQuietly(Connection conn) {
        DbUtils.closeQuietly(conn);
    }

    /**
     * カラム名→プロパティ名マッピングから BeanHandler を生成する。
     */
    protected <T> BeanHandler<T> createBeanHandler(Class<T> clazz, Map<String, String> columnToProperty) {
        BeanProcessor beanProcessor = new BeanProcessor(columnToProperty);
        RowProcessor rowProcessor = new BasicRowProcessor(beanProcessor);
        return new BeanHandler<T>(clazz, rowProcessor);
    }

    /**
     * カラム名→プロパティ名マッピングから BeanListHandler を生成する。
     */
    protected <T> BeanListHandler<T> createBeanListHandler(Class<T> clazz, Map<String, String> columnToProperty) {
        BeanProcessor beanProcessor = new BeanProcessor(columnToProperty);
        RowProcessor rowProcessor = new BasicRowProcessor(beanProcessor);
        return new BeanListHandler<T>(clazz, rowProcessor);
    }

    /**
     * COUNT等のスカラー値取得用ハンドラを返す。
     */
    protected ScalarHandler<Long> createScalarHandler() {
        return new ScalarHandler<Long>();
    }
}
