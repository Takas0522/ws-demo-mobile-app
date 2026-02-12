package com.example.mobileapp.ui

import android.os.Bundle
import android.view.View
import android.widget.ArrayAdapter
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.example.mobileapp.R
import com.example.mobileapp.databinding.ActivityProductDetailBinding
import com.example.mobileapp.databinding.DialogPurchaseBinding
import com.example.mobileapp.models.FavoriteRequest
import com.example.mobileapp.models.Product
import com.example.mobileapp.models.PurchaseRequest
import com.example.mobileapp.services.ApiClient
import kotlinx.coroutines.launch

class ProductDetailActivity : AppCompatActivity() {
    
    private lateinit var binding: ActivityProductDetailBinding
    private var productId: Int = 0
    private var product: Product? = null
    private var isFavorite = false
    private var favoriteId: Int? = null
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityProductDetailBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        productId = intent.getIntExtra("productId", 0)
        
        setupViews()
        loadProductDetail()
    }
    
    private fun setupViews() {
        setSupportActionBar(binding.toolbar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        
        binding.btnPurchase.setOnClickListener {
            showPurchaseDialog()
        }
        
        binding.btnFavorite.setOnClickListener {
            toggleFavorite()
        }
    }
    
    private fun loadProductDetail() {
        setLoading(true)
        
        lifecycleScope.launch {
            try {
                val response = ApiClient.getApiService().getProductDetail(productId)
                
                if (response.isSuccessful) {
                    val detail = response.body()?.data
                    if (detail != null) {
                        product = detail.product
                        isFavorite = detail.isFavorite
                        
                        displayProduct(detail.product)
                        updateFavoriteButton()
                        
                        // お気に入りIDを取得
                        if (isFavorite) {
                            loadFavoriteId()
                        }
                        
                        binding.tvError.visibility = View.GONE
                    }
                } else {
                    showError("商品詳細の読み込みに失敗しました")
                }
            } catch (e: Exception) {
                e.printStackTrace()
                showError("ネットワークエラーが発生しました")
            } finally {
                setLoading(false)
            }
        }
    }
    
    private fun displayProduct(product: Product) {
        binding.tvProductName.text = product.productName
        binding.tvUnitPrice.text = "¥${product.unitPrice}"
        binding.tvDescription.text = product.description ?: "商品説明はありません"
    }
    
    private fun updateFavoriteButton() {
        // TODO: 機能フラグチェック（簡略化のため省略）
        binding.btnFavorite.text = if (isFavorite) "お気に入り済み" else "お気に入りに追加"
        binding.btnFavorite.setIconResource(
            if (isFavorite) R.drawable.ic_favorite else R.drawable.ic_favorite_border
        )
    }
    
    private fun showPurchaseDialog() {
        val product = this.product ?: return
        
        val dialogBinding = DialogPurchaseBinding.inflate(layoutInflater)
        
        // 数量スピナー設定
        val quantities = (1..99).map { it * 100 }
        val adapter = ArrayAdapter(this, android.R.layout.simple_spinner_item, quantities)
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        dialogBinding.spinnerQuantity.adapter = adapter
        
        // 合計金額の更新
        dialogBinding.spinnerQuantity.setOnItemSelectedListener(
            object : android.widget.AdapterView.OnItemSelectedListener {
                override fun onItemSelected(
                    parent: android.widget.AdapterView<*>?,
                    view: View?,
                    position: Int,
                    id: Long
                ) {
                    val quantity = quantities[position]
                    val total = product.unitPrice * quantity
                    dialogBinding.tvTotalAmount.text = "¥$total"
                }
                
                override fun onNothingSelected(parent: android.widget.AdapterView<*>?) {}
            }
        )
        
        // 初期値設定
        dialogBinding.tvProductName.text = product.productName
        dialogBinding.tvUnitPrice.text = "¥${product.unitPrice}"
        dialogBinding.tvTotalAmount.text = "¥${product.unitPrice * 100}"
        
        AlertDialog.Builder(this)
            .setTitle("購入確認")
            .setView(dialogBinding.root)
            .setPositiveButton("購入確定") { _, _ ->
                val selectedQuantity = quantities[dialogBinding.spinnerQuantity.selectedItemPosition]
                purchaseProduct(selectedQuantity)
            }
            .setNegativeButton("キャンセル", null)
            .show()
    }
    
    private fun purchaseProduct(quantity: Int) {
        binding.progressBar.visibility = View.VISIBLE
        
        lifecycleScope.launch {
            try {
                val request = PurchaseRequest(productId, quantity)
                val response = ApiClient.getApiService().purchaseProduct(request)
                
                if (response.isSuccessful) {
                    AlertDialog.Builder(this@ProductDetailActivity)
                        .setTitle("購入完了")
                        .setMessage("ご購入ありがとうございました")
                        .setPositiveButton("OK") { _, _ ->
                            finish()
                        }
                        .show()
                } else {
                    Toast.makeText(
                        this@ProductDetailActivity,
                        "購入に失敗しました",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            } catch (e: Exception) {
                e.printStackTrace()
                Toast.makeText(
                    this@ProductDetailActivity,
                    "ネットワークエラーが発生しました",
                    Toast.LENGTH_SHORT
                ).show()
            } finally {
                binding.progressBar.visibility = View.GONE
            }
        }
    }
    
    private fun toggleFavorite() {
        binding.progressBar.visibility = View.VISIBLE
        
        lifecycleScope.launch {
            try {
                if (isFavorite) {
                    // お気に入り解除
                    favoriteId?.let { id ->
                        val response = ApiClient.getApiService().removeFavorite(id)
                        if (response.isSuccessful) {
                            isFavorite = false
                            favoriteId = null
                            updateFavoriteButton()
                            Toast.makeText(
                                this@ProductDetailActivity,
                                "お気に入りを解除しました",
                                Toast.LENGTH_SHORT
                            ).show()
                        }
                    }
                } else {
                    // お気に入り登録
                    val request = FavoriteRequest(productId)
                    val response = ApiClient.getApiService().addFavorite(request)
                    if (response.isSuccessful) {
                        val favorite = response.body()?.data
                        isFavorite = true
                        favoriteId = favorite?.favoriteId
                        updateFavoriteButton()
                        Toast.makeText(
                            this@ProductDetailActivity,
                            "お気に入りに追加しました",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
                Toast.makeText(
                    this@ProductDetailActivity,
                    "処理に失敗しました",
                    Toast.LENGTH_SHORT
                ).show()
            } finally {
                binding.progressBar.visibility = View.GONE
            }
        }
    }
    
    private fun loadFavoriteId() {
        lifecycleScope.launch {
            try {
                val response = ApiClient.getApiService().getFavorites()
                if (response.isSuccessful) {
                    val favorites = response.body()?.data ?: emptyList()
                    favoriteId = favorites.find { it.productId == productId }?.favoriteId
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }
    
    private fun setLoading(isLoading: Boolean) {
        binding.progressBar.visibility = if (isLoading) View.VISIBLE else View.GONE
        binding.contentLayout.visibility = if (isLoading) View.GONE else View.VISIBLE
    }
    
    private fun showError(message: String) {
        binding.tvError.text = message
        binding.tvError.visibility = View.VISIBLE
        binding.contentLayout.visibility = View.GONE
    }
    
    override fun onSupportNavigateUp(): Boolean {
        finish()
        return true
    }
}
