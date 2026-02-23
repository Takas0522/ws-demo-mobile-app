<template>
  <div class="feature-flag-container">
    <AppHeader />

    <main class="main-content">
      <div class="page-header">
        <router-link to="/" class="back-link">← ダッシュボード</router-link>
        <h1 class="page-title">機能フラグ管理</h1>
      </div>

      <LoadingSpinner :is-visible="isLoading" message="読み込み中..." />

      <div v-if="error" class="error-box">
        <p>{{ error }}</p>
        <button @click="fetchUsersAndFlags" class="retry-button">再試行</button>
      </div>

      <div v-else-if="!isLoading" class="table-container">
        <table class="flag-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>ユーザー名</th>
              <th>ログインID</th>
              <th>お気に入り機能</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="userFlag in userFlags" :key="userFlag.userId">
              <td>{{ userFlag.userId }}</td>
              <td>{{ userFlag.userName }}</td>
              <td>{{ userFlag.loginId }}</td>
              <td>
                <button
                  @click="toggleFlag(userFlag.userId, userFlag.favoriteEnabled)"
                  :class="['toggle-button', userFlag.favoriteEnabled ? 'active' : 'inactive']"
                  :disabled="updatingUserId === userFlag.userId"
                >
                  <span class="toggle-text">
                    {{ userFlag.favoriteEnabled ? 'ON' : 'OFF' }}
                  </span>
                </button>
              </td>
            </tr>
          </tbody>
        </table>

        <div v-if="totalPages > 1" class="pagination">
          <button @click="goToPreviousPage" :disabled="currentPage === 1" class="page-button">
            &lt; 前へ
          </button>
          <span class="page-info"> {{ currentPage }} / {{ totalPages }} </span>
          <button @click="goToNextPage" :disabled="currentPage === totalPages" class="page-button">
            次へ &gt;
          </button>
        </div>
      </div>

      <ToastNotification
        :show="toast.show"
        :message="toast.message"
        :type="toast.type"
        @close="toast.show = false"
      />
    </main>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive, onMounted, computed } from 'vue'
import { adminApi } from '@/api/client'
import type { User } from '@/api/types'
import AppHeader from '@/components/AppHeader.vue'
import LoadingSpinner from '@/components/LoadingSpinner.vue'
import ToastNotification from '@/components/ToastNotification.vue'

interface UserFlagData {
  userId: number
  userName: string
  loginId: string
  favoriteEnabled: boolean
}

const userFlags = ref<UserFlagData[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)
const currentPage = ref(1)
const pageSize = ref(20)
const totalCount = ref(0)
const updatingUserId = ref<number | null>(null)

const toast = reactive({
  show: false,
  message: '',
  type: 'success' as 'success' | 'error' | 'warning' | 'info'
})

const totalPages = computed(() => Math.ceil(totalCount.value / pageSize.value))

async function fetchUsersAndFlags() {
  isLoading.value = true
  error.value = null

  try {
    // ユーザー一覧を取得
    const userResponse = await adminApi.getUsers(currentPage.value, pageSize.value)
    totalCount.value = userResponse.totalCount

    // 各ユーザーの機能フラグを取得
    const flagPromises = userResponse.users.map(async (user: User) => {
      try {
        const flagResponse = await adminApi.getUserFeatureFlags(user.userId)
        const favoriteFlag = flagResponse.flags.find((f) => f.flagKey === 'favorite')

        return {
          userId: user.userId,
          userName: user.userName,
          loginId: user.loginId,
          favoriteEnabled: favoriteFlag?.enabled || false
        }
      } catch {
        return {
          userId: user.userId,
          userName: user.userName,
          loginId: user.loginId,
          favoriteEnabled: false
        }
      }
    })

    userFlags.value = await Promise.all(flagPromises)
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    error.value = axiosError.response?.data?.error?.message || 'データの取得に失敗しました'
  } finally {
    isLoading.value = false
  }
}

async function toggleFlag(userId: number, currentValue: boolean) {
  updatingUserId.value = userId

  try {
    await adminApi.updateUserFeatureFlag(userId, 'favorite', {
      enabled: !currentValue
    })

    // ローカルの状態を更新
    const userFlag = userFlags.value.find((uf) => uf.userId === userId)
    if (userFlag) {
      userFlag.favoriteEnabled = !currentValue
    }

    toast.show = true
    toast.message = '機能フラグを更新しました'
    toast.type = 'success'
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    toast.show = true
    toast.message =
      axiosError.response?.data?.error?.message || '機能フラグの更新に失敗しました'
    toast.type = 'error'
  } finally {
    updatingUserId.value = null
  }
}

function goToPreviousPage() {
  if (currentPage.value > 1) {
    currentPage.value--
    fetchUsersAndFlags()
  }
}

function goToNextPage() {
  if (currentPage.value < totalPages.value) {
    currentPage.value++
    fetchUsersAndFlags()
  }
}

onMounted(() => {
  fetchUsersAndFlags()
})
</script>

<style scoped>
.feature-flag-container {
  min-height: 100vh;
  background-color: #fafafa;
}

.main-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 40px 20px;
}

.page-header {
  margin-bottom: 32px;
}

.back-link {
  display: inline-block;
  margin-bottom: 16px;
  color: #1976d2;
  text-decoration: none;
  font-size: 14px;
}

.back-link:hover {
  text-decoration: underline;
}

.page-title {
  font-size: 32px;
  font-weight: 700;
  color: #212121;
}

.error-box {
  background-color: #ffebee;
  border: 1px solid #f44336;
  border-radius: 4px;
  padding: 16px;
  text-align: center;
}

.error-box p {
  color: #f44336;
  margin-bottom: 12px;
}

.retry-button {
  padding: 8px 16px;
  background-color: #f44336;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.table-container {
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  overflow: hidden;
}

.flag-table {
  width: 100%;
  border-collapse: collapse;
}

.flag-table thead {
  background-color: #f5f5f5;
}

.flag-table th {
  padding: 16px;
  text-align: left;
  font-weight: 600;
  color: #424242;
  font-size: 14px;
}

.flag-table td {
  padding: 16px;
  border-top: 1px solid #e0e0e0;
  font-size: 14px;
  color: #212121;
}

.toggle-button {
  min-width: 80px;
  padding: 8px 16px;
  border: none;
  border-radius: 20px;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;
}

.toggle-button.active {
  background-color: #4caf50;
  color: white;
}

.toggle-button.inactive {
  background-color: #e0e0e0;
  color: #757575;
}

.toggle-button:hover:not(:disabled) {
  opacity: 0.8;
}

.toggle-button:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.toggle-text {
  display: inline-block;
}

.pagination {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 16px;
  padding: 20px;
  border-top: 1px solid #e0e0e0;
}

.page-button {
  padding: 8px 16px;
  background-color: #1976d2;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 14px;
}

.page-button:hover:not(:disabled) {
  background-color: #1565c0;
}

.page-button:disabled {
  background-color: #e0e0e0;
  color: #9e9e9e;
  cursor: not-allowed;
}

.page-info {
  font-size: 14px;
  color: #424242;
}
</style>
