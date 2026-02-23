# Android Mobile App

モバイルアプリシステムのAndroidクライアントアプリケーション

## 概要

**Java** + Retrofit + Material Designで実装されたネイティブAndroidアプリです。Mobile BFF（`http://localhost:8081`）を通じてWeb APIと通信します。

## 機能

### 実装済み機能

- ✅ ログイン/ログアウト
- ✅ 商品一覧表示
- ✅ 商品検索
- ✅ 商品詳細表示
- ✅ 商品購入（100個単位）
- ✅ お気に入り登録/解除（機能フラグ制御）

## 技術スタック

- **言語**: Java 17
- **UI**: Material Design Components
- **HTTPクライアント**: Retrofit 2.9 + OkHttp 4.12
- **非同期処理**: Retrofit Call (Callback)
- **セキュアストレージ**: EncryptedSharedPreferences
- **最小SDK**: Android 7.0 (API 24)
- **ターゲットSDK**: Android 14 (API 34)

## プロジェクト構造

```
app/src/main/
├── java/com/example/mobileapp/
│   ├── MobileApplication.java        # Applicationクラス
│   ├── models/                       # データモデル
│   │   ├── User.java
│   │   ├── Product.java
│   │   ├── Purchase.java
│   │   ├── Favorite.java
│   │   └── ApiError.java
│   ├── services/                     # ビジネスロジック
│   │   ├── ApiClient.java            # Retrofit設定
│   │   └── ApiService.java           # APIインターフェース
│   ├── ui/                           # UI (Activity)
│   │   ├── LoginActivity.java        # ログイン画面
│   │   ├── MainActivity.java         # 商品一覧画面
│   │   ├── ProductDetailActivity.java# 商品詳細画面
│   │   └── ProductAdapter.java       # RecyclerViewアダプター
│   └── utils/                        # ユーティリティ
│       └── SecureStorageManager.java # JWTトークン管理
├── res/
│   ├── layout/                       # レイアウトXML
│   ├── values/                       # 値リソース
│   ├── drawable/                     # 画像リソース
│   └── menu/                         # メニューリソース
└── AndroidManifest.xml
```

## セットアップ

### 必要要件

- Android Studio Hedgehog (2023.1.1) 以上
- JDK 17
- Android SDK 34
- Android Emulator または実機（Android 7.0+）

### ビルド手順

1. **Android Studioでプロジェクトを開く**
   ```bash
   cd android-app
   # Android Studioで開く: File > Open > android-app
   ```

2. **Mobile BFFを起動**
   ```bash
   # プロジェクトルートで
   cd mobile-bff
   npm install
   npm start
   # http://localhost:8081 で起動
   ```

3. **エミュレーターで実行**
   - Android Studio: Run > Run 'app'
   - Gradle コマンド: Android Studio の Gradle Sync 後に実行

### エミュレーターでの接続

Android Emulatorからホストマシンのlocalhostに接続するには、`ApiClient.java`で以下のURLを使用：

```java
private static final String BASE_URL = "http://10.0.2.2:8081";
```

- `10.0.2.2`: エミュレーターからホストマシンのlocalhostを指すエイリアス

### 実機での接続

実機で実行する場合、`ApiClient.java` の `BASE_URL` を変更：

```java
private static final String BASE_URL = "http://{YOUR_MACHINE_IP}:8081";
```

**注意**: 
- 実機とPCが同じWi-Fiネットワークに接続されている必要があります
- `AndroidManifest.xml` に `android:usesCleartextTraffic="true"` が設定されています

## APIエンドポイント

Mobile BFF（`http://localhost:8081`）の以下のエンドポイントを使用：

| エンドポイント | メソッド | 説明 |
|-------------|---------|------|
| `/api/mobile/login` | POST | ログイン |
| `/api/mobile/products` | GET | 商品一覧 |
| `/api/mobile/products/search` | GET | 商品検索 |
| `/api/mobile/products/{id}` | GET | 商品詳細 |
| `/api/mobile/purchases` | POST | 商品購入 |
| `/api/mobile/favorites` | POST | お気に入り登録 |
| `/api/mobile/favorites/{id}` | DELETE | お気に入り解除 |
| `/api/mobile/favorites` | GET | お気に入り一覧 |

## 認証

### JWTトークン管理

- **保存**: EncryptedSharedPreferences（セキュア）
- **場所**: `SecureStorageManager`
- **有効期限**: 24時間
- **自動付与**: OkHttpインターセプターが全リクエストに自動付与

### ログイン

デフォルトユーザー：
- **ユーザーID**: `user001`
- **パスワード**: `password123`

