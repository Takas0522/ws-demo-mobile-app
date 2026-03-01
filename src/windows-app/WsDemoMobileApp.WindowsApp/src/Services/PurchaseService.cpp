#include "Services/PurchaseService.h"
#include "Services/IHttpClient.h"
#include "Services/IAuthService.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

#include <nlohmann/json.hpp>
#include <sstream>

namespace ws::services
{

PurchaseService::PurchaseService(const IHttpClient& httpClient, const IAuthService& authService)
	: m_httpClient(httpClient)
	, m_authService(authService)
{
}

std::expected<ws::models::Purchase, ws::models::ApiError> PurchaseService::CreatePurchase(
	int64_t productId,
	int quantity) const
{
	nlohmann::json requestBody = {
		{"productId", productId},
		{"quantity", quantity}
	};

	auto result = m_httpClient.Post(
		ws::utils::kPurchasesPath,
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
		return std::unexpected(ws::models::ApiError{"PURCHASE_001", "購入に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		return data.at("purchase").get<ws::models::Purchase>();
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

std::expected<PurchaseListResponse, ws::models::ApiError> PurchaseService::FetchPurchases(
	int page,
	int limit) const
{
	std::ostringstream path;
	path << ws::utils::kPurchasesPath
		<< "?page=" << page
		<< "&limit=" << limit;

	auto result = m_httpClient.Get(path.str(), m_authService.GetToken());

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
		return std::unexpected(ws::models::ApiError{"PURCHASE_002", "購入履歴の取得に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		PurchaseListResponse listResponse;
		listResponse.purchases = data.at("purchases").get<std::vector<ws::models::Purchase>>();

		if (data.contains("pagination"))
		{
			listResponse.pagination = data.at("pagination").get<ws::models::Pagination>();
		}

		return listResponse;
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

} // namespace ws::services
