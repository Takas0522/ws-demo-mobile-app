#pragma once

#include <vector>
#include <expected>
#include <cstdint>
#include "Models/Favorite.h"
#include "Models/ApiError.h"

namespace ws::services
{

class IFavoriteService
{
public:
	virtual ~IFavoriteService() = default;

	[[nodiscard]] virtual std::expected<ws::models::Favorite, ws::models::ApiError> AddFavorite(
		int64_t productId) const = 0;

	[[nodiscard]] virtual std::expected<void, ws::models::ApiError> RemoveFavorite(
		int64_t productId) const = 0;

	[[nodiscard]] virtual std::expected<std::vector<ws::models::Favorite>, ws::models::ApiError> FetchFavorites() const = 0;

protected:
	IFavoriteService() = default;
	IFavoriteService(const IFavoriteService&) = default;
	IFavoriteService& operator=(const IFavoriteService&) = default;
};

} // namespace ws::services
