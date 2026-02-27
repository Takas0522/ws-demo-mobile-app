# システムコンテキスト

> 最終更新: 2025-01-08  
> ステータス: Draft  
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 関連機能 |
|-----------|------|---------|---------|
| 1.0 | 2025-01-08 | 初版作成 | mobile-app-system |

---

## 1. システムコンテキスト概要

本ドキュメントでは、mobile-app-system のシステムコンテキストを定義します。
システムの境界、外部アクター、外部システムとの連携を明確にします。

## 2. システムコンテキスト図（C4モデル Level 1）

### 2.1 全体コンテキスト図

```mermaid
graph TB
    subgraph "External Actors"
        EU[エンドユーザー<br/>商品を閲覧・購入]
        AD[管理者<br/>商品・機能フラグ管理]
    end
    
    subgraph "Mobile App System"
        System[mobile-app-system<br/>商品販売システム]
    end
    
    EU -->|"1. ログイン<br/>2. 商品検索<br/>3. 商品購入<br/>4. お気に入り登録"| System
    AD -->|"1. 管理者ログイン<br/>2. 商品管理<br/>3. 機能フラグ管理"| System
    System -.->|"将来拡張:<br/>メール通知<br/>決済連携"| External[外部システム]
    
    style System fill:#ffab91
    style EU fill:#e1f5ff
    style AD fill:#c5e1a5
    style External fill:#e0e0e0,stroke-dasharray: 5 5
```

### 2.2 システム境界

**システム内**（In Scope）:
- モバイルアプリ（iOS/Android/Windows）
- 管理Webアプリ（Vue.js）
- Mobile BFF（Spring Boot）
- Admin BFF（Spring Boot）
- Web API（Spring Boot）
- SQLiteデータベース

**システム外**（Out of Scope）:
- 決済システム
- メール配信システム
- プッシュ通知サービス
- 外部認証プロバイダ（OAuth等）

## 3. アクター定義

### 3.1 エンドユーザー

**役割**: 商品を検索・閲覧・購入するモバイルアプリ利用者

| 項目 | 詳細 |
|------|------|
| **ユーザー数** | 想定100ユーザー（デモ用途） |
| **利用デバイス** | iOS端末、Android端末、Windows PC |
| **認証方式** | ID/パスワード認証（JWT） |
| **権限** | 商品閲覧・購入・お気に入り機能のみ |
| **利用場所** | 任意（インターネット接続環境） |
| **利用時間** | 任意 |

**主要ユースケース**:
1. ログイン
2. 商品一覧表示
3. 商品検索
4. 商品詳細表示
5. 商品購入（100個単位）
6. お気に入り登録/解除（機能フラグでON/OFF）
7. お気に入り一覧表示

### 3.2 管理者

**役割**: 商品情報とアプリ機能を管理する管理者

| 項目 | 詳細 |
|------|------|
| **ユーザー数** | 想定1-5ユーザー |
| **利用デバイス** | PC（Webブラウザ） |
| **認証方式** | 管理者専用ID/パスワード認証（JWT） |
| **権限** | 商品管理・機能フラグ管理のみ |
| **利用場所** | オフィス（社内ネットワーク想定） |
| **利用時間** | 業務時間内 |

**主要ユースケース**:
1. 管理者ログイン
2. 商品一覧表示
3. 商品情報更新（名前・単価）
4. ユーザー一覧表示
5. ユーザー別機能フラグ設定

## 4. システム構成

### 4.1 物理配置図（開発環境）

