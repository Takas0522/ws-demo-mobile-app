package com.example.mobileapp.utils

import android.content.Context
import android.content.SharedPreferences
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey

class SecureStorageManager(context: Context) {
    
    private val masterKey = MasterKey.Builder(context)
        .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
        .build()
    
    private val sharedPreferences: SharedPreferences = EncryptedSharedPreferences.create(
        context,
        "secure_prefs",
        masterKey,
        EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
        EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
    )
    
    companion object {
        private const val KEY_TOKEN = "jwt_token"
        
        @Volatile
        private var instance: SecureStorageManager? = null
        
        fun getInstance(context: Context): SecureStorageManager {
            return instance ?: synchronized(this) {
                instance ?: SecureStorageManager(context.applicationContext).also {
                    instance = it
                }
            }
        }
    }
    
    fun saveToken(token: String) {
        sharedPreferences.edit().putString(KEY_TOKEN, token).apply()
    }
    
    fun getToken(): String? {
        return sharedPreferences.getString(KEY_TOKEN, null)
    }
    
    fun deleteToken() {
        sharedPreferences.edit().remove(KEY_TOKEN).apply()
    }
    
    fun hasToken(): Boolean {
        return getToken() != null
    }
}
