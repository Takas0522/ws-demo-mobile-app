package com.example.adminbff;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;
import org.springframework.web.reactive.function.client.WebClient;

/**
 * Admin BFF Application
 * 
 * Mobile App System の BFF Layer（管理画面向け）
 * 
 * @version 1.0
 * @since 2025-01-08
 */
@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }

    /**
     * WebClient Bean
     * Web APIとの通信用クライアント
     * 
     * @return WebClient.Builder
     */
    @Bean
    public WebClient.Builder webClientBuilder() {
        return WebClient.builder();
    }
}
