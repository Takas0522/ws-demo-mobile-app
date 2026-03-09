package com.example.admin.interceptor;

import com.opensymphony.xwork2.ActionInvocation;
import com.opensymphony.xwork2.interceptor.AbstractInterceptor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Map;

/**
 * 管理画面用セッション認証インターセプタ。
 * セッションに "loginUser" が存在しない場合はログインページにリダイレクトする。
 */
public class AuthInterceptor extends AbstractInterceptor {
    private static final Logger logger = LoggerFactory.getLogger(AuthInterceptor.class);

    @Override
    public String intercept(ActionInvocation invocation) throws Exception {
        Map<String, Object> session = invocation.getInvocationContext().getSession();
        Object loginUser = session.get("loginUser");

        if (loginUser == null) {
            logger.debug("User not authenticated, redirecting to login");
            return "login";
        }

        return invocation.invoke();
    }
}
