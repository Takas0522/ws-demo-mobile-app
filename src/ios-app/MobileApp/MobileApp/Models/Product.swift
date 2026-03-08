//
//  Product.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct Product: Codable, Identifiable, Sendable {
    var id: Int { productId }
    let productId: Int
    let productName: String
    let unitPrice: Int
    let description: String?
    let imageUrl: String?
    let createdAt: String?
    let updatedAt: String?
    
    enum CodingKeys: String, CodingKey {
        case productId, productName, unitPrice, description, imageUrl, createdAt, updatedAt
    }
}

struct ProductListData: Codable, Sendable {
    let products: [Product]
}

struct ProductListResponse: Codable, Sendable {
    let data: ProductListData
    let timestamp: String
}

struct ProductDetailData: Codable, Sendable {
    let product: Product
}

struct ProductDetailResponse: Codable, Sendable {
    let data: ProductDetailData
    let timestamp: String
}
