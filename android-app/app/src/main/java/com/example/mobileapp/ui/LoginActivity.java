package com.example.mobileapp.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import com.example.mobileapp.databinding.ActivityLoginBinding;
import com.example.mobileapp.models.LoginData;
import com.example.mobileapp.models.LoginRequest;
import com.example.mobileapp.models.LoginResponse;
import com.example.mobileapp.services.ApiClient;
import com.example.mobileapp.utils.SecureStorageManager;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class LoginActivity extends AppCompatActivity {
    
    private ActivityLoginBinding binding;
    private SecureStorageManager secureStorage;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityLoginBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        
        secureStorage = SecureStorageManager.getInstance(this);
        
        // すでにログイン済みの場合
        if (secureStorage.hasToken()) {
            navigateToMain();
            return;
        }
        
        setupViews();
    }
    
    private void setupViews() {
        binding.btnLogin.setOnClickListener(v -> {
            String loginId = binding.etLoginId.getText().toString().trim();
            String password = binding.etPassword.getText().toString().trim();
            
            if (loginId.isEmpty()) {
                binding.tilLoginId.setError("ユーザーIDを入力してください");
            } else if (password.isEmpty()) {
                binding.tilPassword.setError("パスワードを入力してください");
            } else {
                binding.tilLoginId.setError(null);
                binding.tilPassword.setError(null);
                performLogin(loginId, password);
            }
        });
    }
    
    private void performLogin(String loginId, String password) {
        setLoading(true);
        
        LoginRequest request = new LoginRequest(loginId, password);
        ApiClient.getApiService().login(request).enqueue(new Callback<LoginResponse>() {
            @Override
            public void onResponse(Call<LoginResponse> call, Response<LoginResponse> response) {
                setLoading(false);
                
                if (response.isSuccessful() && response.body() != null) {
                    LoginData loginData = response.body().getData();
                    if (loginData != null) {
                        // トークンを保存
                        secureStorage.saveToken(loginData.getToken());
                        
                        Toast.makeText(
                            LoginActivity.this,
                            "ログインしました",
                            Toast.LENGTH_SHORT
                        ).show();
                        
                        navigateToMain();
                    } else {
                        showError("ログインに失敗しました");
                    }
                } else {
                    if (response.code() == 401) {
                        showError("ユーザーIDまたはパスワードが正しくありません");
                    } else {
                        showError("ログインに失敗しました (" + response.code() + ")");
                    }
                }
            }
            
            @Override
            public void onFailure(Call<LoginResponse> call, Throwable t) {
                setLoading(false);
                t.printStackTrace();
                showError("ネットワークエラーが発生しました");
            }
        });
    }
    
    private void setLoading(boolean isLoading) {
        binding.progressBar.setVisibility(isLoading ? View.VISIBLE : View.GONE);
        binding.btnLogin.setEnabled(!isLoading);
        binding.etLoginId.setEnabled(!isLoading);
        binding.etPassword.setEnabled(!isLoading);
    }
    
    private void showError(String message) {
        binding.tvError.setText(message);
        binding.tvError.setVisibility(View.VISIBLE);
    }
    
    private void navigateToMain() {
        Intent intent = new Intent(this, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        finish();
    }
}
