#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct Purchase
{
	std::string purchaseId;
	int64_t userId = 0;
	int64_t productId = 0;
	std::string productName;
	int quantity = 0;
	int unitPriceAtPurchase = 0;
	int totalAmount = 0;
	std::string purchasedAt;

	Purchase() = default;
};

inline void from_json(const nlohmann::json& j, Purchase& p)
{
	j.at("purchaseId").get_to(p.purchaseId);
	j.at("userId").get_to(p.userId);
	j.at("productId").get_to(p.productId);
	j.at("productName").get_to(p.productName);
	j.at("quantity").get_to(p.quantity);
	j.at("unitPriceAtPurchase").get_to(p.unitPriceAtPurchase);
	j.at("totalAmount").get_to(p.totalAmount);
	j.at("purchasedAt").get_to(p.purchasedAt);
}

inline void to_json(nlohmann::json& j, const Purchase& p)
{
	j = nlohmann::json{
		{"purchaseId", p.purchaseId},
		{"userId", p.userId},
		{"productId", p.productId},
		{"productName", p.productName},
		{"quantity", p.quantity},
		{"unitPriceAtPurchase", p.unitPriceAtPurchase},
		{"totalAmount", p.totalAmount},
		{"purchasedAt", p.purchasedAt}
	};
}

} // namespace ws::models
