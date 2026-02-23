//
//  AuthService.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import Foundation
import SwiftUI

@MainActor
class AuthService: ObservableObject {
    static let shared = AuthService()
    
    @Published var isAuthenticated = false
    @Published var currentUser: User?
    
    private init() {
        // アプリ起動時にトークンの存在を確認
        if KeychainManager.shared.getToken() != nil {
            isAuthenticated = true
        }
    }
    
    func login(loginId: String, password: String) async throws {
        let loginData = try await APIClient.shared.login(loginId: loginId, password: password)
        
        // トークンを保存
        _ = KeychainManager.shared.saveToken(loginData.token)
        
        // 認証状態を更新
        self.currentUser = loginData.user
        self.isAuthenticated = true
    }
    
    func logout() {
        KeychainManager.shared.deleteToken()
        self.currentUser = nil
        self.isAuthenticated = false
    }
}
