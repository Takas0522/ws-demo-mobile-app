package com.example.mobileapp.models

data class ApiError(
    val error: ErrorDetail,
    val timestamp: String
)

data class ErrorDetail(
    val code: String,
    val message: String,
    val details: String?
)
