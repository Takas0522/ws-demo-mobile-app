-- ======================================
-- mobile-app-system データベーススキーマ (SQLite)
-- ======================================
-- 作成日: 2025-01-08
-- バージョン: 1.1
-- 参照: docs/specs/mobile-app-system/04-data-model.md
-- ======================================

-- 外部キー制約を有効化
PRAGMA foreign_keys = ON;

-- ======================================
-- 1. USERS テーブル
-- ======================================
CREATE TABLE users (
    user_id         INTEGER PRIMARY KEY AUTOINCREMENT,
    user_name       TEXT NOT NULL,
    login_id        TEXT NOT NULL UNIQUE,
    password_hash   TEXT NOT NULL,
    user_type       TEXT NOT NULL DEFAULT 'user',
    created_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 制約
    CONSTRAINT chk_user_type CHECK (user_type IN ('user', 'admin')),
    CONSTRAINT chk_login_id_length CHECK (LENGTH(login_id) >= 4 AND LENGTH(login_id) <= 20),
    CONSTRAINT chk_user_name_length CHECK (LENGTH(user_name) >= 1 AND LENGTH(user_name) <= 100)
);

-- インデックス
CREATE INDEX idx_users_user_type ON users(user_type);

-- ======================================
-- 2. PRODUCTS テーブル
-- ======================================
CREATE TABLE products (
    product_id      INTEGER PRIMARY KEY AUTOINCREMENT,
    product_name    TEXT NOT NULL,
    unit_price      INTEGER NOT NULL,
    description     TEXT,
    image_url       TEXT,
    created_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 制約
    CONSTRAINT chk_unit_price CHECK (unit_price >= 1),
    CONSTRAINT chk_product_name_length CHECK (LENGTH(product_name) >= 1 AND LENGTH(product_name) <= 100)
);

-- インデックス
CREATE INDEX idx_products_product_name ON products(product_name);

-- ======================================
-- 3. PURCHASES テーブル
-- ======================================
-- purchase_id はアプリケーション側で UUID を生成して TEXT として格納
CREATE TABLE purchases (
    purchase_id             TEXT PRIMARY KEY,
    user_id                 INTEGER NOT NULL,
    product_id              INTEGER NOT NULL,
    quantity                INTEGER NOT NULL,
    unit_price_at_purchase  INTEGER NOT NULL,
    total_amount            INTEGER NOT NULL,
    purchased_at            TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 外部キー
    CONSTRAINT fk_purchases_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE RESTRICT,
    CONSTRAINT fk_purchases_product FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE RESTRICT,

    -- 制約
    CONSTRAINT chk_quantity CHECK (quantity > 0),
    CONSTRAINT chk_quantity_multiple CHECK (quantity % 100 = 0),
    CONSTRAINT chk_unit_price_at_purchase CHECK (unit_price_at_purchase >= 1),
    CONSTRAINT chk_total_amount CHECK (total_amount >= 1),
    CONSTRAINT chk_total_amount_calc CHECK (total_amount = unit_price_at_purchase * quantity)
);

-- インデックス
CREATE INDEX idx_purchases_user_id ON purchases(user_id);
CREATE INDEX idx_purchases_product_id ON purchases(product_id);
CREATE INDEX idx_purchases_purchased_at ON purchases(purchased_at);

-- ======================================
-- 4. FAVORITES テーブル
-- ======================================
CREATE TABLE favorites (
    favorite_id     INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER NOT NULL,
    product_id      INTEGER NOT NULL,
    created_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 外部キー
    CONSTRAINT fk_favorites_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    CONSTRAINT fk_favorites_product FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,

    -- 制約
    CONSTRAINT uk_favorites_user_product UNIQUE (user_id, product_id)
);

-- インデックス
CREATE INDEX idx_favorites_user_id ON favorites(user_id);
CREATE INDEX idx_favorites_product_id ON favorites(product_id);

