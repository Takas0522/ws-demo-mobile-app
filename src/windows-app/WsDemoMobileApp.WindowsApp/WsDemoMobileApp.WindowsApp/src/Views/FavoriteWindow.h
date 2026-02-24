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
class FavoriteViewModel;
}

namespace ws::views
{

class FavoriteWindow
{
public:
	using NavigateToDetailCallback = std::function<void(int64_t)>;
	using NavigateBackCallback = std::function<void()>;

	explicit FavoriteWindow(ws::viewmodels::FavoriteViewModel& viewModel);
	~FavoriteWindow() = default;

	FavoriteWindow(const FavoriteWindow&) = delete;
	FavoriteWindow& operator=(const FavoriteWindow&) = delete;

	[[nodiscard]] bool Create(HINSTANCE hInstance, HWND parent, int width, int height);
	void Show(int nCmdShow);
	[[nodiscard]] HWND GetHandle() const;
	void RefreshList();

	void SetOnNavigateToDetail(NavigateToDetailCallback callback);
	void SetOnNavigateBack(NavigateBackCallback callback);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCreate();
	void OnCommand(WPARAM wParam);
	void OnNotify(LPARAM lParam);
	void UpdateListView();

	static constexpr const wchar_t* kClassName = L"FavoriteWindow";
	static constexpr UINT kWmFavoritesChanged = WM_APP + 1;
	static constexpr int kIdListView = 5001;
	static constexpr int kIdBackButton = 5002;

	static HMENU ToHMenu(int id) { return reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)); }

	ws::viewmodels::FavoriteViewModel& m_viewModel;
	HWND m_hwnd = nullptr;
	HWND m_listView = nullptr;

	NavigateToDetailCallback m_onNavigateToDetail;
	NavigateBackCallback m_onNavigateBack;
};

} // namespace ws::views
