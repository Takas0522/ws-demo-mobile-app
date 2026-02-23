//
//  Product.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct Product: Codable, Identifiable {
    let id: Int
    let productId: Int
    let productName: String
    let unitPrice: Int
    let description: String?
    let imageUrl: String?
    let createdAt: String
    let updatedAt: String
    
    var isFavorite: Bool = false
}

struct ProductListResponse: Codable {
    let data: [Product]
    let timestamp: String
}

struct ProductDetailResponse: Codable {
    let data: ProductDetail
    let timestamp: String
}

struct ProductDetail: Codable {
    let product: Product
    let isFavorite: Bool
}
