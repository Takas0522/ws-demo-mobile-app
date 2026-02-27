# 統合テスト計画書

> 最終更新: 2025-01-XX  
> ステータス: Draft  
> バージョン: 1.0

## 変更履歴

| バージョン | 日付 | 変更内容 | 著者 |
|-----------|------|---------|------|
| 1.0 | 2025-01-XX | 初版作成 | Development Team |

---

## 1. 目的

本ドキュメントは、mobile-app-systemの統合テストについて、以下を定義します：
- 統合テストの範囲と目的
- テストケース一覧
- テスト実施手順
- 期待される結果

---

## 2. テストスコープ

### 2.1 テスト対象システム

| コンポーネント | バージョン | テスト範囲 |
|--------------|-----------|----------|
| Web API | Spring Boot latest | 全エンドポイント |
| Mobile BFF | Spring Boot latest | 全エンドポイント |
| Admin BFF | Spring Boot latest | 全エンドポイント |
| Admin Web | Vue.js latest | 全画面 |
| SQLite | Latest | データ整合性 |

### 2.2 テストレベル

本統合テストでは以下のテストレベルを実施します：

1. **コンポーネント統合テスト** - 各サービス間の連携確認
2. **システム統合テスト** - システム全体での動作確認
3. **エンドツーエンドテスト** - ユーザーシナリオベースのテスト

---

## 3. テスト環境

### 3.1 必要な環境

```
┌─────────────────┐
│   SQLite        │ ← ファイルベース
└────────┬────────┘
         │
┌────────┴────────┐
│    Web API      │ ← Port 8080
└────────┬────────┘
         │
    ┌────┴────┐
    │         │
┌───┴───┐ ┌──┴──────┐
│Mobile │ │  Admin  │
│  BFF  │ │   BFF   │
│ 8081  │ │  8082   │
└───┬───┘ └──┬──────┘
    │        │
┌───┴───┐ ┌──┴──────┐
│Mobile │ │  Admin  │
│  App  │ │   Web   │
│(Mock) │ │  3000   │
└───────┘ └─────────┘
```

### 3.2 環境構築手順

詳細は `docs/DEMO-SETUP.md` を参照してください。

---

## 4. テストケース一覧

### 4.1 全体サマリー

| カテゴリ | テストケース数 | 優先度 |
|---------|--------------|--------|
| 全コンポーネント統合 | 5 | 高 |
| 機能フラグシナリオ | 4 | 高 |
| 購入フローE2E | 6 | 高 |
| ログイン・認証 | 5 | 高 |
| エラーシナリオ | 8 | 中 |
| **合計** | **28** | - |

---

## 5. テストケース詳細

### TC-INT-001: 全コンポーネント起動確認

**目的**: すべてのサービスが正常に起動し、疎通できることを確認

**前提条件**:
- Docker環境が利用可能
- ポート 8080, 8081, 8082, 3000 が空いている

**テスト手順**:
1. SQLiteデータベースを初期化
2. Web APIを起動（`./gradlew bootRun` in web-api）
3. Mobile BFFを起動（`./gradlew bootRun` in mobile-bff）
4. Admin BFFを起動（`./gradlew bootRun` in admin-bff）
5. Admin Webを起動（`npm run dev` in admin-web）
6. 各サービスのヘルスチェックエンドポイントを確認

**期待結果**:
```bash
# SQLite
sqlite3 ./data/mobile_app.db "SELECT 1;"
→ 接続成功

# Web API
curl http://localhost:8080/api/v1/health
→ {"status":"UP"}

# Mobile BFF
curl http://localhost:8081/api/v1/health
→ {"status":"UP"}

# Admin BFF
curl http://localhost:8082/api/v1/health
→ {"status":"UP"}

# Admin Web
curl http://localhost:3000/
→ HTML応答
```

**実施記録**: `docs/testing/test-results.md` に記載

---

### TC-INT-002: データベース接続確認

**目的**: 全サービスからSQLiteへの接続を確認

**前提条件**:
- すべてのサービスが起動済み

**テスト手順**:
1. Web APIのログを確認（HikariCP接続ログ）
2. テーブル存在確認クエリを実行
   ```sql
   SELECT table_name FROM information_schema.tables 
   WHERE table_schema='public';
   ```
3. 初期データ存在確認
   ```sql
   SELECT COUNT(*) FROM users;
   SELECT COUNT(*) FROM products;
   ```

**期待結果**:
- テーブル6つ存在: users, products, purchases, favorites, feature_flags, admins
- ユーザー3件以上存在
- 商品3件以上存在
- 管理者1件以上存在

---

### TC-INT-003: JWT認証フロー

**目的**: JWT認証が全体で正常に動作することを確認

**テスト手順**:
1. ログインAPIでトークン取得
   ```bash
   curl -X POST http://localhost:8080/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{"loginId":"user001","password":"password123"}'
   ```
2. 取得したトークンで保護されたAPIを呼び出し
   ```bash
   TOKEN="<取得したトークン>"
   curl http://localhost:8080/api/v1/products \
     -H "Authorization: Bearer $TOKEN"
   ```
3. トークンなしで保護されたAPIを呼び出し（エラー確認）
   ```bash
   curl http://localhost:8080/api/v1/products
   ```

