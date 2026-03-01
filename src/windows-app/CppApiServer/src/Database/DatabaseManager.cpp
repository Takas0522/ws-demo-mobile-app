#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <iostream>

namespace ws::database
{

// ---------------------------------------------------------------------------
// Embedded SQL: Schema
// ---------------------------------------------------------------------------
static const char* const kCreateTablesSql = R"SQL(
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_name TEXT NOT NULL,
    login_id TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    user_type TEXT NOT NULL DEFAULT 'user',
    created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    CONSTRAINT chk_user_type CHECK (user_type IN ('user', 'admin')),
    CONSTRAINT chk_login_id_length CHECK (LENGTH(login_id) >= 4 AND LENGTH(login_id) <= 20),
    CONSTRAINT chk_user_name_length CHECK (LENGTH(user_name) >= 1 AND LENGTH(user_name) <= 100)
);
CREATE INDEX IF NOT EXISTS idx_users_user_type ON users(user_type);

CREATE TABLE IF NOT EXISTS products (
    product_id INTEGER PRIMARY KEY AUTOINCREMENT,
    product_name TEXT NOT NULL,
    unit_price INTEGER NOT NULL,
    description TEXT,
    image_url TEXT,
    created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    CONSTRAINT chk_unit_price CHECK (unit_price >= 1),
    CONSTRAINT chk_product_name_length CHECK (LENGTH(product_name) >= 1 AND LENGTH(product_name) <= 100)
);
CREATE INDEX IF NOT EXISTS idx_products_product_name ON products(product_name);

CREATE TABLE IF NOT EXISTS purchases (
    purchase_id TEXT PRIMARY KEY,
    user_id INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL,
    unit_price_at_purchase INTEGER NOT NULL,
    total_amount INTEGER NOT NULL,
    purchased_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    CONSTRAINT fk_purchases_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE RESTRICT,
    CONSTRAINT fk_purchases_product FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE RESTRICT,
    CONSTRAINT chk_quantity CHECK (quantity > 0),
    CONSTRAINT chk_quantity_multiple CHECK (quantity % 100 = 0),
    CONSTRAINT chk_unit_price_at_purchase CHECK (unit_price_at_purchase >= 1),
    CONSTRAINT chk_total_amount CHECK (total_amount >= 1),
    CONSTRAINT chk_total_amount_calc CHECK (total_amount = unit_price_at_purchase * quantity)
);
CREATE INDEX IF NOT EXISTS idx_purchases_user_id ON purchases(user_id);
CREATE INDEX IF NOT EXISTS idx_purchases_product_id ON purchases(product_id);
CREATE INDEX IF NOT EXISTS idx_purchases_purchased_at ON purchases(purchased_at);

CREATE TABLE IF NOT EXISTS favorites (
    favorite_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    CONSTRAINT fk_favorites_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    CONSTRAINT fk_favorites_product FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,
    CONSTRAINT uk_favorites_user_product UNIQUE (user_id, product_id)
);
CREATE INDEX IF NOT EXISTS idx_favorites_user_id ON favorites(user_id);
CREATE INDEX IF NOT EXISTS idx_favorites_product_id ON favorites(product_id);

CREATE TABLE IF NOT EXISTS feature_flags (
    flag_id INTEGER PRIMARY KEY AUTOINCREMENT,
    flag_key TEXT NOT NULL UNIQUE,
    flag_name TEXT NOT NULL,
    default_value INTEGER NOT NULL DEFAULT 0 CHECK (default_value IN (0, 1)),
    created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime'))
);

CREATE TABLE IF NOT EXISTS user_feature_flags (
    user_flag_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    flag_id INTEGER NOT NULL,
    is_enabled INTEGER NOT NULL DEFAULT 0 CHECK (is_enabled IN (0, 1)),
    created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    CONSTRAINT fk_user_feature_flags_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    CONSTRAINT fk_user_feature_flags_flag FOREIGN KEY (flag_id) REFERENCES feature_flags(flag_id) ON DELETE CASCADE,
    CONSTRAINT uk_user_feature_flags_user_flag UNIQUE (user_id, flag_id)
);
CREATE INDEX IF NOT EXISTS idx_user_feature_flags_user_id ON user_feature_flags(user_id);
CREATE INDEX IF NOT EXISTS idx_user_feature_flags_flag_id ON user_feature_flags(flag_id);

CREATE TRIGGER IF NOT EXISTS update_users_updated_at AFTER UPDATE ON users
    FOR EACH ROW BEGIN
        UPDATE users SET updated_at = datetime('now', 'localtime') WHERE user_id = NEW.user_id;
    END;
