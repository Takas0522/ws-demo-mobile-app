#include "Views/ProductListWindow.h"
#include "ViewModels/ProductListViewModel.h"
#include "Utils/Constants.h"

#include <string>
#include <functional>

#pragma comment(lib, "comctl32.lib")

namespace ws::views
{

ProductListWindow::ProductListWindow(ws::viewmodels::ProductListViewModel& viewModel)
	: m_viewModel(viewModel)
{
}

bool ProductListWindow::Create(HINSTANCE hInstance, HWND parent, int width, int height)
{
	INITCOMMONCONTROLSEX icc = {};
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icc);

	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kClassName;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClassExW(&wc);

	m_hwnd = CreateWindowExW(
		0, kClassName, L"商品一覧",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		parent, nullptr, hInstance, this);

	return m_hwnd != nullptr;
}

void ProductListWindow::Show(int nCmdShow)
{
	ShowWindow(m_hwnd, nCmdShow);
	UpdateWindow(m_hwnd);
}

HWND ProductListWindow::GetHandle() const
{
	return m_hwnd;
}

void ProductListWindow::RefreshList()
{
	m_viewModel.FetchProducts();
}

void ProductListWindow::SetOnNavigateToDetail(NavigateToDetailCallback callback)
{
	m_onNavigateToDetail = std::move(callback);
}

void ProductListWindow::SetOnNavigateToFavorites(NavigateToFavoritesCallback callback)
{
	m_onNavigateToFavorites = std::move(callback);
}

void ProductListWindow::SetOnLogout(LogoutCallback callback)
{
	m_onLogout = std::move(callback);
}

LRESULT CALLBACK ProductListWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ProductListWindow* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		self = static_cast<ProductListWindow*>(cs->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->m_hwnd = hwnd;
	}
	else
	{
		self = reinterpret_cast<ProductListWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (self)
	{
		return self->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT ProductListWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_COMMAND:
		OnCommand(wParam);
		return 0;

	case WM_NOTIFY:
		OnNotify(lParam);
		return 0;

	case kWmProductsChanged:
		UpdateListView();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(m_hwnd, msg, wParam, lParam);
	}
}

void ProductListWindow::OnCreate()
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(m_hwnd, GWLP_HINSTANCE));

	// Search bar
	m_searchEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 10, 280, 28,
		m_hwnd, ToHMenu(kIdSearchEdit), hInstance, nullptr);

	CreateWindowExW(0, L"BUTTON", L"検索",
		WS_CHILD | WS_VISIBLE,
		300, 10, 60, 28,
		m_hwnd, ToHMenu(kIdSearchButton), hInstance, nullptr);

	// Favorites tab button
	CreateWindowExW(0, L"BUTTON", L"お気に入り",
		WS_CHILD | WS_VISIBLE,
		370, 10, 80, 28,
		m_hwnd, ToHMenu(kIdFavoritesTab), hInstance, nullptr);

	// ListView
	m_listView = CreateWindowExW(0, WC_LISTVIEWW, L"",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
		10, 50, 440, 550,
		m_hwnd, ToHMenu(kIdListView), hInstance, nullptr);

	ListView_SetExtendedListViewStyle(m_listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Columns
	LVCOLUMNW col = {};
	col.mask = LVCF_TEXT | LVCF_WIDTH;

	col.pszText = const_cast<LPWSTR>(L"商品名");
	col.cx = 250;
	ListView_InsertColumn(m_listView, 0, &col);

	col.pszText = const_cast<LPWSTR>(L"価格");
	col.cx = 150;
	ListView_InsertColumn(m_listView, 1, &col);

	// Logout button
	CreateWindowExW(0, L"BUTTON", L"ログアウト",
		WS_CHILD | WS_VISIBLE,
		10, 610, 100, 30,
		m_hwnd, ToHMenu(kIdLogoutButton), hInstance, nullptr);

	// Register ViewModel callback to update UI when products change
	m_viewModel.SetOnProductsChanged([hwnd = m_hwnd]()
	{
		PostMessage(hwnd, kWmProductsChanged, 0, 0);
	});
}

void ProductListWindow::OnCommand(WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case kIdSearchButton:
		OnSearch();
		break;

	case kIdFavoritesTab:
		if (m_onNavigateToFavorites)
		{
			m_onNavigateToFavorites();
		}
		break;

	case kIdLogoutButton:
		if (m_onLogout)
		{
			m_onLogout();
		}
		break;
	}
}

void ProductListWindow::OnNotify(LPARAM lParam)
{
	auto* nmhdr = reinterpret_cast<NMHDR*>(lParam);

	if (nmhdr->idFrom == kIdListView && nmhdr->code == NM_DBLCLK)
	{
		auto* nmItem = reinterpret_cast<NMITEMACTIVATE*>(lParam);
		if (nmItem->iItem >= 0)
		{
			const auto& products = m_viewModel.GetProducts();
			if (nmItem->iItem < static_cast<int>(products.size()))
			{
				if (m_onNavigateToDetail)
				{
					m_onNavigateToDetail(products[nmItem->iItem].productId);
				}
			}
		}
	}
}

void ProductListWindow::UpdateListView()
{
	ListView_DeleteAllItems(m_listView);

	const auto& products = m_viewModel.GetProducts();

	for (int i = 0; i < static_cast<int>(products.size()); ++i)
	{
		const auto& product = products[i];

		int nameLen = MultiByteToWideChar(CP_UTF8, 0, product.productName.c_str(), -1, nullptr, 0);
		std::wstring wideName(nameLen - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, product.productName.c_str(), -1, wideName.data(), nameLen);

		std::wstring price = L"¥" + std::to_wstring(product.unitPrice);

		LVITEMW item = {};
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = const_cast<LPWSTR>(wideName.c_str());
		ListView_InsertItem(m_listView, &item);

		ListView_SetItemText(m_listView, i, 1, const_cast<LPWSTR>(price.c_str()));
	}
}

void ProductListWindow::OnSearch()
{
	wchar_t buf[256] = {};
	GetWindowTextW(m_searchEdit, buf, 256);

	int len = WideCharToMultiByte(CP_UTF8, 0, buf, -1, nullptr, 0, nullptr, nullptr);
	std::string keyword(len - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, buf, -1, keyword.data(), len, nullptr, nullptr);

	if (keyword.empty())
	{
		m_viewModel.FetchProducts();
	}
	else
	{
		m_viewModel.SearchProducts(keyword);
	}
}

} // namespace ws::views
