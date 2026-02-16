<template>
  <div class="user-list-container">
    <AppHeader />

    <main class="main-content">
      <div class="page-header">
        <router-link to="/" class="back-link">← ダッシュボード</router-link>
        <h1 class="page-title">ユーザー管理</h1>
      </div>

      <LoadingSpinner :is-visible="isLoading" message="ユーザーを読み込み中..." />

      <div v-if="error" class="error-box">
        <p>{{ error }}</p>
        <button @click="fetchUsers" class="retry-button">再試行</button>
      </div>

      <div v-else-if="!isLoading" class="table-container">
        <table class="user-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>ログインID</th>
              <th>ユーザー名</th>
              <th>権限</th>
              <th>登録日</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="user in users" :key="user.userId">
              <td>{{ user.userId }}</td>
              <td>{{ user.loginId }}</td>
              <td>{{ user.userName }}</td>
              <td>
                <span :class="['role-badge', `role-${user.role}`]">
                  {{ user.role === 'admin' ? '管理者' : 'ユーザー' }}
                </span>
              </td>
              <td>{{ formatDate(user.createdAt) }}</td>
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
    </main>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'
import { adminApi } from '@/api/client'
import type { User } from '@/api/types'
import AppHeader from '@/components/AppHeader.vue'
import LoadingSpinner from '@/components/LoadingSpinner.vue'

const users = ref<User[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)
const currentPage = ref(1)
const pageSize = ref(20)
const totalCount = ref(0)

const totalPages = computed(() => Math.ceil(totalCount.value / pageSize.value))

async function fetchUsers() {
  isLoading.value = true
  error.value = null

  try {
    const response = await adminApi.getUsers(currentPage.value, pageSize.value)
    users.value = response.users
    totalCount.value = response.totalCount
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    error.value = axiosError.response?.data?.error?.message || 'ユーザーの取得に失敗しました'
  } finally {
    isLoading.value = false
  }
}

function goToPreviousPage() {
  if (currentPage.value > 1) {
    currentPage.value--
    fetchUsers()
  }
}

function goToNextPage() {
  if (currentPage.value < totalPages.value) {
    currentPage.value++
    fetchUsers()
  }
}

function formatDate(dateString: string): string {
  const date = new Date(dateString)
  return date.toLocaleDateString('ja-JP', {
    year: 'numeric',
    month: '2-digit',
    day: '2-digit'
  })
}

onMounted(() => {
  fetchUsers()
})
</script>

<style scoped>
.user-list-container {
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

.user-table {
  width: 100%;
  border-collapse: collapse;
}

.user-table thead {
  background-color: #f5f5f5;
}

.user-table th {
  padding: 16px;
  text-align: left;
  font-weight: 600;
  color: #424242;
  font-size: 14px;
}

.user-table td {
  padding: 16px;
  border-top: 1px solid #e0e0e0;
  font-size: 14px;
  color: #212121;
}

.role-badge {
  display: inline-block;
  padding: 4px 12px;
  border-radius: 12px;
  font-size: 12px;
  font-weight: 600;
}

.role-admin {
  background-color: #e3f2fd;
  color: #1976d2;
}

.role-user {
  background-color: #f5f5f5;
  color: #757575;
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
