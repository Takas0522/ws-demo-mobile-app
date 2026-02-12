package com.example.mobileapp.services

import com.example.mobileapp.models.*
import okhttp3.ResponseBody
import retrofit2.Response
import retrofit2.http.*

interface ApiService {
    
    @POST("/api/mobile/login")
    suspend fun login(@Body request: LoginRequest): Response<LoginResponse>
    
    @GET("/api/mobile/products")
    suspend fun getProducts(): Response<ProductListResponse>
    
    @GET("/api/mobile/products/search")
    suspend fun searchProducts(@Query("keyword") keyword: String): Response<ProductListResponse>
    
    @GET("/api/mobile/products/{id}")
    suspend fun getProductDetail(@Path("id") productId: Int): Response<ProductDetailResponse>
    
    @POST("/api/mobile/purchases")
    suspend fun purchaseProduct(@Body request: PurchaseRequest): Response<PurchaseResponse>
    
    @POST("/api/mobile/favorites")
    suspend fun addFavorite(@Body request: FavoriteRequest): Response<FavoriteResponse>
    
    @DELETE("/api/mobile/favorites/{id}")
    suspend fun removeFavorite(@Path("id") favoriteId: Int): Response<ResponseBody>
    
    @GET("/api/mobile/favorites")
    suspend fun getFavorites(): Response<FavoriteListResponse>
}
