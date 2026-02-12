package com.example.mobileapp.ui

import android.content.Intent
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.SearchView
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mobileapp.R
import com.example.mobileapp.databinding.ActivityMainBinding
import com.example.mobileapp.models.Product
import com.example.mobileapp.services.ApiClient
import com.example.mobileapp.utils.SecureStorageManager
import kotlinx.coroutines.launch

class MainActivity : AppCompatActivity() {
    
    private lateinit var binding: ActivityMainBinding
    private lateinit var secureStorage: SecureStorageManager
    private lateinit var productAdapter: ProductAdapter
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        secureStorage = SecureStorageManager.getInstance(this)
        
        // ログインチェック
        if (!secureStorage.hasToken()) {
            navigateToLogin()
            return
        }
        
        setupViews()
        loadProducts()
    }
    
    private fun setupViews() {
        setSupportActionBar(binding.toolbar)
        
        // RecyclerView setup
        productAdapter = ProductAdapter { product ->
            navigateToProductDetail(product.productId)
        }
        
        binding.rvProducts.apply {
            layoutManager = LinearLayoutManager(this@MainActivity)
            adapter = productAdapter
        }
        
        // SwipeRefreshLayout setup
        binding.swipeRefresh.setOnRefreshListener {
            loadProducts()
        }
        
        // SearchView setup
        binding.searchView.setOnQueryTextListener(object : SearchView.OnQueryTextListener {
            override fun onQueryTextSubmit(query: String?): Boolean {
                searchProducts(query ?: "")
                return true
            }
            
            override fun onQueryTextChange(newText: String?): Boolean {
                if (newText.isNullOrEmpty()) {
                    loadProducts()
                }
                return true
            }
        })
    }
    
    private fun loadProducts() {
        setLoading(true)
        
        lifecycleScope.launch {
            try {
                val response = ApiClient.getApiService().getProducts()
                
                if (response.isSuccessful) {
                    val products = response.body()?.data ?: emptyList()
                    productAdapter.submitList(products)
                    
                    binding.tvEmpty.visibility = if (products.isEmpty()) View.VISIBLE else View.GONE
                    binding.tvError.visibility = View.GONE
                } else {
                    showError("商品の読み込みに失敗しました")
                }
            } catch (e: Exception) {
                e.printStackTrace()
                showError("ネットワークエラーが発生しました")
            } finally {
                setLoading(false)
            }
        }
    }
    
    private fun searchProducts(keyword: String) {
        if (keyword.isEmpty()) {
            loadProducts()
            return
        }
        
        setLoading(true)
        
        lifecycleScope.launch {
            try {
                val response = ApiClient.getApiService().searchProducts(keyword)
                
                if (response.isSuccessful) {
                    val products = response.body()?.data ?: emptyList()
                    productAdapter.submitList(products)
                    
                    binding.tvEmpty.text = if (products.isEmpty()) {
                        "該当する商品がありません"
                    } else {
                        "商品がありません"
                    }
                    binding.tvEmpty.visibility = if (products.isEmpty()) View.VISIBLE else View.GONE
                    binding.tvError.visibility = View.GONE
                } else {
                    showError("検索に失敗しました")
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
        binding.swipeRefresh.isRefreshing = isLoading
        binding.progressBar.visibility = if (isLoading && !binding.swipeRefresh.isRefreshing) {
            View.VISIBLE
        } else {
            View.GONE
        }
    }
    
    private fun showError(message: String) {
        binding.tvError.text = message
        binding.tvError.visibility = View.VISIBLE
        binding.tvEmpty.visibility = View.GONE
    }
    
    private fun navigateToProductDetail(productId: Int) {
        val intent = Intent(this, ProductDetailActivity::class.java)
        intent.putExtra("productId", productId)
        startActivity(intent)
    }
    
    private fun navigateToLogin() {
        val intent = Intent(this, LoginActivity::class.java)
        intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        startActivity(intent)
        finish()
    }
    
    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }
    
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.action_logout -> {
                performLogout()
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }
    
    private fun performLogout() {
        secureStorage.deleteToken()
        Toast.makeText(this, "ログアウトしました", Toast.LENGTH_SHORT).show()
        navigateToLogin()
    }
}
