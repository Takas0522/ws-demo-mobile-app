#include "Routes/PurchaseRoutes.h"

#include <nlohmann/json.hpp>

#include "Services/PurchaseService.h"
#include "Middleware/AuthMiddleware.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws::routes
{

PurchaseRoutes::PurchaseRoutes(ws::services::PurchaseService& purchaseService)
	: m_purchaseService(purchaseService)
{
}

void PurchaseRoutes::Register(httplib::Server& server)
{
	server.Post(ws::utils::kPurchasesPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleCreatePurchase(req, res);
		});

	server.Get(ws::utils::kPurchasesPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleGetPurchases(req, res);
		});
}

void PurchaseRoutes::HandleCreatePurchase(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	// Parse request body
	nlohmann::json body;
	try
	{
		body = nlohmann::json::parse(req.body);
	}
	catch (const nlohmann::json::exception&)
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "Invalid JSON in request body"),
			"application/json; charset=utf-8");
		return;
	}

	// Extract fields
	if (!body.contains("productId") || !body["productId"].is_number_integer())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "productId is required and must be an integer"),
			"application/json; charset=utf-8");
		return;
	}
	if (!body.contains("quantity") || !body["quantity"].is_number_integer())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "quantity is required and must be an integer"),
			"application/json; charset=utf-8");
		return;
	}

	int64_t productId = body["productId"].get<int64_t>();
	int quantity = body["quantity"].get<int>();
	int64_t userId = payload->userId;

	auto result = m_purchaseService.CreatePurchase(userId, productId, quantity);

	if (!result.has_value())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", result.error()),
			"application/json; charset=utf-8");
		return;
	}

	nlohmann::json purchaseJson = result.value();

	res.status = 201;
	res.set_content(
		ws::utils::MakeSuccessResponse(purchaseJson),
		"application/json; charset=utf-8");
}

void PurchaseRoutes::HandleGetPurchases(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	int64_t userId = payload->userId;
	auto purchases = m_purchaseService.GetPurchasesByUserId(userId);

	nlohmann::json purchasesJson = nlohmann::json::array();
	for (const auto& purchase : purchases)
	{
		purchasesJson.push_back(purchase);
	}

	nlohmann::json data = {
		{"purchases", purchasesJson}
	};

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

} // namespace ws::routes
