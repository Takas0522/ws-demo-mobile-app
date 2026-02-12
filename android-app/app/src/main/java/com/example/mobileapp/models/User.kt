package com.example.mobileapp.models

data class User(
    val userId: String,
    val userName: String,
    val role: String,
    val featureFlags: Map<String, Boolean>
)

data class LoginRequest(
    val loginId: String,
    val password: String
)

data class LoginResponse(
    val data: LoginData,
    val timestamp: String
)

data class LoginData(
    val token: String,
    val user: User
)
