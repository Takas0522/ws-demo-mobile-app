#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "Product.h"

namespace ws::models
{

struct Favorite
{
	int64_t favoriteId = 0;
	int64_t userId = 0;
	int64_t productId = 0;
	std::optional<Product> product;
	std::string createdAt;

	Favorite() = default;
};

inline void from_json(const nlohmann::json& j, Favorite& f)
{
	j.at("favoriteId").get_to(f.favoriteId);

	if (j.contains("userId"))
	{
		j.at("userId").get_to(f.userId);
	}
	if (j.contains("productId"))
	{
		j.at("productId").get_to(f.productId);
	}
	if (j.contains("product") && !j.at("product").is_null())
	{
		f.product = j.at("product").get<Product>();
	}
	if (j.contains("createdAt"))
	{
		j.at("createdAt").get_to(f.createdAt);
	}
}

inline void to_json(nlohmann::json& j, const Favorite& f)
{
	j = nlohmann::json{
		{"favoriteId", f.favoriteId},
		{"userId", f.userId},
		{"productId", f.productId},
		{"createdAt", f.createdAt}
	};

	if (f.product.has_value())
	{
		j["product"] = f.product.value();
	}
	else
	{
		j["product"] = nullptr;
	}
}

} // namespace ws::models
