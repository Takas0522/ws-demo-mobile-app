#pragma once

#include "Database/IProductRepository.h"

struct sqlite3;
struct sqlite3_stmt;

namespace ws::database
{

class DatabaseManager;

class ProductRepository : public IProductRepository
{
public:
	explicit ProductRepository(DatabaseManager& dbManager);
	~ProductRepository() override = default;

	ProductRepository(const ProductRepository&) = delete;
	ProductRepository& operator=(const ProductRepository&) = delete;

	[[nodiscard]] ProductListResult FindAll(int page, int limit,
		const std::string& sortBy = "created_at", const std::string& sortOrder = "DESC") override;
	[[nodiscard]] ProductListResult SearchByKeyword(const std::string& keyword, int page, int limit) override;
	[[nodiscard]] std::optional<ws::models::Product> FindById(int64_t productId) override;

private:
	ws::models::Product MapRow(sqlite3_stmt* stmt);
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
