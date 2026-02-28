#!/bin/bash
# reset-database.sh
# データベースをリセットして初期データを再投入するスクリプト

set -e

# ディレクトリ確認
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "========================================="
echo "  Resetting Database (SQLite)"
echo "========================================="
echo ""

# SQLiteデータベースファイルのパス
DB_DIR="$PROJECT_ROOT/src/web-api/data"
DB_FILE="$DB_DIR/mobile_app.db"

# dataディレクトリを作成
mkdir -p "$DB_DIR"

# 既存のデータベースファイルを削除
if [ -f "$DB_FILE" ]; then
  echo "Removing existing database..."
  rm "$DB_FILE"
  echo "✓ Old database removed"
  echo ""
fi

# sqlite3コマンドの確認
if ! command -v sqlite3 &> /dev/null; then
  echo "✗ sqlite3 command not found!"
  echo "  Please install SQLite3:"
  echo "  - macOS: brew install sqlite3"
  echo "  - Ubuntu/Debian: sudo apt-get install sqlite3"
  echo "  - Windows: download from https://www.sqlite.org/download.html"
  exit 1
fi

# スキーマ作成
echo "Creating schema..."
if [ -f "$PROJECT_ROOT/src/database/schema/01_create_tables.sql" ]; then
  sqlite3 "$DB_FILE" < "$PROJECT_ROOT/src/database/schema/01_create_tables.sql"
  echo "✓ Schema created"
else
  echo "✗ 01_create_tables.sql not found!"
  exit 1
fi
echo ""

# 初期データを投入
echo "Inserting seed data..."
if [ -f "$PROJECT_ROOT/src/database/seeds/02_seed_data.sql" ]; then
  sqlite3 "$DB_FILE" < "$PROJECT_ROOT/src/database/seeds/02_seed_data.sql"
  echo "✓ Seed data inserted"
else
  echo "✗ 02_seed_data.sql not found!"
  exit 1
fi
echo ""

# データ確認
echo "Verifying data..."
sqlite3 "$DB_FILE" << 'EOF'
.mode column
.headers on
SELECT 'users' as table_name, COUNT(*) as count FROM users
UNION ALL
SELECT 'products', COUNT(*) FROM products
UNION ALL
SELECT 'feature_flags', COUNT(*) FROM feature_flags
UNION ALL
SELECT 'purchases', COUNT(*) FROM purchases
UNION ALL
SELECT 'favorites', COUNT(*) FROM favorites
UNION ALL
SELECT 'user_feature_flags', COUNT(*) FROM user_feature_flags;
EOF
echo ""

echo "========================================="
echo "  Database Reset Complete!"
echo "========================================="
echo ""
echo "Database file: $DB_FILE"
echo ""
