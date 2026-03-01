#include "Server.h"

#include <iostream>

#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws
{

Server::Server() = default;

bool Server::Initialize(const std::string& dbPath)
{
	// Initialize database
	auto dbResult = m_dbManager.Initialize(dbPath);
	if (!dbResult.has_value())
	{
		std::cerr << "[ERROR] Database initialization failed: " << dbResult.error() << std::endl;
		return false;
	}
	std::cout << "[INFO] Database initialized: " << dbPath << std::endl;

	// Create repositories
	m_userRepo = std::make_unique<ws::database::UserRepository>(m_dbManager);
	m_productRepo = std::make_unique<ws::database::ProductRepository>(m_dbManager);
	m_purchaseRepo = std::make_unique<ws::database::PurchaseRepository>(m_dbManager);
	m_favoriteRepo = std::make_unique<ws::database::FavoriteRepository>(m_dbManager);
	m_flagRepo = std::make_unique<ws::database::FeatureFlagRepository>(m_dbManager);
	m_priceHistoryRepo = std::make_unique<ws::database::PriceHistoryRepository>(m_dbManager);

	// Create services (inject dependencies)
	m_authService = std::make_unique<ws::services::AuthService>(*m_userRepo);
	m_productService = std::make_unique<ws::services::ProductService>(*m_productRepo);
	m_purchaseService = std::make_unique<ws::services::PurchaseService>(*m_purchaseRepo, *m_productRepo);
	m_favoriteService = std::make_unique<ws::services::FavoriteService>(*m_favoriteRepo);
	m_flagService = std::make_unique<ws::services::FeatureFlagService>(*m_flagRepo);
	m_priceHistoryService = std::make_unique<ws::services::PriceHistoryService>(*m_priceHistoryRepo);

	// Create route handlers
	m_authRoutes = std::make_unique<ws::routes::AuthRoutes>(*m_authService);
	m_productRoutes = std::make_unique<ws::routes::ProductRoutes>(*m_productService, *m_priceHistoryService);
	m_purchaseRoutes = std::make_unique<ws::routes::PurchaseRoutes>(*m_purchaseService);
	m_favoriteRoutes = std::make_unique<ws::routes::FavoriteRoutes>(*m_favoriteService);
	m_flagRoutes = std::make_unique<ws::routes::FeatureFlagRoutes>(*m_flagService);

	// Setup CORS, logger, and register routes
	SetupCors();
	SetupLogger();
	RegisterRoutes();

	std::cout << "[INFO] Server initialized successfully" << std::endl;
	return true;
}

void Server::Run(int port)
{
	std::cout << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << "  C++ API Server (cpp-httplib)" << std::endl;
	std::cout << "  Listening on http://0.0.0.0:" << port << std::endl;
	std::cout << "  API Base: " << ws::utils::kApiBasePath << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << std::endl;

	m_httpServer.listen("0.0.0.0", port);
}

void Server::Stop()
{
	m_httpServer.stop();
}

void Server::RegisterRoutes()
{
	m_authRoutes->Register(m_httpServer);
	m_productRoutes->Register(m_httpServer);
	m_purchaseRoutes->Register(m_httpServer);
	m_favoriteRoutes->Register(m_httpServer);
	m_flagRoutes->Register(m_httpServer);

	// Global error handler for unmatched routes
	m_httpServer.set_error_handler(
		[](const httplib::Request& /*req*/, httplib::Response& res)
		{
			std::string code;
			std::string message;

			switch (res.status)
			{
			case 404:
				code = "NOT_FOUND";
				message = "The requested resource was not found";
				break;
			case 405:
				code = "METHOD_NOT_ALLOWED";
				message = "Method not allowed";
				break;
			default:
				code = "INTERNAL_ERROR";
				message = "An internal server error occurred";
				break;
			}

			res.set_content(
				ws::utils::MakeErrorResponse(code, message),
				"application/json; charset=utf-8");
		});
}

void Server::SetupCors()
{
	// Pre-routing handler: add CORS headers to all responses
	m_httpServer.set_pre_routing_handler(
		[](const httplib::Request& req, httplib::Response& res) -> httplib::Server::HandlerResponse
		{
			res.set_header("Access-Control-Allow-Origin", "*");
			res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			res.set_header("Access-Control-Allow-Headers",
				"Content-Type, Authorization, Accept");
			res.set_header("Access-Control-Max-Age", "86400");

			// Handle OPTIONS preflight requests
			if (req.method == "OPTIONS")
			{
				res.status = 200;
				res.set_content("", "text/plain");
				return httplib::Server::HandlerResponse::Handled;
			}

			return httplib::Server::HandlerResponse::Unhandled;
		});
}

void Server::SetupLogger()
{
	m_httpServer.set_logger(
		[](const httplib::Request& req, const httplib::Response& res)
		{
			std::cout << "[" << req.method << "] " << req.path
				<< " -> " << res.status << std::endl;
		});
}

} // namespace ws
