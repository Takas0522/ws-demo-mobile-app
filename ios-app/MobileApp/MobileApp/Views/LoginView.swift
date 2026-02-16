//
//  LoginView.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import SwiftUI

struct LoginView: View {
    @StateObject private var authService = AuthService.shared
    @State private var loginId = ""
    @State private var password = ""
    @State private var isLoading = false
    @State private var errorMessage: String?
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                Text("モバイルアプリ")
                    .font(.largeTitle)
                    .fontWeight(.bold)
                    .padding(.bottom, 40)
                
                VStack(alignment: .leading, spacing: 8) {
                    Text("ユーザーID")
                        .font(.caption)
                        .foregroundColor(.secondary)
                    
                    TextField("ユーザーID", text: $loginId)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .autocapitalization(.none)
                        .disableAutocorrection(true)
                }
                
                VStack(alignment: .leading, spacing: 8) {
                    Text("パスワード")
                        .font(.caption)
                        .foregroundColor(.secondary)
                    
                    SecureField("パスワード", text: $password)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                }
                
                if let errorMessage = errorMessage {
                    Text(errorMessage)
                        .font(.caption)
                        .foregroundColor(.red)
                        .padding(.top, 8)
                }
                
                Button(action: handleLogin) {
                    if isLoading {
                        ProgressView()
                            .progressViewStyle(CircularProgressViewStyle(tint: .white))
                    } else {
                        Text("ログイン")
                            .fontWeight(.semibold)
                    }
                }
                .frame(maxWidth: .infinity)
                .padding()
                .background(Color.blue)
                .foregroundColor(.white)
                .cornerRadius(10)
                .disabled(isLoading || loginId.isEmpty || password.isEmpty)
                .padding(.top, 20)
                
                Spacer()
            }
            .padding()
            .navigationTitle("ログイン")
        }
    }
    
    private func handleLogin() {
        isLoading = true
        errorMessage = nil
        
        Task {
            do {
                try await authService.login(loginId: loginId, password: password)
            } catch NetworkError.unauthorized {
                errorMessage = "ユーザーIDまたはパスワードが正しくありません"
            } catch NetworkError.serverError(let message) {
                errorMessage = message
            } catch {
                errorMessage = "ログインに失敗しました"
            }
            isLoading = false
        }
    }
}

struct LoginView_Previews: PreviewProvider {
    static var previews: some View {
        LoginView()
    }
}
