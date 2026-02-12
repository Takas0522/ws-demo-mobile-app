/**
 * 認証ストア
 * ログイン状態とトークンを管理
 */
import { ref, computed } from 'vue'
import { defineStore } from 'pinia'
import { adminApi } from '@/api/client'
import type { LoginRequest, LoginResponse } from '@/api/types'

export const useAuthStore = defineStore('auth', () => {
  // State
  const token = ref<string | null>(null)
  const userId = ref<number | null>(null)
  const userName = ref<string | null>(null)
  const role = ref<'user' | 'admin' | null>(null)
  const isLoading = ref(false)
  const error = ref<string | null>(null)

  // Getters
  const isAuthenticated = computed(() => !!token.value)
  const isAdmin = computed(() => role.value === 'admin')

  /**
   * ストアの初期化（localStorageから復元）
   */
  function initialize() {
    const savedToken = localStorage.getItem('auth_token')
    const savedUserInfo = localStorage.getItem('user_info')

    if (savedToken && savedUserInfo) {
      try {
        const userInfo = JSON.parse(savedUserInfo)
        token.value = savedToken
        userId.value = userInfo.userId
        userName.value = userInfo.userName
        role.value = userInfo.role
      } catch {
        // パースエラー時はクリア
        clearAuth()
      }
    }
  }

  /**
   * ログイン
   */
  async function login(loginId: string, password: string): Promise<void> {
    isLoading.value = true
    error.value = null

    try {
      const request: LoginRequest = { loginId, password }
      const response: LoginResponse = await adminApi.login(request)

      // ストアに保存
      token.value = response.token
      userId.value = response.userId
      userName.value = response.userName
      role.value = response.role

      // localStorageに保存
      localStorage.setItem('auth_token', response.token)
      localStorage.setItem(
        'user_info',
        JSON.stringify({
          userId: response.userId,
          userName: response.userName,
          role: response.role
        })
      )
    } catch (err) {
      const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
      error.value = axiosError.response?.data?.error?.message || 'ログインに失敗しました'
      throw err
    } finally {
      isLoading.value = false
    }
  }

  /**
   * ログアウト
   */
  function logout() {
    clearAuth()
    window.location.href = '/login'
  }

  /**
   * 認証情報をクリア
   */
  function clearAuth() {
    token.value = null
    userId.value = null
    userName.value = null
    role.value = null
    localStorage.removeItem('auth_token')
    localStorage.removeItem('user_info')
  }

  /**
   * エラーをクリア
   */
  function clearError() {
    error.value = null
  }

  // 初期化実行
  initialize()

  return {
    // State
    token,
    userId,
    userName,
    role,
    isLoading,
    error,
    // Getters
    isAuthenticated,
    isAdmin,
    // Actions
    login,
    logout,
    clearError
  }
})
