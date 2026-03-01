#include "Database/ProductRepository.h"
#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <mutex>
#include <set>

namespace ws::database
{

// Allowed sort columns to prevent SQL injection via column names
static const std::set<std::string> kAllowedSortColumns = {
	"created_at", "product_name", "unit_price", "product_id"
};

ProductRepository::ProductRepository(DatabaseManager& dbManager)
	: m_dbManager(dbManager)
{
}

ws::models::Product ProductRepository::MapRow(sqlite3_stmt* stmt)
{
	ws::models::Product product;
	product.productId = sqlite3_column_int64(stmt, 0);
	product.productName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
	product.unitPrice = sqlite3_column_int(stmt, 2);

	if (sqlite3_column_type(stmt, 3) != SQLITE_NULL)
	{
		product.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
	}
	else
	{
		product.description = std::nullopt;
	}

	if (sqlite3_column_type(stmt, 4) != SQLITE_NULL)
	{
		product.imageUrl = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
	}
	else
	{
		product.imageUrl = std::nullopt;
	}

	product.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
	product.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
	return product;
}

ProductListResult ProductRepository::FindAll(int page, int limit,
	const std::string& sortBy, const std::string& sortOrder)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	ProductListResult result;

	// Get total count
	{
		const char* countSql = "SELECT COUNT(*) FROM products;";
		sqlite3_stmt* countStmt = nullptr;
		int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), countSql, -1, &countStmt, nullptr);
		if (rc == SQLITE_OK && sqlite3_step(countStmt) == SQLITE_ROW)
		{
			result.totalCount = sqlite3_column_int(countStmt, 0);
		}
		sqlite3_finalize(countStmt);
	}

	// Validate sort column (whitelist to prevent SQL injection)
	std::string safeSortBy = "created_at";
	if (kAllowedSortColumns.contains(sortBy))
	{
		safeSortBy = sortBy;
	}

	// Validate sort order
	std::string safeSortOrder = "DESC";
	if (sortOrder == "ASC" || sortOrder == "asc")
	{
		safeSortOrder = "ASC";
	}

	// Build query with validated sort parameters
	std::string sql =
		"SELECT product_id, product_name, unit_price, description, image_url, created_at, updated_at "
		"FROM products ORDER BY " + safeSortBy + " " + safeSortOrder + " "
		"LIMIT ? OFFSET ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return result;
	}

	int offset = (page - 1) * limit;
	sqlite3_bind_int(stmt, 1, limit);
	sqlite3_bind_int(stmt, 2, offset);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		result.products.push_back(MapRow(stmt));
	}

	sqlite3_finalize(stmt);
	return result;
}

ProductListResult ProductRepository::SearchByKeyword(const std::string& keyword, int page, int limit)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	ProductListResult result;
	std::string likePattern = "%" + keyword + "%";

	// Get total count for search results
	{
		const char* countSql =
			"SELECT COUNT(*) FROM products "
			"WHERE LOWER(product_name) LIKE LOWER(?) OR LOWER(description) LIKE LOWER(?);";
		sqlite3_stmt* countStmt = nullptr;
		int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), countSql, -1, &countStmt, nullptr);
		if (rc == SQLITE_OK)
		{
			sqlite3_bind_text(countStmt, 1, likePattern.c_str(),
				static_cast<int>(likePattern.size()), SQLITE_STATIC);
			sqlite3_bind_text(countStmt, 2, likePattern.c_str(),
				static_cast<int>(likePattern.size()), SQLITE_STATIC);
			if (sqlite3_step(countStmt) == SQLITE_ROW)
			{
				result.totalCount = sqlite3_column_int(countStmt, 0);
			}
		}
		sqlite3_finalize(countStmt);
	}

	// Fetch paginated search results
	const char* sql =
		"SELECT product_id, product_name, unit_price, description, image_url, created_at, updated_at "
		"FROM products "
		"WHERE LOWER(product_name) LIKE LOWER(?) OR LOWER(description) LIKE LOWER(?) "
		"ORDER BY product_id ASC "
		"LIMIT ? OFFSET ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return result;
	}

	int offset = (page - 1) * limit;
	sqlite3_bind_text(stmt, 1, likePattern.c_str(),
		static_cast<int>(likePattern.size()), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, likePattern.c_str(),
		static_cast<int>(likePattern.size()), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, limit);
	sqlite3_bind_int(stmt, 4, offset);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		result.products.push_back(MapRow(stmt));
	}

	sqlite3_finalize(stmt);
	return result;
}

std::optional<ws::models::Product> ProductRepository::FindById(int64_t productId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT product_id, product_name, unit_price, description, image_url, created_at, updated_at "
		"FROM products WHERE product_id = ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::nullopt;
	}

	sqlite3_bind_int64(stmt, 1, productId);

	std::optional<ws::models::Product> result;
	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		result = MapRow(stmt);
	}

	sqlite3_finalize(stmt);
	return result;
}

} // namespace ws::database
