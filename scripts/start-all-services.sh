#!/bin/bash
# start-all-services.sh
# すべてのサービスを起動するスクリプト

set -e

# ディレクトリ確認
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "========================================="
echo "  Starting All Services"
echo "========================================="
echo ""

# ログとPIDディレクトリを作成
mkdir -p "$PROJECT_ROOT/logs"
mkdir -p "$PROJECT_ROOT/pids"

# PostgreSQLを起動
echo "[1/5] Starting PostgreSQL..."
cd "$PROJECT_ROOT"
docker-compose up -d
echo "✓ PostgreSQL started"
echo ""

# PostgreSQLの起動を待機
echo "Waiting for PostgreSQL to be ready..."
sleep 5

# データベースの接続確認
until docker exec postgres-demo pg_isready -U demouser > /dev/null 2>&1; do
  echo "  Waiting for PostgreSQL..."
  sleep 2
done
echo "✓ PostgreSQL is ready"
echo ""

# Web APIを起動
echo "[2/5] Starting Web API (port 8080)..."
cd "$PROJECT_ROOT/web-api"
if [ -f "gradlew" ]; then
  nohup ./gradlew bootRun > "$PROJECT_ROOT/logs/web-api.log" 2>&1 &
  echo $! > "$PROJECT_ROOT/pids/web-api.pid"
  echo "✓ Web API started (PID: $(cat $PROJECT_ROOT/pids/web-api.pid))"
else
  echo "✗ gradlew not found in web-api/"
  exit 1
fi
echo ""

# Web APIの起動を待機
echo "Waiting for Web API to be ready..."
sleep 15

# Mobile BFFを起動
echo "[3/5] Starting Mobile BFF (port 8081)..."
cd "$PROJECT_ROOT/mobile-bff"
if [ -f "gradlew" ]; then
  nohup ./gradlew bootRun > "$PROJECT_ROOT/logs/mobile-bff.log" 2>&1 &
  echo $! > "$PROJECT_ROOT/pids/mobile-bff.pid"
  echo "✓ Mobile BFF started (PID: $(cat $PROJECT_ROOT/pids/mobile-bff.pid))"
else
  echo "✗ gradlew not found in mobile-bff/"
  exit 1
fi
echo ""

# Admin BFFを起動
echo "[4/5] Starting Admin BFF (port 8082)..."
cd "$PROJECT_ROOT/admin-bff"
if [ -f "gradlew" ]; then
  nohup ./gradlew bootRun > "$PROJECT_ROOT/logs/admin-bff.log" 2>&1 &
  echo $! > "$PROJECT_ROOT/pids/admin-bff.pid"
  echo "✓ Admin BFF started (PID: $(cat $PROJECT_ROOT/pids/admin-bff.pid))"
else
  echo "✗ gradlew not found in admin-bff/"
  exit 1
fi
echo ""

# Admin Webを起動
echo "[5/5] Starting Admin Web (port 3000)..."
cd "$PROJECT_ROOT/admin-web"
if [ -f "package.json" ]; then
  nohup npm run dev > "$PROJECT_ROOT/logs/admin-web.log" 2>&1 &
  echo $! > "$PROJECT_ROOT/pids/admin-web.pid"
  echo "✓ Admin Web started (PID: $(cat $PROJECT_ROOT/pids/admin-web.pid))"
else
  echo "✗ package.json not found in admin-web/"
  exit 1
fi
echo ""

echo "========================================="
echo "  All Services Started!"
echo "========================================="
echo ""
echo "Services:"
echo "  - PostgreSQL:  localhost:5432"
echo "  - Web API:     http://localhost:8080"
echo "  - Mobile BFF:  http://localhost:8081"
echo "  - Admin BFF:   http://localhost:8082"
echo "  - Admin Web:   http://localhost:3000"
echo ""
echo "Logs:"
echo "  - ./logs/web-api.log"
echo "  - ./logs/mobile-bff.log"
echo "  - ./logs/admin-bff.log"
echo "  - ./logs/admin-web.log"
echo ""
echo "PIDs:"
echo "  - ./pids/*.pid"
echo ""
echo "To stop all services, run:"
echo "  ./scripts/stop-all-services.sh"
echo ""
