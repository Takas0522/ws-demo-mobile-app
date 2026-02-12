package com.example.mobileapp.services

import android.content.Context
import com.example.mobileapp.utils.SecureStorageManager
import okhttp3.Interceptor
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import java.util.concurrent.TimeUnit

object ApiClient {
    private const val BASE_URL = "http://10.0.2.2:8081" // Android emulator localhost
    
    private var retrofit: Retrofit? = null
    private var secureStorage: SecureStorageManager? = null
    
    fun initialize(context: Context) {
        secureStorage = SecureStorageManager.getInstance(context)
    }
    
    private fun getAuthInterceptor(): Interceptor {
        return Interceptor { chain ->
            val originalRequest = chain.request()
            val token = secureStorage?.getToken()
            
            val request = if (token != null && !originalRequest.url.encodedPath.contains("/login")) {
                originalRequest.newBuilder()
                    .header("Authorization", "Bearer $token")
                    .build()
            } else {
                originalRequest
            }
            
            chain.proceed(request)
        }
    }
    
    private fun getLoggingInterceptor(): HttpLoggingInterceptor {
        return HttpLoggingInterceptor().apply {
            level = HttpLoggingInterceptor.Level.BODY
        }
    }
    
    private fun getOkHttpClient(): OkHttpClient {
        return OkHttpClient.Builder()
            .addInterceptor(getAuthInterceptor())
            .addInterceptor(getLoggingInterceptor())
            .connectTimeout(10, TimeUnit.SECONDS)
            .readTimeout(10, TimeUnit.SECONDS)
            .writeTimeout(10, TimeUnit.SECONDS)
            .build()
    }
    
    private fun getRetrofit(): Retrofit {
        if (retrofit == null) {
            retrofit = Retrofit.Builder()
                .baseUrl(BASE_URL)
                .client(getOkHttpClient())
                .addConverterFactory(GsonConverterFactory.create())
                .build()
        }
        return retrofit!!
    }
    
    fun getApiService(): ApiService {
        return getRetrofit().create(ApiService::class.java)
    }
}
