#include "Views/ProductDetailWindow.h"
#include "ViewModels/ProductDetailViewModel.h"
#include "Utils/Constants.h"
#include "Utils/ModernTheme.h"

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
	wc.hbrBackground = ws::utils::ModernTheme::Instance().GetBackgroundBrush();
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
	auto& theme = ws::utils::ModernTheme::Instance();

	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_DRAWITEM:
		OnDrawItem(reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
		return TRUE;

	case WM_COMMAND:
		OnCommand(wParam);
		return 0;

	case WM_APP + 101:
		UpdateUI();
		InvalidateRect(m_hwnd, nullptr, TRUE);
		return 0;

	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = reinterpret_cast<HDC>(wParam);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, ws::utils::colors::kTextPrimary);
		return reinterpret_cast<LRESULT>(theme.GetBackgroundBrush());
	}

	case kWmShowError:
	{
		auto* errorMsg = reinterpret_cast<std::wstring*>(wParam);
		MessageBoxW(m_hwnd, errorMsg->c_str(), L"エラー", MB_OK | MB_ICONERROR);
		delete errorMsg;
		return 0;
	}

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
	auto& theme = ws::utils::ModernTheme::Instance();

	// Back button (owner-drawn, secondary)
	CreateWindowExW(0, L"BUTTON", L"← 戻る",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		16, 12, 90, 34,
		m_hwnd, ToHMenu(kIdBackButton), hInstance, nullptr);

	// Favorite button (owner-drawn, secondary)
	m_favoriteButton = CreateWindowExW(0, L"BUTTON", L"♡ お気に入り",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		300, 12, 140, 34,
		m_hwnd, ToHMenu(kIdFavoriteButton), hInstance, nullptr);

	// Image placeholder (styled card)
	CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		16, 56, 430, 200,
		m_hwnd, nullptr, hInstance, nullptr);

	// Product name
	m_nameLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		16, 270, 430, 30,
		m_hwnd, ToHMenu(kIdProductName), hInstance, nullptr);

	// Price
	m_priceLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		16, 308, 430, 30,
		m_hwnd, ToHMenu(kIdProductPrice), hInstance, nullptr);

	// Description
	m_descLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		16, 350, 430, 180,
		m_hwnd, ToHMenu(kIdProductDesc), hInstance, nullptr);

	// Purchase button (owner-drawn, accent)
	CreateWindowExW(0, L"BUTTON", L"購入する",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		16, 600, 430, 48,
		m_hwnd, ToHMenu(kIdPurchaseButton), hInstance, nullptr);

	// Apply font to all controls
	ws::utils::ModernTheme::ApplyFont(m_hwnd, theme.GetFont());
	SendMessageW(m_nameLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontLarge()), TRUE);
	SendMessageW(m_priceLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontBold()), TRUE);

	// Register ViewModel error callback to show error dialog
	m_viewModel.SetOnError([hwnd = m_hwnd](const ws::models::ApiError& error)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, nullptr, 0);
		auto* msg = new std::wstring(len - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, msg->data(), len);
		PostMessage(hwnd, kWmShowError, reinterpret_cast<WPARAM>(msg), 0);
	});
}

void ProductDetailWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);
	auto& theme = ws::utils::ModernTheme::Instance();

	// Header divider
	ws::utils::ModernTheme::DrawDivider(hdc, 16, 52, 430);

	// Image placeholder card
	RECT imgRc = { 16, 56, 446, 256 };
	ws::utils::ModernTheme::FillRoundRect(hdc, imgRc, ws::utils::colors::kSurface, 10);

	// Placeholder icon
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, ws::utils::colors::kTextSecondary);
	SelectObject(hdc, theme.GetFontLarge());
	DrawTextW(hdc, L"\U0001F4F7 商品画像", -1, &imgRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Divider before purchase button
	ws::utils::ModernTheme::DrawDivider(hdc, 16, 586, 430);

	EndPaint(m_hwnd, &ps);
}

void ProductDetailWindow::OnDrawItem(DRAWITEMSTRUCT* dis)
{
	switch (dis->CtlID)
	{
	case kIdBackButton:
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kSecondary,
			ws::utils::colors::kSecondaryHover,
			ws::utils::colors::kTextPrimary, 6);
		break;
	case kIdFavoriteButton:
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kSecondary,
			ws::utils::colors::kSecondaryHover,
			ws::utils::colors::kTextPrimary, 6);
		break;
	case kIdPurchaseButton:
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kAccent,
			ws::utils::colors::kAccentHover,
			ws::utils::colors::kTextOnAccent, 8);
		break;
	}
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
