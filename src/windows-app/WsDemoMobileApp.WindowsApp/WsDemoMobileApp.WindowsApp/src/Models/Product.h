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

	Product() = default;
};

inline void from_json(const nlohmann::json& j, Product& p)
{
	j.at("productId").get_to(p.productId);
	j.at("productName").get_to(p.productName);
	j.at("unitPrice").get_to(p.unitPrice);

	if (j.contains("description") && !j.at("description").is_null())
	{
		p.description = j.at("description").get<std::string>();
	}
	if (j.contains("imageUrl") && !j.at("imageUrl").is_null())
	{
		p.imageUrl = j.at("imageUrl").get<std::string>();
	}
	if (j.contains("createdAt"))
	{
		j.at("createdAt").get_to(p.createdAt);
	}
	if (j.contains("updatedAt"))
	{
		j.at("updatedAt").get_to(p.updatedAt);
	}
}

inline void to_json(nlohmann::json& j, const Product& p)
{
	j = nlohmann::json{
		{"productId", p.productId},
		{"productName", p.productName},
		{"unitPrice", p.unitPrice},
		{"createdAt", p.createdAt},
		{"updatedAt", p.updatedAt}
	};

	if (p.description.has_value())
	{
		j["description"] = p.description.value();
	}
	else
	{
		j["description"] = nullptr;
	}

	if (p.imageUrl.has_value())
	{
		j["imageUrl"] = p.imageUrl.value();
	}
	else
	{
		j["imageUrl"] = nullptr;
	}
}

} // namespace ws::models
