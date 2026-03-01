#pragma once

#include <string>
#include <memory>
#include <httplib.h>

#include "Database/DatabaseManager.h"
#include "Database/UserRepository.h"
#include "Database/ProductRepository.h"
#include "Database/PurchaseRepository.h"
#include "Database/FavoriteRepository.h"
#include "Database/FeatureFlagRepository.h"
#include "Services/AuthService.h"
#include "Services/ProductService.h"
#include "Services/PurchaseService.h"
#include "Services/FavoriteService.h"
#include "Services/FeatureFlagService.h"
#include "Routes/AuthRoutes.h"
#include "Routes/ProductRoutes.h"
#include "Routes/PurchaseRoutes.h"
#include "Routes/FavoriteRoutes.h"
#include "Routes/FeatureFlagRoutes.h"

namespace ws
{

class Server
{
public:
	Server();
	~Server() = default;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	[[nodiscard]] bool Initialize(const std::string& dbPath);
	void Run(int port);
	void Stop();

private:
	void RegisterRoutes();
	void SetupCors();
	void SetupLogger();

	httplib::Server m_httpServer;

	// Database
	ws::database::DatabaseManager m_dbManager;
	std::unique_ptr<ws::database::UserRepository> m_userRepo;
	std::unique_ptr<ws::database::ProductRepository> m_productRepo;
	std::unique_ptr<ws::database::PurchaseRepository> m_purchaseRepo;
	std::unique_ptr<ws::database::FavoriteRepository> m_favoriteRepo;
	std::unique_ptr<ws::database::FeatureFlagRepository> m_flagRepo;

	// Services
	std::unique_ptr<ws::services::AuthService> m_authService;
	std::unique_ptr<ws::services::ProductService> m_productService;
	std::unique_ptr<ws::services::PurchaseService> m_purchaseService;
	std::unique_ptr<ws::services::FavoriteService> m_favoriteService;
	std::unique_ptr<ws::services::FeatureFlagService> m_flagService;

	// Routes
	std::unique_ptr<ws::routes::AuthRoutes> m_authRoutes;
	std::unique_ptr<ws::routes::ProductRoutes> m_productRoutes;
	std::unique_ptr<ws::routes::PurchaseRoutes> m_purchaseRoutes;
	std::unique_ptr<ws::routes::FavoriteRoutes> m_favoriteRoutes;
	std::unique_ptr<ws::routes::FeatureFlagRoutes> m_flagRoutes;
};

} // namespace ws
