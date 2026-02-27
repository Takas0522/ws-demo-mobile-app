#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct Purchase
{
	std::string purchaseId;  // UUID string
	int64_t userId = 0;
	int64_t productId = 0;
	std::string productName; // Joined from products table
	int quantity = 0;
	int unitPriceAtPurchase = 0;
	int totalAmount = 0;
	std::string purchasedAt;
};

inline void to_json(nlohmann::json& j, const Purchase& p)
{
	j = nlohmann::json{
		{"purchaseId", p.purchaseId},
		{"userId", p.userId},
		{"productId", p.productId},
		{"productName", p.productName},
		{"quantity", p.quantity},
		{"unitPrice", p.unitPriceAtPurchase},
		{"totalAmount", p.totalAmount},
		{"purchasedAt", p.purchasedAt}
	};
}

} // namespace ws::models
