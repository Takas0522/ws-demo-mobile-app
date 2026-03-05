package com.example.mobileapp.services;

import android.content.Context;
import android.util.Log;
import com.example.mobileapp.utils.SecureStorageManager;
import okhttp3.Interceptor;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.logging.HttpLoggingInterceptor;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

public class ApiClient {
    private static final String TAG = "ApiClient";
    private static final String BASE_URL = "http://10.0.2.2:8081"; // Mobile BFF (10.0.2.2 = host PC from Android emulator)
    
    private static Retrofit retrofit = null;
    private static SecureStorageManager secureStorage = null;
    
    public static void initialize(Context context) {
        secureStorage = SecureStorageManager.getInstance(context);
        Log.d(TAG, "ApiClient initialized. BASE_URL=" + BASE_URL);
    }
    
    private static Interceptor getAuthInterceptor() {
        return new Interceptor() {
            @Override
            public Response intercept(Chain chain) throws IOException {
                Request originalRequest = chain.request();
                String token = secureStorage != null ? secureStorage.getToken() : null;
                
                Log.d(TAG, "Request: " + originalRequest.method() + " " + originalRequest.url());

                Request request;
                if (token != null && !originalRequest.url().encodedPath().contains("/login")) {
                    request = originalRequest.newBuilder()
                        .header("Authorization", "Bearer " + token)
                        .build();
                    Log.d(TAG, "Authorization header added");
                } else {
                    request = originalRequest;
                    Log.d(TAG, "No authorization header (token=" + (token != null ? "exists" : "null") + ")");
                }
                
                try {
                    Response response = chain.proceed(request);
                    Log.d(TAG, "Response: " + response.code() + " " + response.message() + " for " + request.url());
                    return response;
                } catch (IOException e) {
                    Log.e(TAG, "Network error for " + request.url() + ": " + e.getClass().getSimpleName() + " - " + e.getMessage());
                    throw e;
                }
            }
        };
    }
    
    private static HttpLoggingInterceptor getLoggingInterceptor() {
        HttpLoggingInterceptor interceptor = new HttpLoggingInterceptor(
            message -> Log.d("OkHttp", message)
        );
        interceptor.setLevel(HttpLoggingInterceptor.Level.BODY);
        return interceptor;
    }
    
    private static OkHttpClient getOkHttpClient() {
        return new OkHttpClient.Builder()
            .addInterceptor(getAuthInterceptor())
            .addInterceptor(getLoggingInterceptor())
            .connectTimeout(10, TimeUnit.SECONDS)
            .readTimeout(10, TimeUnit.SECONDS)
            .writeTimeout(10, TimeUnit.SECONDS)
            .build();
    }
    
    private static Retrofit getRetrofit() {
        if (retrofit == null) {
            retrofit = new Retrofit.Builder()
                .baseUrl(BASE_URL)
                .client(getOkHttpClient())
                .addConverterFactory(GsonConverterFactory.create())
                .build();
        }
        return retrofit;
    }
    
    public static ApiService getApiService() {
        return getRetrofit().create(ApiService.class);
    }
}
