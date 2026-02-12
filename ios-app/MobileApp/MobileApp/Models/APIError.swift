//
//  APIError.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

struct APIError: Codable {
    let error: ErrorDetail
    let timestamp: String
}

struct ErrorDetail: Codable {
    let code: String
    let message: String
    let details: String?
}

enum NetworkError: Error {
    case invalidURL
    case noData
    case decodingError
    case serverError(String)
    case unauthorized
    case unknown
}
