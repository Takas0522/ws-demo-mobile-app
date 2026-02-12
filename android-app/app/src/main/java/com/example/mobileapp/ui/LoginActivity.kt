package com.example.mobileapp.ui

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.example.mobileapp.databinding.ActivityLoginBinding
import com.example.mobileapp.models.LoginRequest
import com.example.mobileapp.services.ApiClient
import com.example.mobileapp.utils.SecureStorageManager
import kotlinx.coroutines.launch

class LoginActivity : AppCompatActivity() {
    
    private lateinit var binding: ActivityLoginBinding
    private lateinit var secureStorage: SecureStorageManager
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityLoginBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        secureStorage = SecureStorageManager.getInstance(this)
        
        // すでにログイン済みの場合
        if (secureStorage.hasToken()) {
            navigateToMain()
            return
        }
        
        setupViews()
    }
    
    private fun setupViews() {
        binding.btnLogin.setOnClickListener {
            val loginId = binding.etLoginId.text.toString().trim()
            val password = binding.etPassword.text.toString().trim()
            
            when {
                loginId.isEmpty() -> {
                    binding.tilLoginId.error = "ユーザーIDを入力してください"
                }
                password.isEmpty() -> {
                    binding.tilPassword.error = "パスワードを入力してください"
                }
                else -> {
                    binding.tilLoginId.error = null
                    binding.tilPassword.error = null
                    performLogin(loginId, password)
                }
            }
        }
    }
    
    private fun performLogin(loginId: String, password: String) {
        setLoading(true)
        
        lifecycleScope.launch {
            try {
                val request = LoginRequest(loginId, password)
                val response = ApiClient.getApiService().login(request)
                
                if (response.isSuccessful) {
                    val loginData = response.body()?.data
                    if (loginData != null) {
                        // トークンを保存
                        secureStorage.saveToken(loginData.token)
                        
                        Toast.makeText(
                            this@LoginActivity,
                            "ログインしました",
                            Toast.LENGTH_SHORT
                        ).show()
                        
                        navigateToMain()
                    } else {
                        showError("ログインに失敗しました")
                    }
                } else {
                    when (response.code()) {
                        401 -> showError("ユーザーIDまたはパスワードが正しくありません")
                        else -> showError("ログインに失敗しました (${response.code()})")
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
                showError("ネットワークエラーが発生しました")
            } finally {
                setLoading(false)
            }
        }
    }
    
    private fun setLoading(isLoading: Boolean) {
        binding.progressBar.visibility = if (isLoading) View.VISIBLE else View.GONE
        binding.btnLogin.isEnabled = !isLoading
        binding.etLoginId.isEnabled = !isLoading
        binding.etPassword.isEnabled = !isLoading
    }
    
    private fun showError(message: String) {
        binding.tvError.text = message
        binding.tvError.visibility = View.VISIBLE
    }
    
    private fun navigateToMain() {
        val intent = Intent(this, MainActivity::class.java)
        intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        startActivity(intent)
        finish()
    }
}
