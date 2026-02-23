package com.example.webapi.exception;

/**
 * リソース未発見例外
 * 
 * 指定されたリソースが見つからない場合にスローされる
 */
public class ResourceNotFoundException extends RuntimeException {
    
    private final String errorCode;
    
    public ResourceNotFoundException(String message) {
        super(message);
        this.errorCode = "NOT_FOUND_001";
    }
    
    public ResourceNotFoundException(String message, String errorCode) {
        super(message);
        this.errorCode = errorCode;
    }
    
    public String getErrorCode() {
        return errorCode;
    }
}
