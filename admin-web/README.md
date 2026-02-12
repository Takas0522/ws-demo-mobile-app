# admin-web

管理Webアプリケーション（Vue 3）

## 概要

mobile-app-systemの管理者向けWebアプリケーションです。
Admin BFF (http://localhost:8082) 経由でバックエンドAPIと通信します。

## 技術スタック

- **Vue 3** - Composition API
- **TypeScript** - 型安全性
- **Vite** - ビルドツール
- **Pinia** - 状態管理
- **Vue Router** - ルーティング
- **Axios** - HTTPクライアント

## 主な機能

- 管理者ログイン
- 商品管理（一覧表示、編集）
- ユーザー管理（一覧表示）
- 機能フラグ管理（ユーザー別ON/OFF切り替え）

## セットアップ

### 必要なもの

- Node.js (v20.19.0以上 または v22.12.0以上)
- npm

### インストール

```bash
npm install
```

### 環境変数設定

`.env.example`をコピーして`.env`を作成：

```bash
cp .env.example .env
```

`.env`ファイルの内容：
```
VITE_ADMIN_BFF_URL=http://localhost:8082
```

## 開発

### 開発サーバー起動

```bash
npm run dev
```

ブラウザで http://localhost:5173 にアクセス

### ビルド

```bash
npm run build
```

### 型チェック

```bash
npm run type-check
```

### Lint

```bash
npm run lint
```

## ログイン情報（開発用）

管理者アカウント：
- ログインID: `admin001`
- パスワード: `adminpass123`

## プロジェクト構造

```
admin-web/
├── src/
│   ├── api/              # API関連
│   │   ├── client.ts     # Axiosクライアント
│   │   └── types.ts      # 型定義
│   ├── components/       # 共通コンポーネント
│   │   ├── AppHeader.vue
│   │   ├── LoadingSpinner.vue
│   │   └── ToastNotification.vue
│   ├── router/           # ルーティング
│   │   └── index.ts
│   ├── stores/           # 状態管理（Pinia）
│   │   └── auth.ts
│   ├── views/            # 画面コンポーネント
│   │   ├── DashboardView.vue
│   │   ├── LoginView.vue
│   │   ├── ProductListView.vue
│   │   ├── ProductEditView.vue
│   │   ├── UserListView.vue
│   │   └── FeatureFlagView.vue
│   ├── App.vue
│   └── main.ts
├── .env                  # 環境変数
├── package.json
├── tsconfig.json
└── vite.config.ts
```

## 画面一覧

| パス | 画面名 | 説明 |
|------|--------|------|
| `/login` | ログイン画面 | 管理者ログイン |
| `/` | ダッシュボード | メニュー選択 |
| `/products` | 商品一覧 | 商品リスト表示 |
| `/products/:id/edit` | 商品編集 | 商品情報編集 |
| `/users` | ユーザー一覧 | ユーザーリスト表示 |
| `/feature-flags` | 機能フラグ管理 | ユーザー別フラグ設定 |

## API接続

Admin BFF (http://localhost:8082) に接続します。

主要エンドポイント：
- `POST /api/admin/login` - ログイン
- `GET /api/admin/products` - 商品一覧取得
- `PUT /api/admin/products/:id` - 商品更新
- `GET /api/admin/users` - ユーザー一覧取得
- `GET /api/admin/users/:id/flags` - 機能フラグ取得
- `PUT /api/admin/users/:id/flags/:key` - 機能フラグ更新

## 認証

- JWT Bearer tokenによる認証
- ログイン後、tokenをlocalStorageに保存
- APIリクエスト時に自動的にAuthorizationヘッダーに付与
- 401エラー時は自動的にログアウト

## ライセンス

Proprietary
