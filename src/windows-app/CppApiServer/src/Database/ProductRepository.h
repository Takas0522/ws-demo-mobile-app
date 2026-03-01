#pragma once

#include <string>
#include <optional>
#include <vector>
#include "Models/Product.h"

struct sqlite3;
struct sqlite3_stmt;

namespace ws::database
{

class DatabaseManager;

struct ProductListResult
{
	std::vector<ws::models::Product> products;
	int totalCount = 0;
};

class ProductRepository
{
public:
	explicit ProductRepository(DatabaseManager& dbManager);

	[[nodiscard]] ProductListResult FindAll(int page, int limit,
		const std::string& sortBy = "created_at", const std::string& sortOrder = "DESC");
	[[nodiscard]] ProductListResult SearchByKeyword(const std::string& keyword, int page, int limit);
	[[nodiscard]] std::optional<ws::models::Product> FindById(int64_t productId);

private:
	ws::models::Product MapRow(sqlite3_stmt* stmt);
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