CREATE TRIGGER IF NOT EXISTS update_products_updated_at AFTER UPDATE ON products
    FOR EACH ROW BEGIN
        UPDATE products SET updated_at = datetime('now', 'localtime') WHERE product_id = NEW.product_id;
    END;
CREATE TRIGGER IF NOT EXISTS update_user_feature_flags_updated_at AFTER UPDATE ON user_feature_flags
    FOR EACH ROW BEGIN
        UPDATE user_feature_flags SET updated_at = datetime('now', 'localtime') WHERE user_flag_id = NEW.user_flag_id;
    END;
)SQL";

// ---------------------------------------------------------------------------
// Embedded SQL: Seed data
// ---------------------------------------------------------------------------
static const char* const kSeedDataSql = R"SQL(
INSERT OR IGNORE INTO users (user_name, login_id, password_hash, user_type) VALUES
    ('管理者', 'admin001', '$2a$10$9XQy779t3Zn.q2vGeMn6we/cHZw5nDdKd7oD1hsn8Li5TWRKPD3HC', 'admin');
INSERT OR IGNORE INTO users (user_name, login_id, password_hash, user_type) VALUES
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

INSERT OR IGNORE INTO products (product_name, unit_price, description, image_url) VALUES
    ('オーガニック緑茶 100g', 1000, '静岡県産の一番茶のみを使用した有機栽培の緑茶。', 'https://via.placeholder.com/300x300?text=Green+Tea'),
    ('天然はちみつ 国産百花蜜 250g', 1500, '国内の養蜂場で採取された百花蜜。', 'https://via.placeholder.com/300x300?text=Honey'),
    ('手摘みダージリンティー 50g', 2000, 'インド・ダージリン地方の茶園で手摘みされたファーストフラッシュ。', 'https://via.placeholder.com/300x300?text=Darjeeling'),
    ('北海道産 小麦粉 はるゆたか 1kg', 2500, '北海道十勝産の強力粉。', 'https://via.placeholder.com/300x300?text=Flour'),
    ('エクストラバージンオリーブオイル 500ml', 3000, 'スペイン産コールドプレス製法。', 'https://via.placeholder.com/300x300?text=Olive+Oil'),
    ('熟成バルサミコ酢 250ml', 3500, 'イタリア・モデナ産5年熟成。', 'https://via.placeholder.com/300x300?text=Balsamic'),
    ('有機マカダミアナッツ 200g', 4000, 'オーストラリア産有機栽培。', 'https://via.placeholder.com/300x300?text=Macadamia'),
    ('十勝産あずき使用 つぶあん 500g', 4500, '北海道十勝産の大粒小豆。', 'https://via.placeholder.com/300x300?text=Azuki+Paste'),
    ('スペシャルティコーヒー エチオピア 200g', 5000, 'エチオピア・イルガチェフェ産アラビカ種。', 'https://via.placeholder.com/300x300?text=Coffee'),
    ('宇治抹茶 薄茶用 30g缶入り', 5500, '京都府宇治産の石臼挽き抹茶。', 'https://via.placeholder.com/300x300?text=Matcha'),
    ('国産 天日干し切り干し大根 100g×5袋', 6000, '宮崎県産の天日干し大根。', 'https://via.placeholder.com/300x300?text=Dried+Radish'),
    ('マヌカハニー UMF10+ 250g', 6500, 'ニュージーランド産UMF10+認定。', 'https://via.placeholder.com/300x300?text=Manuka+Honey'),
    ('黒トリュフ塩 50g', 7000, 'イタリア産黒トリュフとゲランド塩。', 'https://via.placeholder.com/300x300?text=Truffle+Salt'),
    ('有機ルイボスティー 100包入り', 7500, '南アフリカ産オーガニックルイボス。', 'https://via.placeholder.com/300x300?text=Rooibos+Tea'),
    ('純粋国産アカシアはちみつ 500g', 8000, '長野県産アカシア花の純粋はちみつ。', 'https://via.placeholder.com/300x300?text=Acacia+Honey'),
    ('瀬戸内レモン レモンオリーブオイル 250ml', 8500, '国産レモンとイタリア産オリーブオイル。', 'https://via.placeholder.com/300x300?text=Lemon+Oil'),
    ('最高級キューバ産バニラビーンズ 5本入り', 9000, 'キューバ産の希少なバニラビーンズ。', 'https://via.placeholder.com/300x300?text=Vanilla+Beans'),
    ('有機カカオニブ コロンビア産 150g', 9500, 'コロンビア産有機カカオ豆のニブ。', 'https://via.placeholder.com/300x300?text=Cacao+Nibs'),
    ('天然サフラン スペイン産 2g', 10000, 'スペイン・ラ・マンチャ産の最高品質サフラン。', 'https://via.placeholder.com/300x300?text=Saffron'),
    ('白トリュフオイル 100ml', 10500, 'イタリア・ピエモンテ産白トリュフオイル。', 'https://via.placeholder.com/300x300?text=Truffle+Oil');