```mermaid
graph TB
    subgraph "開発者PC"
        subgraph "iOS開発"
            Xcode[Xcode]
            iOS[iOSアプリ]
        end
        
        subgraph "Android開発"
            AndroidStudio[Android Studio]
            Android[Androidアプリ]
        end
        
        subgraph "Windows開発"
            VisualStudio[Visual Studio]
            Windows[Windowsアプリ]
        end
        
        subgraph "DevContainer"
            Vue[Vue.js開発]
            MobileBFF[Mobile BFF開発]
            AdminBFF[Admin BFF開発]
            WebAPI[Web API開発]
        end
        
        subgraph "Docker"
            DB[(SQLite)]
        end
    end
    
    iOS -.->|localhost:8081| MobileBFF
    Android -.->|localhost:8081| MobileBFF
    Windows -.->|localhost:8081| MobileBFF
    Vue -.->|localhost:8082| AdminBFF
    MobileBFF -.->|localhost:8080| WebAPI
    AdminBFF -.->|localhost:8080| WebAPI
    WebAPI -.->|file://| DB
    
    style Xcode fill:#e1f5ff
    style AndroidStudio fill:#a5d6a7
    style VisualStudio fill:#fff9c4
    style Vue fill:#c5e1a5
    style MobileBFF fill:#ffccbc
    style AdminBFF fill:#ffccbc
    style WebAPI fill:#ffab91
    style DB fill:#ce93d8
```

### 4.2 ネットワーク構成

#### 開発環境ポート一覧

| コンポーネント | ホスト | ポート | プロトコル | アクセス元 |
|--------------|------|-------|----------|----------|
| SQLite | localhost | - | ファイルアクセス | Web API |
| Web API | localhost | 8080 | HTTP | BFF |
| Mobile BFF | localhost | 8081 | HTTP | モバイルアプリ / Windowsアプリ |
| Admin BFF | localhost | 8082 | HTTP | 管理Webアプリ |
| 管理Web（開発サーバー） | localhost | 3000 | HTTP | ブラウザ |

#### ネットワークセグメント

```mermaid
graph TB
    subgraph "Public Network"
        Mobile[モバイルアプリ]
        Browser[Webブラウザ]
    end
    
    subgraph "Application Network"
        BFF1[Mobile BFF<br/>:8081]
        BFF2[Admin BFF<br/>:8082]
        API[Web API<br/>:8080]
    end
    
    subgraph "Database Network"
        DB[(SQLite)]
    end
    
    Mobile -->|HTTP| BFF1
    Browser -->|HTTP| BFF2
    BFF1 -->|HTTP| API
    BFF2 -->|HTTP| API
    API -->|TCP| DB
```

## 5. 外部連携

### 5.1 現在の外部連携（なし）

本システムは、現時点で外部システムとの連携はありません。
すべての機能は内部で完結します。

### 5.2 将来の外部連携候補（Out of Scope）

以下は将来拡張の可能性がある外部連携です（現在は実装しません）：

#### 5.2.1 決済システム連携

```mermaid
sequenceDiagram
    participant User
    participant MobileApp
    participant BFF
    participant WebAPI
    participant PaymentGateway
    
    User->>MobileApp: 購入確定
    MobileApp->>BFF: POST /purchases
    BFF->>WebAPI: POST /api/v1/purchases
    WebAPI->>PaymentGateway: 決済リクエスト
    PaymentGateway-->>WebAPI: 決済結果
    WebAPI-->>BFF: 購入結果
    BFF-->>MobileApp: 購入結果
    MobileApp-->>User: 購入完了
```

**連携候補**:
- Stripe
- PayPal
- Square

**理由**: デモ用途のため、決済連携は不要

---

#### 5.2.2 メール通知連携

```mermaid
graph LR
    WebAPI[Web API] -->|購入通知| EmailService[メール配信サービス]
    EmailService -->|メール送信| User[ユーザー]
```

**連携候補**:
- SendGrid
- Amazon SES
- Mailgun

**理由**: デモ用途のため、メール通知は不要

---

#### 5.2.3 プッシュ通知連携

```mermaid
graph LR
    WebAPI[Web API] -->|通知リクエスト| PushService[プッシュ通知サービス]
    PushService -->|APNs| iOS[iOSデバイス]
    PushService -->|FCM| Android[Androidデバイス]
```

**連携候補**:
- Firebase Cloud Messaging（FCM）
- Apple Push Notification service（APNs）

**理由**: デモ用途のため、プッシュ通知は不要

## 6. データフロー

