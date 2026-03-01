package com.example.mobileapp.utils;

import android.content.Context;
import android.content.SharedPreferences;
import androidx.security.crypto.EncryptedSharedPreferences;
import androidx.security.crypto.MasterKey;

import java.io.IOException;
import java.security.GeneralSecurityException;

public class SecureStorageManager {
    private static final String KEY_TOKEN = "jwt_token";
    private static final String PREFS_NAME = "secure_prefs";
    
    private static volatile SecureStorageManager instance = null;
    private final SharedPreferences sharedPreferences;
    
    private SecureStorageManager(Context context) {
        try {
            MasterKey masterKey = new MasterKey.Builder(context)
                .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
                .build();
            
            sharedPreferences = EncryptedSharedPreferences.create(
                context,
                PREFS_NAME,
                masterKey,
                EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
                EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
            );
        } catch (GeneralSecurityException | IOException e) {
            throw new RuntimeException("Failed to create encrypted shared preferences", e);
        }
    }
    
    public static SecureStorageManager getInstance(Context context) {
        if (instance == null) {
            synchronized (SecureStorageManager.class) {
                if (instance == null) {
                    instance = new SecureStorageManager(context.getApplicationContext());
                }
            }
        }
        return instance;
    }
    
    public void saveToken(String token) {
        sharedPreferences.edit().putString(KEY_TOKEN, token).apply();
    }
    
    public String getToken() {
        return sharedPreferences.getString(KEY_TOKEN, null);
    }
    
    public void deleteToken() {
        sharedPreferences.edit().remove(KEY_TOKEN).apply();
    }
    
    public boolean hasToken() {
        return getToken() != null;
    }
}
