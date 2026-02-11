package com.example.mobilebff.config;

import java.time.Duration;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.client.reactive.ReactorClientHttpConnector;
import org.springframework.web.reactive.function.client.WebClient;

import io.netty.channel.ChannelOption;
import io.netty.handler.timeout.ReadTimeoutHandler;
import io.netty.handler.timeout.WriteTimeoutHandler;
import lombok.RequiredArgsConstructor;
import reactor.netty.http.client.HttpClient;

import java.util.concurrent.TimeUnit;

/**
 * WebClient設定
 * 
 * Web APIとの通信に使用するWebClientを設定
 */
@Configuration
@RequiredArgsConstructor
public class WebClientConfig {
    
    private final WebApiConfig webApiConfig;
    
    /**
     * Web API用WebClient
     * 
     * @return WebClient
     */
    @Bean
    public WebClient webClient() {
        HttpClient httpClient = HttpClient.create()
            .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, webApiConfig.getTimeout().getConnect())
            .responseTimeout(Duration.ofMillis(webApiConfig.getTimeout().getRead()))
            .doOnConnected(conn -> 
                conn.addHandlerLast(new ReadTimeoutHandler(webApiConfig.getTimeout().getRead(), TimeUnit.MILLISECONDS))
                    .addHandlerLast(new WriteTimeoutHandler(webApiConfig.getTimeout().getRead(), TimeUnit.MILLISECONDS)));
        
        return WebClient.builder()
            .baseUrl(webApiConfig.getBaseUrl())
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .build();
    }
}
