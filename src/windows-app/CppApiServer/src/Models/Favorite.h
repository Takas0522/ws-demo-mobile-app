#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct Favorite
{
	int64_t favoriteId = 0;
	int64_t userId = 0;
	int64_t productId = 0;
	std::string productName;
	int unitPrice = 0;
	std::string imageUrl;
	std::string createdAt;
};

inline void to_json(nlohmann::json& j, const Favorite& f)
{
	j = nlohmann::json{
		{"favoriteId", f.favoriteId},
		{"productId", f.productId},
		{"productName", f.productName},
		{"unitPrice", f.unitPrice},
		{"imageUrl", f.imageUrl},
		{"createdAt", f.createdAt}
	};
}

} // namespace ws::models
