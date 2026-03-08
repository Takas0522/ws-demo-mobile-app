# Mobile BFF 改修 — 価格履歴取得エンドポイント追加

## 1. 改修概要

Mobile BFF に価格履歴取得エンドポイントを追加し、Web API（API-020）へリクエストを転送する。
既存の BFF アーキテクチャ（薄い BFF プロキシパターン）に準拠し、`WebApiClient` を使用して Web API にパススルーする。

## 2. 追加エンドポイント

| 項目               | 値                                                   |
| ------------------ | ---------------------------------------------------- |
| BFF エンドポイント | `GET /api/mobile/products/{id}/price-history`        |
| 転送先 Web API     | `GET /api/v1/products/{id}/price-history`            |
| 認証               | JWT 認証必須（Authorization ヘッダーをそのまま転送） |

### クエリパラメータ（Web API へ転送）

| パラメータ名 | 型                | 必須 | デフォルト  | 説明                         |
| ------------ | ----------------- | ---- | ----------- | ---------------------------- |
| startDate    | string (ISO 8601) | ─    | 5年前の日付 | 検索開始日 (例: 2021-03-01)  |
| endDate      | string (ISO 8601) | ─    | 現在日付    | 検索終了日 (例: 2026-03-01)  |
| page         | integer           | ─    | 1           | ページ番号（1以上）          |
| limit        | integer           | ─    | 20          | 1ページあたり件数（最大100） |

## 3. 変更対象ファイル

### 3.1 新規作成

#### 3.1.1 DTO: `PriceHistoryDto.java`

- **パッケージ**: `com.example.mobilebff.dto`
- **ファイルパス**: `src/mobile-bff/src/main/java/com/example/mobilebff/dto/PriceHistoryDto.java`

```java
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PriceHistoryDto {
    private Integer priceHistoryId;
    private Integer productId;
    private Integer oldPrice;
    private Integer newPrice;
    private String changedAt;
    private ChangedByDto changedBy;
    private String changeReason;
}
```

#### 3.1.2 DTO: `ChangedByDto.java`

- **パッケージ**: `com.example.mobilebff.dto`
- **ファイルパス**: `src/mobile-bff/src/main/java/com/example/mobilebff/dto/ChangedByDto.java`

```java
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ChangedByDto {
    private Integer userId;
    private String userName;
}
```

#### 3.1.3 DTO: `PriceHistoryListDto.java`

- **パッケージ**: `com.example.mobilebff.dto`
- **ファイルパス**: `src/mobile-bff/src/main/java/com/example/mobilebff/dto/PriceHistoryListDto.java`

Web API のレスポンス `data` 内のフィールドをマッピングする。

```java
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PriceHistoryListDto {
    private List<PriceHistoryDto> priceHistory;
    private PaginationDto pagination;
}
```

#### 3.1.4 DTO: `PaginationDto.java`

- **パッケージ**: `com.example.mobilebff.dto`
- **ファイルパス**: `src/mobile-bff/src/main/java/com/example/mobilebff/dto/PaginationDto.java`

```java
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PaginationDto {
    private Integer currentPage;
    private Integer totalPages;
    private Integer totalItems;
    private Integer itemsPerPage;
}
```

### 3.2 既存ファイル変更

#### 3.2.1 `ProductController.java` にエンドポイント追加

- **ファイルパス**: `src/mobile-bff/src/main/java/com/example/mobilebff/controller/ProductController.java`
- **変更内容**: 価格履歴取得メソッドを追加

```java
/**
 * 商品価格履歴取得
 *
 * @param id 商品ID
 * @param startDate 検索開始日（任意）
 * @param endDate 検索終了日（任意）
 * @param page ページ番号（任意）
 * @param limit 1ページあたり件数（任意）
 * @param authorization 認証トークン
 * @return 価格履歴一覧
 */
@GetMapping("/{id}/price-history")
public ResponseEntity<ApiResponse<PriceHistoryListDto>> getPriceHistory(
        @PathVariable Long id,
        @RequestParam(required = false) String startDate,
        @RequestParam(required = false) String endDate,
        @RequestParam(required = false) Integer page,
        @RequestParam(required = false) Integer limit,
        @RequestHeader(value = "Authorization", required = false) String authorization) {
    log.info("Get price history request: productId={}", id);

    String token = extractToken(authorization);

    // クエリパラメータを構築
    StringBuilder path = new StringBuilder("/api/v1/products/" + id + "/price-history");
    List<String> params = new ArrayList<>();
    if (startDate != null) params.add("startDate=" + startDate);
    if (endDate != null) params.add("endDate=" + endDate);
    if (page != null) params.add("page=" + page);
    if (limit != null) params.add("limit=" + limit);
    if (!params.isEmpty()) {
        path.append("?").append(String.join("&", params));
    }

    // Web APIにリクエストを転送
    ApiResponse<PriceHistoryListDto> response = webApiClient.get(
        path.toString(),
        (Class<ApiResponse<PriceHistoryListDto>>) (Class<?>) ApiResponse.class,
        token
    );

    return ResponseEntity.ok(response);
}
```

## 4. レスポンス形式

### 4.1 成功時 (200 OK)

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

### 4.2 該当なし (200 OK)

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

### 4.3 エラー時

Web API からのエラーレスポンスをそのまま転送する（既存の `GlobalExceptionHandler` で処理される）。

| HTTP | エラーコード | メッセージ                       | 発生条件                   |
| ---- | ------------ | -------------------------------- | -------------------------- |
| 400  | VAL_001      | 入力値が不正です                 | startDate/endDate 形式不正 |
| 401  | AUTH_001     | 認証トークンが無効です           | トークン未送信・不正       |
| 401  | AUTH_004     | トークンの有効期限が切れています | 有効期限切れ               |
| 403  | AUTH_005     | 権限がありません                 | 権限なしユーザー           |
| 404  | PRODUCT_001  | 商品が見つかりません             | 存在しない productId 指定  |
| 500  | SYS_001      | サーバー内部エラー               | 予期しないエラー           |

## 5. 設計上の考慮事項

- 既存の BFF パターン（薄いプロキシ）に準拠し、ビジネスロジックは持たない
- クエリパラメータは Web API にそのまま転送する
- 認証トークンは既存の `extractToken()` メソッドで抽出し、`WebApiClient` に渡す
- エラーハンドリングは既存の `GlobalExceptionHandler` に委譲する
