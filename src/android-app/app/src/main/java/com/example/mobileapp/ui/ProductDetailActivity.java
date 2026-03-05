package com.example.mobileapp.ui;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import com.example.mobileapp.R;
import com.example.mobileapp.databinding.ActivityProductDetailBinding;
import com.example.mobileapp.databinding.DialogPurchaseBinding;
import com.example.mobileapp.models.*;
import com.example.mobileapp.services.ApiClient;
import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import java.util.ArrayList;
import java.util.List;

public class ProductDetailActivity extends AppCompatActivity {
    
    private static final String TAG = "ProductDetailActivity";
    private ActivityProductDetailBinding binding;
    private int productId = 0;
    private Product product = null;
    private boolean isFavorite = false;
    private Integer favoriteId = null;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityProductDetailBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        
        productId = getIntent().getIntExtra("productId", 0);
        
        setupViews();
        loadProductDetail();
    }
    
    private void setupViews() {
        setSupportActionBar(binding.toolbar);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }
        
        binding.btnPurchase.setOnClickListener(v -> showPurchaseDialog());
        binding.btnFavorite.setOnClickListener(v -> toggleFavorite());
    }
    
    private void loadProductDetail() {
        setLoading(true);
        Log.d(TAG, "Loading product detail: productId=" + productId);

        ApiClient.getApiService().getProductDetail(productId).enqueue(new Callback<ProductDetailResponse>() {
            @Override
            public void onResponse(Call<ProductDetailResponse> call, Response<ProductDetailResponse> response) {
                setLoading(false);
                Log.d(TAG, "getProductDetail response: code=" + response.code() + ", url=" + call.request().url());

                if (response.isSuccessful() && response.body() != null) {
                    ProductDetail detail = response.body().getData();
                    if (detail != null) {
                        product = detail.getProduct();
                        isFavorite = detail.isFavorite();
                        
                        displayProduct(detail.getProduct());
                        updateFavoriteButton();
                        
                        // お気に入りIDを取得
                        if (isFavorite) {
                            loadFavoriteId();
                        }
                        
                        binding.tvError.setVisibility(View.GONE);
                    }
                } else {
                    showError("商品詳細の読み込みに失敗しました");
                }
            }
            
            @Override
            public void onFailure(Call<ProductDetailResponse> call, Throwable t) {
                setLoading(false);
                Log.e(TAG, "getProductDetail network error: url=" + call.request().url() + ", error=" + t.getClass().getSimpleName() + " - " + t.getMessage(), t);
                showError("ネットワークエラーが発生しました");
            }
        });
    }
    
    private void displayProduct(Product product) {
        binding.tvProductName.setText(product.getProductName());
        binding.tvUnitPrice.setText("¥" + product.getUnitPrice());
        binding.tvDescription.setText(
            product.getDescription() != null ? product.getDescription() : "商品説明はありません"
        );
    }
    
    private void updateFavoriteButton() {
        // TODO: 機能フラグチェック（簡略化のため省略）
        binding.btnFavorite.setText(isFavorite ? "お気に入り済み" : "お気に入りに追加");
        binding.btnFavorite.setIconResource(
            isFavorite ? R.drawable.ic_favorite : R.drawable.ic_favorite_border
        );
    }
    
    private void showPurchaseDialog() {
        if (this.product == null) return;
        
        final Product currentProduct = this.product;
        DialogPurchaseBinding dialogBinding = DialogPurchaseBinding.inflate(getLayoutInflater());
        
        // 数量スピナー設定
        List<Integer> quantities = new ArrayList<>();
        for (int i = 1; i <= 99; i++) {
            quantities.add(i * 100);
        }
        
        ArrayAdapter<Integer> adapter = new ArrayAdapter<>(
            this, 
            android.R.layout.simple_spinner_item, 
            quantities
        );
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        dialogBinding.spinnerQuantity.setAdapter(adapter);
        
        // 合計金額の更新
        dialogBinding.spinnerQuantity.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                int quantity = quantities.get(position);
                int total = currentProduct.getUnitPrice() * quantity;
                dialogBinding.tvTotalAmount.setText("¥" + total);
            }
            
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });
        
        // 初期値設定
        dialogBinding.tvProductName.setText(currentProduct.getProductName());
        dialogBinding.tvUnitPrice.setText("¥" + currentProduct.getUnitPrice());
        dialogBinding.tvTotalAmount.setText("¥" + (currentProduct.getUnitPrice() * 1));

        new AlertDialog.Builder(this)
            .setTitle("購入確認")
            .setView(dialogBinding.getRoot())
            .setPositiveButton("購入確定", (dialog, which) -> {
                int selectedQuantity = quantities.get(dialogBinding.spinnerQuantity.getSelectedItemPosition());
                purchaseProduct(selectedQuantity);
            })
            .setNegativeButton("キャンセル", null)
            .show();
    }
    
    private void purchaseProduct(int quantity) {
        binding.progressBar.setVisibility(View.VISIBLE);
        
        PurchaseRequest request = new PurchaseRequest(productId, quantity);
        ApiClient.getApiService().purchaseProduct(request).enqueue(new Callback<PurchaseResponse>() {
            @Override
            public void onResponse(Call<PurchaseResponse> call, Response<PurchaseResponse> response) {
                binding.progressBar.setVisibility(View.GONE);
                
                if (response.isSuccessful()) {
                    new AlertDialog.Builder(ProductDetailActivity.this)
                        .setTitle("購入完了")
                        .setMessage("ご購入ありがとうございました")
                        .setPositiveButton("OK", (dialog, which) -> finish())
                        .show();
                } else {
                    Toast.makeText(
                        ProductDetailActivity.this,
                        "購入に失敗しました",
                        Toast.LENGTH_SHORT
                    ).show();
                }
            }
            
            @Override
            public void onFailure(Call<PurchaseResponse> call, Throwable t) {
                binding.progressBar.setVisibility(View.GONE);
                Log.e(TAG, "purchaseProduct network error: " + t.getClass().getSimpleName() + " - " + t.getMessage(), t);
                Toast.makeText(
                    ProductDetailActivity.this,
                    "ネットワークエラーが発生しました",
                    Toast.LENGTH_SHORT
                ).show();
            }
        });
    }
    
    private void toggleFavorite() {
        binding.progressBar.setVisibility(View.VISIBLE);
        
        if (isFavorite) {
            // お気に入り解除
            if (favoriteId != null) {
                ApiClient.getApiService().removeFavorite(favoriteId).enqueue(new Callback<ResponseBody>() {
                    @Override
                    public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                        binding.progressBar.setVisibility(View.GONE);
                        
                        if (response.isSuccessful()) {
                            isFavorite = false;
                            favoriteId = null;
                            updateFavoriteButton();
                            Toast.makeText(
                                ProductDetailActivity.this,
                                "お気に入りを解除しました",
                                Toast.LENGTH_SHORT
                            ).show();
                        }
                    }
                    
                    @Override
                    public void onFailure(Call<ResponseBody> call, Throwable t) {
                        binding.progressBar.setVisibility(View.GONE);
                        Log.e(TAG, "removeFavorite network error: " + t.getClass().getSimpleName() + " - " + t.getMessage(), t);
                        Toast.makeText(
                            ProductDetailActivity.this,
                            "処理に失敗しました",
                            Toast.LENGTH_SHORT
                        ).show();
                    }
                });
            }
        } else {
            // お気に入り登録
            FavoriteRequest request = new FavoriteRequest(productId);
            ApiClient.getApiService().addFavorite(request).enqueue(new Callback<FavoriteResponse>() {
                @Override
                public void onResponse(Call<FavoriteResponse> call, Response<FavoriteResponse> response) {
                    binding.progressBar.setVisibility(View.GONE);
                    
                    if (response.isSuccessful() && response.body() != null) {
                        Favorite favorite = response.body().getData();
                        isFavorite = true;
                        favoriteId = favorite != null ? favorite.getFavoriteId() : null;
                        updateFavoriteButton();
                        Toast.makeText(
                            ProductDetailActivity.this,
                            "お気に入りに追加しました",
                            Toast.LENGTH_SHORT
                        ).show();
                    }
                }
                
                @Override
                public void onFailure(Call<FavoriteResponse> call, Throwable t) {
                    binding.progressBar.setVisibility(View.GONE);
                    Log.e(TAG, "addFavorite network error: " + t.getClass().getSimpleName() + " - " + t.getMessage(), t);
                    Toast.makeText(
                        ProductDetailActivity.this,
                        "処理に失敗しました",
                        Toast.LENGTH_SHORT
                    ).show();
                }
            });
        }
    }
    
    private void loadFavoriteId() {
        ApiClient.getApiService().getFavorites().enqueue(new Callback<FavoriteListResponse>() {
            @Override
            public void onResponse(Call<FavoriteListResponse> call, Response<FavoriteListResponse> response) {
                if (response.isSuccessful() && response.body() != null) {
                    List<Favorite> favorites = response.body().getData();
                    if (favorites != null) {
                        for (Favorite favorite : favorites) {
                            if (favorite.getProductId() == productId) {
                                favoriteId = favorite.getFavoriteId();
                                break;
                            }
                        }
                    }
                }
            }
            
            @Override
            public void onFailure(Call<FavoriteListResponse> call, Throwable t) {
                Log.e(TAG, "getFavorites network error: " + t.getClass().getSimpleName() + " - " + t.getMessage(), t);
            }
        });
    }
    
    private void setLoading(boolean isLoading) {
        binding.progressBar.setVisibility(isLoading ? View.VISIBLE : View.GONE);
        binding.contentLayout.setVisibility(isLoading ? View.GONE : View.VISIBLE);
    }
    
    private void showError(String message) {
        binding.tvError.setText(message);
        binding.tvError.setVisibility(View.VISIBLE);
        binding.contentLayout.setVisibility(View.GONE);
    }
    
    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return true;
    }
}
