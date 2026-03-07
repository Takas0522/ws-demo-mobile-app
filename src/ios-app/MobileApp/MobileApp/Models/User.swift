//
//  User.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct User: Codable {
    let userId: Int
    let userName: String
    let loginId: String
    let userType: String
}

struct LoginRequest: Codable {
    let loginId: String
    let password: String
}

struct LoginResponse: Codable {
    let data: LoginData
    let timestamp: String
}

struct LoginData: Codable {
    let token: String
    let tokenType: String
    let expiresIn: Int
    let user: User
}
