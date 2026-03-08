//
//  APIError.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct APIError: Codable, Sendable {
    let error: ErrorDetail
    let timestamp: String
}

struct ErrorDetail: Codable, Sendable {
    let code: String
    let message: String
    let details: String?
}

enum NetworkError: Error, Sendable {
    case invalidURL
    case noData
    case decodingError
    case serverError(String)
    case unauthorized
    case unknown
}
