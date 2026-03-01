#include "Database/FavoriteRepository.h"
#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <mutex>

namespace ws::database
{

FavoriteRepository::FavoriteRepository(DatabaseManager& dbManager)
	: m_dbManager(dbManager)
{
}

std::expected<ws::models::Favorite, std::string> FavoriteRepository::Create(int64_t userId, int64_t productId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* insertSql =
		"INSERT INTO favorites (user_id, product_id) VALUES (?, ?);";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), insertSql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::unexpected(std::string("Failed to prepare INSERT: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	sqlite3_bind_int64(stmt, 1, userId);
	sqlite3_bind_int64(stmt, 2, productId);

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE)
	{
		std::string errMsg = sqlite3_errmsg(m_dbManager.GetDb());
		// Check for UNIQUE constraint violation (already favorited)
		if (errMsg.find("UNIQUE") != std::string::npos)
		{
			return std::unexpected("This product is already in favorites");
		}
		return std::unexpected(std::string("Failed to insert favorite: ") + errMsg);
	}

	int64_t favoriteId = sqlite3_last_insert_rowid(m_dbManager.GetDb());

	// Retrieve the created favorite with product details
	const char* selectSql =
		"SELECT f.favorite_id, f.user_id, f.product_id, p.product_name, p.unit_price, "
		"COALESCE(p.image_url, '') AS image_url, f.created_at "
		"FROM favorites f "
		"INNER JOIN products p ON f.product_id = p.product_id "
		"WHERE f.favorite_id = ?;";

	sqlite3_stmt* selectStmt = nullptr;
	rc = sqlite3_prepare_v2(m_dbManager.GetDb(), selectSql, -1, &selectStmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::unexpected(std::string("Failed to retrieve created favorite: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	sqlite3_bind_int64(selectStmt, 1, favoriteId);

	if (sqlite3_step(selectStmt) != SQLITE_ROW)
	{
		sqlite3_finalize(selectStmt);
		return std::unexpected("Created favorite not found after insert");
	}

	ws::models::Favorite favorite;
	favorite.favoriteId = sqlite3_column_int64(selectStmt, 0);
	favorite.userId = sqlite3_column_int64(selectStmt, 1);
	favorite.productId = sqlite3_column_int64(selectStmt, 2);
	favorite.productName = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 3));
	favorite.unitPrice = sqlite3_column_int(selectStmt, 4);
	favorite.imageUrl = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 5));
	favorite.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 6));

	sqlite3_finalize(selectStmt);
	return favorite;
}

std::expected<void, std::string> FavoriteRepository::Delete(int64_t favoriteId, int64_t userId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	// First verify ownership: the favorite must belong to the requesting user
	const char* checkSql =
		"SELECT favorite_id FROM favorites WHERE favorite_id = ? AND user_id = ?;";

	sqlite3_stmt* checkStmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), checkSql, -1, &checkStmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::unexpected(std::string("Failed to prepare ownership check: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	sqlite3_bind_int64(checkStmt, 1, favoriteId);
	sqlite3_bind_int64(checkStmt, 2, userId);

	bool exists = (sqlite3_step(checkStmt) == SQLITE_ROW);
	sqlite3_finalize(checkStmt);

	if (!exists)
	{
		return std::unexpected("Favorite not found or does not belong to this user");
	}

	// Delete the favorite
	const char* deleteSql = "DELETE FROM favorites WHERE favorite_id = ? AND user_id = ?;";

	sqlite3_stmt* deleteStmt = nullptr;
	rc = sqlite3_prepare_v2(m_dbManager.GetDb(), deleteSql, -1, &deleteStmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::unexpected(std::string("Failed to prepare DELETE: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	sqlite3_bind_int64(deleteStmt, 1, favoriteId);
	sqlite3_bind_int64(deleteStmt, 2, userId);

	rc = sqlite3_step(deleteStmt);
	sqlite3_finalize(deleteStmt);

	if (rc != SQLITE_DONE)
	{
		return std::unexpected(std::string("Failed to delete favorite: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	return {};
}

std::vector<ws::models::Favorite> FavoriteRepository::FindByUserId(int64_t userId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT f.favorite_id, f.user_id, f.product_id, p.product_name, p.unit_price, "
		"COALESCE(p.image_url, '') AS image_url, f.created_at "
		"FROM favorites f "
		"INNER JOIN products p ON f.product_id = p.product_id "
		"WHERE f.user_id = ? "
		"ORDER BY f.created_at DESC;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return {};
	}

	sqlite3_bind_int64(stmt, 1, userId);

	std::vector<ws::models::Favorite> favorites;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ws::models::Favorite favorite;
		favorite.favoriteId = sqlite3_column_int64(stmt, 0);
		favorite.userId = sqlite3_column_int64(stmt, 1);
		favorite.productId = sqlite3_column_int64(stmt, 2);
		favorite.productName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		favorite.unitPrice = sqlite3_column_int(stmt, 4);
		favorite.imageUrl = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
		favorite.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
		favorites.push_back(favorite);
	}

	sqlite3_finalize(stmt);
	return favorites;
}

bool FavoriteRepository::ExistsByUserAndProduct(int64_t userId, int64_t productId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT 1 FROM favorites WHERE user_id = ? AND product_id = ? LIMIT 1;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int64(stmt, 1, userId);
	sqlite3_bind_int64(stmt, 2, productId);

	bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
	sqlite3_finalize(stmt);
	return exists;
}

} // namespace ws::database
