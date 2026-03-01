/**
 * Admin BFF API クライアント
 */
import axios, { type AxiosInstance, type AxiosError, type InternalAxiosRequestConfig } from 'axios'
import type {
  ApiResponse,
  ApiErrorResponse,
  LoginRequest,
  LoginResponse,
  ProductListResponse,
  Product,
  UpdateProductRequest,
  UserListResponse,
  UserFeatureFlagsResponse,
  UpdateFeatureFlagRequest
} from './types'

// 環境変数からベースURLを取得（デフォルト: http://localhost:8082）
const API_BASE_URL = import.meta.env.VITE_ADMIN_BFF_URL || 'http://localhost:8082'

/**
 * Axiosインスタンス作成
 */
const createAxiosInstance = (): AxiosInstance => {
  const instance = axios.create({
    baseURL: API_BASE_URL,
    timeout: 10000,
    headers: {
      'Content-Type': 'application/json'
    }
  })

  // リクエストインターセプター（認証トークン自動付与）
  instance.interceptors.request.use(
    (config: InternalAxiosRequestConfig) => {
      const token = localStorage.getItem('auth_token')
      if (token && config.headers) {
        config.headers.Authorization = `Bearer ${token}`
      }
      return config
    },
    (error) => {
      return Promise.reject(error)
    }
  )

  // レスポンスインターセプター（エラーハンドリング）
  instance.interceptors.response.use(
    (response) => response,
    (error: AxiosError<ApiErrorResponse>) => {
      // 認証エラー時はログアウト
      if (error.response?.status === 401) {
        localStorage.removeItem('auth_token')
        localStorage.removeItem('user_info')
        window.location.href = '/login'
      }
      return Promise.reject(error)
    }
  )

  return instance
}

const apiClient = createAxiosInstance()

/**
 * Admin BFF API Client
 */
export const adminApi = {
  /**
   * 管理者ログイン
   */
  async login(request: LoginRequest): Promise<LoginResponse> {
    const response = await apiClient.post<ApiResponse<LoginResponse>>(
      '/api/admin/auth/login',
      request
    )
    return response.data.data
  },

  /**
   * 商品一覧取得
   */
  async getProducts(page: number = 1, pageSize: number = 20): Promise<ProductListResponse> {
    const response = await apiClient.get<ApiResponse<ProductListResponse>>('/api/admin/products', {
      params: { page, pageSize }
    })
    return response.data.data
  },

  /**
   * 商品更新
   */
  async updateProduct(productId: number, request: UpdateProductRequest): Promise<Product> {
    const response = await apiClient.put<ApiResponse<Product>>(
      `/api/admin/products/${productId}`,
      request
    )
    return response.data.data
  },

  /**
   * ユーザー一覧取得
   */
  async getUsers(page: number = 1, pageSize: number = 20): Promise<UserListResponse> {
    const response = await apiClient.get<ApiResponse<UserListResponse>>('/api/admin/users', {
      params: { page, pageSize }
    })
    return response.data.data
  },

  /**
   * ユーザー別機能フラグ取得
   */
  async getUserFeatureFlags(userId: number): Promise<UserFeatureFlagsResponse> {
    const response = await apiClient.get<ApiResponse<UserFeatureFlagsResponse>>(
      `/api/admin/users/${userId}/flags`
    )
    return response.data.data
  },

  /**
   * 機能フラグ変更
   */
  async updateUserFeatureFlag(
    userId: number,
    flagKey: string,
    request: UpdateFeatureFlagRequest
  ): Promise<void> {
    await apiClient.put(`/api/admin/users/${userId}/flags/${flagKey}`, request)
  }
}

export default apiClient
