#pragma once

#include <string>

namespace ws::utils
{

// BFF API settings
constexpr const char* kBffBaseUrl = "http://localhost:8081";
constexpr const char* kApiBasePath = "/api/mobile";
constexpr const wchar_t* kBffHost = L"localhost";
constexpr int kBffPort = 8081;

// API endpoints
constexpr const char* kLoginPath = "/api/mobile/login";
constexpr const char* kProductsPath = "/api/mobile/products";
constexpr const char* kProductsSearchPath = "/api/mobile/products/search";
constexpr const char* kPurchasesPath = "/api/mobile/purchases";
constexpr const char* kFavoritesPath = "/api/mobile/favorites";
constexpr const char* kFeatureFlagsPath = "/api/mobile/feature-flags";

// Application settings
constexpr const wchar_t* kAppTitle = L"WS Demo Mobile App";
constexpr const wchar_t* kAppClassName = L"WsDemoMobileApp";
constexpr int kDefaultWindowWidth = 420;
constexpr int kDefaultWindowHeight = 720;

// Credential Manager
constexpr const wchar_t* kCredentialTarget = L"WsDemoMobileApp/JWT";

// Polling interval (milliseconds)
constexpr int kPollingIntervalMs = 30000;

// HTTP settings
constexpr int kHttpTimeoutMs = 10000;
constexpr int kMaxRetryCount = 3;

// Purchase constraints
constexpr int kMinQuantity = 100;
constexpr int kMaxQuantity = 9900;
constexpr int kQuantityStep = 100;

// Pagination
constexpr int kDefaultPageSize = 20;

// Custom window messages for async HTTP callbacks
constexpr unsigned int WM_HTTP_RESPONSE = 0x8001;  // WM_APP + 1
constexpr unsigned int WM_HTTP_ERROR = 0x8002;      // WM_APP + 2

} // namespace ws::utils
