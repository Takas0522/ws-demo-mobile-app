#pragma once

#include <vector>
#include <expected>
#include <cstdint>
#include "Models/Purchase.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

namespace ws::services
{

struct PurchaseListResponse
{
	std::vector<ws::models::Purchase> purchases;
	ws::models::Pagination pagination;
};

class IPurchaseService
{
public:
	virtual ~IPurchaseService() = default;

	[[nodiscard]] virtual std::expected<ws::models::Purchase, ws::models::ApiError> CreatePurchase(
		int64_t productId,
		int quantity) const = 0;

	[[nodiscard]] virtual std::expected<PurchaseListResponse, ws::models::ApiError> FetchPurchases(
		int page = 1,
		int limit = 20) const = 0;

protected:
	IPurchaseService() = default;
	IPurchaseService(const IPurchaseService&) = default;
	IPurchaseService& operator=(const IPurchaseService&) = default;
};

} // namespace ws::services
