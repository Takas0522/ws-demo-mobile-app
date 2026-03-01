#pragma once

#include <string>
#include <optional>
#include <vector>
#include "Models/Product.h"

namespace ws::database
{

struct ProductListResult
{
	std::vector<ws::models::Product> products;
	int totalCount = 0;
};

class IProductRepository
{
public:
	virtual ~IProductRepository() = default;

	[[nodiscard]] virtual ProductListResult FindAll(int page, int limit,
		const std::string& sortBy = "created_at", const std::string& sortOrder = "DESC") = 0;
	[[nodiscard]] virtual ProductListResult SearchByKeyword(const std::string& keyword, int page, int limit) = 0;
	[[nodiscard]] virtual std::optional<ws::models::Product> FindById(int64_t productId) = 0;

protected:
	IProductRepository() = default;
	IProductRepository(const IProductRepository&) = default;
	IProductRepository& operator=(const IProductRepository&) = default;
};

} // namespace ws::database
