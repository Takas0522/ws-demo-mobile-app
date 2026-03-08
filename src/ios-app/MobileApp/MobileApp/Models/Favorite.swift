//
//  Favorite.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct Favorite: Codable, Identifiable, Sendable {
    let id: Int
    let favoriteId: Int
    let userId: String
    let productId: Int
    let createdAt: String
}

struct FavoriteRequest: Codable, Sendable {
    let productId: Int
}

struct FavoriteResponse: Codable, Sendable {
    let data: Favorite
    let timestamp: String
}

struct FavoriteListResponse: Codable, Sendable {
    let data: [Favorite]
    let timestamp: String
}
