-- ======================================
-- mobile-app-system 初期データ投入 (SQLite)
-- ======================================
-- 作成日: 2025-01-08
-- バージョン: 1.1
-- 参照: docs/specs/mobile-app-system/04-data-model.md
-- ======================================

-- 外部キー制約を有効化
PRAGMA foreign_keys = ON;

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
    ('オーガニック緑茶 100g', 1000, '静岡県産の一番茶のみを使用した有機栽培の緑茶。深い旨味とすっきりとした後味が特徴です。', 'https://via.placeholder.com/300x300?text=Green+Tea'),
    ('天然はちみつ 国産百花蜜 250g', 1500, '国内の養蜂場で採取された百花蜜。非加熱処理で酵素や栄養素をそのまま閉じ込めています。', 'https://via.placeholder.com/300x300?text=Honey'),
    ('手摘みダージリンティー 50g', 2000, 'インド・ダージリン地方の標高2,000mの茶園で手摘みされたファーストフラッシュ。華やかなマスカテルフレーバーが楽しめます。', 'https://via.placeholder.com/300x300?text=Darjeeling'),
    ('北海道産 小麦粉 はるゆたか 1kg', 2500, '北海道十勝産「はるゆたか」を100%使用した強力粉。パンやピザ生地に最適で、もっちりとした食感に仕上がります。', 'https://via.placeholder.com/300x300?text=Flour'),
    ('エクストラバージンオリーブオイル 500ml', 3000, 'スペイン・アンダルシア産のピクアル種を使用。コールドプレス製法で搾油し、フルーティーな香りが特徴です。', 'https://via.placeholder.com/300x300?text=Olive+Oil'),
    ('熟成バルサミコ酢 250ml', 3500, 'イタリア・モデナ産のぶどうを使用し、木樽で5年以上熟成。濃厚な甘みと芳醇な香りが料理を引き立てます。', 'https://via.placeholder.com/300x300?text=Balsamic'),
    ('有機マカダミアナッツ 200g', 4000, 'オーストラリア産の有機栽培マカダミアナッツ。低温でじっくりローストし、クリーミーな味わいに仕上げました。', 'https://via.placeholder.com/300x300?text=Macadamia'),
    ('十勝産あずき使用 つぶあん 500g', 4500, '北海道十勝産の大粒小豆を丁寧に炊き上げたつぶあん。和菓子作りやトーストのお供にどうぞ。', 'https://via.placeholder.com/300x300?text=Azuki+Paste'),
    ('スペシャルティコーヒー エチオピア イルガチェフェ 200g', 5000, 'エチオピア・イルガチェフェ地区の標高1,800m以上で栽培されたアラビカ種。ジャスミンのような華やかな香りとレモンティーのような風味。', 'https://via.placeholder.com/300x300?text=Coffee'),
    ('宇治抹茶 薄茶用 30g缶入り', 5500, '京都府宇治産の碾茶を石臼で丁寧に挽いた薄茶用抹茶。鮮やかな緑色と上品な甘みが特徴です。', 'https://via.placeholder.com/300x300?text=Matcha'),
    ('国産 天日干し切り干し大根 100g×5袋', 6000, '宮崎県産の大根を寒風と天日でじっくり乾燥。歯ごたえが良く、煮物や和え物にぴったりの自然食品です。', 'https://via.placeholder.com/300x300?text=Dried+Radish'),
    ('マヌカハニー UMF10+ 250g', 6500, 'ニュージーランド産マヌカハニー。UMF10+認定の高品質品。そのままスプーンで、またはヨーグルトに混ぜてどうぞ。', 'https://via.placeholder.com/300x300?text=Manuka+Honey'),
    ('黒トリュフ塩 50g', 7000, 'イタリア・ウンブリア産の黒トリュフとフランス・ゲランドの塩をブレンド。パスタや卵料理に振りかけるだけで本格的な風味に。', 'https://via.placeholder.com/300x300?text=Truffle+Salt'),
    ('有機ルイボスティー 100包入り', 7500, '南アフリカ・セダルバーグ山脈産のオーガニックルイボス。カフェインフリーでお子様からお年寄りまで安心して楽しめます。', 'https://via.placeholder.com/300x300?text=Rooibos+Tea'),
    ('純粋国産アカシアはちみつ 500g', 8000, '長野県産のアカシア花から採取した純粋はちみつ。クセがなく上品な甘さで、パンケーキやヨーグルトに最適です。', 'https://via.placeholder.com/300x300?text=Acacia+Honey'),
    ('瀬戸内レモン使用 レモンオリーブオイル 250ml', 8500, '瀬戸内海の温暖な気候で育った国産レモンとイタリア産オリーブオイルを独自製法でブレンド。サラダやカルパッチョに。', 'https://via.placeholder.com/300x300?text=Lemon+Oil'),
    ('最高級キューバ産バニラビーンズ 5本入り', 9000, 'キューバ産の希少なバニラビーンズ。濃厚な甘い香りが特徴で、お菓子作りやアイスクリームに贅沢な風味を加えます。', 'https://via.placeholder.com/300x300?text=Vanilla+Beans'),
    ('有機カカオニブ コロンビア産 150g', 9500, 'コロンビアの契約農家で有機栽培されたカカオ豆を発酵・乾燥・焙煎し砕いたニブ。そのままおやつに、またはスムージーのトッピングに。', 'https://via.placeholder.com/300x300?text=Cacao+Nibs'),
    ('天然サフラン スペイン ラ・マンチャ産 2g', 10000, '世界最高品質と言われるスペイン・ラ・マンチャ産のサフラン。パエリアやブイヤベースに鮮やかな黄金色と独特の芳香を与えます。', 'https://via.placeholder.com/300x300?text=Saffron'),
    ('白トリュフオイル 100ml', 10500, 'イタリア・ピエモンテ産の白トリュフで香り付けしたエクストラバージンオリーブオイル。リゾットやパスタの仕上げに数滴垂らすだけで芳醇な香りが広がります。', 'https://via.placeholder.com/300x300?text=Truffle+Oil');

-- ======================================
-- 3. 機能フラグデータ
-- ======================================

INSERT INTO feature_flags (flag_key, flag_name, default_value) VALUES
    ('favorite_feature', 'お気に入り機能', 0);

-- ======================================
-- 4. ユーザー別機能フラグ設定
-- ======================================

-- user001: お気に入り機能ON
-- user002: お気に入り機能ON
-- その他のユーザーはデフォルト値（OFF）を使用
INSERT INTO user_feature_flags (user_id, flag_id, is_enabled) VALUES
    (2, 1, 1),  -- user001
    (3, 1, 1);  -- user002

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
INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567801', 2, 1, 100, 1000, 100000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567802', 2, 2, 200, 1500, 300000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567803', 2, 3, 100, 2000, 200000);

-- user002の購入履歴
INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567804', 3, 1, 300, 1000, 300000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567805', 3, 4, 100, 2500, 250000);

-- user003の購入履歴
INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567806', 4, 5, 100, 3000, 300000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567807', 4, 6, 200, 3500, 700000);

-- user004の購入履歴
INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567808', 5, 7, 100, 4000, 400000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567809', 5, 8, 100, 4500, 450000);

-- user005の購入履歴
INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567810', 6, 9, 200, 5000, 1000000);

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
