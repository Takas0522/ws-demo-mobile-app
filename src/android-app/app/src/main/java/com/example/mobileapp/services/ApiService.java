package com.example.mobileapp.services;

import com.example.mobileapp.models.*;
import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.http.*;

public interface ApiService {
    
    @POST("/api/mobile/login")
    Call<LoginResponse> login(@Body LoginRequest request);
    
    @GET("/api/mobile/products")
    Call<ProductListResponse> getProducts();
    
    @GET("/api/mobile/products/search")
    Call<ProductListResponse> searchProducts(@Query("keyword") String keyword);
    
    @GET("/api/mobile/products/{id}")
    Call<ProductDetailResponse> getProductDetail(@Path("id") int productId);
    
    @POST("/api/mobile/purchases")
    Call<PurchaseResponse> purchaseProduct(@Body PurchaseRequest request);
    
    @POST("/api/mobile/favorites")
    Call<FavoriteResponse> addFavorite(@Body FavoriteRequest request);
    
    @DELETE("/api/mobile/favorites/{id}")
    Call<ResponseBody> removeFavorite(@Path("id") int favoriteId);
    
    @GET("/api/mobile/favorites")
    Call<FavoriteListResponse> getFavorites();
}
