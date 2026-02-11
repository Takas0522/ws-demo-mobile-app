package com.example.mobilebff.config;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

import lombok.Data;

/**
 * Web API接続設定
 * 
 * application.ymlから設定を読み込む
 */
@Configuration
@ConfigurationProperties(prefix = "webapi")
@Data
public class WebApiConfig {
    
    /**
     * Web API ベースURL
     */
    private String baseUrl;
    
    /**
     * タイムアウト設定
     */
    private Timeout timeout = new Timeout();
    
    @Data
    public static class Timeout {
        /**
         * 接続タイムアウト（ミリ秒）
         */
        private int connect = 5000;
        
        /**
         * 読み込みタイムアウト（ミリ秒）
         */
        private int read = 30000;
    }
}
