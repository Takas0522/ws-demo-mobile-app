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

# Struts2 管理画面アプリ（Tomcat 7）の停止
echo "Stopping Struts2 Admin Application (Tomcat 7)..."
CATALINA_HOME="${CATALINA_HOME:-/opt/tomcat7}"
if [ -f "${CATALINA_HOME}/bin/shutdown.sh" ]; then
  JAVA7_HOME="${JAVA7_HOME:-/usr/lib/jvm/zulu7}"
  export JAVA_HOME="$JAVA7_HOME"
  "${CATALINA_HOME}/bin/shutdown.sh" 2>/dev/null || true
  sleep 3
  if [ -f "${CATALINA_HOME}/temp/catalina.pid" ]; then
    TOMCAT_PID=$(cat "${CATALINA_HOME}/temp/catalina.pid")
    if kill -0 "$TOMCAT_PID" 2>/dev/null; then
      echo "  Force killing Tomcat (PID: $TOMCAT_PID)..."
      kill -9 "$TOMCAT_PID" 2>/dev/null || true
    fi
    rm -f "${CATALINA_HOME}/temp/catalina.pid"
  fi
  echo "✓ Struts2 Admin Application stopped"
else
  echo "✓ Tomcat 7 not installed (skipped)"
fi
echo ""

# SQLiteはファイルベースのため、停止処理は不要です
echo "SQLite database: no shutdown required (file-based)"
echo ""

# Javaプロセスの残留確認
echo "Checking for remaining Java processes..."
JAVA_PIDS=$(pgrep -f 'java.*-jar.*\(web-api\|mobile-bff\|admin-bff\)' 2>/dev/null || true)
if [ -n "$JAVA_PIDS" ]; then
  echo "  Killing remaining Java processes: $JAVA_PIDS"
  echo "$JAVA_PIDS" | xargs kill 2>/dev/null || true
else
  echo "✓ No remaining Java processes"
fi
echo ""

echo "========================================="
echo "  All Services Stopped!"
echo "========================================="
echo ""
