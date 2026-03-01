//
//  MobileAppApp.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import SwiftUI

@main
struct MobileAppApp: App {
    @StateObject private var authService = AuthService.shared
    
    var body: some Scene {
        WindowGroup {
            if authService.isAuthenticated {
                ProductListView()
            } else {
                LoginView()
            }
        }
    }
}
