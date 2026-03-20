# 価格履歴機能 — API 仕様

## API 基本情報

| 項目           | 内容                                                                    |
| -------------- | ----------------------------------------------------------------------- |
| API ID         | API-020                                                                 |
| API名          | 価格履歴取得                                                            |
| エンドポイント | `GET /api/v1/products/{productId}/price-history`                        |
| HTTPメソッド   | GET                                                                     |
| 認証           | JWT 認証必須（`Authorization: Bearer {TOKEN}`）                         |
| 権限           | `user` / `admin`                                                        |
| 概要           | 指定商品の価格変更履歴を取得する。最大5年分の履歴を日時降順で返却する。 |

## パスパラメータ

| No  | パラメータ名 | 型      | 必須 | 説明         |
| --- | ------------ | ------- | ---- | ------------ |
| 1   | `productId`  | integer | ○    | 対象商品のID |

## クエリパラメータ

| No  | パラメータ名 | 型                | 必須 | デフォルト  | 説明                          |
| --- | ------------ | ----------------- | ---- | ----------- | ----------------------------- |
| 1   | `startDate`  | string (ISO 8601) | —    | 5年前の日付 | 検索開始日 (例: `2021-03-01`) |
| 2   | `endDate`    | string (ISO 8601) | —    | 現在日付    | 検索終了日 (例: `2026-03-01`) |
| 3   | `page`       | integer           | —    | `1`         | ページ番号（1以上）           |
| 4   | `limit`      | integer           | —    | `20`        | 1ページあたり件数（最大100）  |

## リクエスト例（cURL）

```bash
curl -X GET \
  "http://localhost:8080/api/v1/products/1/price-history?startDate=2021-03-01&endDate=2026-03-01&page=1&limit=20" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN"
```

## レスポンス（成功時: 200 OK）

```json
{
  "data": {
    "priceHistory": [
      {
        "priceHistoryId": 1,
        "productId": 1,
        "oldPrice": 1000,
        "newPrice": 1200,
        "changedAt": "2025-06-15T10:30:00",
        "changedBy": {
          "userId": 3,
          "userName": "管理者"
        },
        "changeReason": "原材料費高騰に伴う価格改定"
      }
    ],
    "pagination": {
      "currentPage": 1,
      "totalPages": 3,
      "totalItems": 45,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2026-03-01T12:00:00Z"
}
```

## レスポンスフィールド定義

| No  | フィールド                          | 型           | 説明                     |
| --- | ----------------------------------- | ------------ | ------------------------ |
| 1   | `priceHistory`                      | array        | 価格履歴レコードの配列   |
| 2   | `priceHistory[].priceHistoryId`     | integer      | 価格履歴ID               |
| 3   | `priceHistory[].productId`          | integer      | 商品ID                   |
| 4   | `priceHistory[].oldPrice`           | integer      | 変更前単価（円）         |
| 5   | `priceHistory[].newPrice`           | integer      | 変更後単価（円）         |
| 6   | `priceHistory[].changedAt`          | string       | 変更日時（ISO 8601形式） |
| 7   | `priceHistory[].changedBy.userId`   | integer      | 変更者ユーザーID         |
| 8   | `priceHistory[].changedBy.userName` | string       | 変更者ユーザー名         |
| 9   | `priceHistory[].changeReason`       | string\|null | 変更理由（任意項目）     |
| 10  | `pagination.currentPage`            | integer      | 現在のページ番号         |
| 11  | `pagination.totalPages`             | integer      | 総ページ数               |
| 12  | `pagination.totalItems`             | integer      | 総件数                   |
| 13  | `pagination.itemsPerPage`           | integer      | 1ページあたりの件数      |

## レスポンス（該当なし: 200 OK）

```json
{
  "data": {
    "priceHistory": [],
    "pagination": {
      "currentPage": 1,
      "totalPages": 0,
      "totalItems": 0,
      "itemsPerPage": 20
    }
  },
  "timestamp": "2026-03-01T12:00:00Z"
}
```

