#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct Product
{
	int64_t productId = 0;
	std::string productName;
	int unitPrice = 0;
	std::optional<std::string> description;
	std::optional<std::string> imageUrl;
	std::string createdAt;
	std::string updatedAt;
};

inline void to_json(nlohmann::json& j, const Product& p)
{
	j = nlohmann::json{
		{"productId", p.productId},
		{"productName", p.productName},
		{"unitPrice", p.unitPrice},
		{"createdAt", p.createdAt},
		{"updatedAt", p.updatedAt}
	};
	j["description"] = p.description.has_value() ? nlohmann::json(p.description.value()) : nlohmann::json(nullptr);
	j["imageUrl"] = p.imageUrl.has_value() ? nlohmann::json(p.imageUrl.value()) : nlohmann::json(nullptr);
}

} // namespace ws::models
