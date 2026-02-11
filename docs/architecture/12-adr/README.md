# Architecture Decision Records (ADR)

このディレクトリには、mobile-app-system のアーキテクチャに関する重要な決定を記録します。

## ADR とは

Architecture Decision Records（ADR）は、ソフトウェアアーキテクチャに関する重要な決定を文書化するための軽量な記録方式です。

## ADR 一覧

| No | タイトル | ステータス | 日付 |
|----|---------|----------|------|
| [ADR-001](./ADR-001-bff-pattern.md) | BFFパターンの採用 | Accepted | 2025-01-08 |
| [ADR-002](./ADR-002-jwt-authentication.md) | JWT認証の採用 | Accepted | 2025-01-08 |

## ADR ステータス

| ステータス | 説明 |
|-----------|------|
| **Proposed** | 提案中 |
| **Accepted** | 承認済み |
| **Deprecated** | 非推奨（新しいADRに置き換え） |
| **Superseded** | 置き換えられた |

## ADR テンプレート

新しいADRを作成する際は、以下のテンプレートを使用してください。

```markdown
# ADR-{番号}: {タイトル}

## ステータス

Proposed | Accepted | Deprecated | Superseded

## コンテキスト

{なぜこの決定が必要になったか}

## 決定

{何を決定したか}

## 理由

{なぜこの決定をしたか}

### メリット

- メリット1
- メリット2

### デメリットと対策

| デメリット | 対策 |
|----------|------|
| デメリット1 | 対策1 |

## 代替案

### 代替案1: {代替案名}

**メリット**: ...
**デメリット**: ...
**却下理由**: ...

## 結果

{この決定によってもたらされる影響}

### 期待される効果

- 効果1
- 効果2

### 実装への影響

- 影響1
- 影響2

## 参照

- 関連ドキュメント
- 外部リンク

---

**承認者**: {承認者名}
**レビュー日**: {日付}
```

## 命名規則

- ファイル名: `ADR-{3桁番号}-{kebab-case-title}.md`
- 例: `ADR-001-bff-pattern.md`

## ADR作成ガイドライン

1. **簡潔に**: 長すぎず、要点を押さえる
2. **コンテキスト重視**: なぜその決定が必要だったかを明確に
3. **代替案を記載**: 検討した他の選択肢も記録
4. **将来の参照**: 後から見ても理解できるように
5. **更新**: 状況が変わったら新しいADRで置き換え

## 参照

- [ADR GitHub](https://adr.github.io/)
- [Architecture Decision Records in Action](https://www.thoughtworks.com/radar/techniques/lightweight-architecture-decision-records)
