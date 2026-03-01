#include "Services/ProductService.h"
#include "Services/IHttpClient.h"
#include "Services/IAuthService.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

#include <nlohmann/json.hpp>
#include <sstream>

namespace ws::services
{

ProductService::ProductService(const IHttpClient& httpClient, const IAuthService& authService)
	: m_httpClient(httpClient)
	, m_authService(authService)
{
}

std::expected<ProductListResponse, ws::models::ApiError> ProductService::FetchProducts(
	int page,
	int limit,
	const std::string& sortBy,
	const std::string& sortOrder) const
{
	std::ostringstream path;
	path << ws::utils::kProductsPath
		<< "?page=" << page
		<< "&limit=" << limit
		<< "&sortBy=" << sortBy
		<< "&sortOrder=" << sortOrder;

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
		return std::unexpected(ws::models::ApiError{"PRODUCT_001", "商品一覧の取得に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		ProductListResponse listResponse;
		listResponse.products = data.at("products").get<std::vector<ws::models::Product>>();

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

std::expected<ProductListResponse, ws::models::ApiError> ProductService::SearchProducts(
	const std::string& keyword,
	int page,
	int limit) const
{
	std::ostringstream path;
	path << ws::utils::kProductsSearchPath
		<< "?keyword=" << keyword
		<< "&page=" << page
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
		return std::unexpected(ws::models::ApiError{"PRODUCT_002", "商品検索に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		ProductListResponse listResponse;
		listResponse.products = data.at("products").get<std::vector<ws::models::Product>>();

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

std::expected<ws::models::Product, ws::models::ApiError> ProductService::FetchProductDetail(
	int64_t productId) const
{
	std::string path = std::string(ws::utils::kProductsPath) + "/" + std::to_string(productId);

	auto result = m_httpClient.Get(path, m_authService.GetToken());

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
		return std::unexpected(ws::models::ApiError{"PRODUCT_003", "商品詳細の取得に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		return data.at("product").get<ws::models::Product>();
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

} // namespace ws::services
