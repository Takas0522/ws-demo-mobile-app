#!/bin/bash
# Struts2 管理画面アプリケーションの起動スクリプト
# Maven Tomcat プラグインで組込み Tomcat を使用（外部 Tomcat 不要）

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
ADMIN_STRUTS_DIR="${PROJECT_ROOT}/src/admin-struts"
LOG_DIR="${PROJECT_ROOT}/logs"
PID_DIR="${PROJECT_ROOT}/pids"

mkdir -p "$LOG_DIR" "$PID_DIR"

echo "========================================="
echo " Struts2 Admin Application - Start"
echo "========================================="

# WAR ビルド
echo ""
echo "[1/2] Building WAR..."
cd "$ADMIN_STRUTS_DIR"
mvn clean package -q -DskipTests
echo "  Build complete"

# Maven Tomcat プラグインで起動
echo ""
echo "[2/2] Starting embedded Tomcat on port 8082..."
nohup mvn tomcat7:run -DskipTests > "${LOG_DIR}/admin-struts.log" 2>&1 &
STRUTS_PID=$!
echo "$STRUTS_PID" > "${PID_DIR}/admin-struts.pid"

# 起動待ち
echo "  Waiting for startup..."
for i in $(seq 1 30); do
    if curl -s -o /dev/null -w '' http://localhost:8082/admin-struts/admin/login 2>/dev/null; then
        echo ""
        echo "========================================="
        echo " Struts2 Admin Application Started"
        echo "========================================="
        echo "  URL:   http://localhost:8082/admin-struts/admin/login"
        echo "  Logs:  ${LOG_DIR}/admin-struts.log"
        echo "  PID:   $STRUTS_PID"
        echo "  Login: admin001 / admin123"
        echo "========================================="
        exit 0
    fi
    sleep 2
done

echo "  Application may still be starting. Check logs: ${LOG_DIR}/admin-struts.log"
echo "  PID: $STRUTS_PID"
