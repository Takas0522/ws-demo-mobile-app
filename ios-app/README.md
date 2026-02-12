# iOS Mobile App

モバイルアプリシステムのiOSクライアントアプリケーション

## 概要

SwiftUI + URLSessionで実装されたネイティブiOSアプリです。Mobile BFF（`http://localhost:8081`）を通じてWeb APIと通信します。

## 機能

### 実装済み機能

- ✅ ログイン/ログアウト
- ✅ 商品一覧表示
- ✅ 商品検索
- ✅ 商品詳細表示
- ✅ 商品購入（100個単位）
- ✅ お気に入り登録/解除（機能フラグ制御）

## 技術スタック

- **言語**: Swift 5.0+
- **UI**: SwiftUI
- **HTTPクライアント**: URLSession
- **セキュアストレージ**: Keychain Services
- **最小iOS**: iOS 15.0+

## プロジェクト構造

```
MobileApp/
├── MobileAppApp.swift           # アプリエントリーポイント
├── Models/                      # データモデル
│   ├── User.swift
│   ├── Product.swift
│   ├── Purchase.swift
│   ├── Favorite.swift
│   └── APIError.swift
├── Services/                    # ビジネスロジック
│   ├── APIClient.swift          # API通信クライアント
│   └── AuthService.swift        # 認証サービス
├── Views/                       # UI画面
│   ├── LoginView.swift          # ログイン画面
│   ├── ProductListView.swift    # 商品一覧画面
│   └── ProductDetailView.swift  # 商品詳細画面
└── Utilities/                   # ユーティリティ
    └── KeychainManager.swift    # JWTトークン管理
```

## セットアップ

### 必要要件

- Xcode 14.0+
- macOS Ventura 13.0+
- iOS 15.0+ デバイスまたはシミュレーター

### ビルド手順

1. **プロジェクトを開く**
   ```bash
   cd ios-app/MobileApp
   open MobileApp.xcodeproj
   ```

2. **ビルド設定**
   - Target: MobileApp
   - Scheme: MobileApp
   - Deployment Target: iOS 15.0

3. **Mobile BFFを起動**
   ```bash
   # プロジェクトルートで
   cd mobile-bff
   npm install
   npm start
   # http://localhost:8081 で起動
   ```

4. **ビルド & 実行**
   - Xcodeで `Cmd+R` または Product > Run

### シミュレーターでの実行

localhostへの接続は、シミュレーターからホストマシンの `localhost:8081` に自動的に接続されます。

### 実機での実行

実機で実行する場合、`APIClient.swift` の `baseURL` を変更してください：

```swift
private let baseURL = "http://{YOUR_MACHINE_IP}:8081"
```

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

- **保存**: Keychain Services（セキュア）
- **場所**: `KeychainManager.shared`
- **有効期限**: 24時間
- **自動付与**: APIClient が全リクエストに自動付与

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

機能フラグはログイン時にサーバーから取得され、`User.featureFlags` に格納されます。

## 画面フロー

```
[ログイン画面]
    ↓ ログイン成功
[商品一覧画面]
    ↓ 商品タップ
[商品詳細画面]
    ↓ 購入ボタン
[購入確認シート]
    ↓ 購入確定
[購入完了アラート]
```

## エラーハンドリング

- ネットワークエラー: リトライボタン表示
- 認証エラー: ログイン画面に自動遷移
- バリデーションエラー: エラーメッセージ表示
- サーバーエラー: エラーメッセージ表示

## テスト

### 動作確認手順

1. **ログインテスト**
   - ユーザーID: `user001`、パスワード: `password123` でログイン成功
   - 誤ったパスワードでログイン失敗

2. **商品一覧テスト**
   - ログイン後、商品一覧が表示される
   - Pull to refresh で再読み込み

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
- ファイアウォール設定を確認
- `APIClient.swift` の `baseURL` を確認

### ビルドエラー

- Xcodeのバージョンを確認（14.0+）
- Clean Build Folder: `Cmd+Shift+K`
- Derived Dataを削除: `Xcode > Preferences > Locations > Derived Data`

### 認証エラー

- トークンをリセット: アプリを削除して再インストール
- ログイン情報を確認

## 制限事項

- デモ用途のため、最小限のUIデザイン
- 商品画像は未実装（プレースホルダー表示）
- オフライン対応なし
- エラーログはコンソールのみ

## 今後の拡張

- [ ] 商品画像の実装
- [ ] 購入履歴画面
- [ ] お気に入り一覧画面
- [ ] プロフィール画面
- [ ] オフラインキャッシュ
- [ ] プッシュ通知

## ライセンス

このプロジェクトはデモ用途です。
