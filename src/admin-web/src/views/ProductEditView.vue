<template>
  <div class="product-edit-container">
    <AppHeader />

    <main class="main-content">
      <div class="page-header">
        <router-link to="/products" class="back-link">← 商品一覧</router-link>
        <h1 class="page-title">商品編集</h1>
      </div>

      <LoadingSpinner :is-visible="isLoading" message="保存中..." />

      <div v-if="error" class="error-box">
        <p>{{ error }}</p>
      </div>

      <div v-if="product" class="edit-card">
        <form @submit.prevent="handleSubmit" class="edit-form">
          <div class="form-group">
            <label>商品ID</label>
            <input type="text" :value="product.productId" disabled class="form-input disabled" />
            <span class="form-note">※変更不可</span>
          </div>

          <div class="form-group">
            <label for="productName">商品名 <span class="required">*</span></label>
            <input
              id="productName"
              v-model="formData.productName"
              type="text"
              required
              minlength="1"
              maxlength="100"
              class="form-input"
              :disabled="isLoading"
            />
            <span v-if="validationErrors.productName" class="error-text">
              {{ validationErrors.productName }}
            </span>
          </div>

          <div class="form-group">
            <label for="unitPrice">単価 <span class="required">*</span></label>
            <div class="input-with-unit">
              <input
                id="unitPrice"
                v-model.number="formData.unitPrice"
                type="number"
                required
                min="1"
                class="form-input"
                :disabled="isLoading"
              />
              <span class="unit">円</span>
            </div>
            <span v-if="validationErrors.unitPrice" class="error-text">
              {{ validationErrors.unitPrice }}
            </span>
          </div>

          <div class="form-group">
            <label for="description">商品説明</label>
            <textarea
              id="description"
              v-model="formData.description"
              rows="4"
              class="form-textarea"
              :disabled="isLoading"
            ></textarea>
          </div>

          <div class="form-group">
            <label for="imageUrl">画像URL</label>
            <input
              id="imageUrl"
              v-model="formData.imageUrl"
              type="url"
              class="form-input"
              placeholder="https://example.com/image.jpg"
              :disabled="isLoading"
            />
          </div>

          <div class="form-actions">
            <button type="button" @click="handleCancel" class="cancel-button" :disabled="isLoading">
              キャンセル
            </button>
            <button type="submit" class="save-button" :disabled="isLoading">
              {{ isLoading ? '保存中...' : '保存' }}
            </button>
          </div>
        </form>
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
import { ref, reactive, onMounted } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { adminApi } from '@/api/client'
import type { Product, UpdateProductRequest } from '@/api/types'
import AppHeader from '@/components/AppHeader.vue'
import LoadingSpinner from '@/components/LoadingSpinner.vue'
import ToastNotification from '@/components/ToastNotification.vue'

const router = useRouter()
const route = useRoute()

const product = ref<Product | null>(null)
const isLoading = ref(false)
const error = ref<string | null>(null)

const formData = reactive({
  productName: '',
  unitPrice: 0,
  description: '',
  imageUrl: ''
})

const validationErrors = reactive({
  productName: '',
  unitPrice: ''
})

const toast = reactive({
  show: false,
  message: '',
  type: 'success' as 'success' | 'error' | 'warning' | 'info'
})

async function fetchProduct() {
  const productId = Number(route.params.id)
  if (isNaN(productId)) {
    error.value = '無効な商品IDです'
    return
  }

  isLoading.value = true
  error.value = null

  try {
    const response = await adminApi.getProducts()
    product.value = response.products.find((p) => p.productId === productId) || null

    if (product.value) {
      formData.productName = product.value.productName
      formData.unitPrice = product.value.unitPrice
      formData.description = product.value.description || ''
      formData.imageUrl = product.value.imageUrl || ''
    } else {
      error.value = '商品が見つかりません'
    }
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    error.value = axiosError.response?.data?.error?.message || '商品の取得に失敗しました'
  } finally {
    isLoading.value = false
  }
}

function validateForm(): boolean {
  validationErrors.productName = ''
  validationErrors.unitPrice = ''

  if (!formData.productName || formData.productName.trim().length === 0) {
    validationErrors.productName = '商品名を入力してください'
    return false
  }

  if (formData.productName.length > 100) {
    validationErrors.productName = '商品名は100文字以内で入力してください'
    return false
  }

  if (!formData.unitPrice || formData.unitPrice < 1) {
    validationErrors.unitPrice = '単価は1円以上で入力してください'
    return false
  }

  return true
}

async function handleSubmit() {
  if (!validateForm() || !product.value) {
    return
  }

  isLoading.value = true
  error.value = null

  try {
    const request: UpdateProductRequest = {
      productName: formData.productName,
      unitPrice: formData.unitPrice,
      description: formData.description || undefined,
      imageUrl: formData.imageUrl || undefined
    }

    await adminApi.updateProduct(product.value.productId, request)

    toast.show = true
    toast.message = '商品を更新しました'
    toast.type = 'success'

    setTimeout(() => {
      router.push('/products')
    }, 1500)
  } catch (err) {
    const axiosError = err as { response?: { data?: { error?: { message?: string } } } }
    const errorMessage = axiosError.response?.data?.error?.message || '商品の更新に失敗しました'
    error.value = errorMessage
    toast.show = true
    toast.message = errorMessage
    toast.type = 'error'
  } finally {
    isLoading.value = false
  }
}

function handleCancel() {
  router.push('/products')
}

onMounted(() => {
  fetchProduct()
})
</script>

<style scoped>
.product-edit-container {
  min-height: 100vh;
  background-color: #fafafa;
}

.main-content {
  max-width: 800px;
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
  margin-bottom: 24px;
}

.error-box p {
  color: #f44336;
  margin: 0;
}

.edit-card {
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  padding: 32px;
}

.edit-form {
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.form-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.form-group label {
  font-size: 14px;
  font-weight: 600;
  color: #424242;
}

.required {
  color: #f44336;
}

.form-input,
.form-textarea {
  padding: 12px;
  font-size: 14px;
  border: 1px solid #e0e0e0;
  border-radius: 4px;
  transition: border-color 0.2s;
}

.form-input:focus,
.form-textarea:focus {
  outline: none;
  border-color: #1976d2;
}

.form-input.disabled {
  background-color: #f5f5f5;
  color: #9e9e9e;
  cursor: not-allowed;
}

.form-input:disabled,
.form-textarea:disabled {
  background-color: #f5f5f5;
  cursor: not-allowed;
}

.input-with-unit {
  display: flex;
  align-items: center;
  gap: 8px;
}

.input-with-unit .form-input {
  flex: 1;
}

.unit {
  font-size: 14px;
  color: #757575;
}

.form-note {
  font-size: 12px;
  color: #757575;
}

.error-text {
  font-size: 12px;
  color: #f44336;
}

.form-actions {
  display: flex;
  justify-content: flex-end;
  gap: 12px;
  margin-top: 16px;
}

.cancel-button,
.save-button {
  padding: 12px 24px;
  font-size: 14px;
  font-weight: 600;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.cancel-button {
  background-color: #e0e0e0;
  color: #424242;
}

.cancel-button:hover:not(:disabled) {
  background-color: #d5d5d5;
}

.save-button {
  background-color: #1976d2;
  color: white;
}

.save-button:hover:not(:disabled) {
  background-color: #1565c0;
}

.cancel-button:disabled,
.save-button:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}
</style>
