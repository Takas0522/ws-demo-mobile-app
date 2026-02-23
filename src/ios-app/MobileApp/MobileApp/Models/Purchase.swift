//
//  Purchase.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct Purchase: Codable, Identifiable {
    let id: String
    let purchaseId: String
    let userId: String
    let productId: Int
    let quantity: Int
    let unitPrice: Int
    let totalAmount: Int
    let purchasedAt: String
}

struct PurchaseRequest: Codable {
    let productId: Int
    let quantity: Int
}

struct PurchaseResponse: Codable {
    let data: Purchase
    let timestamp: String
}
