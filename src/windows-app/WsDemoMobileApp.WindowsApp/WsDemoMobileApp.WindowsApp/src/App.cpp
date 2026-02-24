#include "App.h"

#include "ViewModels/LoginViewModel.h"
#include "ViewModels/ProductListViewModel.h"
#include "ViewModels/ProductDetailViewModel.h"
#include "ViewModels/PurchaseViewModel.h"
#include "ViewModels/FavoriteViewModel.h"

#include "Views/LoginWindow.h"
#include "Views/ProductListWindow.h"
#include "Views/ProductDetailWindow.h"
#include "Views/PurchaseWindow.h"
#include "Views/FavoriteWindow.h"

#include "Utils/Constants.h"

namespace ws
{

App::App()
	: m_authService(m_httpClient, m_credentialManager)
	, m_productService(m_httpClient, m_authService)
	, m_purchaseService(m_httpClient, m_authService)
	, m_favoriteService(m_httpClient, m_authService)
{
}

App::~App()
{
	m_pollingService.Stop();
}

bool App::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	m_hInstance = hInstance;

	// Initialize feature flag service
	m_featureFlagService = std::make_unique<ws::services::FeatureFlagService>(m_httpClient);

	// Try to restore saved token
	auto savedToken = m_credentialManager.LoadToken();
	if (savedToken.has_value())
	{
		m_authService.SetToken(savedToken.value());
		(void)m_featureFlagService->FetchFeatureFlags(m_authService.GetToken());
	}

	// Create ViewModels
	m_loginViewModel = std::make_unique<ws::viewmodels::LoginViewModel>(
		m_authService, m_credentialManager);
	m_productListViewModel = std::make_unique<ws::viewmodels::ProductListViewModel>(
		m_productService);
	m_productDetailViewModel = std::make_unique<ws::viewmodels::ProductDetailViewModel>(
		m_productService, m_favoriteService);
	m_purchaseViewModel = std::make_unique<ws::viewmodels::PurchaseViewModel>(
		m_purchaseService);
	m_favoriteViewModel = std::make_unique<ws::viewmodels::FavoriteViewModel>(
		m_favoriteService);

	// Setup callbacks
	SetupCallbacks();

	// Show login or product list based on saved token
	if (m_authService.IsAuthenticated())
	{
		ShowProductListWindow();
	}
	else
	{
		ShowLoginWindow();
	}

	return true;
}

