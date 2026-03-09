#!/bin/bash
# Struts2 管理画面アプリケーションの停止スクリプト

CATALINA_HOME="${CATALINA_HOME:-/opt/tomcat7}"
JAVA7_HOME="${JAVA7_HOME:-/usr/lib/jvm/zulu7}"

echo "Stopping Struts2 Admin Application (Tomcat 7)..."

export JAVA_HOME="$JAVA7_HOME"

if [ -f "${CATALINA_HOME}/bin/shutdown.sh" ]; then
    "${CATALINA_HOME}/bin/shutdown.sh" 2>/dev/null || true
    sleep 3
    # PID ファイルで確認
    if [ -f "${CATALINA_HOME}/temp/catalina.pid" ]; then
        PID=$(cat "${CATALINA_HOME}/temp/catalina.pid")
        if kill -0 "$PID" 2>/dev/null; then
            echo "Force killing Tomcat (PID: $PID)..."
            kill -9 "$PID" 2>/dev/null || true
        fi
        rm -f "${CATALINA_HOME}/temp/catalina.pid"
    fi
    echo "Tomcat 7 stopped."
else
    echo "Tomcat not found at $CATALINA_HOME"
fi