## エラーレスポンス

| No  | HTTP | エラーコード  | メッセージ                       | 発生条件                       |
| --- | ---- | ------------- | -------------------------------- | ------------------------------ |
| 1   | 400  | `VAL_001`     | 入力値が不正です                 | `startDate`/`endDate` 形式不正 |
| 2   | 401  | `AUTH_001`    | 認証トークンが無効です           | トークン未送信・不正           |
| 3   | 401  | `AUTH_004`    | トークンの有効期限が切れています | 有効期限切れ                   |
| 4   | 403  | `AUTH_005`    | 権限がありません                 | 権限なしユーザー               |
| 5   | 404  | `PRODUCT_001` | 商品が見つかりません             | 存在しない `productId` 指定    |
| 6   | 500  | `SYS_001`     | サーバー内部エラー               | 予期しないエラー               |

## エラーレスポンス形式

```json
{
  "error": {
    "code": "PRODUCT_001",
    "message": "商品が見つかりません"
  },
  "timestamp": "2026-03-01T12:00:00Z"
}
```

## BFF 転送仕様

| No  | BFF    | BFF エンドポイント                            | 転送先 Web API                            |
| --- | ------ | --------------------------------------------- | ----------------------------------------- |
| 1   | Mobile | `GET /api/mobile/products/{id}/price-history` | `GET /api/v1/products/{id}/price-history` |
| 2   | Admin  | `GET /api/admin/products/{id}/price-history`  | `GET /api/v1/products/{id}/price-history` |

## 備考・補足事項

1. 価格履歴は商品更新API（API-013: `PUT /api/v1/products/{id}`）で価格変更が行われた際に、サーバー側で自動的に `product_price_history` テーブルに記録される。
2. データ保持期間は5年間。超過データは定期バッチ（日次実行）にて物理削除する。
3. `startDate` / `endDate` を省略した場合、保持期間内の全履歴が返却される。
4. レスポンスは `changed_at` の降順（新しい順）でソートされる。
5. 管理者・一般ユーザーともに参照可能。価格変更操作は管理者のみ。
6. ページネーション仕様は既存の商品一覧API（API-010）と同一。
7. 本APIは既存のAPI体系（`/api/v1/`）に準拠して設計されている。

## 現行コードとの差分

### admin-struts における現状

現在 `struts-api.xml` に以下のアクションマッピングが存在する:

```xml
<!-- GET /api/v1/products-price-history?productId=xxx — 価格履歴 -->
<action name="products-price-history" class="com.example.admin.api.product.ApiProductAction" method="priceHistory">
    <interceptor-ref name="apiSecuredStack"/>
</action>
```

### 設計書の仕様とのギャップ

| 観点               | 設計書仕様                                                      | 現行実装                                                              | ギャップ                                                                                               |
| ------------------ | --------------------------------------------------------------- | --------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| エンドポイント形式 | `GET /api/v1/products/{productId}/price-history` (RESTful パス) | `GET /api/v1/products-price-history?productId=xxx` (クエリパラメータ) | Struts 2 のルーティング制約により RESTful パスの完全準拠は困難。現行形式を維持し、BFF がパス変換を担当 |
| ページネーション   | `page` / `limit` クエリパラメータ対応                           | 全件返却（ページネーション未実装）                                    | DAO・Service・Action にページネーション対応が必要                                                      |
| 日付フィルタ       | `startDate` / `endDate` クエリパラメータ                        | 未実装                                                                | DAO に日付範囲条件の SQL 追加が必要                                                                    |
| レスポンス形式     | `changedBy` がオブジェクト（`userId` + `userName`）             | `changedBy` が `Long` 型の ID のみ                                    | `users` テーブルとの JOIN またはユーザー名取得ロジックの追加が必要                                     |
| エラーハンドリング | 設計書記載のエラーコード体系                                    | 一部実装済み（`PROD_001` / `PROD_002` 等）                            | 設計書のエラーコード（`VAL_001` 等）との整合確認が必要                                                 |
