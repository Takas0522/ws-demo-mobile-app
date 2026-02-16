//
//  ProductListView.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import SwiftUI

struct ProductListView: View {
    @StateObject private var authService = AuthService.shared
    @StateObject private var viewModel = ProductListViewModel()
    
    var body: some View {
        NavigationView {
            VStack {
                // 検索バー
                SearchBar(text: $viewModel.searchKeyword, onSearchButtonClicked: {
                    Task {
                        await viewModel.searchProducts()
                    }
                })
                .padding(.horizontal)
                
                // 商品リスト
                if viewModel.isLoading {
                    ProgressView()
                        .padding()
                } else if let errorMessage = viewModel.errorMessage {
                    VStack(spacing: 16) {
                        Text(errorMessage)
                            .foregroundColor(.red)
                        Button("再試行") {
                            Task {
                                await viewModel.loadProducts()
                            }
                        }
                    }
                    .padding()
                } else if viewModel.products.isEmpty {
                    Text("商品がありません")
                        .foregroundColor(.secondary)
                        .padding()
                } else {
                    List(viewModel.products) { product in
                        NavigationLink(destination: ProductDetailView(productId: product.productId)) {
                            ProductRow(product: product)
                        }
                    }
                    .refreshable {
                        await viewModel.loadProducts()
                    }
                }
            }
            .navigationTitle("商品一覧")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("ログアウト") {
                        authService.logout()
                    }
                }
            }
            .task {
                await viewModel.loadProducts()
            }
        }
    }
}

struct ProductRow: View {
    let product: Product
    
    var body: some View {
        HStack(spacing: 12) {
            // 商品画像（プレースホルダー）
            RoundedRectangle(cornerRadius: 8)
                .fill(Color.gray.opacity(0.3))
                .frame(width: 60, height: 60)
                .overlay(
                    Image(systemName: "photo")
                        .foregroundColor(.gray)
                )
            
            VStack(alignment: .leading, spacing: 4) {
                Text(product.productName)
                    .font(.headline)
                
                Text("¥\(product.unitPrice)")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
        }
        .padding(.vertical, 4)
    }
}

struct SearchBar: View {
    @Binding var text: String
    var onSearchButtonClicked: () -> Void
    
    var body: some View {
        HStack {
            HStack {
                Image(systemName: "magnifyingglass")
                    .foregroundColor(.gray)
                
                TextField("商品名で検索", text: $text)
                    .autocapitalization(.none)
                    .disableAutocorrection(true)
                    .onSubmit {
                        onSearchButtonClicked()
                    }
                
                if !text.isEmpty {
                    Button(action: {
                        text = ""
                        onSearchButtonClicked()
                    }) {
                        Image(systemName: "xmark.circle.fill")
                            .foregroundColor(.gray)
                    }
                }
            }
            .padding(8)
            .background(Color(.systemGray6))
            .cornerRadius(10)
        }
        .padding(.vertical, 8)
    }
}

@MainActor
class ProductListViewModel: ObservableObject {
    @Published var products: [Product] = []
    @Published var searchKeyword = ""
    @Published var isLoading = false
    @Published var errorMessage: String?
    
    func loadProducts() async {
        isLoading = true
        errorMessage = nil
        
        do {
            products = try await APIClient.shared.getProducts()
        } catch {
            errorMessage = "商品の読み込みに失敗しました"
        }
        
        isLoading = false
    }
    
    func searchProducts() async {
        guard !searchKeyword.isEmpty else {
            await loadProducts()
            return
        }
        
        isLoading = true
        errorMessage = nil
        
        do {
            products = try await APIClient.shared.searchProducts(keyword: searchKeyword)
        } catch {
            errorMessage = "検索に失敗しました"
        }
        
        isLoading = false
    }
}

struct ProductListView_Previews: PreviewProvider {
    static var previews: some View {
        ProductListView()
    }
}
