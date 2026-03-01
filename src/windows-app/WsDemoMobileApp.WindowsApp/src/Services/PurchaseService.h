#pragma once

#include "Services/IPurchaseService.h"

namespace ws::services
{

class IHttpClient;
class IAuthService;

class PurchaseService : public IPurchaseService
{
public:
	PurchaseService(const IHttpClient& httpClient, const IAuthService& authService);
	~PurchaseService() override = default;

	PurchaseService(const PurchaseService&) = delete;
	PurchaseService& operator=(const PurchaseService&) = delete;

	[[nodiscard]] std::expected<ws::models::Purchase, ws::models::ApiError> CreatePurchase(
		int64_t productId,
		int quantity) const override;

	[[nodiscard]] std::expected<PurchaseListResponse, ws::models::ApiError> FetchPurchases(
		int page = 1,
		int limit = 20) const override;

private:
	const IHttpClient& m_httpClient;
	const IAuthService& m_authService;
};

} // namespace ws::services