**期待結果**:
- Step 1: 200 OK、`{"token":"...", "expiresIn":3600}` 返却
- Step 2: 200 OK、商品一覧返却
- Step 3: 401 Unauthorized、`{"errorCode":"AUTH_005"}`

---

### TC-INT-004: Mobile BFF → Web API 連携

**目的**: Mobile BFFがWeb APIと正常に連携することを確認

**テスト手順**:
1. Mobile BFF経由でログイン
   ```bash
   curl -X POST http://localhost:8081/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{"loginId":"user001","password":"password123"}'
   ```
2. Mobile BFF経由で商品一覧取得
   ```bash
   curl http://localhost:8081/api/v1/products \
     -H "Authorization: Bearer $TOKEN"
   ```

**期待結果**:
- Mobile BFFのレスポンスがWeb APIと同一形式
- ログにWeb API呼び出しが記録される

---

### TC-INT-005: Admin BFF → Web API 連携

**目的**: Admin BFFがWeb APIと正常に連携することを確認

**テスト手順**:
1. Admin BFF経由で管理者ログイン
   ```bash
   curl -X POST http://localhost:8082/api/v1/auth/admin/login \
     -H "Content-Type: application/json" \
     -d '{"loginId":"admin001","password":"adminpass123"}'
   ```
2. Admin BFF経由で商品一覧取得
   ```bash
   curl http://localhost:8082/api/v1/admin/products \
     -H "Authorization: Bearer $TOKEN"
   ```

**期待結果**:
- Admin BFFのレスポンスがWeb APIと同一形式
- 管理者トークンが正常に機能

---

## 6. 機能フラグシナリオテスト

詳細は `docs/testing/test-scenarios/feature-flag-scenario.md` を参照

---

## 7. 購入フローエンドツーエンドテスト

詳細は `docs/testing/test-scenarios/purchase-flow-scenario.md` を参照

---

## 8. ログイン・認証シナリオテスト

詳細は `docs/testing/test-scenarios/auth-scenario.md` を参照

---

## 9. エラーシナリオテスト

詳細は `docs/testing/test-scenarios/error-scenario.md` を参照

---

## 10. テスト実施スケジュール

### 10.1 推奨実施順序

1. **Day 1**: 環境セットアップと基本疎通（TC-INT-001〜005）
2. **Day 2**: 機能フラグシナリオテスト
3. **Day 3**: 購入フローE2Eテスト
4. **Day 4**: ログイン・認証シナリオテスト
5. **Day 5**: エラーシナリオテスト

### 10.2 所要時間見積もり

| テストカテゴリ | 所要時間（目安） |
|--------------|----------------|
| 環境セットアップ | 30分 |
| 全コンポーネント統合 | 1時間 |
| 機能フラグシナリオ | 1時間 |
| 購入フローE2E | 2時間 |
| ログイン・認証 | 1時間 |
| エラーシナリオ | 2時間 |
| **合計** | **約7.5時間** |

---

## 11. テスト完了基準

### 11.1 合格基準

- [ ] すべての優先度「高」テストケースがPASS
- [ ] 致命的な不具合が0件
- [ ] 全サービスが安定して起動・動作する
- [ ] 主要シナリオ（ログイン→購入）が正常動作する

### 11.2 不合格時の対応

1. 不具合を `docs/testing/test-results.md` に記録
2. 優先度「高」の不具合は即座に修正
3. 修正後、関連テストケースを再実行

---

## 12. テストツール

### 12.1 必要なツール

| ツール | 用途 | インストール |
|-------|------|------------|
| curl | APIテスト | 標準装備 |
| jq | JSON整形 | `apt-get install jq` |
| sqlite3 | DB確認 | SQLite Client |
| Postman | API手動テスト（オプション） | https://postman.com |

### 12.2 便利なコマンド

```bash
# JSONを整形して表示
curl ... | jq .

# ヘッダーも含めて表示
curl -v ...

# レスポンスタイムを計測
curl -w "@curl-format.txt" -o /dev/null -s ...
```

---

## 13. トラブルシューティング

### 13.1 よくある問題

| 問題 | 原因 | 解決方法 |
|------|------|---------|
| ポートが使用中 | 既にサービスが起動している | `lsof -i :8080` で確認、プロセス停止 |
| DB接続エラー | SQLiteファイルが存在しない | `ls ./data/mobile_app.db` で確認 |
| 401エラー | トークンが無効または期限切れ | 新しいトークンを取得 |
| 404エラー | エンドポイントが間違っている | URLを確認 |

### 13.2 ログ確認方法

```bash
# Web API
tail -f web-api/logs/application.log

# SQLite
sqlite3 ./data/mobile_app.db "PRAGMA integrity_check;"

# すべてのサービス
tail -f web-api/logs/application.log \
     mobile-bff/logs/application.log \
     admin-bff/logs/application.log
```

---

## 14. 参考資料

- [テスト戦略](../specs/mobile-app-system/12-testing-strategy.md)
- [API仕様](../specs/mobile-app-system/05-api-spec.md)
- [エラーハンドリング](../specs/mobile-app-system/07-error-handling.md)
- [デモセットアップ](../DEMO-SETUP.md)

---

**End of Document**