int App::Run()
{
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

void App::ShowLoginWindow()
{
	// Hide other windows
	if (m_productListWindow)
	{
		ShowWindow(m_productListWindow->GetHandle(), SW_HIDE);
	}

	if (!m_loginWindow)
	{
		m_loginWindow = std::make_unique<ws::views::LoginWindow>(*m_loginViewModel);
		(void)m_loginWindow->Create(m_hInstance,
			ws::utils::kDefaultWindowWidth,
			ws::utils::kDefaultWindowHeight);

		m_loginWindow->SetOnLoginSuccessNavigate([this]()
		{
			if (m_featureFlagService)
			{
				(void)m_featureFlagService->FetchFeatureFlags(m_authService.GetToken());
			}
			ShowProductListWindow();
		});
	}

	m_loginWindow->Show(SW_SHOW);
}

void App::ShowProductListWindow()
{
	// Hide login window
	if (m_loginWindow)
	{
		ShowWindow(m_loginWindow->GetHandle(), SW_HIDE);
	}
	if (m_productDetailWindow)
	{
		ShowWindow(m_productDetailWindow->GetHandle(), SW_HIDE);
	}
	if (m_favoriteWindow)
	{
		ShowWindow(m_favoriteWindow->GetHandle(), SW_HIDE);
	}

	if (!m_productListWindow)
	{
		m_productListWindow = std::make_unique<ws::views::ProductListWindow>(*m_productListViewModel);
		(void)m_productListWindow->Create(m_hInstance, nullptr,
			ws::utils::kDefaultWindowWidth,
			ws::utils::kDefaultWindowHeight);

		m_productListWindow->SetOnNavigateToDetail([this](int64_t productId)
		{
			ShowProductDetailWindow(productId);
		});

		m_productListWindow->SetOnNavigateToFavorites([this]()
		{
			ShowFavoriteWindow();
		});

		m_productListWindow->SetOnLogout([this]()
		{
			OnLogout();
		});
	}

	m_productListWindow->Show(SW_SHOW);

	// Control favorites tab visibility based on feature flag
	if (m_featureFlagService)
	{
		// kIdFavoritesTab = 2005 (defined in ProductListWindow)
		HWND favButton = GetDlgItem(m_productListWindow->GetHandle(), 2005);
		if (favButton)
		{
			ShowWindow(favButton, m_featureFlagService->IsFavoriteEnabled() ? SW_SHOW : SW_HIDE);
		}
	}

	m_productListWindow->RefreshList();

	// Start polling
	m_pollingService.Start(
		m_productListWindow->GetHandle(),
		[this]()
		{
			m_productListViewModel->FetchProducts();
		},
		ws::utils::kPollingIntervalMs);
}

void App::ShowProductDetailWindow(int64_t productId)
{
	if (m_productListWindow)
	{
		ShowWindow(m_productListWindow->GetHandle(), SW_HIDE);
	}

	if (!m_productDetailWindow)
	{
		m_productDetailWindow = std::make_unique<ws::views::ProductDetailWindow>(
			*m_productDetailViewModel);
		(void)m_productDetailWindow->Create(m_hInstance, nullptr,
			ws::utils::kDefaultWindowWidth,
			ws::utils::kDefaultWindowHeight);

		m_productDetailWindow->SetOnNavigateToPurchase([this]()
		{
			ShowPurchaseWindow();
		});

		m_productDetailWindow->SetOnNavigateBack([this]()
		{
			ShowProductListWindow();
		});
	}

	// Check if product is favorited
	m_productDetailViewModel->SetIsFavorite(
		m_favoriteViewModel->IsProductFavorited(productId));

	m_productDetailViewModel->FetchProductDetail(productId);
	m_productDetailWindow->Show(SW_SHOW);
}

void App::ShowPurchaseWindow()
{
	const auto& product = m_productDetailViewModel->GetProduct();
	if (!product.has_value())
	{
		return;
	}

	m_purchaseViewModel->SetProduct(product.value());

	if (!m_purchaseWindow)
	{
		m_purchaseWindow = std::make_unique<ws::views::PurchaseWindow>(*m_purchaseViewModel);
		(void)m_purchaseWindow->Create(m_hInstance, nullptr,
			ws::utils::kDefaultWindowWidth, 500);

		m_purchaseWindow->SetOnCancel([this]()
		{
			ShowWindow(m_purchaseWindow->GetHandle(), SW_HIDE);
		});

		m_purchaseWindow->SetOnPurchaseComplete([this]()
		{
			ShowProductListWindow();
		});
	}

	m_purchaseWindow->UpdateUI();
	m_purchaseWindow->Show(SW_SHOW);
}

void App::ShowFavoriteWindow()
{
	if (m_productListWindow)
	{
		ShowWindow(m_productListWindow->GetHandle(), SW_HIDE);
	}

	if (!m_favoriteWindow)
	{
		m_favoriteWindow = std::make_unique<ws::views::FavoriteWindow>(*m_favoriteViewModel);
		(void)m_favoriteWindow->Create(m_hInstance, nullptr,
			ws::utils::kDefaultWindowWidth,
			ws::utils::kDefaultWindowHeight);

		m_favoriteWindow->SetOnNavigateToDetail([this](int64_t productId)
		{
			ShowProductDetailWindow(productId);
		});

		m_favoriteWindow->SetOnNavigateBack([this]()
		{
			ShowProductListWindow();
		});
	}

	m_favoriteWindow->Show(SW_SHOW);
	m_favoriteWindow->RefreshList();
}

void App::OnLogout()
{
	m_pollingService.Stop();
	m_authService.Logout();

	// Hide all windows
	if (m_productListWindow)
	{
		ShowWindow(m_productListWindow->GetHandle(), SW_HIDE);
	}
	if (m_productDetailWindow)
	{
		ShowWindow(m_productDetailWindow->GetHandle(), SW_HIDE);
	}
	if (m_purchaseWindow)
	{
		ShowWindow(m_purchaseWindow->GetHandle(), SW_HIDE);
	}
	if (m_favoriteWindow)
	{
		ShowWindow(m_favoriteWindow->GetHandle(), SW_HIDE);
	}

	ShowLoginWindow();
}

void App::SetupCallbacks()
{
	// Login success -> navigate to product list (called from background thread)
	m_loginViewModel->SetOnLoginSuccess([this](const ws::models::User&)
	{
		if (m_loginWindow)
		{
			PostMessage(m_loginWindow->GetHandle(), WM_APP + 100, 0, 0);
		}
	});

	// Purchase success -> show completion message (called from background thread)
	m_purchaseViewModel->SetOnPurchaseSuccess([this](const ws::models::Purchase&)
	{
		if (m_purchaseWindow)
		{
			PostMessage(m_purchaseWindow->GetHandle(), WM_APP + 102, 0, 0);
		}
	});

	// Product detail loaded -> update UI (called from background thread)
	m_productDetailViewModel->SetOnProductChanged([this]()
	{
		if (m_productDetailWindow)
		{
			PostMessage(m_productDetailWindow->GetHandle(), WM_APP + 101, 0, 0);
		}
	});

	// Favorite changed -> update detail UI (called from background thread)
	m_productDetailViewModel->SetOnFavoriteChanged([this](bool)
	{
		if (m_productDetailWindow)
		{
			PostMessage(m_productDetailWindow->GetHandle(), WM_APP + 101, 0, 0);
		}
	});
}

} // namespace ws
