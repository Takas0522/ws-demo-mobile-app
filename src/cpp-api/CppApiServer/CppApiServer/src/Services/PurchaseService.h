#pragma once

#include <string>
#include <expected>
#include <vector>
#include "Models/Purchase.h"

namespace ws::database
{
class PurchaseRepository;
class ProductRepository;
}

namespace ws::services
{

class PurchaseService
{
public:
	PurchaseService(ws::database::PurchaseRepository& purchaseRepo,
		ws::database::ProductRepository& productRepo);

	PurchaseService(const PurchaseService&) = delete;
	PurchaseService& operator=(const PurchaseService&) = delete;

	[[nodiscard]] std::expected<ws::models::Purchase, std::string> CreatePurchase(
		int64_t userId, int64_t productId, int quantity);
	[[nodiscard]] std::vector<ws::models::Purchase> GetPurchasesByUserId(int64_t userId);

private:
	ws::database::PurchaseRepository& m_purchaseRepo;
	ws::database::ProductRepository& m_productRepo;
};

} // namespace ws::services
