---
name: struts-unit-testing
description: admin-struts Web アプリのユニットテストを構築するスキル。JUnit 4 を使ったテストシナリオの作成・ブラックボックスレビュー・テスト実装・実行・レポート出力まで一連のプロセスをガイドする。「admin-struts のテストを作成して」「Struts アクションのテストを書いて」「Service のユニットテストを実装して」「テストシナリオをレビューして」「テストを実行してレポートを出して」などの依頼で自動的にロードされる。
license: Complete terms in LICENSE.txt
---

# admin-struts ユニットテスト構築スキル

Struts 2 Web アプリケーション（`admin-struts`）を対象に、JUnit 4 によるテストシナリオの作成からレポート出力までの一連のプロセスを提供するスキル。

## このスキルを使う場面

- `admin-struts` のユニットテストを新規に作成するとき
- 既存のテストシナリオをブラックボックステストの観点でレビューするとき
- JUnit のテストコードを実装するとき
- テストを実行して結果レポートを出力するとき

## 前提条件

| 項目 | 詳細 |
|---|---|
| Java バージョン | 1.7 |
| ビルドツール | Apache Maven |
| テストフレームワーク | JUnit 4.12（`pom.xml` 設定済み） |
| アプリケーションフレームワーク | Struts 2.3.37 / Spring 3.2.18.RELEASE |
| DB | SQLite（commons-dbutils 経由） |
| モックライブラリ | Mockito（未設定の場合は `pom.xml` に追記が必要。Java 7 互換の Mockito 2.x を使用すること） |

**プロジェクトパス**:

| 項目 | パス |
|---|---|
| プロジェクトルート | `src/admin-struts/` |
| ソースパス | `src/admin-struts/src/main/java/com/example/admin/` |
| テスト配置先 | `src/admin-struts/src/test/java/com/example/admin/` |
| pom.xml | `src/admin-struts/pom.xml` |

**レイヤー構成と主要テスト対象**:

| レイヤー | パッケージ | 主なクラス | テストの観点 |
|---|---|---|---|
| Entity | `entity/` | Product, User, Purchase, Favorite, FeatureFlag, UserFeatureFlag, ProductPriceHistory | getter/setter、Serializable |
| DAO | `dao/` | ProductDao, UserDao, PurchaseDao, FavoriteDao, FeatureFlagDao, UserFeatureFlagDao, ProductPriceHistoryDao | SQL 実行、DB 操作、カラムマッピング |
| Service | `service/` | ProductService, AuthService, UserService, PurchaseService, FavoriteService, FeatureFlagService | ビジネスロジック、例外ハンドリング、DAO 呼び出し |
| Action（Web） | `action/` | LoginAction, LogoutAction, ProductListAction, ProductDetailAction, ProductUpdateAction, UserListAction, FeatureFlagAction, FeatureFlagUpdateAction | バリデーション、サービス呼び出し、戻り値（SUCCESS/ERROR/INPUT） |
| Action（API） | `api/` | ApiAuthAction, ApiProductAction, ApiPurchaseAction, ApiFavoriteAction, ApiFeatureFlagAction, ApiHealthAction | JSON レスポンス、HTTP ステータス、認証 |
| Interceptor | `interceptor/` | AuthInterceptor, JwtInterceptor | セッション/JWT 認証の判定ロジック |
| Util | `util/` | DatabaseUtil, JwtUtil, PasswordUtil, ApiResponseUtil | ユーティリティメソッドの正確性 |
| DTO | `dto/` | ApiResponse | レスポンス構造の生成 |

## ワークフロー

> **すでにテストシナリオが存在する場合**: ステップ 2 から開始し、現在の実装をもとにシナリオをレビューする。

- [ ] ステップ 1: テストシナリオの作成 — 詳細は [workflow.md](./references/workflow.md#ステップ-1-テストシナリオの作成) を参照
- [ ] ステップ 2: テストシナリオのレビュー（ブラックボックス観点、最大 2 回）— 詳細は [workflow.md](./references/workflow.md#ステップ-2-テストシナリオのレビュー) を参照
- [ ] ステップ 3: テストの実装 — 詳細は [workflow.md](./references/workflow.md#ステップ-3-テストの実装) を参照
  - JUnit テンプレート: [junit-test.java](./templates/junit-test.java)
- [ ] ステップ 4: テストの実施（問題発生時は報告のみ）— 詳細は [workflow.md](./references/workflow.md#ステップ-4-テストの実施) を参照
- [ ] ステップ 5: テスト結果レポートの作成 — 詳細は [workflow.md](./references/workflow.md#ステップ-5-テスト結果レポートの作成) を参照
  - 出力先: `temp/テストレポート_{YYYYMMDD_HHmmss}.md`

## トラブルシューティング

| 問題 | 解決策 |
|---|---|
| `mvn test` で `ClassNotFoundException: org.mockito.*` | Mockito が未設定。`pom.xml` に `<dependency><groupId>org.mockito</groupId><artifactId>mockito-core</artifactId><version>2.28.2</version><scope>test</scope></dependency>` を追加する（Java 7 互換の最終版） |
| `mvn test` で `UnsupportedClassVersionError` | Mockito のバージョンが Java 7 に非対応。Mockito 2.x 系を使用すること（3.x 以降は Java 8 必須） |
| SQLite の `DatabaseUtil` 初期化エラー | テスト用の `@Before` で `DatabaseUtil.initialize("target/test-data/test.db")` を呼び出す |
| Struts2 の `ServletActionContext` が null | Action のテストではサービス層のモックに集中し、`ServletActionContext` への依存を避ける。API Action のテストでは `HttpServletRequest`/`HttpServletResponse` をモックする |
| Spring DI が効かない | テストでは手動でモックを setter インジェクションする。Spring コンテキストの起動は不要 |
| `BCrypt` 関連のテストが遅い | PasswordUtil テストではハッシュ生成の回数を最小限にする |

## 参考資料

- [ワークフロー詳細](./references/workflow.md)
- [JUnit テストテンプレート](./templates/junit-test.java)
