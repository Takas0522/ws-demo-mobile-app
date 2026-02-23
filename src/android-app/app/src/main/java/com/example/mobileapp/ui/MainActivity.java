package com.example.mobileapp.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SearchView;
import androidx.recyclerview.widget.LinearLayoutManager;
import com.example.mobileapp.R;
import com.example.mobileapp.databinding.ActivityMainBinding;
import com.example.mobileapp.models.Product;
import com.example.mobileapp.models.ProductListResponse;
import com.example.mobileapp.services.ApiClient;
import com.example.mobileapp.utils.SecureStorageManager;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    
    private ActivityMainBinding binding;
    private SecureStorageManager secureStorage;
    private ProductAdapter productAdapter;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        
        secureStorage = SecureStorageManager.getInstance(this);
        
        // ログインチェック
        if (!secureStorage.hasToken()) {
            navigateToLogin();
            return;
        }
        
        setupViews();
        loadProducts();
    }
    
    private void setupViews() {
        setSupportActionBar(binding.toolbar);
        
        // RecyclerView setup
        productAdapter = new ProductAdapter(product -> 
            navigateToProductDetail(product.getProductId())
        );
        
        binding.rvProducts.setLayoutManager(new LinearLayoutManager(this));
        binding.rvProducts.setAdapter(productAdapter);
        
        // SwipeRefreshLayout setup
        binding.swipeRefresh.setOnRefreshListener(this::loadProducts);
        
        // SearchView setup
        binding.searchView.setOnQueryTextListener(new SearchView.OnQueryTextListener() {
            @Override
            public boolean onQueryTextSubmit(String query) {
                searchProducts(query != null ? query : "");
                return true;
            }
            
            @Override
            public boolean onQueryTextChange(String newText) {
                if (newText == null || newText.isEmpty()) {
                    loadProducts();
                }
                return true;
            }
        });
    }
    
    private void loadProducts() {
        setLoading(true);
        
        ApiClient.getApiService().getProducts().enqueue(new Callback<ProductListResponse>() {
            @Override
            public void onResponse(Call<ProductListResponse> call, Response<ProductListResponse> response) {
                setLoading(false);
                
                if (response.isSuccessful() && response.body() != null) {
                    List<Product> products = response.body().getData();
                    if (products == null) {
                        products = new ArrayList<>();
                    }
                    productAdapter.submitList(products);
                    
                    binding.tvEmpty.setVisibility(products.isEmpty() ? View.VISIBLE : View.GONE);
                    binding.tvError.setVisibility(View.GONE);
                } else {
                    showError("商品の読み込みに失敗しました");
                }
            }
            
            @Override
            public void onFailure(Call<ProductListResponse> call, Throwable t) {
                setLoading(false);
                t.printStackTrace();
                showError("ネットワークエラーが発生しました");
            }
        });
    }
    
    private void searchProducts(String keyword) {
        if (keyword.isEmpty()) {
            loadProducts();
            return;
        }
        
        setLoading(true);
        
        ApiClient.getApiService().searchProducts(keyword).enqueue(new Callback<ProductListResponse>() {
            @Override
            public void onResponse(Call<ProductListResponse> call, Response<ProductListResponse> response) {
                setLoading(false);
                
                if (response.isSuccessful() && response.body() != null) {
                    List<Product> products = response.body().getData();
                    if (products == null) {
                        products = new ArrayList<>();
                    }
                    productAdapter.submitList(products);
                    
                    binding.tvEmpty.setText(products.isEmpty() ? "該当する商品がありません" : "商品がありません");
                    binding.tvEmpty.setVisibility(products.isEmpty() ? View.VISIBLE : View.GONE);
                    binding.tvError.setVisibility(View.GONE);
                } else {
                    showError("検索に失敗しました");
                }
            }
            
            @Override
            public void onFailure(Call<ProductListResponse> call, Throwable t) {
                setLoading(false);
                t.printStackTrace();
                showError("ネットワークエラーが発生しました");
            }
        });
    }
    
    private void setLoading(boolean isLoading) {
        binding.swipeRefresh.setRefreshing(isLoading);
        binding.progressBar.setVisibility(
            isLoading && !binding.swipeRefresh.isRefreshing() ? View.VISIBLE : View.GONE
        );
    }
    
    private void showError(String message) {
        binding.tvError.setText(message);
        binding.tvError.setVisibility(View.VISIBLE);
        binding.tvEmpty.setVisibility(View.GONE);
    }
    
    private void navigateToProductDetail(int productId) {
        Intent intent = new Intent(this, ProductDetailActivity.class);
        intent.putExtra("productId", productId);
        startActivity(intent);
    }
    
    private void navigateToLogin() {
        Intent intent = new Intent(this, LoginActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        finish();
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.action_logout) {
            performLogout();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    private void performLogout() {
        secureStorage.deleteToken();
        Toast.makeText(this, "ログアウトしました", Toast.LENGTH_SHORT).show();
        navigateToLogin();
    }
}
