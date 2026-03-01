#include "Views/PurchaseWindow.h"
#include "ViewModels/PurchaseViewModel.h"
#include "Utils/ModernTheme.h"

#include <string>

namespace ws::views
{

PurchaseWindow::PurchaseWindow(ws::viewmodels::PurchaseViewModel& viewModel)
	: m_viewModel(viewModel)
{
}

bool PurchaseWindow::Create(HINSTANCE hInstance, HWND parent, int width, int height)
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
		0, kClassName, L"購入確認",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		parent, nullptr, hInstance, this);

	return m_hwnd != nullptr;
}

void PurchaseWindow::Show(int nCmdShow)
{
	ShowWindow(m_hwnd, nCmdShow);
	UpdateWindow(m_hwnd);
}

HWND PurchaseWindow::GetHandle() const
{
	return m_hwnd;
}

void PurchaseWindow::UpdateUI()
{
	std::wstring quantityText = L"数量: " + std::to_wstring(m_viewModel.GetQuantity());
	SetWindowTextW(m_quantityLabel, quantityText.c_str());

	std::wstring totalText = L"合計: ¥" + std::to_wstring(m_viewModel.GetTotalAmount());
	SetWindowTextW(m_totalLabel, totalText.c_str());
}

void PurchaseWindow::SetOnPurchaseComplete(PurchaseCompleteCallback callback)
{
	m_onPurchaseComplete = std::move(callback);
}

void PurchaseWindow::SetOnCancel(CancelCallback callback)
{
	m_onCancel = std::move(callback);
}

LRESULT CALLBACK PurchaseWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PurchaseWindow* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		self = static_cast<PurchaseWindow*>(cs->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->m_hwnd = hwnd;
	}
	else
	{
		self = reinterpret_cast<PurchaseWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (self)
	{
		return self->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT PurchaseWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
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

	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = reinterpret_cast<HDC>(wParam);
		HWND hCtrl = reinterpret_cast<HWND>(lParam);
		SetBkMode(hdc, TRANSPARENT);
		if (hCtrl == m_totalLabel)
		{
			SetTextColor(hdc, ws::utils::colors::kAccent);
		}
		else
		{
			SetTextColor(hdc, ws::utils::colors::kTextPrimary);
		}
		return reinterpret_cast<LRESULT>(theme.GetBackgroundBrush());
	}

	case WM_APP + 102:
		MessageBoxW(m_hwnd,
			L"購入が完了しました。\nありがとうございます。",
			L"購入完了",
			MB_OK | MB_ICONINFORMATION);
		ShowWindow(m_hwnd, SW_HIDE);
		if (m_onPurchaseComplete)
		{
			m_onPurchaseComplete();
		}
		return 0;

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

void PurchaseWindow::OnCreate()
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(m_hwnd, GWLP_HINSTANCE));
	auto& theme = ws::utils::ModernTheme::Instance();

	RECT clientRc;
	GetClientRect(m_hwnd, &clientRc);
	int centerX = (clientRc.right - 320) / 2;
	if (centerX < 20) centerX = 20;
	int startY = 50;

	// Title label
	HWND titleLabel = CreateWindowExW(0, L"STATIC", L"購入確認",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX, startY, 320, 30,
		m_hwnd, nullptr, hInstance, nullptr);
	SendMessageW(titleLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontLarge()), TRUE);

	// Quantity controls
	CreateWindowExW(0, L"BUTTON", L"−",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		centerX, startY + 80, 60, 44,
		m_hwnd, ToHMenu(kIdDecrementButton), hInstance, nullptr);

	m_quantityLabel = CreateWindowExW(0, L"STATIC", L"数量: 1",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX + 70, startY + 80, 180, 44,
		m_hwnd, ToHMenu(kIdQuantityLabel), hInstance, nullptr);

	CreateWindowExW(0, L"BUTTON", L"+",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		centerX + 260, startY + 80, 60, 44,
		m_hwnd, ToHMenu(kIdIncrementButton), hInstance, nullptr);

	// Total
	m_totalLabel = CreateWindowExW(0, L"STATIC", L"合計: ¥0",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX, startY + 150, 320, 36,
		m_hwnd, ToHMenu(kIdTotalLabel), hInstance, nullptr);
	SendMessageW(m_totalLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontLarge()), TRUE);

	// Confirm button (accent)
	CreateWindowExW(0, L"BUTTON", L"購入確定",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		centerX, startY + 220, 320, 48,
		m_hwnd, ToHMenu(kIdConfirmButton), hInstance, nullptr);

	// Cancel button (secondary)
	CreateWindowExW(0, L"BUTTON", L"キャンセル",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		centerX, startY + 284, 320, 44,
		m_hwnd, ToHMenu(kIdCancelButton), hInstance, nullptr);

	// Apply font to all controls
	ws::utils::ModernTheme::ApplyFont(m_hwnd, theme.GetFont());
	SendMessageW(titleLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontLarge()), TRUE);
	SendMessageW(m_quantityLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontBold()), TRUE);
	SendMessageW(m_totalLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontLarge()), TRUE);

	// Register ViewModel error callback to show error dialog
	m_viewModel.SetOnError([hwnd = m_hwnd](const ws::models::ApiError& error)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, nullptr, 0);
		auto* msg = new std::wstring(len - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, msg->data(), len);
		PostMessage(hwnd, kWmShowError, reinterpret_cast<WPARAM>(msg), 0);
	});
}

void PurchaseWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);

	RECT clientRc;
	GetClientRect(m_hwnd, &clientRc);
	int centerX = (clientRc.right - 320) / 2;
	if (centerX < 20) centerX = 20;

	// Card surface behind quantity area
	RECT cardRc = { centerX - 10, 40, centerX + 330, 260 };
	ws::utils::ModernTheme::FillRoundRect(hdc, cardRc, ws::utils::colors::kSurface, 10);

	// Divider between total and buttons
	ws::utils::ModernTheme::DrawDivider(hdc, centerX, 258, 320);

	EndPaint(m_hwnd, &ps);
}

void PurchaseWindow::OnDrawItem(DRAWITEMSTRUCT* dis)
{
	switch (dis->CtlID)
	{
	case kIdDecrementButton:
	case kIdIncrementButton:
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kSecondary,
			ws::utils::colors::kSecondaryHover,
			ws::utils::colors::kTextPrimary, 8);
		break;
	case kIdConfirmButton:
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kAccent,
			ws::utils::colors::kAccentHover,
			ws::utils::colors::kTextOnAccent, 8);
		break;
	case kIdCancelButton:
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kSecondary,
			ws::utils::colors::kSecondaryHover,
			ws::utils::colors::kTextPrimary, 6);
		break;
	}
}

void PurchaseWindow::OnCommand(WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case kIdIncrementButton:
		m_viewModel.IncrementQuantity();
		UpdateUI();
		break;

	case kIdDecrementButton:
		m_viewModel.DecrementQuantity();
		UpdateUI();
		break;

	case kIdConfirmButton:
		m_viewModel.ConfirmPurchase();
		break;

	case kIdCancelButton:
		if (m_onCancel)
		{
			m_onCancel();
		}
		break;
	}
}

} // namespace ws::views
