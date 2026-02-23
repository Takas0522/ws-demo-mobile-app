<template>
  <div class="dashboard-container">
    <AppHeader />

    <main class="dashboard-main">
      <h1 class="dashboard-title">管理ダッシュボード</h1>

      <div class="menu-grid">
        <router-link to="/products" class="menu-card">
          <div class="menu-icon">📦</div>
          <h2 class="menu-title">商品管理</h2>
          <p class="menu-description">商品の一覧表示と編集</p>
        </router-link>

        <router-link to="/feature-flags" class="menu-card">
          <div class="menu-icon">🚩</div>
          <h2 class="menu-title">機能フラグ管理</h2>
          <p class="menu-description">ユーザー別機能フラグの設定</p>
        </router-link>

        <router-link to="/users" class="menu-card">
          <div class="menu-icon">👥</div>
          <h2 class="menu-title">ユーザー管理</h2>
          <p class="menu-description">ユーザー一覧表示</p>
        </router-link>
      </div>
    </main>
  </div>
</template>

<script setup lang="ts">
import AppHeader from '@/components/AppHeader.vue'
</script>

<style scoped>
.dashboard-container {
  min-height: 100vh;
  background-color: #fafafa;
}

.dashboard-main {
  max-width: 1200px;
  margin: 0 auto;
  padding: 40px 20px;
}

.dashboard-title {
  font-size: 32px;
  font-weight: 700;
  color: #212121;
  margin-bottom: 40px;
  text-align: center;
}

.menu-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 24px;
}

.menu-card {
  background: white;
  padding: 32px;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  text-decoration: none;
  color: inherit;
  transition: transform 0.2s, box-shadow 0.2s;
  cursor: pointer;
}

.menu-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.15);
}

.menu-icon {
  font-size: 48px;
  margin-bottom: 16px;
}

.menu-title {
  font-size: 20px;
  font-weight: 600;
  color: #212121;
  margin-bottom: 8px;
}

.menu-description {
  font-size: 14px;
  color: #757575;
}
</style>
