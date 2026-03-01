#pragma once

#include <httplib.h>

namespace ws::services { class FavoriteService; }

namespace ws::routes
{

class FavoriteRoutes
{
public:
	explicit FavoriteRoutes(ws::services::FavoriteService& favoriteService);

	FavoriteRoutes(const FavoriteRoutes&) = delete;
	FavoriteRoutes& operator=(const FavoriteRoutes&) = delete;

	void Register(httplib::Server& server);

private:
	void HandleAddFavorite(const httplib::Request& req, httplib::Response& res);
	void HandleRemoveFavorite(const httplib::Request& req, httplib::Response& res);
	void HandleGetFavorites(const httplib::Request& req, httplib::Response& res);

	ws::services::FavoriteService& m_favoriteService;
};

} // namespace ws::routes
