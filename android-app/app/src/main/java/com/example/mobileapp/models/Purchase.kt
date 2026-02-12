package com.example.mobileapp.models

data class Purchase(
    val id: String,
    val purchaseId: String,
    val userId: String,
    val productId: Int,
    val quantity: Int,
    val unitPrice: Int,
    val totalAmount: Int,
    val purchasedAt: String
)

data class PurchaseRequest(
    val productId: Int,
    val quantity: Int
)

data class PurchaseResponse(
    val data: Purchase,
    val timestamp: String
)
