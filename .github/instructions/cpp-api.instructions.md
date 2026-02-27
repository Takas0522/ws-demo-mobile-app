---
description: C++ REST API サーバー (CppApiServer) のコーディング規約とプロジェクト構造
applyTo: "src/cpp-api/**/*.{h,hpp,cpp}"
---

# C++ API サーバー コーディング規約

## プロジェクト概要

- SQLite に直接アクセスする軽量 C++ REST API サーバー（デモ用途）
- Windows アプリ (WsDemoMobileApp.WindowsApp) 専用のバックエンド
- Mobile BFF と Web API を1プロセスに統合した構成
- ポート 8081 でリッスンし `/api/mobile/*` エンドポイントを提供

## 技術スタック

- C++20 / MSBuild (.vcxproj)
- HTTP サーバー: cpp-httplib (header-only, `httplib.h`)
- データベース: SQLite3 (amalgamation, C API)
- JSON: nlohmann/json (NuGet, header-only)
- JWT 認証: Windows BCrypt API による HMAC-SHA256 自前実装 (`JwtHelper.h`)
- パスワードハッシュ: bcrypt (C 実装を include/bcrypt/ に同梱)

## コードスタイル

Windows アプリと同一規約を適用:

- インデント: タブ（幅4スペース相当）
- 行の最大長: 120文字
- 中括弧: Allman スタイル（次の行に開始括弧）
- ヘッダーガード: `#pragma once`
- トップレベル名前空間: `ws::`

## 命名規則

- クラス / 構造体: PascalCase (`ProductRepository`, `AuthService`)
- メンバ関数: PascalCase (`FindByLoginId()`, `CreateJwt()`)
- メンバ変数: m_ + camelCase (`m_db`, `m_jwtSecret`)
- 静的メンバ変数: s_ + camelCase (`s_instance`)
- ローカル変数: camelCase (`userId`, `productName`)
- 定数 / constexpr: k + PascalCase (`kDefaultPort`, `kJwtSecret`)
- マクロ: UPPER_SNAKE_CASE
- 名前空間: lowercase (`ws::database`, `ws::services`, `ws::routes`)
- enum class: PascalCase（型・値共に）

## 設計原則

- スマートポインタ使用、生ポインタは最小限
- 文字列: `std::string` を基本使用（Win32 API 呼び出しが少ないため）
- エラーハンドリング: 例外は使用しない。`std::expected` または `std::optional` で処理
- リソース管理は RAII パターンを徹底
- `[[nodiscard]]` 属性を戻り値が無視されるべきでない関数に付与
- コピー不要なクラスは `= delete` で明示的にコピーを禁止

## レイヤー構造

```
Models/      → データモデル（構造体、JSON シリアライズ）
Database/    → SQLite アクセス（Repository パターン、PreparedStatement）
Services/    → ビジネスロジック（認証、商品、購入、お気に入り、機能フラグ）
Middleware/  → JWT 認証ミドルウェア
Routes/      → HTTP ルーティング（エンドポイント定義、リクエスト/レスポンス処理）
Utils/       → JWT ヘルパー、UUID 生成、JSON レスポンスビルダー、定数
```

## API エンドポイント

- ベースURL: `http://localhost:8081/api/mobile`
- 認証: `Authorization: Bearer {JWT_TOKEN}` ヘッダー（ログイン API 以外）
- レスポンス形式: `{ "data": { ... }, "timestamp": "..." }`
- エラー形式: `{ "error": { "code": "...", "message": "...", "details": "..." }, "timestamp": "..." }`

## スレッディング

- cpp-httplib はリクエストごとにスレッドプールから処理
- SQLite アクセスは `std::mutex` で排他制御
- WAL モードを有効化してリード並行性を確保
