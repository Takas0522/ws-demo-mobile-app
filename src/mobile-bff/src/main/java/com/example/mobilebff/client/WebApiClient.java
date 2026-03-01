package com.example.mobilebff.client;

import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.stereotype.Component;
import org.springframework.web.reactive.function.client.WebClient;
import org.springframework.web.reactive.function.client.WebClientResponseException;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import reactor.core.publisher.Mono;

/**
 * Web APIクライアント
 * 
 * Web APIへのHTTPリクエストを実行する共通クライアント
 */
@Component
@RequiredArgsConstructor
@Slf4j
public class WebApiClient {
    
    private final WebClient webClient;
    
    /**
     * GET リクエスト
     * 
     * @param path リクエストパス
     * @param responseType レスポンス型
     * @param authToken 認証トークン（オプション）
     * @return レスポンスボディ
     */
    public <T> T get(String path, Class<T> responseType, String authToken) {
        log.debug("GET request: path={}", path);
        
        try {
            return webClient.get()
                .uri(path)
                .headers(headers -> {
                    if (authToken != null && !authToken.isEmpty()) {
                        headers.set(HttpHeaders.AUTHORIZATION, "Bearer " + authToken);
                    }
                })
                .retrieve()
                .bodyToMono(responseType)
                .block();
        } catch (WebClientResponseException e) {
            log.error("GET request failed: path={}, status={}, body={}", 
                path, e.getStatusCode(), e.getResponseBodyAsString());
            throw e;
        }
    }
    
    /**
     * POST リクエスト
     * 
     * @param path リクエストパス
     * @param requestBody リクエストボディ
     * @param responseType レスポンス型
     * @param authToken 認証トークン（オプション）
     * @return レスポンスボディ
     */
    public <T, R> R post(String path, T requestBody, Class<R> responseType, String authToken) {
        log.debug("POST request: path={}", path);
        
        try {
            return webClient.post()
                .uri(path)
                .headers(headers -> {
                    headers.setContentType(MediaType.APPLICATION_JSON);
                    if (authToken != null && !authToken.isEmpty()) {
                        headers.set(HttpHeaders.AUTHORIZATION, "Bearer " + authToken);
                    }
                })
                .bodyValue(requestBody)
                .retrieve()
                .bodyToMono(responseType)
                .block();
        } catch (WebClientResponseException e) {
            log.error("POST request failed: path={}, status={}, body={}", 
                path, e.getStatusCode(), e.getResponseBodyAsString());
            throw e;
        }
    }
    
    /**
     * PUT リクエスト
     * 
     * @param path リクエストパス
     * @param requestBody リクエストボディ
     * @param responseType レスポンス型
     * @param authToken 認証トークン（オプション）
     * @return レスポンスボディ
     */
    public <T, R> R put(String path, T requestBody, Class<R> responseType, String authToken) {
        log.debug("PUT request: path={}", path);
        
        try {
            return webClient.put()
                .uri(path)
                .headers(headers -> {
                    headers.setContentType(MediaType.APPLICATION_JSON);
                    if (authToken != null && !authToken.isEmpty()) {
                        headers.set(HttpHeaders.AUTHORIZATION, "Bearer " + authToken);
                    }
                })
                .bodyValue(requestBody)
                .retrieve()
                .bodyToMono(responseType)
                .block();
        } catch (WebClientResponseException e) {
            log.error("PUT request failed: path={}, status={}, body={}", 
                path, e.getStatusCode(), e.getResponseBodyAsString());
            throw e;
        }
    }
    
    /**
     * DELETE リクエスト
     * 
     * @param path リクエストパス
     * @param responseType レスポンス型
     * @param authToken 認証トークン（オプション）
     * @return レスポンスボディ
     */
    public <T> T delete(String path, Class<T> responseType, String authToken) {
        log.debug("DELETE request: path={}", path);
        
        try {
            return webClient.delete()
                .uri(path)
                .headers(headers -> {
                    if (authToken != null && !authToken.isEmpty()) {
                        headers.set(HttpHeaders.AUTHORIZATION, "Bearer " + authToken);
                    }
                })
                .retrieve()
                .bodyToMono(responseType)
                .block();
        } catch (WebClientResponseException e) {
            log.error("DELETE request failed: path={}, status={}, body={}", 
                path, e.getStatusCode(), e.getResponseBodyAsString());
            throw e;
        }
    }
}
