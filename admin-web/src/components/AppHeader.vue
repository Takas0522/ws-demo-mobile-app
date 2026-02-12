<template>
  <header class="app-header">
    <div class="header-container">
      <div class="header-left">
        <router-link to="/" class="header-logo"> 管理画面 </router-link>
      </div>

      <div class="header-right">
        <span class="user-name">{{ userName }}</span>
        <button @click="handleLogout" class="logout-button">ログアウト</button>
      </div>
    </div>
  </header>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useAuthStore } from '@/stores/auth'

const authStore = useAuthStore()
const userName = computed(() => authStore.userName || '管理者')

function handleLogout() {
  if (confirm('ログアウトしますか？')) {
    authStore.logout()
  }
}
</script>

<style scoped>
.app-header {
  background-color: #1976d2;
  color: white;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.header-container {
  max-width: 1400px;
  margin: 0 auto;
  padding: 16px 24px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.header-left {
  display: flex;
  align-items: center;
}

.header-logo {
  font-size: 20px;
  font-weight: 700;
  color: white;
  text-decoration: none;
}

.header-right {
  display: flex;
  align-items: center;
  gap: 16px;
}

.user-name {
  font-size: 14px;
  font-weight: 500;
}

.logout-button {
  padding: 8px 16px;
  font-size: 14px;
  font-weight: 600;
  color: #1976d2;
  background-color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.logout-button:hover {
  background-color: #f5f5f5;
}
</style>
