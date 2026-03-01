#include "Database/PriceHistoryRepository.h"
#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <mutex>

namespace ws::database
{

PriceHistoryRepository::PriceHistoryRepository(DatabaseManager& dbManager)
	: m_dbManager(dbManager)
{
}

ws::models::PriceHistory PriceHistoryRepository::MapRow(sqlite3_stmt* stmt)
{
	ws::models::PriceHistory ph;
	ph.priceHistoryId = sqlite3_column_int64(stmt, 0);
	ph.productId      = sqlite3_column_int64(stmt, 1);
	ph.oldPrice       = sqlite3_column_int(stmt, 2);
	ph.newPrice       = sqlite3_column_int(stmt, 3);
	ph.changedAt      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
	ph.changedBy.userId   = sqlite3_column_int64(stmt, 5);
	ph.changedBy.userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

	if (sqlite3_column_type(stmt, 7) != SQLITE_NULL)
	{
		ph.changeReason = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
	}
	else
	{
		ph.changeReason = std::nullopt;
	}

	return ph;
}

PriceHistoryResult PriceHistoryRepository::FindByProductId(
	int64_t productId,
	const std::string& startDate,
	const std::string& endDate,
	int page,
	int limit)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	PriceHistoryResult result;

	// Get total count
	{
		const char* countSql =
			"SELECT COUNT(*) FROM product_price_history "
			"WHERE product_id = ? AND changed_at >= ? AND changed_at <= ?;";

		sqlite3_stmt* countStmt = nullptr;
		int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), countSql, -1, &countStmt, nullptr);
		if (rc == SQLITE_OK)
		{
			sqlite3_bind_int64(countStmt, 1, productId);
			sqlite3_bind_text(countStmt, 2, startDate.c_str(),
				static_cast<int>(startDate.size()), SQLITE_STATIC);
			sqlite3_bind_text(countStmt, 3, endDate.c_str(),
				static_cast<int>(endDate.size()), SQLITE_STATIC);

			if (sqlite3_step(countStmt) == SQLITE_ROW)
			{
				result.totalCount = sqlite3_column_int(countStmt, 0);
			}
		}
		sqlite3_finalize(countStmt);
	}

	// Fetch paginated results with user name
	const char* sql =
		"SELECT ph.price_history_id, ph.product_id, ph.old_price, ph.new_price, "
		"ph.changed_at, ph.changed_by, "
		"COALESCE(u.user_name, '不明') AS user_name, "
		"ph.change_reason "
		"FROM product_price_history ph "
		"LEFT JOIN users u ON ph.changed_by = u.user_id "
		"WHERE ph.product_id = ? AND ph.changed_at >= ? AND ph.changed_at <= ? "
		"ORDER BY ph.changed_at DESC "
		"LIMIT ? OFFSET ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return result;
	}

	int offset = (page - 1) * limit;
	sqlite3_bind_int64(stmt, 1, productId);
	sqlite3_bind_text(stmt, 2, startDate.c_str(),
		static_cast<int>(startDate.size()), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, endDate.c_str(),
		static_cast<int>(endDate.size()), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 4, limit);
	sqlite3_bind_int(stmt, 5, offset);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		result.items.push_back(MapRow(stmt));
	}

	sqlite3_finalize(stmt);
	return result;
}

bool PriceHistoryRepository::ProductExists(int64_t productId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql = "SELECT 1 FROM products WHERE product_id = ? LIMIT 1;";
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int64(stmt, 1, productId);
	bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
	sqlite3_finalize(stmt);
	return exists;
}

} // namespace ws::database
