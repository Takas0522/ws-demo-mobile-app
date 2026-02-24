#pragma once

#include <string>
#include <vector>
#include <expected>
#include "Models/Product.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

namespace ws::services
{

class HttpClient;
class AuthService;

struct ProductListResponse
{
	std::vector<ws::models::Product> products;
	ws::models::Pagination pagination;
};

class ProductService
{
public:
	ProductService(const HttpClient& httpClient, const AuthService& authService);
	~ProductService() = default;

	ProductService(const ProductService&) = delete;
	ProductService& operator=(const ProductService&) = delete;

	[[nodiscard]] std::expected<ProductListResponse, ws::models::ApiError> FetchProducts(
		int page = 1,
		int limit = 20,
		const std::string& sortBy = "createdAt",
		const std::string& sortOrder = "desc") const;

	[[nodiscard]] std::expected<ProductListResponse, ws::models::ApiError> SearchProducts(
		const std::string& keyword,
		int page = 1,
		int limit = 20) const;

	[[nodiscard]] std::expected<ws::models::Product, ws::models::ApiError> FetchProductDetail(
		int64_t productId) const;

private:
	const HttpClient& m_httpClient;
	const AuthService& m_authService;
};

} // namespace ws::services
