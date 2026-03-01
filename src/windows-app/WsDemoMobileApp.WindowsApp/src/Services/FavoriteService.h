#pragma once

#include "Services/IFavoriteService.h"

namespace ws::services
{

class IHttpClient;
class IAuthService;

class FavoriteService : public IFavoriteService
{
public:
	FavoriteService(const IHttpClient& httpClient, const IAuthService& authService);
	~FavoriteService() override = default;

	FavoriteService(const FavoriteService&) = delete;
	FavoriteService& operator=(const FavoriteService&) = delete;

	[[nodiscard]] std::expected<ws::models::Favorite, ws::models::ApiError> AddFavorite(
		int64_t productId) const override;

	[[nodiscard]] std::expected<void, ws::models::ApiError> RemoveFavorite(
		int64_t productId) const override;

	[[nodiscard]] std::expected<std::vector<ws::models::Favorite>, ws::models::ApiError> FetchFavorites() const override;

private:
	const IHttpClient& m_httpClient;
	const IAuthService& m_authService;
};

} // namespace ws::services
