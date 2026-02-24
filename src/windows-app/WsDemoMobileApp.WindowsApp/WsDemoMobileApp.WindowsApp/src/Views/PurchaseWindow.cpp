#include "Views/PurchaseWindow.h"
#include "ViewModels/PurchaseViewModel.h"

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
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
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
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_COMMAND:
		OnCommand(wParam);
		return 0;

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

	int centerX = 60;
	int startY = 50;

	// Product info label
	CreateWindowExW(0, L"STATIC", L"購入確認",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX, startY, 300, 30,
		m_hwnd, nullptr, hInstance, nullptr);

	// Quantity controls
	CreateWindowExW(0, L"BUTTON", L"−",
		WS_CHILD | WS_VISIBLE,
		centerX, startY + 80, 50, 40,
		m_hwnd, ToHMenu(kIdDecrementButton), hInstance, nullptr);

	m_quantityLabel = CreateWindowExW(0, L"STATIC", L"数量: 100",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX + 60, startY + 80, 180, 40,
		m_hwnd, ToHMenu(kIdQuantityLabel), hInstance, nullptr);

	CreateWindowExW(0, L"BUTTON", L"+",
		WS_CHILD | WS_VISIBLE,
		centerX + 250, startY + 80, 50, 40,
		m_hwnd, ToHMenu(kIdIncrementButton), hInstance, nullptr);

	// Total
	m_totalLabel = CreateWindowExW(0, L"STATIC", L"合計: ¥0",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX, startY + 150, 300, 30,
		m_hwnd, ToHMenu(kIdTotalLabel), hInstance, nullptr);

	// Confirm button
	CreateWindowExW(0, L"BUTTON", L"購入確定",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		centerX, startY + 220, 300, 40,
		m_hwnd, ToHMenu(kIdConfirmButton), hInstance, nullptr);

	// Cancel button
	CreateWindowExW(0, L"BUTTON", L"キャンセル",
		WS_CHILD | WS_VISIBLE,
		centerX, startY + 280, 300, 40,
		m_hwnd, ToHMenu(kIdCancelButton), hInstance, nullptr);
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
