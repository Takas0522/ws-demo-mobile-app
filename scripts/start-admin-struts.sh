#!/bin/bash
# Struts2 管理画面アプリケーションの起動スクリプト

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
ADMIN_STRUTS_DIR="${PROJECT_ROOT}/src/admin-struts"
CATALINA_HOME="${CATALINA_HOME:-/opt/tomcat7}"
JAVA7_HOME="${JAVA7_HOME:-/usr/lib/jvm/zulu7}"

echo "========================================="
echo " Struts2 Admin Application - Start"
echo "========================================="

# JDK 7 の確認
if [ ! -d "$JAVA7_HOME" ]; then
    echo "WARNING: JDK 7 not found at $JAVA7_HOME"
    echo "Trying alternative paths..."
    for path in /usr/lib/jvm/java-7-openjdk-amd64 /usr/lib/jvm/zulu-7-amd64; do
        if [ -d "$path" ]; then
            JAVA7_HOME="$path"
            echo "Found JDK 7 at $JAVA7_HOME"
            break
        fi
    done
fi

# Tomcat の確認
if [ ! -d "$CATALINA_HOME" ]; then
    echo "ERROR: Tomcat 7 not found at $CATALINA_HOME"
    exit 1
fi

# WAR ビルド
echo ""
echo "[1/4] Building WAR..."
cd "$ADMIN_STRUTS_DIR"
mvn clean package -q -DskipTests
echo "  WAR built: target/admin-struts.war"

# 既存の WAR を削除してデプロイ
echo ""
echo "[2/4] Deploying WAR to Tomcat..."
rm -rf "${CATALINA_HOME}/webapps/admin-struts"
rm -f "${CATALINA_HOME}/webapps/admin-struts.war"
cp target/admin-struts.war "${CATALINA_HOME}/webapps/"
echo "  Deployed to ${CATALINA_HOME}/webapps/admin-struts.war"

# Tomcat の server.xml でポート 8082 を設定（初回のみ）
CONNECTOR_PORT=$(grep 'Connector port=' "${CATALINA_HOME}/conf/server.xml" | head -1 | grep -o 'port="[0-9]*"' | grep -o '[0-9]*')
if [ "$CONNECTOR_PORT" != "8082" ]; then
    echo ""
    echo "[2.5/4] Configuring Tomcat port to 8082..."
    sed -i 's/Connector port="[0-9]*"/Connector port="8082"/' "${CATALINA_HOME}/conf/server.xml"
    # Shutdown port も変更（8005 → 8006 で衝突回避）
    sed -i 's/Server port="[0-9]*" shutdown/Server port="8006" shutdown/' "${CATALINA_HOME}/conf/server.xml"
    echo "  Tomcat configured to port 8082 (shutdown: 8006)"
fi

# setenv.sh の作成（JDK 7 指定）
echo ""
echo "[3/4] Configuring JDK 7 for Tomcat..."
cat > "${CATALINA_HOME}/bin/setenv.sh" << 'SETENV'
#!/bin/bash
export JAVA_HOME="${JAVA7_HOME:-/usr/lib/jvm/zulu7}"
export CATALINA_PID="${CATALINA_HOME}/temp/catalina.pid"
export JAVA_OPTS="-Xms256m -Xmx512m"
SETENV
chmod +x "${CATALINA_HOME}/bin/setenv.sh"
echo "  setenv.sh created"

# Tomcat 起動
echo ""
echo "[4/4] Starting Tomcat 7..."
export JAVA_HOME="$JAVA7_HOME"
"${CATALINA_HOME}/bin/startup.sh"

echo ""
echo "========================================="
echo " Struts2 Admin Application Started"
echo "========================================="
echo "  URL: http://localhost:8082/admin-struts/admin/login"
echo "  Logs: ${CATALINA_HOME}/logs/catalina.out"
echo "  Login: admin001 / admin123"
echo "========================================="
