package com.example.mobileapp.models

data class Favorite(
    val id: Int,
    val favoriteId: Int,
    val userId: String,
    val productId: Int,
    val createdAt: String
)

data class FavoriteRequest(
    val productId: Int
)

data class FavoriteResponse(
    val data: Favorite,
    val timestamp: String
)

data class FavoriteListResponse(
    val data: List<Favorite>,
    val timestamp: String
)
