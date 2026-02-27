#include "Routes/FavoriteRoutes.h"

#include <nlohmann/json.hpp>

#include "Services/FavoriteService.h"
#include "Middleware/AuthMiddleware.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws::routes
{

FavoriteRoutes::FavoriteRoutes(ws::services::FavoriteService& favoriteService)
	: m_favoriteService(favoriteService)
{
}

void FavoriteRoutes::Register(httplib::Server& server)
{
	server.Post(ws::utils::kFavoritesPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleAddFavorite(req, res);
		});

	server.Delete(R"(/api/mobile/favorites/(\d+))",
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleRemoveFavorite(req, res);
		});

	server.Get(ws::utils::kFavoritesPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleGetFavorites(req, res);
		});
}

void FavoriteRoutes::HandleAddFavorite(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	// Parse request body
	nlohmann::json body;
	try
	{
		body = nlohmann::json::parse(req.body);
	}
	catch (const nlohmann::json::exception&)
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "Invalid JSON in request body"),
			"application/json; charset=utf-8");
		return;
	}

	if (!body.contains("productId") || !body["productId"].is_number_integer())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "productId is required and must be an integer"),
			"application/json; charset=utf-8");
		return;
	}

	int64_t productId = body["productId"].get<int64_t>();
	int64_t userId = payload->userId;

	auto result = m_favoriteService.AddFavorite(userId, productId);

	if (!result.has_value())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", result.error()),
			"application/json; charset=utf-8");
		return;
	}

	nlohmann::json favoriteJson = result.value();

	res.status = 201;
	res.set_content(
		ws::utils::MakeSuccessResponse(favoriteJson),
		"application/json; charset=utf-8");
}

void FavoriteRoutes::HandleRemoveFavorite(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	// Extract favorite ID from regex match
	int64_t favoriteId = 0;
	try
	{
		favoriteId = std::stoll(req.matches[1].str());
	}
	catch (...)
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "Invalid favorite ID"),
			"application/json; charset=utf-8");
		return;
	}

	int64_t userId = payload->userId;

	auto result = m_favoriteService.RemoveFavorite(favoriteId, userId);

	if (!result.has_value())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", result.error()),
			"application/json; charset=utf-8");
		return;
	}

	nlohmann::json data = {
		{"message", "Favorite removed successfully"}
	};

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

void FavoriteRoutes::HandleGetFavorites(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	int64_t userId = payload->userId;
	auto favorites = m_favoriteService.GetFavorites(userId);

	nlohmann::json favoritesJson = nlohmann::json::array();
	for (const auto& favorite : favorites)
	{
		favoritesJson.push_back(favorite);
	}

	nlohmann::json data = {
		{"favorites", favoritesJson}
	};

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

} // namespace ws::routes