### 6.1 エンドユーザーのデータフロー

```mermaid
sequenceDiagram
    participant User
    participant Mobile
    participant MobileBFF
    participant WebAPI
    participant DB
    
    Note over User,DB: 1. ログインフロー
    User->>Mobile: ID/パスワード入力
    Mobile->>MobileBFF: POST /api/mobile/login
    MobileBFF->>WebAPI: POST /api/v1/auth/login
    WebAPI->>DB: ユーザー認証
    DB-->>WebAPI: ユーザー情報
    WebAPI-->>MobileBFF: JWT Token
    MobileBFF-->>Mobile: JWT Token
    Mobile->>Mobile: Tokenをセキュア保存
    
    Note over User,DB: 2. 商品取得フロー
    User->>Mobile: 商品一覧表示
    Mobile->>MobileBFF: GET /api/mobile/products (Token)
    MobileBFF->>WebAPI: GET /api/v1/products (Token)
    WebAPI->>WebAPI: Token検証
    WebAPI->>DB: 商品一覧取得
    DB-->>WebAPI: 商品データ
    WebAPI-->>MobileBFF: 商品レスポンス
    MobileBFF-->>Mobile: 商品レスポンス
    Mobile-->>User: 商品一覧表示
```

### 6.2 管理者のデータフロー

```mermaid
sequenceDiagram
    participant Admin
    participant Web
    participant AdminBFF
    participant WebAPI
    participant DB
    
    Note over Admin,DB: 1. 管理者ログインフロー
    Admin->>Web: ID/パスワード入力
    Web->>AdminBFF: POST /api/admin/login
    AdminBFF->>WebAPI: POST /api/v1/auth/admin/login
    WebAPI->>DB: 管理者認証
    DB-->>WebAPI: 管理者情報
    WebAPI-->>AdminBFF: JWT Token (admin)
    AdminBFF-->>Web: JWT Token
    Web->>Web: TokenをlocalStorageに保存
    
    Note over Admin,DB: 2. 商品更新フロー
    Admin->>Web: 商品情報更新
    Web->>AdminBFF: PUT /api/admin/products/{id} (Token)
    AdminBFF->>WebAPI: PUT /api/v1/products/{id} (Token)
    WebAPI->>WebAPI: Token検証（admin権限チェック）
    WebAPI->>DB: 商品情報更新
    DB-->>WebAPI: 更新成功
    WebAPI-->>AdminBFF: 更新レスポンス
    AdminBFF-->>Web: 更新レスポンス
    Web-->>Admin: 更新完了通知
```

## 7. セキュリティ境界

### 7.1 信頼境界

```mermaid
graph TB
    subgraph "Untrusted Zone（信頼できないゾーン）"
        Client[クライアント<br/>モバイル/Web]
    end
    
    subgraph "DMZ（非武装地帯）"
        BFF[BFF Layer<br/>Mobile BFF / Admin BFF]
    end
    
    subgraph "Trusted Zone（信頼できるゾーン）"
        API[Web API<br/>ビジネスロジック]
        DB[(Database<br/>SQLite)]
    end
    
    Client -->|HTTPS<br/>入力検証不可| BFF
    BFF -->|HTTPS+JWT<br/>入力検証必須| API
    API -->|内部ネットワーク<br/>パラメータ化クエリ| DB
    
    style Client fill:#ffcdd2
    style BFF fill:#fff9c4
    style API fill:#c8e6c9
    style DB fill:#b2dfdb
```

### 7.2 認証・認可の境界

| ゾーン | 認証 | 認可 | 検証 |
|-------|-----|------|------|
| **クライアント** | なし | なし | クライアント側バリデーション（UX向上） |
| **BFF** | JWT転送 | なし | エラーハンドリング |
| **Web API** | JWT検証 ✅ | ロールベース権限チェック ✅ | サーバー側バリデーション ✅ |
| **Database** | アプリケーション認証 | なし | DB制約チェック |

### 7.3 データ暗号化ポイント

