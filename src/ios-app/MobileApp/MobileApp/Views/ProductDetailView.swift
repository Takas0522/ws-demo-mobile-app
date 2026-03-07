//
//  ProductDetailView.swift
//  MobileApp
//
//  Created on 2025-01-08.
//

import SwiftUI

struct ProductDetailView: View {
    let productId: Int
    @StateObject private var viewModel: ProductDetailViewModel
    @StateObject private var authService = AuthService.shared
    @Environment(\.presentationMode) var presentationMode
    
    init(productId: Int) {
        self.productId = productId
        _viewModel = StateObject(wrappedValue: ProductDetailViewModel(productId: productId))
    }
    
    var body: some View {
        ScrollView {
            if viewModel.isLoading {
                ProgressView()
                    .padding()
            } else if let errorMessage = viewModel.errorMessage {
                VStack(spacing: 16) {
                    Text(errorMessage)
                        .foregroundColor(.red)
                    Button("再試行") {
                        Task {
                            await viewModel.loadProductDetail()
                        }
                    }
                }
                .padding()
            } else if let product = viewModel.product {
                VStack(alignment: .leading, spacing: 20) {
                    // 商品画像
                    RoundedRectangle(cornerRadius: 12)
                        .fill(Color.gray.opacity(0.3))
                        .frame(height: 250)
                        .overlay(
                            Image(systemName: "photo")
                                .font(.system(size: 60))
                                .foregroundColor(.gray)
                        )
                    
                    // 商品名
                    Text(product.productName)
                        .font(.title2)
                        .fontWeight(.bold)
                    
                    // 単価
                    Text("¥\(product.unitPrice)")
                        .font(.title3)
                        .foregroundColor(.blue)
                    
                    // 商品説明
                    if let description = product.description {
                        Text(description)
                            .font(.body)
                            .foregroundColor(.secondary)
                            .padding(.top, 8)
                    }
                    
                    Divider()
                        .padding(.vertical, 8)
                    
                    // お気に入りボタン（機能フラグチェック）
                    if shouldShowFavoriteButton() {
                        Button(action: {
                            Task {
                                await viewModel.toggleFavorite()
                            }
                        }) {
                            HStack {
                                Image(systemName: viewModel.isFavorite ? "heart.fill" : "heart")
                                Text(viewModel.isFavorite ? "お気に入り済み" : "お気に入りに追加")
                            }
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(viewModel.isFavorite ? Color.red.opacity(0.1) : Color.gray.opacity(0.1))
                            .foregroundColor(viewModel.isFavorite ? .red : .primary)
                            .cornerRadius(10)
                        }
                        .disabled(viewModel.isFavoriteLoading)
                    }
                    
                    // 購入ボタン
                    Button(action: {
                        viewModel.showPurchaseSheet = true
                    }) {
                        Text("購入する")
                            .fontWeight(.semibold)
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(Color.blue)
                            .foregroundColor(.white)
                            .cornerRadius(10)
                    }
                }
                .padding()
            }
        }
        .navigationTitle("商品詳細")
        .navigationBarTitleDisplayMode(.inline)
        .task {
            await viewModel.loadProductDetail()
        }
        .sheet(isPresented: $viewModel.showPurchaseSheet) {
            PurchaseSheet(
                product: viewModel.product,
                onPurchase: { quantity in
                    await viewModel.purchaseProduct(quantity: quantity)
                }
            )
        }
        .alert("購入完了", isPresented: $viewModel.showPurchaseSuccess) {
            Button("OK") {
                presentationMode.wrappedValue.dismiss()
            }
        } message: {
            Text("ご購入ありがとうございました")
        }
        .alert("エラー", isPresented: $viewModel.showPurchaseError) {
            Button("OK", role: .cancel) {}
        } message: {
            Text(viewModel.purchaseErrorMessage ?? "購入に失敗しました")
        }
    }
    
    private func shouldShowFavoriteButton() -> Bool {
        return authService.currentUser != nil
    }
}

struct PurchaseSheet: View {
    let product: Product?
    let onPurchase: (Int) async -> Void
    
