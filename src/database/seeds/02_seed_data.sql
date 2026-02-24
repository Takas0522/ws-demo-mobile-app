-- ======================================
-- mobile-app-system 初期データ投入
-- ======================================
-- 作成日: 2025-01-08
-- バージョン: 1.0
-- 参照: docs/specs/mobile-app-system/04-data-model.md
-- ======================================

-- ======================================
-- 1. ユーザーデータ
-- ======================================

-- 管理者ユーザー（パスワード: admin123）
-- bcryptハッシュ: $2a$10$9XQy779t3Zn.q2vGeMn6we/cHZw5nDdKd7oD1hsn8Li5TWRKPD3HC
INSERT INTO users (user_name, login_id, password_hash, user_type) VALUES
    ('管理者', 'admin001', '$2a$10$9XQy779t3Zn.q2vGeMn6we/cHZw5nDdKd7oD1hsn8Li5TWRKPD3HC', 'admin');

-- エンドユーザー（パスワード: password123）
-- bcryptハッシュ: $2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW
INSERT INTO users (user_name, login_id, password_hash, user_type) VALUES
    ('山田太郎', 'user001', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('佐藤花子', 'user002', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('鈴木一郎', 'user003', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('田中美咲', 'user004', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('伊藤健太', 'user005', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('渡辺さくら', 'user006', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('山本大輔', 'user007', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('中村愛', 'user008', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('小林拓也', 'user009', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user'),
    ('加藤由美', 'user010', '$2a$10$Q2B8dO3dcrihBd5YUW.EUey01qRaHYhY0ReMrahFVIOMyq6BLtTuW', 'user');

-- ======================================
-- 2. 商品データ
-- ======================================

INSERT INTO products (product_name, unit_price, description, image_url) VALUES
    ('商品A', 1000, '商品Aの詳細説明です。高品質で使いやすい商品です。', 'https://via.placeholder.com/300x300?text=Product+A'),
    ('商品B', 1500, '商品Bの詳細説明です。多機能でコストパフォーマンスに優れています。', 'https://via.placeholder.com/300x300?text=Product+B'),
    ('商品C', 2000, '商品Cの詳細説明です。プレミアム品質の商品です。', 'https://via.placeholder.com/300x300?text=Product+C'),
    ('商品D', 2500, '商品Dの詳細説明です。最新技術を搭載した商品です。', 'https://via.placeholder.com/300x300?text=Product+D'),
    ('商品E', 3000, '商品Eの詳細説明です。デザイン性に優れた商品です。', 'https://via.placeholder.com/300x300?text=Product+E'),
    ('商品F', 3500, '商品Fの詳細説明です。耐久性抜群の商品です。', 'https://via.placeholder.com/300x300?text=Product+F'),
    ('商品G', 4000, '商品Gの詳細説明です。環境に優しいエコ商品です。', 'https://via.placeholder.com/300x300?text=Product+G'),
    ('商品H', 4500, '商品Hの詳細説明です。使いやすさを追求した商品です。', 'https://via.placeholder.com/300x300?text=Product+H'),
    ('商品I', 5000, '商品Iの詳細説明です。プロフェッショナル向けの商品です。', 'https://via.placeholder.com/300x300?text=Product+I'),
    ('商品J', 5500, '商品Jの詳細説明です。限定生産の特別な商品です。', 'https://via.placeholder.com/300x300?text=Product+J'),
    ('商品K', 6000, '商品Kの詳細説明です。多用途に使える万能商品です。', 'https://via.placeholder.com/300x300?text=Product+K'),
    ('商品L', 6500, '商品Lの詳細説明です。軽量で持ち運びやすい商品です。', 'https://via.placeholder.com/300x300?text=Product+L'),
    ('商品M', 7000, '商品Mの詳細説明です。高級感あふれる商品です。', 'https://via.placeholder.com/300x300?text=Product+M'),
    ('商品N', 7500, '商品Nの詳細説明です。初心者にも扱いやすい商品です。', 'https://via.placeholder.com/300x300?text=Product+N'),
    ('商品O', 8000, '商品Oの詳細説明です。メンテナンスが簡単な商品です。', 'https://via.placeholder.com/300x300?text=Product+O'),
    ('商品P', 8500, '商品Pの詳細説明です。安全性を重視した商品です。', 'https://via.placeholder.com/300x300?text=Product+P'),
    ('商品Q', 9000, '商品Qの詳細説明です。長時間使用できる商品です。', 'https://via.placeholder.com/300x300?text=Product+Q'),
    ('商品R', 9500, '商品Rの詳細説明です。コンパクト設計の商品です。', 'https://via.placeholder.com/300x300?text=Product+R'),
    ('商品S', 10000, '商品Sの詳細説明です。最高品質の商品です。', 'https://via.placeholder.com/300x300?text=Product+S'),
    ('商品T', 10500, '商品Tの詳細説明です。革新的な機能を持つ商品です。', 'https://via.placeholder.com/300x300?text=Product+T');

-- ======================================
-- 3. 機能フラグデータ
-- ======================================

INSERT INTO feature_flags (flag_key, flag_name, default_value) VALUES
    ('favorite_feature', 'お気に入り機能', false);

-- ======================================
-- 4. ユーザー別機能フラグ設定
-- ======================================

-- user001: お気に入り機能ON
-- user002: お気に入り機能ON
-- その他のユーザーはデフォルト値（OFF）を使用
INSERT INTO user_feature_flags (user_id, flag_id, is_enabled) VALUES
    (2, 1, true),  -- user001
    (3, 1, true);  -- user002

-- ======================================
-- 5. お気に入りデータ（サンプル）
-- ======================================

-- user001のお気に入り
INSERT INTO favorites (user_id, product_id) VALUES
    (2, 1),  -- 商品A
    (2, 3),  -- 商品C
    (2, 5);  -- 商品E

-- user002のお気に入り
INSERT INTO favorites (user_id, product_id) VALUES
    (3, 2),  -- 商品B
    (3, 4);  -- 商品D

-- ======================================
-- 6. 購入履歴データ（サンプル）
-- ======================================

-- user001の購入履歴
INSERT INTO purchases (user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    (2, 1, 100, 1000, 100000),
    (2, 2, 200, 1500, 300000),
    (2, 3, 100, 2000, 200000);

-- user002の購入履歴
INSERT INTO purchases (user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    (3, 1, 300, 1000, 300000),
    (3, 4, 100, 2500, 250000);

-- user003の購入履歴
INSERT INTO purchases (user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    (4, 5, 100, 3000, 300000),
    (4, 6, 200, 3500, 700000);

-- user004の購入履歴
INSERT INTO purchases (user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    (5, 7, 100, 4000, 400000),
    (5, 8, 100, 4500, 450000);

-- user005の購入履歴
INSERT INTO purchases (user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    (6, 9, 200, 5000, 1000000);

-- ======================================
-- データ投入完了
-- ======================================

-- 投入データ確認クエリ
-- SELECT COUNT(*) AS user_count FROM users;
-- SELECT COUNT(*) AS product_count FROM products;
-- SELECT COUNT(*) AS purchase_count FROM purchases;
-- SELECT COUNT(*) AS favorite_count FROM favorites;
-- SELECT COUNT(*) AS feature_flag_count FROM feature_flags;
-- SELECT COUNT(*) AS user_feature_flag_count FROM user_feature_flags;
