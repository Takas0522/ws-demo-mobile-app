package com.example.webapi.entity;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;

/**
 * 機能フラグエンティティ
 * 
 * テーブル: feature_flags
 * 用途: 機能の有効/無効を管理するマスターデータ
 */
@Entity
@Table(name = "feature_flags")
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class FeatureFlag {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "flag_id")
    private Long flagId;
    
    @Column(name = "flag_key", nullable = false, unique = true, length = 50)
    private String flagKey;
    
    @Column(name = "flag_name", nullable = false, length = 100)
    private String flagName;
    
    @Column(name = "default_value", nullable = false)
    private Boolean defaultValue;
    
    @Column(name = "created_at", nullable = false, updatable = false)
    private LocalDateTime createdAt;
    
    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
    }
}
