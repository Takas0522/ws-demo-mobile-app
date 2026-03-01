#include "Services/ProductService.h"

#include "Database/IProductRepository.h"

namespace ws::services
{

ProductService::ProductService(ws::database::IProductRepository& productRepo)
	: m_productRepo(productRepo)
{
}

nlohmann::json ProductService::GetProducts(int page, int limit,
	const std::string& sortBy, const std::string& sortOrder)
{
	auto result = m_productRepo.FindAll(page, limit, sortBy, sortOrder);

	int totalPages = (result.totalCount > 0)
		? (result.totalCount + limit - 1) / limit
		: 0;

	nlohmann::json productsJson = nlohmann::json::array();
	for (const auto& product : result.products)
	{
		productsJson.push_back(product);
	}

	nlohmann::json data;
	data["products"] = std::move(productsJson);
	data["pagination"] = {
		{"currentPage", page},
		{"pageSize", limit},
		{"totalCount", result.totalCount},
		{"totalPages", totalPages}
	};

	return data;
}

nlohmann::json ProductService::SearchProducts(const std::string& keyword,
	int page, int limit)
{
	auto result = m_productRepo.SearchByKeyword(keyword, page, limit);

	int totalPages = (result.totalCount > 0)
		? (result.totalCount + limit - 1) / limit
		: 0;

	nlohmann::json productsJson = nlohmann::json::array();
	for (const auto& product : result.products)
	{
		productsJson.push_back(product);
	}

	nlohmann::json data;
	data["products"] = std::move(productsJson);
	data["pagination"] = {
		{"currentPage", page},
		{"pageSize", limit},
		{"totalCount", result.totalCount},
		{"totalPages", totalPages}
	};
	data["keyword"] = keyword;

	return data;
}

std::optional<ws::models::Product> ProductService::GetProductById(int64_t productId)
{
	return m_productRepo.FindById(productId);
}

} // namespace ws::services
