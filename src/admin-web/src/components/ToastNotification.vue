<template>
  <Transition name="toast">
    <div v-if="isVisible" :class="['toast', `toast-${type}`]">
      <div class="toast-icon">{{ icon }}</div>
      <div class="toast-content">
        <p class="toast-message">{{ message }}</p>
      </div>
      <button @click="close" class="toast-close">×</button>
    </div>
  </Transition>
</template>

<script setup lang="ts">
import { ref, computed, watch } from 'vue'

interface Props {
  message: string
  type?: 'success' | 'error' | 'warning' | 'info'
  duration?: number
  show?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  type: 'info',
  duration: 3000,
  show: false
})

const emit = defineEmits<{
  close: []
}>()

const isVisible = ref(props.show)
let timer: number | null = null

const icon = computed(() => {
  switch (props.type) {
    case 'success':
      return '✓'
    case 'error':
      return '✕'
    case 'warning':
      return '⚠'
    case 'info':
    default:
      return 'ℹ'
  }
})

watch(
  () => props.show,
  (newValue) => {
    isVisible.value = newValue
    if (newValue && props.duration > 0) {
      if (timer) {
        clearTimeout(timer)
      }
      timer = window.setTimeout(() => {
        close()
      }, props.duration)
    }
  }
)

function close() {
  isVisible.value = false
  if (timer) {
    clearTimeout(timer)
    timer = null
  }
  emit('close')
}
</script>

<style scoped>
.toast {
  position: fixed;
  top: 20px;
  right: 20px;
  min-width: 300px;
  max-width: 500px;
  padding: 16px;
  background: white;
  border-radius: 8px;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
  display: flex;
  align-items: center;
  gap: 12px;
  z-index: 10000;
}

.toast-success {
  border-left: 4px solid #4caf50;
}

.toast-error {
  border-left: 4px solid #f44336;
}

.toast-warning {
  border-left: 4px solid #ff9800;
}

.toast-info {
  border-left: 4px solid #2196f3;
}

.toast-icon {
  font-size: 24px;
  font-weight: bold;
}

.toast-success .toast-icon {
  color: #4caf50;
}

.toast-error .toast-icon {
  color: #f44336;
}

.toast-warning .toast-icon {
  color: #ff9800;
}

.toast-info .toast-icon {
  color: #2196f3;
}

.toast-content {
  flex: 1;
}

.toast-message {
  margin: 0;
  font-size: 14px;
  color: #212121;
}

.toast-close {
  background: none;
  border: none;
  font-size: 24px;
  color: #757575;
  cursor: pointer;
  padding: 0;
  width: 24px;
  height: 24px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.toast-close:hover {
  color: #212121;
}

.toast-enter-active,
.toast-leave-active {
  transition: all 0.3s ease;
}

.toast-enter-from {
  opacity: 0;
  transform: translateX(100%);
}

.toast-leave-to {
  opacity: 0;
  transform: translateX(100%);
}
</style>