INSERT OR IGNORE INTO feature_flags (flag_key, flag_name, default_value) VALUES
    ('favorite_feature', 'お気に入り機能', 0);

INSERT OR IGNORE INTO user_feature_flags (user_id, flag_id, is_enabled) VALUES
    (2, 1, 1),
    (3, 1, 1);

INSERT OR IGNORE INTO favorites (user_id, product_id) VALUES
    (2, 1),
    (2, 3),
    (2, 5),
    (3, 2),
    (3, 4);

INSERT OR IGNORE INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) VALUES
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567801', 2, 1, 100, 1000, 100000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567802', 2, 2, 200, 1500, 300000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567803', 2, 3, 100, 2000, 200000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567804', 3, 1, 300, 1000, 300000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567805', 3, 4, 100, 2500, 250000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567806', 4, 5, 100, 3000, 300000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567807', 4, 6, 200, 3500, 700000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567808', 5, 7, 100, 4000, 400000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567809', 5, 8, 100, 4500, 450000),
    ('a1b2c3d4-e5f6-7890-abcd-ef1234567810', 6, 9, 200, 5000, 1000000);
)SQL";

// ---------------------------------------------------------------------------
// DatabaseManager implementation
// ---------------------------------------------------------------------------

DatabaseManager::DatabaseManager() = default;

DatabaseManager::~DatabaseManager()
{
	if (m_db)
	{
		sqlite3_close(m_db);
		m_db = nullptr;
	}
}

std::expected<void, std::string> DatabaseManager::Initialize(const std::string& dbPath)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_dbPath = dbPath;

	int rc = sqlite3_open(dbPath.c_str(), &m_db);
	if (rc != SQLITE_OK)
	{
		std::string err = "Failed to open database: ";
		if (m_db)
		{
			err += sqlite3_errmsg(m_db);
			sqlite3_close(m_db);
			m_db = nullptr;
		}
		else
		{
			err += "out of memory";
		}
		return std::unexpected(err);
	}

	// Enable WAL mode for better concurrency
	auto walResult = ExecuteSql("PRAGMA journal_mode=WAL;");
	if (!walResult)
	{
		return std::unexpected("Failed to enable WAL mode: " + walResult.error());
	}

	// Enable foreign key constraints
	auto fkResult = ExecuteSql("PRAGMA foreign_keys=ON;");
	if (!fkResult)
	{
		return std::unexpected("Failed to enable foreign keys: " + fkResult.error());
	}

	// Initialize schema if not already done
	if (!IsSchemaInitialized())
	{
		auto schemaResult = InitializeSchema();
		if (!schemaResult)
		{
			return std::unexpected("Failed to initialize schema: " + schemaResult.error());
		}
		std::cerr << "[INFO] Database schema initialized and seed data inserted." << std::endl;
	}
	else
	{
		std::cerr << "[INFO] Database schema already initialized." << std::endl;
	}

	return {};
}

sqlite3* DatabaseManager::GetDb() const
{
	return m_db;
}

std::mutex& DatabaseManager::GetMutex()
{
	return m_mutex;
}

std::expected<void, std::string> DatabaseManager::ExecuteSql(const std::string& sql)
{
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK)
	{
		std::string err = errMsg ? errMsg : "Unknown SQLite error";
		if (errMsg)
		{
			sqlite3_free(errMsg);
		}
		return std::unexpected(err);
	}
	return {};
}

std::expected<void, std::string> DatabaseManager::InitializeSchema()
{
	// Create tables
	auto createResult = ExecuteSql(kCreateTablesSql);
	if (!createResult)
	{
		return std::unexpected("Schema creation failed: " + createResult.error());
	}

	// Insert seed data
	auto seedResult = ExecuteSql(kSeedDataSql);
	if (!seedResult)
	{
		return std::unexpected("Seed data insertion failed: " + seedResult.error());
	}

	return {};
}

bool DatabaseManager::IsSchemaInitialized()
{
	if (!m_db)
	{
		return false;
	}

	sqlite3_stmt* stmt = nullptr;
	const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='users';";
	int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return false;
	}

	bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
	sqlite3_finalize(stmt);
	return exists;
}

} // namespace ws::database
