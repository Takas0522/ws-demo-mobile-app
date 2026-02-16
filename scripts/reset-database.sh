#!/bin/bash
# reset-database.sh
# データベースをリセットして初期データを再投入するスクリプト

set -e

# ディレクトリ確認
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "========================================="
echo "  Resetting Database"
echo "========================================="
echo ""

# PostgreSQLの接続情報
DB_HOST="localhost"
DB_PORT="5432"
DB_NAME="demo_db"
DB_USER="demouser"

# データベースが起動しているか確認
echo "Checking PostgreSQL connection..."
if ! docker exec postgres-demo pg_isready -U "$DB_USER" > /dev/null 2>&1; then
  echo "✗ PostgreSQL is not running!"
  echo "  Please start PostgreSQL first:"
  echo "  docker-compose up -d"
  exit 1
fi
echo "✓ PostgreSQL is running"
echo ""

# データのみ削除（TRUNCATE）
echo "Truncating all tables..."
docker exec -i postgres-demo psql -U "$DB_USER" -d "$DB_NAME" << 'EOF'
TRUNCATE TABLE purchases CASCADE;
TRUNCATE TABLE favorites CASCADE;
TRUNCATE TABLE feature_flags CASCADE;
TRUNCATE TABLE products RESTART IDENTITY CASCADE;
TRUNCATE TABLE users RESTART IDENTITY CASCADE;
TRUNCATE TABLE admins RESTART IDENTITY CASCADE;
EOF
echo "✓ All tables truncated"
echo ""

# 初期データを再投入
echo "Inserting seed data..."
if [ -f "$PROJECT_ROOT/database/seed.sql" ]; then
  docker exec -i postgres-demo psql -U "$DB_USER" -d "$DB_NAME" < "$PROJECT_ROOT/database/seed.sql"
  echo "✓ Seed data inserted"
else
  echo "✗ seed.sql not found!"
  exit 1
fi
echo ""

# データ確認
echo "Verifying data..."
docker exec -i postgres-demo psql -U "$DB_USER" -d "$DB_NAME" << 'EOF'
SELECT 'users' as table_name, COUNT(*) as count FROM users
UNION ALL
SELECT 'admins', COUNT(*) FROM admins
UNION ALL
SELECT 'products', COUNT(*) FROM products
UNION ALL
SELECT 'feature_flags', COUNT(*) FROM feature_flags
UNION ALL
SELECT 'purchases', COUNT(*) FROM purchases
UNION ALL
SELECT 'favorites', COUNT(*) FROM favorites;
EOF
echo ""

echo "========================================="
echo "  Database Reset Complete!"
echo "========================================="
echo ""
