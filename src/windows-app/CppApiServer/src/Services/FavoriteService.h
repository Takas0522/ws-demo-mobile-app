#pragma once

#include <string>
#include <expected>
#include <vector>
#include "Models/Favorite.h"

namespace ws::database { class FavoriteRepository; }

namespace ws::services
{

class FavoriteService
{
public:
	explicit FavoriteService(ws::database::FavoriteRepository& favoriteRepo);

	FavoriteService(const FavoriteService&) = delete;
	FavoriteService& operator=(const FavoriteService&) = delete;

	[[nodiscard]] std::expected<ws::models::Favorite, std::string> AddFavorite(
		int64_t userId, int64_t productId);
	[[nodiscard]] std::expected<void, std::string> RemoveFavorite(
		int64_t favoriteId, int64_t userId);
	[[nodiscard]] std::vector<ws::models::Favorite> GetFavorites(int64_t userId);

private:
	ws::database::FavoriteRepository& m_favoriteRepo;
};

} // namespace ws::services
