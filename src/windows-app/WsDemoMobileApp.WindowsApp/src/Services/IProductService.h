#pragma once

#include <string>
#include <vector>
#include <expected>
#include "Models/Product.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

namespace ws::services
{

struct ProductListResponse
{
	std::vector<ws::models::Product> products;
	ws::models::Pagination pagination;
};

class IProductService
{
public:
	virtual ~IProductService() = default;

	[[nodiscard]] virtual std::expected<ProductListResponse, ws::models::ApiError> FetchProducts(
		int page = 1,
		int limit = 20,
		const std::string& sortBy = "createdAt",
		const std::string& sortOrder = "desc") const = 0;

	[[nodiscard]] virtual std::expected<ProductListResponse, ws::models::ApiError> SearchProducts(
		const std::string& keyword,
		int page = 1,
		int limit = 20) const = 0;

	[[nodiscard]] virtual std::expected<ws::models::Product, ws::models::ApiError> FetchProductDetail(
		int64_t productId) const = 0;

protected:
	IProductService() = default;
	IProductService(const IProductService&) = default;
	IProductService& operator=(const IProductService&) = default;
};

} // namespace ws::services
