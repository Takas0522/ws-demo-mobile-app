//
//  APIClient.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation

class APIClient {
    static let shared = APIClient()
    
    private let baseURL = "http://localhost:8081"
    private let session: URLSession
    
    private init() {
        let config = URLSessionConfiguration.default
        config.timeoutIntervalForRequest = 10.0
        config.timeoutIntervalForResource = 10.0
        self.session = URLSession(configuration: config)
    }
    
    // MARK: - Generic Request
    
    private func request<T: Decodable>(
        endpoint: String,
        method: String = "GET",
        body: Encodable? = nil,
        requiresAuth: Bool = true
    ) async throws -> T {
        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
            throw NetworkError.invalidURL
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = method
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        
        // 認証トークンの追加
        if requiresAuth, let token = KeychainManager.shared.getToken() {
            request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")
        }
        
        // リクエストボディの追加
        if let body = body {
            request.httpBody = try JSONEncoder().encode(body)
        }
        
        let (data, response) = try await session.data(for: request)
        
        guard let httpResponse = response as? HTTPURLResponse else {
            throw NetworkError.unknown
        }
        
        // エラーレスポンスのハンドリング
        if httpResponse.statusCode == 401 {
            throw NetworkError.unauthorized
        }
        
        if httpResponse.statusCode >= 400 {
            if let apiError = try? JSONDecoder().decode(APIError.self, from: data) {
                throw NetworkError.serverError(apiError.error.message)
            }
            throw NetworkError.serverError("Server error: \(httpResponse.statusCode)")
        }
        
        // 成功レスポンスのデコード
        do {
            let decoded = try JSONDecoder().decode(T.self, from: data)
            return decoded
        } catch {
            print("Decoding error: \(error)")
            throw NetworkError.decodingError
        }
    }
    
    // MARK: - Auth API
    
    func login(loginId: String, password: String) async throws -> LoginData {
        let request = LoginRequest(loginId: loginId, password: password)
        let response: LoginResponse = try await self.request(
            endpoint: "/api/mobile/login",
            method: "POST",
            body: request,
            requiresAuth: false
        )
        return response.data
    }
    
    // MARK: - Product API
    
    func getProducts() async throws -> [Product] {
        let response: ProductListResponse = try await request(endpoint: "/api/mobile/products")
        return response.data.products
    }
    
    func searchProducts(keyword: String) async throws -> [Product] {
        let endpoint = "/api/mobile/products/search?keyword=\(keyword.addingPercentEncoding(withAllowedCharacters: .urlQueryAllowed) ?? "")"
        let response: ProductListResponse = try await request(endpoint: endpoint)
        return response.data.products
    }
    
    func getProductDetail(productId: Int) async throws -> Product {
        let response: ProductDetailResponse = try await request(endpoint: "/api/mobile/products/\(productId)")
        return response.data.product
    }
    
    // MARK: - Purchase API
    
    func purchaseProduct(productId: Int, quantity: Int) async throws -> Purchase {
        let request = PurchaseRequest(productId: productId, quantity: quantity)
        let response: PurchaseResponse = try await self.request(
            endpoint: "/api/mobile/purchases",
            method: "POST",
            body: request
        )
        return response.data
    }
    
    // MARK: - Favorite API
    
    func addFavorite(productId: Int) async throws -> Favorite {
        let request = FavoriteRequest(productId: productId)
        let response: FavoriteResponse = try await self.request(
            endpoint: "/api/mobile/favorites",
            method: "POST",
            body: request
        )
        return response.data
    }
    
    func removeFavorite(favoriteId: Int) async throws {
        struct EmptyResponse: Codable {}
        let _: EmptyResponse = try await request(
            endpoint: "/api/mobile/favorites/\(favoriteId)",
            method: "DELETE"
        )
    }
    
    func getFavorites() async throws -> [Favorite] {
        let response: FavoriteListResponse = try await request(endpoint: "/api/mobile/favorites")
        return response.data
    }
}
