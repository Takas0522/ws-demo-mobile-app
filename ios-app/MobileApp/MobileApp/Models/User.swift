//
//  User.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct User: Codable {
    let userId: String
    let userName: String
    let role: String
    let featureFlags: [String: Bool]
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
    let user: User
}
