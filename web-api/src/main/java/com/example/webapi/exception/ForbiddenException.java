package com.example.webapi.exception;

/**
 * 権限なし例外
 * 
 * リソースへのアクセス権限がない場合にスローされる
 */
public class ForbiddenException extends RuntimeException {
    
    private final String errorCode;
    
    public ForbiddenException(String message) {
        super(message);
        this.errorCode = "FORBIDDEN_001";
    }
    
    public ForbiddenException(String message, String errorCode) {
        super(message);
        this.errorCode = errorCode;
    }
    
    public String getErrorCode() {
        return errorCode;
    }
}
