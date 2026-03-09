package com.example.admin.util;

import com.example.admin.dto.ApiResponse;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;

import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

/**
 * API レスポンスを HttpServletResponse に JSON 形式で書き込むユーティリティ。
 */
public class ApiResponseUtil {
    private static final ObjectMapper objectMapper = new ObjectMapper();

    static {
        objectMapper.configure(SerializationFeature.WRITE_DATES_AS_TIMESTAMPS, false);
    }

    /**
     * 成功レスポンスを書き込む (HTTP 200)。
     *
     * @param response HttpServletResponse
     * @param data     レスポンスデータ
     * @throws IOException 書き込みエラー
     */
    public static void writeSuccess(HttpServletResponse response, Object data) throws IOException {
        writeJson(response, HttpServletResponse.SC_OK, ApiResponse.success(data));
    }

    /**
     * 成功レスポンスを指定のステータスコードで書き込む。
     *
     * @param response   HttpServletResponse
     * @param statusCode HTTP ステータスコード
     * @param data       レスポンスデータ
     * @throws IOException 書き込みエラー
     */
    public static void writeSuccess(HttpServletResponse response, int statusCode, Object data) throws IOException {
        writeJson(response, statusCode, ApiResponse.success(data));
    }

    /**
     * エラーレスポンスを書き込む。
     *
     * @param response   HttpServletResponse
     * @param statusCode HTTP ステータスコード
     * @param code       エラーコード
     * @param message    エラーメッセージ
     * @throws IOException 書き込みエラー
     */
    public static void writeError(HttpServletResponse response, int statusCode, String code, String message) throws IOException {
        writeJson(response, statusCode, ApiResponse.error(code, message));
    }

    /**
     * エラーレスポンスを詳細付きで書き込む。
     *
     * @param response   HttpServletResponse
     * @param statusCode HTTP ステータスコード
     * @param code       エラーコード
     * @param message    エラーメッセージ
     * @param details    エラー詳細
     * @throws IOException 書き込みエラー
     */
    public static void writeError(HttpServletResponse response, int statusCode, String code, String message, Object details) throws IOException {
        writeJson(response, statusCode, ApiResponse.error(code, message, details));
    }

    private static void writeJson(HttpServletResponse response, int statusCode, ApiResponse apiResponse) throws IOException {
        response.setStatus(statusCode);
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        objectMapper.writeValue(response.getWriter(), apiResponse);
    }
}
