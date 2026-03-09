package com.example.admin.dto;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;
import java.util.HashMap;

/**
 * API レスポンスラッパー。
 * 成功時: {"data": {...}, "timestamp": "..."}
 * エラー時: {"error": {"code": "...", "message": "...", "details": null}, "timestamp": "..."}
 */
public class ApiResponse {
    private static final String DATE_FORMAT = "yyyy-MM-dd'T'HH:mm:ss";

    private Object data;
    private Map<String, Object> error;
    private String timestamp;

    private ApiResponse() {
        SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT);
        this.timestamp = sdf.format(new Date());
    }

    /**
     * 成功レスポンスを生成する。
     *
     * @param data レスポンスデータ
     * @return ApiResponse
     */
    public static ApiResponse success(Object data) {
        ApiResponse response = new ApiResponse();
        response.data = data;
        return response;
    }

    /**
     * エラーレスポンスを生成する。
     *
     * @param code    エラーコード
     * @param message エラーメッセージ
     * @return ApiResponse
     */
    public static ApiResponse error(String code, String message) {
        return error(code, message, null);
    }

    /**
     * エラーレスポンスを詳細付きで生成する。
     *
     * @param code    エラーコード
     * @param message エラーメッセージ
     * @param details エラー詳細
     * @return ApiResponse
     */
    public static ApiResponse error(String code, String message, Object details) {
        ApiResponse response = new ApiResponse();
        Map<String, Object> errorMap = new HashMap<String, Object>();
        errorMap.put("code", code);
        errorMap.put("message", message);
        errorMap.put("details", details);
        response.error = errorMap;
        return response;
    }

    // Getters
    public Object getData() {
        return data;
    }

    public Map<String, Object> getError() {
        return error;
    }

    public String getTimestamp() {
        return timestamp;
    }
}
