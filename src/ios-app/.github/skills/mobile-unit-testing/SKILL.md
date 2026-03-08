---
name: mobile-unit-testing
description: iOS アプリのユニットテストを構築するスキル。XCTest を使ったテストシナリオの作成・ブラックボックスレビュー・テスト実装・実行・レポート出力まで一連のプロセスをガイドする。「ユニットテストを作成して」「テストシナリオをレビューして」「XCTest テストを書いて」「テストを実行してレポートを出して」「テストコードを実装して」などの依頼で自動的にロードされる。
argument-hint: [対象コンポーネント名]
license: Complete terms in LICENSE.txt
---

# iOS ユニットテスト構築スキル

iOS（XCTest）を対象に、テストシナリオの作成からレポート出力までの一連のプロセスを提供するスキル。

## このスキルを使う場面

- iOS のユニットテストを新規に作成するとき
- 既存のテストシナリオをブラックボックステストの観点でレビューするとき
- XCTest のテストコードを実装するとき
- テストを実行して結果レポートを出力するとき

## 前提条件

| 項目 | 詳細 |
|---|---|
| ビルドツール | Xcode + xcodebuild |
| テストフレームワーク | XCTest（Xcode 標準） |

**プロジェクトパス**:

| 項目 | パス |
|---|---|
| ソースパス | `MobileApp/MobileApp/` |
| テスト配置先 | `MobileApp/MobileAppTests/` |

## ワークフロー

> **すでにテストシナリオが存在する場合**: ステップ 2 から開始し、現在の実装をもとにシナリオをレビューする。

- [ ] ステップ 1: テストシナリオの作成 — 詳細は [workflow.md](./references/workflow.md#ステップ-1-テストシナリオの作成) を参照
- [ ] ステップ 2: テストシナリオのレビュー（ブラックボックス観点、最大 2 回）— 詳細は [workflow.md](./references/workflow.md#ステップ-2-テストシナリオのレビュー) を参照
- [ ] ステップ 3: テストの実装 — 詳細は [workflow.md](./references/workflow.md#ステップ-3-テストの実装) を参照
  - XCTest テンプレート: [xctest-test.swift](./templates/xctest-test.swift)
- [ ] ステップ 4: テストの実施（問題発生時は報告のみ）— 詳細は [workflow.md](./references/workflow.md#ステップ-4-テストの実施) を参照
- [ ] ステップ 5: テスト結果レポートの作成 — 詳細は [workflow.md](./references/workflow.md#ステップ-5-テスト結果レポートの作成) を参照
  - 出力先: `temp/テストレポート_{YYYYMMDD_HHmmss}.md`

## トラブルシューティング

| 問題 | 解決策 |
|---|---|
| `xcodebuild test` でシミュレーターが見つからない | `xcrun simctl list devices` でデバイス名を確認し `-destination` 引数を更新する |
| XCTest で `@MainActor` 警告 | テストクラスに `@MainActor` を付与するか `await MainActor.run { ... }` でラップする |

## 参考資料

- [ワークフロー詳細](./references/workflow.md)
- [XCTest テストテンプレート](./templates/xctest-test.swift)
