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

# SQLiteデータベースの初期化
DB_DIR="$PROJECT_ROOT/src/web-api/data"
DB_FILE="$DB_DIR/mobile_app.db"
echo "[1/4] Checking SQLite database..."
mkdir -p "$DB_DIR"
if [ ! -f "$DB_FILE" ]; then
  echo "  Database not found. Initializing..."
  sqlite3 "$DB_FILE" < "$PROJECT_ROOT/src/database/schema/01_create_tables.sql"
  sqlite3 "$DB_FILE" < "$PROJECT_ROOT/src/database/seeds/02_seed_data.sql"
  echo "✓ SQLite database initialized"
else
  echo "✓ SQLite database exists"
fi
echo ""

# Web APIを起動
echo "[2/4] Starting Web API (port 8080)..."
cd "$PROJECT_ROOT/src/web-api"
JAR_FILE="target/web-api-1.0.0-SNAPSHOT.jar"
if [ ! -f "$JAR_FILE" ]; then
  echo "  JAR not found. Building..."
  mvn clean package -q -DskipTests
fi
nohup java -jar "$JAR_FILE" > "$PROJECT_ROOT/logs/web-api.log" 2>&1 &
echo $! > "$PROJECT_ROOT/pids/web-api.pid"
echo "✓ Web API started (PID: $(cat $PROJECT_ROOT/pids/web-api.pid))"
echo ""

# Web APIの起動を待機
echo "Waiting for Web API to be ready..."
sleep 15

# Mobile BFFを起動
echo "[3/4] Starting Mobile BFF (port 8081)..."
cd "$PROJECT_ROOT/src/mobile-bff"
JAR_FILE="target/mobile-bff-1.0.0-SNAPSHOT.jar"
if [ ! -f "$JAR_FILE" ]; then
  echo "  JAR not found. Building..."
  mvn clean package -q -DskipTests
fi
nohup java -jar "$JAR_FILE" > "$PROJECT_ROOT/logs/mobile-bff.log" 2>&1 &
echo $! > "$PROJECT_ROOT/pids/mobile-bff.pid"
echo "✓ Mobile BFF started (PID: $(cat $PROJECT_ROOT/pids/mobile-bff.pid))"
echo ""

# [MIGRATED TO STRUTS2] Admin BFFを起動（旧構成 - Struts2 移行により無効化）
# echo "[4/5] Starting Admin BFF (port 8082)..."
# cd "$PROJECT_ROOT/src/admin-bff"
# JAR_FILE="target/admin-bff-1.0.0-SNAPSHOT.jar"
# if [ ! -f "$JAR_FILE" ]; then
#   echo "  JAR not found. Building..."
#   mvn clean package -q -DskipTests
# fi
# nohup java -jar "$JAR_FILE" > "$PROJECT_ROOT/logs/admin-bff.log" 2>&1 &
# echo $! > "$PROJECT_ROOT/pids/admin-bff.pid"
# echo "✓ Admin BFF started (PID: $(cat $PROJECT_ROOT/pids/admin-bff.pid))"
# echo ""

# [MIGRATED TO STRUTS2] Admin Webを起動（旧構成 - Struts2 移行により無効化）
# echo "[5/5] Starting Admin Web (port 5173)..."
# cd "$PROJECT_ROOT/src/admin-web"
# if [ ! -d "node_modules" ]; then
#   echo "  Installing dependencies..."
#   npm install --silent
# fi
# nohup npm run dev > "$PROJECT_ROOT/logs/admin-web.log" 2>&1 &
# echo $! > "$PROJECT_ROOT/pids/admin-web.pid"
# echo "✓ Admin Web started (PID: $(cat $PROJECT_ROOT/pids/admin-web.pid))"
# echo ""

# Struts2 管理画面アプリを起動（Admin BFF + Admin Web の後継）
echo "[4/4] Starting Struts2 Admin Application (Tomcat 7, port 8082)..."
"$SCRIPT_DIR/start-admin-struts.sh"
echo "✓ Struts2 Admin Application started"
echo ""

echo "========================================="
echo "  All Services Started!"
echo "========================================="
echo ""
echo "Services:"
echo "  - SQLite DB:   $DB_FILE"
echo "  - Web API:     http://localhost:8080"
echo "  - Mobile BFF:  http://localhost:8081"
echo "  - Struts2 Admin: http://localhost:8082/admin-struts/admin/login"
echo ""
echo "Logs:"
echo "  - ./logs/web-api.log"
echo "  - ./logs/mobile-bff.log"
echo "  - /opt/tomcat7/logs/catalina.out (Struts2 Admin)"
echo ""
echo "PIDs:"
echo "  - ./pids/*.pid"
echo ""
echo "To stop all services, run:"
echo "  ./scripts/stop-all-services.sh"
echo ""
