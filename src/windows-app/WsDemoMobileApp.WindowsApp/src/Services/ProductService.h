#pragma once

#include "Services/IProductService.h"

namespace ws::services
{

class IHttpClient;
class IAuthService;

class ProductService : public IProductService
{
public:
	ProductService(const IHttpClient& httpClient, const IAuthService& authService);
	~ProductService() override = default;

	ProductService(const ProductService&) = delete;
	ProductService& operator=(const ProductService&) = delete;

	[[nodiscard]] std::expected<ProductListResponse, ws::models::ApiError> FetchProducts(
		int page = 1,
		int limit = 20,
		const std::string& sortBy = "createdAt",
		const std::string& sortOrder = "desc") const override;

	[[nodiscard]] std::expected<ProductListResponse, ws::models::ApiError> SearchProducts(
		const std::string& keyword,
		int page = 1,
		int limit = 20) const override;

	[[nodiscard]] std::expected<ws::models::Product, ws::models::ApiError> FetchProductDetail(
		int64_t productId) const override;

private:
	const IHttpClient& m_httpClient;
	const IAuthService& m_authService;
};

} // namespace ws::services
