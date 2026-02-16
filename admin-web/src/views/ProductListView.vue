<template>
  <div class="product-list-container">
    <AppHeader />

    <main class="main-content">
      <div class="page-header">
        <router-link to="/" class="back-link">← ダッシュボード</router-link>
        <h1 class="page-title">商品管理</h1>
      </div>

      <LoadingSpinner :is-visible="isLoading" message="商品を読み込み中..." />

      <div v-if="error" class="error-box">
        <p>{{ error }}</p>
        <button @click="fetchProducts" class="retry-button">再試行</button>
      </div>

      <div v-else-if="!isLoading" class="table-container">
        <table class="product-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>商品名</th>
              <th>単価</th>
              <th>操作</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="product in products" :key="product.productId">
              <td>{{ product.productId }}</td>
              <td>{{ product.productName }}</td>
              <td class="price">¥{{ product.unitPrice.toLocaleString() }}</td>
              <td>
                <router-link
                  :to="`/products/${product.productId}/edit`"
                  class="edit-button"
                >
                  編集
                </router-link>
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
    </main>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'
import { adminApi } from '@/api/client'
import type { Product } from '@/api/types'
import AppHeader from '@/components/AppHeader.vue'
import LoadingSpinner from '@/components/LoadingSpinner.vue'

const products = ref<Product[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)
const currentPage = ref(1)
const pageSize = ref(20)
const totalCount = ref(0)

const totalPages = computed(() => Math.ceil(totalCount.value / pageSize.value))

async function fetchProducts() {
  isLoading.value = true
  error.value = null

  try {
    const response = await adminApi.getProducts(currentPage.value, pageSize.value)
    products.value = response.products
    totalCount.value = response.totalCount
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    error.value = axiosError.response?.data?.error?.message || '商品の取得に失敗しました'
  } finally {
    isLoading.value = false
  }
}

function goToPreviousPage() {
  if (currentPage.value > 1) {
    currentPage.value--
    fetchProducts()
  }
}

function goToNextPage() {
  if (currentPage.value < totalPages.value) {
    currentPage.value++
    fetchProducts()
  }
}

onMounted(() => {
  fetchProducts()
})
</script>

<style scoped>
.product-list-container {
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

.product-table {
  width: 100%;
  border-collapse: collapse;
}

.product-table thead {
  background-color: #f5f5f5;
}

.product-table th {
  padding: 16px;
  text-align: left;
  font-weight: 600;
  color: #424242;
  font-size: 14px;
}

.product-table td {
  padding: 16px;
  border-top: 1px solid #e0e0e0;
  font-size: 14px;
  color: #212121;
}

.product-table td.price {
  text-align: right;
  font-weight: 600;
}

.edit-button {
  display: inline-block;
  padding: 6px 12px;
  background-color: #1976d2;
  color: white;
  text-decoration: none;
  border-radius: 4px;
  font-size: 13px;
  transition: background-color 0.2s;
}

.edit-button:hover {
  background-color: #1565c0;
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
