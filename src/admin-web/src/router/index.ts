import { createRouter, createWebHistory } from 'vue-router'
import { useAuthStore } from '@/stores/auth'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/login',
      name: 'login',
      component: () => import('../views/LoginView.vue'),
      meta: { requiresAuth: false }
    },
    {
      path: '/',
      name: 'dashboard',
      component: () => import('../views/DashboardView.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/products',
      name: 'products',
      component: () => import('../views/ProductListView.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/products/:id/edit',
      name: 'product-edit',
      component: () => import('../views/ProductEditView.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/users',
      name: 'users',
      component: () => import('../views/UserListView.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/feature-flags',
      name: 'feature-flags',
      component: () => import('../views/FeatureFlagView.vue'),
      meta: { requiresAuth: true }
    }
  ]
})

// ナビゲーションガード（認証チェック）
router.beforeEach((to, from, next) => {
  const authStore = useAuthStore()
  const requiresAuth = to.meta.requiresAuth !== false

  if (requiresAuth && !authStore.isAuthenticated) {
    // 認証が必要だが未認証の場合、ログイン画面へ
    next({ name: 'login' })
  } else if (to.name === 'login' && authStore.isAuthenticated) {
    // ログイン画面だが認証済みの場合、ダッシュボードへ
    next({ name: 'dashboard' })
  } else {
    next()
  }
})

export default router
