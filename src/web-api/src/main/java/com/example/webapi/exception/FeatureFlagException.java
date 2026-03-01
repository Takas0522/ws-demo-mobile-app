package com.example.webapi.exception;

/**
 * 機能フラグ例外
 * 
 * 機能フラグチェックエラー時にスローされる例外
 */
public class FeatureFlagException extends RuntimeException {
    
    private final String code;
    
    public FeatureFlagException(String message) {
        super(message);
        this.code = "FEATURE_001";
    }
    
    public FeatureFlagException(String code, String message) {
        super(message);
        this.code = code;
    }
    
    public String getCode() {
        return code;
    }
}
