#pragma once

#include <string>
#include <vector>
#include <expected>
#include "Models/Favorite.h"
#include "Models/ApiError.h"

namespace ws::services
{

class HttpClient;
class AuthService;

class FavoriteService
{
public:
	FavoriteService(const HttpClient& httpClient, const AuthService& authService);
	~FavoriteService() = default;

	FavoriteService(const FavoriteService&) = delete;
	FavoriteService& operator=(const FavoriteService&) = delete;

	[[nodiscard]] std::expected<ws::models::Favorite, ws::models::ApiError> AddFavorite(
		int64_t productId) const;

	[[nodiscard]] std::expected<void, ws::models::ApiError> RemoveFavorite(
		int64_t productId) const;

	[[nodiscard]] std::expected<std::vector<ws::models::Favorite>, ws::models::ApiError> FetchFavorites() const;

private:
	const HttpClient& m_httpClient;
	const AuthService& m_authService;
};

} // namespace ws::services