お気に入り機能が有効なユーザー：
- **ユーザーID**: `user002`
- **パスワード**: `password456`

## 機能フラグ

お気に入り機能は機能フラグで制御されます：

- `user001`: お気に入りボタン非表示
- `user002`: お気に入りボタン表示

## 画面フロー

```
[ログイン画面] (LoginActivity)
    ↓ ログイン成功
[商品一覧画面] (MainActivity)
    ↓ 商品タップ
[商品詳細画面] (ProductDetailActivity)
    ↓ 購入ボタン
[購入確認ダイアログ]
    ↓ 購入確定
[購入完了ダイアログ]
```

## 主要コンポーネント

### SecureStorageManager

EncryptedSharedPreferencesを使用してJWTトークンを安全に保存：

```java
SecureStorageManager storage = SecureStorageManager.getInstance(context);
storage.saveToken(token);
String token = storage.getToken();
storage.deleteToken();
```

### ApiClient

Retrofit + OkHttpを使用したAPIクライアント：

- 自動認証ヘッダー付与
- HTTPロギング
- タイムアウト設定（10秒）

### RecyclerView

商品一覧はRecyclerViewで実装：

- DiffUtilによる効率的な更新
- ViewBindingによる型安全なビュー参照

### 非同期処理

Retrofitのコールバックを使用：

```java
ApiClient.getApiService().getProducts().enqueue(new Callback<ProductListResponse>() {
    @Override
    public void onResponse(Call<ProductListResponse> call, Response<ProductListResponse> response) {
        // 成功処理
    }
    
    @Override
    public void onFailure(Call<ProductListResponse> call, Throwable t) {
        // エラー処理
    }
});
```

## ビルド設定

### Gradle依存関係

主要な依存関係：

```gradle
// Networking
implementation 'com.squareup.retrofit2:retrofit:2.9.0'
implementation 'com.squareup.retrofit2:converter-gson:2.9.0'
implementation 'com.squareup.okhttp3:okhttp:4.12.0'

// Security
implementation 'androidx.security:security-crypto:1.1.0-alpha06'

// UI
implementation 'com.google.android.material:material:1.11.0'
```

### ProGuard

本番ビルド時のコード難読化設定は `proguard-rules.pro` で定義可能です。

## テスト

### 動作確認手順

1. **ログインテスト**
   - ユーザーID: `user001`、パスワード: `password123` でログイン成功
   - 誤ったパスワードでログイン失敗

2. **商品一覧テスト**
   - ログイン後、商品一覧が表示される
   - Swipe to refresh で再読み込み

3. **商品検索テスト**
   - 検索バーにキーワード入力（例: "商品A"）
   - 該当商品のみ表示される

4. **商品詳細テスト**
   - 商品タップで詳細画面に遷移
   - 商品情報が正しく表示される

5. **購入テスト**
   - 購入ボタンタップ
   - 数量選択（100, 200, ...）
   - 合計金額計算が正しい
   - 購入確定で完了

6. **お気に入りテスト（user002でログイン）**
   - お気に入りボタンが表示される
   - タップで登録/解除がトグルする

## トラブルシューティング

### Mobile BFFに接続できない

- Mobile BFFが起動しているか確認: `http://localhost:8081`
- エミュレーター: `10.0.2.2:8081` を使用
- 実機: PCのIPアドレス（同じWi-Fi）を使用
- `android:usesCleartextTraffic="true"` が設定されているか確認

### ビルドエラー

- Gradle Sync: File > Sync Project with Gradle Files
- Clean Build: Build > Clean Project
- Invalidate Caches: File > Invalidate Caches / Restart

### 認証エラー

- トークンをリセット: アプリをアンインストールして再インストール
- ログイン情報を確認

### エミュレーターが遅い

- Hardware Acceleration (HAXM/KVM) を有効化
- エミュレーターのRAMを増やす

## セキュリティ

- **トークン保存**: AES256で暗号化されたSharedPreferences
- **通信**: HTTPS推奨（本番環境）
- **クリアテキスト通信**: 開発環境のみ許可

## 制限事項

- デモ用途のため、最小限のUIデザイン
- 商品画像は未実装（プレースホルダー表示）
- オフライン対応なし
- エラーログはLogcatのみ

## 今後の拡張

- [ ] 商品画像の実装
- [ ] 購入履歴画面
- [ ] お気に入り一覧画面
- [ ] プロフィール画面
- [ ] オフラインキャッシュ (Room Database)
- [ ] プッシュ通知 (FCM)
- [ ] ダークモード対応

## ライセンス

このプロジェクトはデモ用途です。
