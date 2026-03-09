package com.example.admin.interceptor;

import com.example.admin.util.JwtUtil;
import com.opensymphony.xwork2.ActionInvocation;
import com.opensymphony.xwork2.interceptor.AbstractInterceptor;
import io.jsonwebtoken.Claims;
import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.http.HttpServletRequest;

/**
 * Web API 用 JWT 認証インターセプタ。
 * Authorization: Bearer &lt;token&gt; ヘッダーを検証する。
 */
public class JwtInterceptor extends AbstractInterceptor {
    private static final Logger logger = LoggerFactory.getLogger(JwtInterceptor.class);

    // JwtUtil は Spring DI で注入（Struts2-Spring Plugin）
    // ただし Interceptor は Struts2 が管理するため、Spring DI が自動で効かない場合がある
    // その場合は ApplicationContext から手動取得する
    private JwtUtil jwtUtil;

    public void setJwtUtil(JwtUtil jwtUtil) {
        this.jwtUtil = jwtUtil;
    }

    @Override
    public String intercept(ActionInvocation invocation) throws Exception {
        // JwtUtil が DI されていない場合、ApplicationContext から取得
        if (jwtUtil == null) {
            jwtUtil = (JwtUtil) org.springframework.web.context.support.WebApplicationContextUtils
                    .getWebApplicationContext(ServletActionContext.getServletContext())
                    .getBean("jwtUtil");
        }

        HttpServletRequest request = ServletActionContext.getRequest();
        String authHeader = request.getHeader("Authorization");

        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            logger.debug("Missing or invalid Authorization header");
            return "unauthorized";
        }

        String token = authHeader.substring(7);
        Claims claims = jwtUtil.parseToken(token);

        if (claims == null) {
            logger.debug("Invalid JWT token");
            return "unauthorized";
        }

        // リクエスト属性にユーザー情報を格納
        request.setAttribute("userId", Long.parseLong(claims.getSubject()));
        request.setAttribute("loginId", claims.get("loginId"));
        request.setAttribute("userType", claims.get("userType"));

        return invocation.invoke();
    }
}
