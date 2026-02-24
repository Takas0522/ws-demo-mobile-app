---
description: Windows C++ アプリケーション (Win32 API) のコーディング規約とプロジェクト構造
applyTo: "src/windows-app/**/*.{h,hpp,cpp,rc}"
---

# Windows C++ アプリケーション コーディング規約

## プロジェクト概要

- 商品閲覧・購入を行う Win32 ネイティブ Windows デスクトップアプリケーション
- MVVM パターンを採用（iOS/Android クライアントと同様の設計）
- Mobile BFF (`http://localhost:8081/api/mobile`) に接続
- JWT トークンによる認証
- 定期ポーリングによる状態更新機能あり

## 技術スタック

- C++20 / Win32 API / MSBuild (.vcxproj)
- HTTP通信: WinHTTP (`winhttp.h`)
- JSON: nlohmann/json (header-only)
- セキュアストレージ: Windows Credential Manager (DPAPI, `wincred.h`)

## コードスタイル

- インデント: タブ（幅4スペース相当）
- 行の最大長: 120文字
- 中括弧: Allman スタイル（次の行に開始括弧）
- ヘッダーガード: `#pragma once`
- トップレベル名前空間: `ws::`

## 命名規則

- クラス / 構造体: PascalCase (`ProductListViewModel`)
- メンバ関数: PascalCase (`FetchProducts()`)
- メンバ変数: m_ + camelCase (`m_products`, `m_isLoading`)
- 静的メンバ変数: s_ + camelCase (`s_instance`)
- ローカル変数: camelCase (`userId`, `productName`)
- 定数 / constexpr: k + PascalCase (`kMaxRetryCount`)
- マクロ: UPPER_SNAKE_CASE (`WS_APP_VERSION`)
- 名前空間: lowercase (`ws::models`, `ws::services`)
- enum class: PascalCase（型・値共に）(`enum class ApiError { NetworkError, Unauthorized }`)

## 設計原則

- スマートポインタ (`std::unique_ptr`, `std::shared_ptr`) を使用、生ポインタは最小限
- 文字列: `std::wstring` を Windows API 連携に、`std::string` を内部処理に使用
- エラーハンドリング: 例外は使用しない。`std::expected` または `std::optional` + エラーコードで処理
- リソース管理は RAII パターンを徹底
- `[[nodiscard]]` 属性を戻り値が無視されるべきでない関数に付与
- コピー不要なクラスは `= delete` で明示的にコピーを禁止

## レイヤー構造

```
Models/      → データモデル（構造体、JSON シリアライズ）
ViewModels/  → ビジネスロジック、状態管理、変更通知コールバック
Views/       → Win32 ウィンドウプロシージャ、UI 描画
Services/    → HTTP 通信、認証、ポーリング
Utils/       → Credential Manager、JSON ヘルパー、定数
```

## Mobile BFF API

- ベースURL: `http://localhost:8081/api/mobile`
- 認証: `Authorization: Bearer {JWT_TOKEN}` ヘッダー（ログインAPI以外）
- レスポンス形式: `{ "data": { ... }, "timestamp": "..." }`
- エラー形式: `{ "error": { "code": "...", "message": "...", "details": "..." }, "timestamp": "..." }`

## スレッディング

- HTTP リクエストはバックグラウンドスレッドで実行
- UI 更新は `PostMessage` / `SendMessage` で UI スレッドに通知
- 定期更新は `SetTimer` / `WM_TIMER` でトリガー
