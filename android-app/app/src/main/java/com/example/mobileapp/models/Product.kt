package com.example.mobileapp.models

data class Product(
    val id: Int,
    val productId: Int,
    val productName: String,
    val unitPrice: Int,
    val description: String?,
    val imageUrl: String?,
    val createdAt: String,
    val updatedAt: String
)

data class ProductListResponse(
    val data: List<Product>,
    val timestamp: String
)

data class ProductDetailResponse(
    val data: ProductDetail,
    val timestamp: String
)

data class ProductDetail(
    val product: Product,
    val isFavorite: Boolean
)
