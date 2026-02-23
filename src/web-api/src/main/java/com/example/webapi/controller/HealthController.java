package com.example.webapi.controller;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.time.Instant;
import java.util.HashMap;
import java.util.Map;

/**
 * ヘルスチェックコントローラー
 * 
 * APIの稼働状態を確認するためのエンドポイント
 */
@RestController
@RequestMapping("/api/v1")
public class HealthController {

    /**
     * ヘルスチェック
     * 
     * @return ステータス情報
     */
    @GetMapping("/health")
    public ResponseEntity<Map<String, Object>> health() {
        Map<String, Object> response = new HashMap<>();
        response.put("status", "UP");
        response.put("service", "web-api");
        response.put("timestamp", Instant.now().toString());
        
        return ResponseEntity.ok(response);
    }
}
