#!/usr/bin/env bash
set -euo pipefail

# ==================================================
# Dev Container postCreateCommand
# ==================================================

# --- 1. SQLite データベースの初期化 ---
DB_DIR="/workspace/src/web-api/data"
DB_FILE="${DB_DIR}/mobile_app.db"
SCHEMA="/workspace/src/database/schema/01_create_tables.sql"
SEEDS="/workspace/src/database/seeds/02_seed_data.sql"

mkdir -p "${DB_DIR}"
if [ ! -f "${DB_FILE}" ]; then
    sqlite3 "${DB_FILE}" < "${SCHEMA}"
    sqlite3 "${DB_FILE}" < "${SEEDS}"
    echo "SQLite database initialized"
else
    echo "SQLite database already exists"
fi

# --- 2. Playwright ブラウザのインストール ---
echo "Installing Playwright browsers..."
if [ -f /workspace/src/e2e/package.json ]; then
    (cd /workspace/src/e2e && npm ci && npx playwright install --with-deps chromium && npm install -g @playwright/cli@latest) \
        || echo "WARNING: Playwright install failed, continuing"
else
    echo "No e2e package.json found, skipping Playwright install"
fi

# --- 完了 ---
echo "Dev Container is ready!"
