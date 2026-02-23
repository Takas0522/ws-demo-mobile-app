#!/bin/bash
# stop-all-services.sh
# すべてのサービスを停止するスクリプト

set -e

# ディレクトリ確認
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "========================================="
echo "  Stopping All Services"
echo "========================================="
echo ""

# PIDファイルから各サービスを停止
if [ -d "$PROJECT_ROOT/pids" ]; then
  for pid_file in "$PROJECT_ROOT/pids"/*.pid; do
    if [ -f "$pid_file" ]; then
      service_name=$(basename "$pid_file" .pid)
      pid=$(cat "$pid_file")

      if ps -p "$pid" > /dev/null 2>&1; then
        echo "Stopping $service_name (PID: $pid)..."
        kill "$pid" 2>/dev/null || true

        # プロセスが終了するまで待機（最大10秒）
        for i in {1..10}; do
          if ! ps -p "$pid" > /dev/null 2>&1; then
            echo "✓ $service_name stopped"
            break
          fi
          sleep 1
        done

        # まだ動いている場合は強制終了
        if ps -p "$pid" > /dev/null 2>&1; then
          echo "  Force killing $service_name..."
          kill -9 "$pid" 2>/dev/null || true
        fi
      else
        echo "✓ $service_name already stopped"
      fi

      rm "$pid_file"
    fi
  done
else
  echo "No PID files found."
fi
echo ""

# Docker Composeを停止
echo "Stopping PostgreSQL (Docker)..."
cd "$PROJECT_ROOT"
docker-compose down
echo "✓ PostgreSQL stopped"
echo ""

# Gradleデーモンを停止（オプション）
echo "Stopping Gradle daemons..."
cd "$PROJECT_ROOT/src/web-api" && ./gradlew --stop 2>/dev/null || true
echo "✓ Gradle daemons stopped"
echo ""

echo "========================================="
echo "  All Services Stopped!"
echo "========================================="
echo ""
