#pragma once

#include <string>
#include <optional>
#include <vector>
#include <expected>
#include "Models/Favorite.h"

namespace ws::database
{

class DatabaseManager;

class FavoriteRepository
{
public:
	explicit FavoriteRepository(DatabaseManager& dbManager);

	[[nodiscard]] std::expected<ws::models::Favorite, std::string> Create(int64_t userId, int64_t productId);
	[[nodiscard]] std::expected<void, std::string> Delete(int64_t favoriteId, int64_t userId);
	[[nodiscard]] std::vector<ws::models::Favorite> FindByUserId(int64_t userId);
	[[nodiscard]] bool ExistsByUserAndProduct(int64_t userId, int64_t productId);

private:
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
