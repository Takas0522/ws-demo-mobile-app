package com.example.webapi.exception;

/**
 * バリデーション例外
 * 
 * 入力値のバリデーションエラー時にスローされる例外
 */
public class ValidationException extends RuntimeException {
    
    private final String code;
    private final String details;
    
    public ValidationException(String message) {
        super(message);
        this.code = "VALIDATION_001";
        this.details = null;
    }
    
    public ValidationException(String message, String details) {
        super(message);
        this.code = "VALIDATION_001";
        this.details = details;
    }
    
    public ValidationException(String code, String message, String details) {
        super(message);
        this.code = code;
        this.details = details;
    }
    
    public String getCode() {
        return code;
    }
    
    public String getDetails() {
        return details;
    }
}
