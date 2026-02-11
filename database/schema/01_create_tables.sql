-- ======================================
-- mobile-app-system データベーススキーマ
-- ======================================
-- 作成日: 2025-01-08
-- バージョン: 1.0
-- 参照: docs/specs/mobile-app-system/04-data-model.md
-- ======================================

-- UUID拡張を有効化
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- ======================================
-- 1. USERS テーブル
-- ======================================
CREATE TABLE users (
    user_id         BIGSERIAL PRIMARY KEY,
    user_name       VARCHAR(100) NOT NULL,
    login_id        VARCHAR(50) NOT NULL UNIQUE,
    password_hash   VARCHAR(255) NOT NULL,
    user_type       VARCHAR(20) NOT NULL DEFAULT 'user',
    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    -- 制約
    CONSTRAINT chk_user_type CHECK (user_type IN ('user', 'admin')),
    CONSTRAINT chk_login_id_length CHECK (LENGTH(login_id) >= 4 AND LENGTH(login_id) <= 20),
    CONSTRAINT chk_user_name_length CHECK (LENGTH(user_name) >= 1 AND LENGTH(user_name) <= 100)
);

-- インデックス
CREATE INDEX idx_users_user_type ON users(user_type);

-- コメント
COMMENT ON TABLE users IS 'ユーザーテーブル（エンドユーザーと管理者）';
COMMENT ON COLUMN users.user_id IS 'ユーザーID（主キー）';
COMMENT ON COLUMN users.user_name IS 'ユーザー表示名';
COMMENT ON COLUMN users.login_id IS 'ログインID（一意）';
COMMENT ON COLUMN users.password_hash IS 'パスワードハッシュ（bcrypt）';
COMMENT ON COLUMN users.user_type IS 'ユーザー種別（user/admin）';

-- ======================================
-- 2. PRODUCTS テーブル
-- ======================================
CREATE TABLE products (
    product_id      BIGSERIAL PRIMARY KEY,
    product_name    VARCHAR(100) NOT NULL,
    unit_price      INTEGER NOT NULL,
    description     TEXT,
    image_url       VARCHAR(500),
    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    -- 制約
    CONSTRAINT chk_unit_price CHECK (unit_price >= 1),
    CONSTRAINT chk_product_name_length CHECK (LENGTH(product_name) >= 1 AND LENGTH(product_name) <= 100)
);

-- インデックス
CREATE INDEX idx_products_product_name ON products(product_name);

-- コメント
COMMENT ON TABLE products IS '商品テーブル';
COMMENT ON COLUMN products.product_id IS '商品ID（主キー）';
COMMENT ON COLUMN products.product_name IS '商品名';
COMMENT ON COLUMN products.unit_price IS '単価（円）';
COMMENT ON COLUMN products.description IS '商品説明';
COMMENT ON COLUMN products.image_url IS '商品画像URL';

-- ======================================
-- 3. PURCHASES テーブル
-- ======================================
CREATE TABLE purchases (
    purchase_id             UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id                 BIGINT NOT NULL,
    product_id              BIGINT NOT NULL,
    quantity                INTEGER NOT NULL,
    unit_price_at_purchase  INTEGER NOT NULL,
    total_amount            INTEGER NOT NULL,
    purchased_at            TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
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

-- コメント
COMMENT ON TABLE purchases IS '購入履歴テーブル';
COMMENT ON COLUMN purchases.purchase_id IS '購入ID（主キー、UUID）';
COMMENT ON COLUMN purchases.user_id IS 'ユーザーID（外部キー）';
COMMENT ON COLUMN purchases.product_id IS '商品ID（外部キー）';
COMMENT ON COLUMN purchases.quantity IS '購入個数（100の倍数）';
COMMENT ON COLUMN purchases.unit_price_at_purchase IS '購入時単価';
COMMENT ON COLUMN purchases.total_amount IS '合計金額';

-- ======================================
-- 4. FAVORITES テーブル
-- ======================================
CREATE TABLE favorites (
    favorite_id     BIGSERIAL PRIMARY KEY,
    user_id         BIGINT NOT NULL,
    product_id      BIGINT NOT NULL,
    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    -- 外部キー
    CONSTRAINT fk_favorites_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    CONSTRAINT fk_favorites_product FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,
    
    -- 制約
    CONSTRAINT uk_favorites_user_product UNIQUE (user_id, product_id)
);

-- インデックス
CREATE INDEX idx_favorites_user_id ON favorites(user_id);
CREATE INDEX idx_favorites_product_id ON favorites(product_id);

-- コメント
COMMENT ON TABLE favorites IS 'お気に入りテーブル';
COMMENT ON COLUMN favorites.favorite_id IS 'お気に入りID（主キー）';
COMMENT ON COLUMN favorites.user_id IS 'ユーザーID（外部キー）';
COMMENT ON COLUMN favorites.product_id IS '商品ID（外部キー）';

-- ======================================
-- 5. FEATURE_FLAGS テーブル
-- ======================================
CREATE TABLE feature_flags (
    flag_id         BIGSERIAL PRIMARY KEY,
    flag_key        VARCHAR(50) NOT NULL UNIQUE,
    flag_name       VARCHAR(100) NOT NULL,
    default_value   BOOLEAN NOT NULL DEFAULT false,
    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- コメント
COMMENT ON TABLE feature_flags IS '機能フラグマスタテーブル';
COMMENT ON COLUMN feature_flags.flag_id IS 'フラグID（主キー）';
COMMENT ON COLUMN feature_flags.flag_key IS 'フラグキー（一意）';
COMMENT ON COLUMN feature_flags.flag_name IS 'フラグ表示名';
COMMENT ON COLUMN feature_flags.default_value IS 'デフォルト値';

-- ======================================
-- 6. USER_FEATURE_FLAGS テーブル
-- ======================================
CREATE TABLE user_feature_flags (
    user_flag_id    BIGSERIAL PRIMARY KEY,
    user_id         BIGINT NOT NULL,
    flag_id         BIGINT NOT NULL,
    is_enabled      BOOLEAN NOT NULL DEFAULT false,
    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    -- 外部キー
    CONSTRAINT fk_user_feature_flags_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    CONSTRAINT fk_user_feature_flags_flag FOREIGN KEY (flag_id) REFERENCES feature_flags(flag_id) ON DELETE CASCADE,
    
    -- 制約
    CONSTRAINT uk_user_feature_flags_user_flag UNIQUE (user_id, flag_id)
);

-- インデックス
CREATE INDEX idx_user_feature_flags_user_id ON user_feature_flags(user_id);
CREATE INDEX idx_user_feature_flags_flag_id ON user_feature_flags(flag_id);

-- コメント
COMMENT ON TABLE user_feature_flags IS 'ユーザー別機能フラグテーブル';
COMMENT ON COLUMN user_feature_flags.user_flag_id IS '設定ID（主キー）';
COMMENT ON COLUMN user_feature_flags.user_id IS 'ユーザーID（外部キー）';
COMMENT ON COLUMN user_feature_flags.flag_id IS 'フラグID（外部キー）';
COMMENT ON COLUMN user_feature_flags.is_enabled IS '有効/無効';

-- ======================================
-- トリガー: updated_at 自動更新
-- ======================================
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- users テーブル
CREATE TRIGGER update_users_updated_at BEFORE UPDATE ON users
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- products テーブル
CREATE TRIGGER update_products_updated_at BEFORE UPDATE ON products
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- user_feature_flags テーブル
CREATE TRIGGER update_user_feature_flags_updated_at BEFORE UPDATE ON user_feature_flags
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- ======================================
-- スキーマ作成完了
-- ======================================
