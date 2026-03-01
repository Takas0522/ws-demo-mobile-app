#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <CommCtrl.h>

#include <cstdint>
#include <functional>

namespace ws::viewmodels
{
class ProductListViewModel;
}

namespace ws::views
{

class ProductListWindow
{
public:
	using NavigateToDetailCallback = std::function<void(int64_t)>;
	using NavigateToFavoritesCallback = std::function<void()>;
	using LogoutCallback = std::function<void()>;

	explicit ProductListWindow(ws::viewmodels::ProductListViewModel& viewModel);
	~ProductListWindow() = default;

	ProductListWindow(const ProductListWindow&) = delete;
	ProductListWindow& operator=(const ProductListWindow&) = delete;

	[[nodiscard]] bool Create(HINSTANCE hInstance, HWND parent, int width, int height);
	void Show(int nCmdShow);
	[[nodiscard]] HWND GetHandle() const;
	void RefreshList();

	void SetOnNavigateToDetail(NavigateToDetailCallback callback);
	void SetOnNavigateToFavorites(NavigateToFavoritesCallback callback);
	void SetOnLogout(LogoutCallback callback);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCreate();
	void OnCommand(WPARAM wParam);
	void OnNotify(LPARAM lParam);
	void OnPaint();
	void OnDrawItem(DRAWITEMSTRUCT* dis);
	void UpdateListView();
	void OnSearch();

	static constexpr const wchar_t* kClassName = L"ProductListWindow";
	static constexpr UINT kWmProductsChanged = WM_APP + 1;
	static constexpr UINT kWmShowError = WM_APP + 200;
	static constexpr int kIdSearchEdit = 2001;
	static constexpr int kIdSearchButton = 2002;
	static constexpr int kIdListView = 2003;
	static constexpr int kIdLogoutButton = 2004;
	static constexpr int kIdFavoritesTab = 2005;
	static constexpr int kIdStatusLabel = 2006;

	static HMENU ToHMenu(int id) { return reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)); }

	ws::viewmodels::ProductListViewModel& m_viewModel;
	HWND m_hwnd = nullptr;
	HWND m_searchEdit = nullptr;
	HWND m_listView = nullptr;
	HWND m_statusLabel = nullptr;

	NavigateToDetailCallback m_onNavigateToDetail;
	NavigateToFavoritesCallback m_onNavigateToFavorites;
	LogoutCallback m_onLogout;
};

} // namespace ws::views
