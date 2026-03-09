#!/bin/bash
# Struts2 管理画面アプリケーションの停止スクリプト

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
PID_FILE="${PROJECT_ROOT}/pids/admin-struts.pid"

echo "Stopping Struts2 Admin Application..."

if [ -f "$PID_FILE" ]; then
    PID=$(cat "$PID_FILE")
    if kill -0 "$PID" 2>/dev/null; then
        kill "$PID"
        echo "  Sent SIGTERM to PID $PID"
        sleep 3
        if kill -0 "$PID" 2>/dev/null; then
            kill -9 "$PID" 2>/dev/null || true
            echo "  Force killed PID $PID"
        fi
    else
        echo "  Process $PID is not running"
    fi
    rm -f "$PID_FILE"
    echo "  Stopped."
else
    echo "  PID file not found. Application may not be running."
fi
