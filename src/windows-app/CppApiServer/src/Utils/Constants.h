#pragma once
#include <string>

namespace ws::utils
{

// Server settings
constexpr int kDefaultPort = 8081;
constexpr const char* kDefaultDbPath = "./data/mobile_app.db";

// API base path
constexpr const char* kApiBasePath = "/api/mobile";

// JWT settings
constexpr const char* kJwtSecret = "ws-demo-mobile-app-jwt-secret-key-2025";
constexpr int kJwtExpirationSeconds = 86400;  // 24 hours

// API endpoints
constexpr const char* kLoginPath = "/api/mobile/login";
constexpr const char* kLogoutPath = "/api/mobile/auth/logout";
constexpr const char* kProductsPath = "/api/mobile/products";
constexpr const char* kProductsSearchPath = "/api/mobile/products/search";
constexpr const char* kPurchasesPath = "/api/mobile/purchases";
constexpr const char* kFavoritesPath = "/api/mobile/favorites";
constexpr const char* kFeatureFlagsPath = "/api/mobile/feature-flags";

// Price history settings
// Default start date: 5 years (1825 days) ago — matches data retention period
constexpr const char* kPriceHistoryDefaultStartDate = "0001-01-01 00:00:00";
constexpr const char* kPriceHistoryDefaultEndDate   = "9999-12-31 23:59:59";

// Purchase constraints
constexpr int kMinQuantity = 100;
constexpr int kMaxQuantity = 9900;
constexpr int kQuantityStep = 100;

// Pagination defaults
constexpr int kDefaultPageSize = 20;
constexpr int kMaxPageSize = 100;

} // namespace ws::utils
