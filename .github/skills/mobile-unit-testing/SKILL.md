---
name: mobile-unit-testing
description: モバイルアプリ（Android / iOS）のユニットテストを構築するスキル。JUnit（Android）と XCTest（iOS）を使ったテストシナリオの作成・ブラックボックスレビュー・テスト実装・実行・レポート出力まで一連のプロセスをガイドする。「ユニットテストを作成して」「テストシナリオをレビューして」「JUnit テストを書いて」「XCTest テストを実行してレポートを出して」「テストコードを実装して」などの依頼で自動的にロードされる。
argument-hint: [android|ios|both] [対象コンポーネント名]
license: Complete terms in LICENSE.txt
---

# モバイルアプリ ユニットテスト構築スキル

Android（JUnit 4）と iOS（XCTest）を対象に、テストシナリオの作成からレポート出力までの一連のプロセスを提供するスキル。

## このスキルを使う場面

- Android または iOS のユニットテストを新規に作成するとき
- 既存のテストシナリオをブラックボックステストの観点でレビューするとき
- JUnit または XCTest のテストコードを実装するとき
- テストを実行して結果レポートを出力するとき

## 前提条件

| 項目 | 詳細 |
|---|---|
| Android ビルドツール | Gradle 8.2.2 / Java 17 |
| Android テストフレームワーク | JUnit 4.13.2（`build.gradle` 設定済み） |
| iOS ビルドツール | Xcode + xcodebuild |
| iOS テストフレームワーク | XCTest（Xcode 標準） |
| モックライブラリ（Android） | Mockito（未設定の場合は `build.gradle` に追記が必要） |

**プロジェクトパス**:

| プラットフォーム | ソースパス | テスト配置先 |
|---|---|---|
| Android | `src/android-app/app/src/main/java/com/example/mobileapp/` | `src/android-app/app/src/test/java/com/example/mobileapp/` |
| iOS | `src/ios-app/MobileApp/MobileApp/` | `src/ios-app/MobileApp/MobileAppTests/` |

## ワークフロー

> **すでにテストシナリオが存在する場合**: ステップ 2 から開始し、現在の実装をもとにシナリオをレビューする。

- [ ] ステップ 1: テストシナリオの作成 — 詳細は [workflow.md](./references/workflow.md#ステップ-1-テストシナリオの作成) を参照
- [ ] ステップ 2: テストシナリオのレビュー（ブラックボックス観点、最大 2 回）— 詳細は [workflow.md](./references/workflow.md#ステップ-2-テストシナリオのレビュー) を参照
- [ ] ステップ 3: テストの実装 — 詳細は [workflow.md](./references/workflow.md#ステップ-3-テストの実装) を参照
  - JUnit テンプレート: [junit-test.java](./templates/junit-test.java)
  - XCTest テンプレート: [xctest-test.swift](./templates/xctest-test.swift)
- [ ] ステップ 4: テストの実施（問題発生時は報告のみ）— 詳細は [workflow.md](./references/workflow.md#ステップ-4-テストの実施) を参照
- [ ] ステップ 5: テスト結果レポートの作成 — 詳細は [workflow.md](./references/workflow.md#ステップ-5-テスト結果レポートの作成) を参照
  - 出力先: `temp/テストレポート_{YYYYMMDD_HHmmss}.md`

## トラブルシューティング

| 問題 | 解決策 |
|---|---|
| `./gradlew test` で `ClassNotFound` | Mockito が未設定。`build.gradle` に `testImplementation 'org.mockito:mockito-core:5.x.x'` を追加する |
| Android SDK クラス（Context 等）のエラー | Robolectric を追加: `testImplementation 'org.robolectric:robolectric:4.x.x'` |
| `xcodebuild test` でシミュレーターが見つからない | `xcrun simctl list devices` でデバイス名を確認し `-destination` 引数を更新する |
| XCTest で `@MainActor` 警告 | テストクラスに `@MainActor` を付与するか `await MainActor.run { ... }` でラップする |
| Mockito と Android SDK の競合 | `testImplementation 'org.mockito:mockito-android:5.x.x'` を使用する |

## 参考資料

- [ワークフロー詳細](./references/workflow.md)
- [JUnit テストテンプレート](./templates/junit-test.java)
- [XCTest テストテンプレート](./templates/xctest-test.swift)