```mermaid
graph LR
    subgraph "Client"
        App[アプリ]
    end
    
    subgraph "Transport"
        TLS[TLS 1.2+<br/>暗号化通信]
    end
    
    subgraph "Server"
        BFF[BFF]
        API[Web API]
    end
    
    subgraph "Storage"
        Keychain[Keychain/EncryptedPrefs/<br/>Credential Manager<br/>暗号化ストレージ]
        DB[(SQLite<br/>パスワードハッシュ化)]
    end
    
    App -->|HTTPS| TLS
    TLS --> BFF
    BFF -->|HTTPS| API
    App -.->|JWT保存| Keychain
    API -.->|パスワード保存| DB
    
    style TLS fill:#c8e6c9
    style Keychain fill:#b2dfdb
    style DB fill:#b2dfdb
```

## 8. 非機能要件の文脈

### 8.1 パフォーマンス要件

| 項目 | 要件 | 測定ポイント |
|------|------|------------|
| API応答時間 | 3秒以内 | BFF → Web API → DB |
| 画面表示時間 | 3秒以内 | クライアント → BFF → Web API |
| 同時接続ユーザー | 100ユーザー | Web API |
| DBコネクションプール | 1接続（SQLite） | Web API → DB |

### 8.2 スケーラビリティ制約

**水平スケーリング**: 非対応（デモ用途）
**垂直スケーリング**: 非対応（デモ用途）

**理由**: デモンストレーション用途のため、スケーリングは考慮しない

### 8.3 可用性制約

**稼働率**: 規定なし（デモ用途）
**冗長化**: なし
**バックアップ**: 手動

**理由**: デモンストレーション用途のため、高可用性構成は不要

## 9. システム間インターフェース

### 9.1 プロトコル一覧

| インターフェース | プロトコル | データ形式 | 認証 |
|---------------|----------|----------|------|
| モバイルアプリ / Windowsアプリ ↔ Mobile BFF | HTTPS/REST | JSON | JWT（ログイン後） |
| 管理Webアプリ ↔ Admin BFF | HTTPS/REST | JSON | JWT（ログイン後） |
| BFF ↔ Web API | HTTPS/REST | JSON | JWT転送 |
| Web API ↔ SQLite | JDBC/ファイル | SQL | 不要（ファイルベース） |

### 9.2 API バージョニング戦略

**現状**: バージョニングなし（v1固定）
**理由**: デモ用途のため、APIバージョン管理は不要

**将来拡張時の推奨**:
- URL パス方式: `/api/v1/products`, `/api/v2/products`
- ヘッダー方式: `Accept: application/vnd.api.v1+json`

## 10. 環境定義

### 10.1 環境一覧

| 環境 | 用途 | ホスティング | データベース |
|------|------|------------|------------|
| **開発環境（Local）** | 開発・デバッグ | localhost / DevContainer | SQLite（ファイルベース） |
| **本番環境** | デモ実行 | TBD（現在未定義） | TBD |

### 10.2 環境ごとの設定

#### 開発環境

| 項目 | 設定値 |
|------|-------|
| Web API | `http://localhost:8080` |
| Mobile BFF | `http://localhost:8081` |
| Admin BFF | `http://localhost:8082` |
| SQLite | `./data/mobile_app.db` |
| DB名 | `mobile_app_db` |
| ログレベル | DEBUG |

#### 本番環境（未定義）

| 項目 | 設定値 |
|------|-------|
| Web API | TBD |
| Mobile BFF | TBD |
| Admin BFF | TBD |
| SQLite | TBD |
| DB名 | TBD |
| ログレベル | INFO |

## 11. 参照ドキュメント

| ドキュメント | パス |
|------------|------|
| アーキテクチャ概要 | `00-overview.md` |
| コンポーネント設計 | `02-component-design.md` |
| セキュリティアーキテクチャ | `05-security-architecture.md` |
| 機能要件 | `/docs/specs/mobile-app-system/02-functional-requirements.md` |
| 非機能要件 | `/docs/specs/mobile-app-system/03-non-functional-requirements.md` |

---

**End of Document**
