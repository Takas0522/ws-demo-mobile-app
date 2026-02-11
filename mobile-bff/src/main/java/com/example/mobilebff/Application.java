package com.example.mobilebff;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;
import org.springframework.web.reactive.function.client.WebClient;

/**
 * Mobile BFF Application
 * 
 * Mobile App System の BFF Layer（モバイルアプリ向け）
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
