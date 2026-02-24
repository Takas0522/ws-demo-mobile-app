#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <memory>

#include "Services/HttpClient.h"
#include "Services/AuthService.h"
#include "Services/ProductService.h"
#include "Services/PurchaseService.h"
#include "Services/FavoriteService.h"
#include "Services/StatePollingService.h"
#include "Utils/CredentialManager.h"

namespace ws::viewmodels
{
class LoginViewModel;
class ProductListViewModel;
class ProductDetailViewModel;
class PurchaseViewModel;
class FavoriteViewModel;
}

namespace ws::views
{
class LoginWindow;
class ProductListWindow;
class ProductDetailWindow;
class PurchaseWindow;
class FavoriteWindow;
}

namespace ws
{

class App
{
public:
	App();
	~App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	[[nodiscard]] bool Initialize(HINSTANCE hInstance, int nCmdShow);
	int Run();

private:
	void ShowLoginWindow();
	void ShowProductListWindow();
	void ShowProductDetailWindow(int64_t productId);
	void ShowPurchaseWindow();
	void ShowFavoriteWindow();
	void OnLogout();
	void SetupCallbacks();

	HINSTANCE m_hInstance = nullptr;

	// Services
	ws::services::HttpClient m_httpClient;
	ws::services::AuthService m_authService;
	ws::services::ProductService m_productService;
	ws::services::PurchaseService m_purchaseService;
	ws::services::FavoriteService m_favoriteService;
	ws::services::StatePollingService m_pollingService;
	ws::utils::CredentialManager m_credentialManager;

	// ViewModels
	std::unique_ptr<ws::viewmodels::LoginViewModel> m_loginViewModel;
	std::unique_ptr<ws::viewmodels::ProductListViewModel> m_productListViewModel;
	std::unique_ptr<ws::viewmodels::ProductDetailViewModel> m_productDetailViewModel;
	std::unique_ptr<ws::viewmodels::PurchaseViewModel> m_purchaseViewModel;
	std::unique_ptr<ws::viewmodels::FavoriteViewModel> m_favoriteViewModel;

	// Views
	std::unique_ptr<ws::views::LoginWindow> m_loginWindow;
	std::unique_ptr<ws::views::ProductListWindow> m_productListWindow;
	std::unique_ptr<ws::views::ProductDetailWindow> m_productDetailWindow;
	std::unique_ptr<ws::views::PurchaseWindow> m_purchaseWindow;
	std::unique_ptr<ws::views::FavoriteWindow> m_favoriteWindow;
};

} // namespace ws
