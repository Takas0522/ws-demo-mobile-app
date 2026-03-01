#pragma once

#include <string>
#include <vector>
#include "Models/PriceHistory.h"

struct sqlite3_stmt;

namespace ws::database
{

class DatabaseManager;

struct PriceHistoryResult
{
	std::vector<ws::models::PriceHistory> items;
	int totalCount = 0;
};

class PriceHistoryRepository
{
public:
	explicit PriceHistoryRepository(DatabaseManager& dbManager);

	PriceHistoryRepository(const PriceHistoryRepository&) = delete;
	PriceHistoryRepository& operator=(const PriceHistoryRepository&) = delete;

	[[nodiscard]] PriceHistoryResult FindByProductId(
		int64_t productId,
		const std::string& startDate,
		const std::string& endDate,
		int page,
		int limit);

	[[nodiscard]] bool ProductExists(int64_t productId);

private:
	ws::models::PriceHistory MapRow(sqlite3_stmt* stmt);
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
