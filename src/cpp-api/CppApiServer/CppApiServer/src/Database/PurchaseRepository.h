#pragma once

#include <string>
#include <optional>
#include <vector>
#include <expected>
#include "Models/Purchase.h"

namespace ws::database
{

class DatabaseManager;

class PurchaseRepository
{
public:
	explicit PurchaseRepository(DatabaseManager& dbManager);

	[[nodiscard]] std::expected<ws::models::Purchase, std::string> Create(
		const std::string& purchaseId, int64_t userId, int64_t productId,
		int quantity, int unitPrice, int totalAmount);
	[[nodiscard]] std::vector<ws::models::Purchase> FindByUserId(int64_t userId);

private:
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