    @Environment(\.presentationMode) var presentationMode
    @State private var selectedQuantity = 100
    @State private var isPurchasing = false
    
    let quantities = Array(stride(from: 100, through: 9900, by: 100))
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                if let product = product {
                    Text(product.productName)
                        .font(.headline)
                        .padding(.top)
                    
                    Text("単価: ¥\(product.unitPrice)")
                        .foregroundColor(.secondary)
                    
                    Divider()
                    
                    VStack(alignment: .leading, spacing: 12) {
                        Text("購入個数")
                            .font(.headline)
                        
                        Picker("数量", selection: $selectedQuantity) {
                            ForEach(quantities, id: \.self) { quantity in
                                Text("\(quantity)個").tag(quantity)
                            }
                        }
                        .pickerStyle(WheelPickerStyle())
                        .frame(height: 150)
                        
                        HStack {
                            Text("合計金額")
                                .font(.headline)
                            Spacer()
                            Text("¥\(product.unitPrice * selectedQuantity)")
                                .font(.title2)
                                .fontWeight(.bold)
                                .foregroundColor(.blue)
                        }
                        .padding(.top, 8)
                    }
                    .padding()
                    
                    Spacer()
                    
                    Button(action: {
                        isPurchasing = true
                        Task {
                            await onPurchase(selectedQuantity)
                            isPurchasing = false
                            presentationMode.wrappedValue.dismiss()
                        }
                    }) {
                        if isPurchasing {
                            ProgressView()
                                .progressViewStyle(CircularProgressViewStyle(tint: .white))
                        } else {
                            Text("購入確定")
                                .fontWeight(.semibold)
                        }
                    }
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Color.blue)
                    .foregroundColor(.white)
                    .cornerRadius(10)
                    .disabled(isPurchasing)
                    .padding()
                }
            }
            .navigationTitle("購入確認")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarLeading) {
                    Button("キャンセル") {
                        presentationMode.wrappedValue.dismiss()
                    }
                }
            }
        }
    }
}

@MainActor
class ProductDetailViewModel: ObservableObject {
    let productId: Int
    
    @Published var product: Product?
    @Published var isFavorite = false
    @Published var isLoading = false
    @Published var isFavoriteLoading = false
    @Published var errorMessage: String?
    @Published var showPurchaseSheet = false
    @Published var showPurchaseSuccess = false
    @Published var showPurchaseError = false
    @Published var purchaseErrorMessage: String?
    
    private var favoriteId: Int?
    
    init(productId: Int) {
        self.productId = productId
    }
    
    func loadProductDetail() async {
        isLoading = true
        errorMessage = nil
        
        do {
            let productData = try await APIClient.shared.getProductDetail(productId: productId)
            product = productData
        } catch {
            errorMessage = "商品詳細の読み込みに失敗しました"
        }
        
        isLoading = false
    }
    
    func toggleFavorite() async {
        isFavoriteLoading = true
        
        do {
            if isFavorite {
                // お気に入り解除
                if let favoriteId = favoriteId {
                    try await APIClient.shared.removeFavorite(favoriteId: favoriteId)
                    isFavorite = false
                    self.favoriteId = nil
                }
            } else {
                // お気に入り登録
                let favorite = try await APIClient.shared.addFavorite(productId: productId)
                isFavorite = true
                favoriteId = favorite.favoriteId
            }
        } catch {
            // エラー処理（ここでは無視）
        }
        
        isFavoriteLoading = false
    }
    
    func purchaseProduct(quantity: Int) async {
        do {
            _ = try await APIClient.shared.purchaseProduct(productId: productId, quantity: quantity)
            showPurchaseSuccess = true
        } catch {
            purchaseErrorMessage = "購入処理に失敗しました"
            showPurchaseError = true
        }
    }
    
    private func loadFavoriteId() async {
        do {
            let favorites = try await APIClient.shared.getFavorites()
            favoriteId = favorites.first(where: { $0.productId == productId })?.favoriteId
        } catch {
            // エラー処理（ここでは無視）
        }
    }
}

struct ProductDetailView_Previews: PreviewProvider {
    static var previews: some View {
        ProductDetailView(productId: 1)
    }
}
