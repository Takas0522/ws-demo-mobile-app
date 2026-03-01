#pragma once

#include <string>
#include <vector>
#include <expected>
#include "Models/Purchase.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

namespace ws::services
{

class HttpClient;
class AuthService;

struct PurchaseListResponse
{
	std::vector<ws::models::Purchase> purchases;
	ws::models::Pagination pagination;
};

class PurchaseService
{
public:
	PurchaseService(const HttpClient& httpClient, const AuthService& authService);
	~PurchaseService() = default;

	PurchaseService(const PurchaseService&) = delete;
	PurchaseService& operator=(const PurchaseService&) = delete;

	[[nodiscard]] std::expected<ws::models::Purchase, ws::models::ApiError> CreatePurchase(
		int64_t productId,
		int quantity) const;

	[[nodiscard]] std::expected<PurchaseListResponse, ws::models::ApiError> FetchPurchases(
		int page = 1,
		int limit = 20) const;

private:
	const HttpClient& m_httpClient;
	const AuthService& m_authService;
};

} // namespace ws::services
