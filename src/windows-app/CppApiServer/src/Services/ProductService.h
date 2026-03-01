#pragma once

#include <string>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>
#include "Models/Product.h"

namespace ws::database { class IProductRepository; }

namespace ws::services
{

class ProductService
{
public:
	explicit ProductService(ws::database::IProductRepository& productRepo);

	ProductService(const ProductService&) = delete;
	ProductService& operator=(const ProductService&) = delete;

	[[nodiscard]] nlohmann::json GetProducts(int page, int limit,
		const std::string& sortBy, const std::string& sortOrder);
	[[nodiscard]] nlohmann::json SearchProducts(const std::string& keyword,
		int page, int limit);
	[[nodiscard]] std::optional<ws::models::Product> GetProductById(int64_t productId);

private:
	ws::database::IProductRepository& m_productRepo;
};

} // namespace ws::services
