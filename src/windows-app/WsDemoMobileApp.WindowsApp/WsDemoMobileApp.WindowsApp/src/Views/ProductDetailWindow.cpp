#include "Views/ProductDetailWindow.h"
#include "ViewModels/ProductDetailViewModel.h"
#include "Utils/Constants.h"

#include <string>

namespace ws::views
{

ProductDetailWindow::ProductDetailWindow(ws::viewmodels::ProductDetailViewModel& viewModel)
	: m_viewModel(viewModel)
{
}

bool ProductDetailWindow::Create(HINSTANCE hInstance, HWND parent, int width, int height)
{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kClassName;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClassExW(&wc);

	m_hwnd = CreateWindowExW(
		0, kClassName, L"商品詳細",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		parent, nullptr, hInstance, this);

	return m_hwnd != nullptr;
}

void ProductDetailWindow::Show(int nCmdShow)
{
	ShowWindow(m_hwnd, nCmdShow);
	UpdateWindow(m_hwnd);
}

HWND ProductDetailWindow::GetHandle() const
{
	return m_hwnd;
}

void ProductDetailWindow::UpdateUI()
{
	const auto& product = m_viewModel.GetProduct();
	if (!product.has_value())
	{
		return;
	}

	int nameLen = MultiByteToWideChar(CP_UTF8, 0, product->productName.c_str(), -1, nullptr, 0);
	std::wstring wideName(nameLen - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, product->productName.c_str(), -1, wideName.data(), nameLen);
	SetWindowTextW(m_nameLabel, wideName.c_str());

	std::wstring price = L"¥" + std::to_wstring(product->unitPrice);
	SetWindowTextW(m_priceLabel, price.c_str());

	if (product->description.has_value())
	{
		int descLen = MultiByteToWideChar(CP_UTF8, 0, product->description->c_str(), -1, nullptr, 0);
		std::wstring wideDesc(descLen - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, product->description->c_str(), -1, wideDesc.data(), descLen);
		SetWindowTextW(m_descLabel, wideDesc.c_str());
	}

	SetWindowTextW(m_favoriteButton, m_viewModel.IsFavorite() ? L"♥ お気に入り解除" : L"♡ お気に入り");
}

void ProductDetailWindow::SetOnNavigateToPurchase(NavigateToPurchaseCallback callback)
{
	m_onNavigateToPurchase = std::move(callback);
}

void ProductDetailWindow::SetOnNavigateBack(NavigateBackCallback callback)
{
	m_onNavigateBack = std::move(callback);
}

LRESULT CALLBACK ProductDetailWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ProductDetailWindow* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		self = static_cast<ProductDetailWindow*>(cs->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->m_hwnd = hwnd;
	}
	else
	{
		self = reinterpret_cast<ProductDetailWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (self)
	{
		return self->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT ProductDetailWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_COMMAND:
		OnCommand(wParam);
		return 0;

	case WM_APP + 101:
		UpdateUI();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(m_hwnd, msg, wParam, lParam);
	}
}

void ProductDetailWindow::OnCreate()
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(m_hwnd, GWLP_HINSTANCE));

	// Back button
	CreateWindowExW(0, L"BUTTON", L"← 戻る",
		WS_CHILD | WS_VISIBLE,
		10, 10, 80, 30,
		m_hwnd, ToHMenu(kIdBackButton), hInstance, nullptr);

	// Favorite button
	m_favoriteButton = CreateWindowExW(0, L"BUTTON", L"♡ お気に入り",
		WS_CHILD | WS_VISIBLE,
		300, 10, 140, 30,
		m_hwnd, ToHMenu(kIdFavoriteButton), hInstance, nullptr);

	// Image placeholder
	CreateWindowExW(0, L"STATIC", L"[商品画像]",
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SUNKEN,
		10, 50, 430, 200,
		m_hwnd, nullptr, hInstance, nullptr);

	// Product name
	m_nameLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		10, 260, 430, 30,
		m_hwnd, ToHMenu(kIdProductName), hInstance, nullptr);

	// Price
	m_priceLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		10, 300, 430, 30,
		m_hwnd, ToHMenu(kIdProductPrice), hInstance, nullptr);

	// Description
	m_descLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		10, 340, 430, 200,
		m_hwnd, ToHMenu(kIdProductDesc), hInstance, nullptr);

	// Purchase button (bottom)
	CreateWindowExW(0, L"BUTTON", L"購入する",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		10, 600, 430, 50,
		m_hwnd, ToHMenu(kIdPurchaseButton), hInstance, nullptr);
}

void ProductDetailWindow::OnCommand(WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case kIdBackButton:
		if (m_onNavigateBack)
		{
			m_onNavigateBack();
		}
		break;

	case kIdFavoriteButton:
		m_viewModel.ToggleFavorite();
		break;

	case kIdPurchaseButton:
		if (m_onNavigateToPurchase)
		{
			m_onNavigateToPurchase();
		}
		break;
	}
}

} // namespace ws::views
