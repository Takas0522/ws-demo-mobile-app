#include "Services/FavoriteService.h"
#include "Services/IHttpClient.h"
#include "Services/IAuthService.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

#include <nlohmann/json.hpp>

namespace ws::services
{

FavoriteService::FavoriteService(const IHttpClient& httpClient, const IAuthService& authService)
	: m_httpClient(httpClient)
	, m_authService(authService)
{
}

std::expected<ws::models::Favorite, ws::models::ApiError> FavoriteService::AddFavorite(
	int64_t productId) const
{
	nlohmann::json requestBody = {
		{"productId", productId}
	};

	auto result = m_httpClient.Post(
		ws::utils::kFavoritesPath,
		requestBody.dump(),
		m_authService.GetToken());

	if (!result.has_value())
	{
		return std::unexpected(result.error());
	}

	const auto& response = result.value();

	if (response.statusCode != 201 && response.statusCode != 200)
	{
		auto error = ws::utils::ParseApiError(response.body);
		if (error.has_value())
		{
			return std::unexpected(std::move(error.value()));
		}
		return std::unexpected(ws::models::ApiError{"FAVORITE_001", "お気に入り登録に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		return data.at("favorite").get<ws::models::Favorite>();
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

std::expected<void, ws::models::ApiError> FavoriteService::RemoveFavorite(
	int64_t productId) const
{
	std::string path = std::string(ws::utils::kFavoritesPath) + "/" + std::to_string(productId);

	auto result = m_httpClient.Delete(path, m_authService.GetToken());

	if (!result.has_value())
	{
		return std::unexpected(result.error());
	}

	const auto& response = result.value();

	if (response.statusCode != 200 && response.statusCode != 204)
	{
		auto error = ws::utils::ParseApiError(response.body);
		if (error.has_value())
		{
			return std::unexpected(std::move(error.value()));
		}
		return std::unexpected(ws::models::ApiError{"FAVORITE_002", "お気に入り解除に失敗しました"});
	}

	return {};
}

std::expected<std::vector<ws::models::Favorite>, ws::models::ApiError> FavoriteService::FetchFavorites() const
{
	auto result = m_httpClient.Get(ws::utils::kFavoritesPath, m_authService.GetToken());

	if (!result.has_value())
	{
		return std::unexpected(result.error());
	}

	const auto& response = result.value();

	if (response.statusCode != 200)
	{
		auto error = ws::utils::ParseApiError(response.body);
		if (error.has_value())
		{
			return std::unexpected(std::move(error.value()));
		}
		return std::unexpected(ws::models::ApiError{"FAVORITE_003", "お気に入り一覧の取得に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		return data.at("favorites").get<std::vector<ws::models::Favorite>>();
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

} // namespace ws::services
