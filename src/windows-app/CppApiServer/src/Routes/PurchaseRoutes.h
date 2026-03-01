#pragma once

#include <httplib.h>

namespace ws::services { class PurchaseService; }

namespace ws::routes
{

class PurchaseRoutes
{
public:
	explicit PurchaseRoutes(ws::services::PurchaseService& purchaseService);

	PurchaseRoutes(const PurchaseRoutes&) = delete;
	PurchaseRoutes& operator=(const PurchaseRoutes&) = delete;

	void Register(httplib::Server& server);

private:
	void HandleCreatePurchase(const httplib::Request& req, httplib::Response& res);
	void HandleGetPurchases(const httplib::Request& req, httplib::Response& res);

	ws::services::PurchaseService& m_purchaseService;
};

} // namespace ws::routes
