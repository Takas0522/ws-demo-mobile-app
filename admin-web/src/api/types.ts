/**
 * API共通型定義
 */

// API共通レスポンス型
export interface ApiResponse<T> {
  data: T
  timestamp: string
}

export interface ApiErrorResponse {
  error: {
    code: string
    message: string
    details?: string
  }
  timestamp: string
}

// ログイン関連
export interface LoginRequest {
  loginId: string
  password: string
}

export interface LoginResponse {
  token: string
  userId: number
  userName: string
  role: 'user' | 'admin'
}

// 商品関連
export interface Product {
  productId: number
  productName: string
  unitPrice: number
  description?: string
  imageUrl?: string
  createdAt: string
  updatedAt: string
}

export interface ProductListResponse {
  products: Product[]
  totalCount: number
  page: number
  pageSize: number
}

export interface UpdateProductRequest {
  productName: string
  unitPrice: number
  description?: string
  imageUrl?: string
}

// ユーザー関連
export interface User {
  userId: number
  loginId: string
  userName: string
  role: 'user' | 'admin'
  createdAt: string
}

export interface UserListResponse {
  users: User[]
  totalCount: number
  page: number
  pageSize: number
}

// 機能フラグ関連
export interface FeatureFlag {
  flagKey: string
  flagName: string
  enabled: boolean
}

export interface UserFeatureFlagsResponse {
  userId: number
  flags: FeatureFlag[]
}

export interface UpdateFeatureFlagRequest {
  enabled: boolean
}