-- ======================================
-- 5. FEATURE_FLAGS テーブル
-- ======================================
CREATE TABLE feature_flags (
    flag_id         INTEGER PRIMARY KEY AUTOINCREMENT,
    flag_key        TEXT NOT NULL UNIQUE,
    flag_name       TEXT NOT NULL,
    default_value   INTEGER NOT NULL DEFAULT 0 CHECK (default_value IN (0, 1)),
    created_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime'))
);

-- ======================================
-- 6. USER_FEATURE_FLAGS テーブル
-- ======================================
CREATE TABLE user_feature_flags (
    user_flag_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER NOT NULL,
    flag_id         INTEGER NOT NULL,
    is_enabled      INTEGER NOT NULL DEFAULT 0 CHECK (is_enabled IN (0, 1)),
    created_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 外部キー
    CONSTRAINT fk_user_feature_flags_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    CONSTRAINT fk_user_feature_flags_flag FOREIGN KEY (flag_id) REFERENCES feature_flags(flag_id) ON DELETE CASCADE,

    -- 制約
    CONSTRAINT uk_user_feature_flags_user_flag UNIQUE (user_id, flag_id)
);

-- インデックス
CREATE INDEX idx_user_feature_flags_user_id ON user_feature_flags(user_id);
CREATE INDEX idx_user_feature_flags_flag_id ON user_feature_flags(flag_id);

-- ======================================
-- 7. PRODUCT_PRICE_HISTORY テーブル
-- ======================================
CREATE TABLE product_price_history (
    price_history_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id        INTEGER NOT NULL,
    old_price         INTEGER NOT NULL,
    new_price         INTEGER NOT NULL,
    changed_at        TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    changed_by        INTEGER NOT NULL,
    change_reason     TEXT,
    created_at        TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),

    -- 外部キー
    CONSTRAINT fk_price_hist_product FOREIGN KEY (product_id)
        REFERENCES products(product_id) ON DELETE RESTRICT,
    CONSTRAINT fk_price_hist_user FOREIGN KEY (changed_by)
        REFERENCES users(user_id) ON DELETE RESTRICT,

    -- 制約
    CONSTRAINT chk_old_price     CHECK (old_price >= 1),
    CONSTRAINT chk_new_price     CHECK (new_price >= 1),
    CONSTRAINT chk_price_changed CHECK (old_price <> new_price)
);

-- インデックス
CREATE INDEX idx_price_hist_product_id
    ON product_price_history(product_id);
CREATE INDEX idx_price_hist_changed_at
    ON product_price_history(changed_at);
CREATE INDEX idx_price_hist_prod_changed
    ON product_price_history(product_id, changed_at);

-- ======================================
-- トリガー: updated_at 自動更新
-- ======================================

-- users テーブル
CREATE TRIGGER update_users_updated_at AFTER UPDATE ON users
    FOR EACH ROW
    BEGIN
        UPDATE users SET updated_at = datetime('now', 'localtime') WHERE user_id = NEW.user_id;
    END;

-- products テーブル
CREATE TRIGGER update_products_updated_at AFTER UPDATE ON products
    FOR EACH ROW
    BEGIN
        UPDATE products SET updated_at = datetime('now', 'localtime') WHERE product_id = NEW.product_id;
    END;

-- user_feature_flags テーブル
CREATE TRIGGER update_user_feature_flags_updated_at AFTER UPDATE ON user_feature_flags
    FOR EACH ROW
    BEGIN
        UPDATE user_feature_flags SET updated_at = datetime('now', 'localtime') WHERE user_flag_id = NEW.user_flag_id;
    END;

-- 価格変更時に product_price_history へ自動記録
CREATE TRIGGER trg_record_price_history
AFTER UPDATE OF unit_price ON products
FOR EACH ROW
WHEN OLD.unit_price <> NEW.unit_price
BEGIN
    INSERT INTO product_price_history
        (product_id, old_price, new_price, changed_by, change_reason)
    VALUES
        (NEW.product_id, OLD.unit_price, NEW.unit_price,
         0, '商品情報更新による自動記録');
END;

-- ======================================
-- スキーマ作成完了
-- ======================================
