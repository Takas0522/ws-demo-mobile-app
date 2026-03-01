#include "Routes/ProductRoutes.h"

#include <string>
#include <nlohmann/json.hpp>

#include "Services/ProductService.h"
#include "Services/PriceHistoryService.h"
#include "Middleware/AuthMiddleware.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws::routes
{

ProductRoutes::ProductRoutes(ws::services::ProductService& productService,
	ws::services::PriceHistoryService& priceHistoryService)
	: m_productService(productService)
	, m_priceHistoryService(priceHistoryService)
{
}

void ProductRoutes::Register(httplib::Server& server)
{
	// Search must be registered before the regex pattern to avoid conflict
	server.Get(ws::utils::kProductsSearchPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleSearchProducts(req, res);
		});

	// Price history must be registered before the product detail pattern
	server.Get(R"(/api/mobile/products/(\d+)/price-history)",
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleGetPriceHistory(req, res);
		});

	server.Get(R"(/api/mobile/products/(\d+))",
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleGetProductDetail(req, res);
		});

	server.Get(ws::utils::kProductsPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleGetProducts(req, res);
		});
}

void ProductRoutes::HandleGetProducts(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	// Extract query params with defaults
	int page = 1;
	int limit = ws::utils::kDefaultPageSize;
	std::string sortBy = "created_at";
	std::string sortOrder = "DESC";

	if (req.has_param("page"))
	{
		try { page = std::stoi(req.get_param_value("page")); }
		catch (...) { page = 1; }
		if (page < 1) page = 1;
	}
	if (req.has_param("limit"))
	{
		try { limit = std::stoi(req.get_param_value("limit")); }
		catch (...) { limit = ws::utils::kDefaultPageSize; }
		if (limit < 1) limit = 1;
		if (limit > ws::utils::kMaxPageSize) limit = ws::utils::kMaxPageSize;
	}
	if (req.has_param("sortBy"))
	{
		sortBy = req.get_param_value("sortBy");
	}
	if (req.has_param("sortOrder"))
	{
		sortOrder = req.get_param_value("sortOrder");
	}

	auto data = m_productService.GetProducts(page, limit, sortBy, sortOrder);

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

void ProductRoutes::HandleSearchProducts(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	std::string keyword;
	if (req.has_param("keyword"))
	{
		keyword = req.get_param_value("keyword");
	}

	int page = 1;
	int limit = ws::utils::kDefaultPageSize;

	if (req.has_param("page"))
	{
		try { page = std::stoi(req.get_param_value("page")); }
		catch (...) { page = 1; }
		if (page < 1) page = 1;
	}
	if (req.has_param("limit"))
	{
		try { limit = std::stoi(req.get_param_value("limit")); }
		catch (...) { limit = ws::utils::kDefaultPageSize; }
		if (limit < 1) limit = 1;
		if (limit > ws::utils::kMaxPageSize) limit = ws::utils::kMaxPageSize;
	}

	auto data = m_productService.SearchProducts(keyword, page, limit);

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

void ProductRoutes::HandleGetProductDetail(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	// Extract product ID from regex match
	int64_t productId = 0;
	try
	{
		productId = std::stoll(req.matches[1].str());
	}
	catch (...)
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "Invalid product ID"),
			"application/json; charset=utf-8");
		return;
	}

	auto product = m_productService.GetProductById(productId);
	if (!product.has_value())
	{
		res.status = 404;
		res.set_content(
			ws::utils::MakeErrorResponse("NOT_FOUND", "Product not found"),
			"application/json; charset=utf-8");
		return;
	}

	nlohmann::json productJson = product.value();

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(productJson),
		"application/json; charset=utf-8");
}

void ProductRoutes::HandleGetPriceHistory(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	// Extract product ID from regex match
	int64_t productId = 0;
	try
	{
		productId = std::stoll(req.matches[1].str());
	}
	catch (...)
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("VAL_001", "入力値が不正です"),
			"application/json; charset=utf-8");
		return;
	}

	// Check product existence
	if (!m_priceHistoryService.ProductExists(productId))
	{
		res.status = 404;
		res.set_content(
			ws::utils::MakeErrorResponse("PRODUCT_001", "商品が見つかりません"),
			"application/json; charset=utf-8");
		return;
	}

	// Parse query parameters
	int page = 1;
	int limit = ws::utils::kDefaultPageSize;
	std::string startDate = ws::utils::kPriceHistoryDefaultStartDate;
	std::string endDate   = ws::utils::kPriceHistoryDefaultEndDate;

	if (req.has_param("page"))
	{
		try { page = std::stoi(req.get_param_value("page")); }
		catch (...) { page = 1; }
		if (page < 1) page = 1;
	}
	if (req.has_param("limit"))
	{
		try { limit = std::stoi(req.get_param_value("limit")); }
		catch (...) { limit = ws::utils::kDefaultPageSize; }
		if (limit < 1) limit = 1;
		if (limit > ws::utils::kMaxPageSize) limit = ws::utils::kMaxPageSize;
	}
	if (req.has_param("startDate"))
	{
		const std::string& sd = req.get_param_value("startDate");
		// Validate ISO 8601 date format (YYYY-MM-DD): must be 10 chars
		if (sd.size() != 10 || sd[4] != '-' || sd[7] != '-')
		{
			res.status = 400;
			res.set_content(
				ws::utils::MakeErrorResponse("VAL_001", "入力値が不正です",
					"startDate は YYYY-MM-DD 形式で指定してください"),
				"application/json; charset=utf-8");
			return;
		}
		startDate = sd + " 00:00:00";
	}
	if (req.has_param("endDate"))
	{
		const std::string& ed = req.get_param_value("endDate");
		if (ed.size() != 10 || ed[4] != '-' || ed[7] != '-')
		{
			res.status = 400;
			res.set_content(
				ws::utils::MakeErrorResponse("VAL_001", "入力値が不正です",
					"endDate は YYYY-MM-DD 形式で指定してください"),
				"application/json; charset=utf-8");
			return;
		}
		endDate = ed + " 23:59:59";
	}

	auto data = m_priceHistoryService.GetPriceHistory(productId, startDate, endDate, page, limit);

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

} // namespace ws::routes
