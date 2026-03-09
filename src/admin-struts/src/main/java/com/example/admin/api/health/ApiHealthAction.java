package com.example.admin.api.health;

import com.example.admin.util.ApiResponseUtil;
import com.opensymphony.xwork2.ActionSupport;
import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.http.HttpServletResponse;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

/**
 * ヘルスチェック API Action。
 */
public class ApiHealthAction extends ActionSupport {

    private static final Logger logger = LoggerFactory.getLogger(ApiHealthAction.class);

    /**
     * GET /api/v1/health — ヘルスチェック
     */
    @Override
    public String execute() {
        HttpServletResponse response = ServletActionContext.getResponse();
        try {
            Map<String, Object> data = new HashMap<String, Object>();
            data.put("status", "UP");
            data.put("service", "struts2-web-api");
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");
            data.put("timestamp", sdf.format(new Date()));

            ApiResponseUtil.writeSuccess(response, data);
        } catch (Exception e) {
            logger.error("Health check error", e);
            try {
                ApiResponseUtil.writeError(response, 500, "SYS_001", "Internal server error");
            } catch (Exception ex) {
                logger.error("Failed to write error response", ex);
            }
        }
        return null;
    }
}
