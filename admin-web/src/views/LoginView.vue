<template>
  <div class="login-container">
    <div class="login-card">
      <h1 class="login-title">管理者ログイン</h1>

      <form @submit.prevent="handleSubmit" class="login-form">
        <div class="form-group">
          <label for="loginId">ログインID</label>
          <input
            id="loginId"
            v-model="loginId"
            type="text"
            required
            minlength="4"
            maxlength="20"
            placeholder="ログインIDを入力"
            class="form-input"
            :disabled="isLoading"
          />
        </div>

        <div class="form-group">
          <label for="password">パスワード</label>
          <input
            id="password"
            v-model="password"
            type="password"
            required
            minlength="8"
            maxlength="50"
            placeholder="パスワードを入力"
            class="form-input"
            :disabled="isLoading"
          />
        </div>

        <button type="submit" class="login-button" :disabled="isLoading">
          {{ isLoading ? 'ログイン中...' : 'ログイン' }}
        </button>

        <div v-if="errorMessage" class="error-message">
          {{ errorMessage }}
        </div>
      </form>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '@/stores/auth'

const router = useRouter()
const authStore = useAuthStore()

const loginId = ref('')
const password = ref('')
const errorMessage = ref('')
const isLoading = ref(false)

async function handleSubmit() {
  errorMessage.value = ''
  isLoading.value = true

  try {
    await authStore.login(loginId.value, password.value)
    router.push({ name: 'dashboard' })
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    errorMessage.value =
      axiosError.response?.data?.error?.message || 'ログインIDまたはパスワードが正しくありません'
  } finally {
    isLoading.value = false
  }
}
</script>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #fafafa;
}

.login-card {
  background: white;
  padding: 40px;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  width: 100%;
  max-width: 400px;
}

.login-title {
  text-align: center;
  margin-bottom: 32px;
  color: #212121;
  font-size: 24px;
  font-weight: 600;
}

.login-form {
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.form-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.form-group label {
  font-size: 14px;
  font-weight: 500;
  color: #424242;
}

.form-input {
  padding: 12px;
  font-size: 14px;
  border: 1px solid #e0e0e0;
  border-radius: 4px;
  transition: border-color 0.2s;
}

.form-input:focus {
  outline: none;
  border-color: #1976d2;
}

.form-input:disabled {
  background-color: #f5f5f5;
  cursor: not-allowed;
}

.login-button {
  padding: 12px;
  font-size: 16px;
  font-weight: 600;
  color: white;
  background-color: #1976d2;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.login-button:hover:not(:disabled) {
  background-color: #1565c0;
}

.login-button:disabled {
  background-color: #90caf9;
  cursor: not-allowed;
}

.error-message {
  padding: 12px;
  font-size: 14px;
  color: #f44336;
  background-color: #ffebee;
  border-radius: 4px;
  text-align: center;
}
</style>
