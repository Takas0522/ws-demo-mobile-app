#include "Views/FavoriteWindow.h"
#include "ViewModels/FavoriteViewModel.h"
#include "Utils/Constants.h"
#include "Utils/ModernTheme.h"

#include <string>

namespace ws::views
{

FavoriteWindow::FavoriteWindow(ws::viewmodels::FavoriteViewModel& viewModel)
	: m_viewModel(viewModel)
{
}

bool FavoriteWindow::Create(HINSTANCE hInstance, HWND parent, int width, int height)
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
		0, kClassName, L"お気に入り",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		parent, nullptr, hInstance, this);

	return m_hwnd != nullptr;
}

void FavoriteWindow::Show(int nCmdShow)
{
	ShowWindow(m_hwnd, nCmdShow);
	UpdateWindow(m_hwnd);
}

HWND FavoriteWindow::GetHandle() const
{
	return m_hwnd;
}

void FavoriteWindow::RefreshList()
{
	SetWindowTextW(m_statusLabel, L"読み込み中...");
	ShowWindow(m_statusLabel, SW_SHOW);
	ShowWindow(m_listView, SW_HIDE);
	m_viewModel.FetchFavorites();
}

void FavoriteWindow::SetOnNavigateToDetail(NavigateToDetailCallback callback)
{
	m_onNavigateToDetail = std::move(callback);
}

void FavoriteWindow::SetOnNavigateBack(NavigateBackCallback callback)
{
	m_onNavigateBack = std::move(callback);
}

LRESULT CALLBACK FavoriteWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FavoriteWindow* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		self = static_cast<FavoriteWindow*>(cs->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->m_hwnd = hwnd;
	}
	else
	{
		self = reinterpret_cast<FavoriteWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (self)
	{
		return self->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT FavoriteWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
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

	case WM_NOTIFY:
		OnNotify(lParam);
		return 0;

	case kWmFavoritesChanged:
		UpdateListView();
		return 0;

	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = reinterpret_cast<HDC>(wParam);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, ws::utils::colors::kTextSecondary);
		return reinterpret_cast<LRESULT>(theme.GetBackgroundBrush());
	}

	case kWmShowError:
	{
		auto* errorMsg = reinterpret_cast<std::wstring*>(wParam);
		ShowWindow(m_statusLabel, SW_HIDE);
		ShowWindow(m_listView, SW_SHOW);
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

void FavoriteWindow::OnCreate()
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(m_hwnd, GWLP_HINSTANCE));
	auto& theme = ws::utils::ModernTheme::Instance();

	// Back button (owner-drawn, secondary)
	CreateWindowExW(0, L"BUTTON", L"← 商品一覧",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		16, 12, 130, 34,
		m_hwnd, ToHMenu(kIdBackButton), hInstance, nullptr);

	// ListView
	m_listView = CreateWindowExW(0, WC_LISTVIEWW, L"",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
		16, 56, 430, 590,
		m_hwnd, ToHMenu(kIdListView), hInstance, nullptr);

	ListView_SetExtendedListViewStyle(m_listView, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	ListView_SetBkColor(m_listView, ws::utils::colors::kSurface);
	ListView_SetTextBkColor(m_listView, ws::utils::colors::kSurface);
	ListView_SetTextColor(m_listView, ws::utils::colors::kTextPrimary);

	LVCOLUMNW col = {};
	col.mask = LVCF_TEXT | LVCF_WIDTH;

	col.pszText = const_cast<LPWSTR>(L"♥ 商品名");
	col.cx = 270;
	ListView_InsertColumn(m_listView, 0, &col);

	col.pszText = const_cast<LPWSTR>(L"価格");
	col.cx = 140;
	ListView_InsertColumn(m_listView, 1, &col);

	// Status label (loading / empty state)
	m_statusLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | SS_CENTER,
		16, 300, 430, 30,
		m_hwnd, ToHMenu(kIdStatusLabel), hInstance, nullptr);

	// Apply font to all controls
	ws::utils::ModernTheme::ApplyFont(m_hwnd, theme.GetFont());
	SendMessageW(m_listView, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFont()), TRUE);

	// Register ViewModel callback to update UI when favorites change
	m_viewModel.SetOnFavoritesChanged([hwnd = m_hwnd]()
	{
		PostMessage(hwnd, kWmFavoritesChanged, 0, 0);
	});

	// Register ViewModel error callback to show error dialog
	m_viewModel.SetOnError([hwnd = m_hwnd](const ws::models::ApiError& error)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, nullptr, 0);
		auto* msg = new std::wstring(len - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, msg->data(), len);
		PostMessage(hwnd, kWmShowError, reinterpret_cast<WPARAM>(msg), 0);
	});
}

void FavoriteWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);

	// Header divider
	ws::utils::ModernTheme::DrawDivider(hdc, 16, 52, 430);

	EndPaint(m_hwnd, &ps);
}

void FavoriteWindow::OnDrawItem(DRAWITEMSTRUCT* dis)
{
	if (dis->CtlID == kIdBackButton)
	{
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kSecondary,
			ws::utils::colors::kSecondaryHover,
			ws::utils::colors::kTextPrimary, 6);
	}
}

void FavoriteWindow::OnCommand(WPARAM wParam)
{
	if (LOWORD(wParam) == kIdBackButton)
	{
		if (m_onNavigateBack)
		{
			m_onNavigateBack();
		}
	}
}

void FavoriteWindow::OnNotify(LPARAM lParam)
{
	auto* nmhdr = reinterpret_cast<NMHDR*>(lParam);

	if (nmhdr->idFrom == kIdListView && nmhdr->code == NM_DBLCLK)
	{
		auto* nmItem = reinterpret_cast<NMITEMACTIVATE*>(lParam);
		if (nmItem->iItem >= 0)
		{
			const auto& favorites = m_viewModel.GetFavorites();
			if (nmItem->iItem < static_cast<int>(favorites.size()))
			{
				auto productId = favorites[nmItem->iItem].productId;
				if (favorites[nmItem->iItem].product.has_value())
				{
					productId = favorites[nmItem->iItem].product->productId;
				}
				if (m_onNavigateToDetail)
				{
					m_onNavigateToDetail(productId);
				}
			}
		}
	}
}

void FavoriteWindow::UpdateListView()
{
	ListView_DeleteAllItems(m_listView);

	const auto& favorites = m_viewModel.GetFavorites();

	if (favorites.empty())
	{
		SetWindowTextW(m_statusLabel, L"お気に入りはありません");
		ShowWindow(m_statusLabel, SW_SHOW);
		ShowWindow(m_listView, SW_HIDE);
		return;
	}

	ShowWindow(m_statusLabel, SW_HIDE);
	ShowWindow(m_listView, SW_SHOW);

	for (int i = 0; i < static_cast<int>(favorites.size()); ++i)
	{
		const auto& fav = favorites[i];

		std::string name;
		int price = 0;

		if (fav.product.has_value())
		{
			name = fav.product->productName;
			price = fav.product->unitPrice;
		}

		int nameLen = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
		std::wstring wideName(nameLen - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wideName.data(), nameLen);

		std::wstring priceStr = L"¥" + std::to_wstring(price);

		LVITEMW item = {};
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = const_cast<LPWSTR>(wideName.c_str());
		ListView_InsertItem(m_listView, &item);

		ListView_SetItemText(m_listView, i, 1, const_cast<LPWSTR>(priceStr.c_str()));
	}
}

} // namespace ws::views
