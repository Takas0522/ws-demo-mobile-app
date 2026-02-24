#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <functional>

namespace ws::viewmodels
{
class ProductDetailViewModel;
}

namespace ws::views
{

class ProductDetailWindow
{
public:
	using NavigateToPurchaseCallback = std::function<void()>;
	using NavigateBackCallback = std::function<void()>;

	explicit ProductDetailWindow(ws::viewmodels::ProductDetailViewModel& viewModel);
	~ProductDetailWindow() = default;

	ProductDetailWindow(const ProductDetailWindow&) = delete;
	ProductDetailWindow& operator=(const ProductDetailWindow&) = delete;

	[[nodiscard]] bool Create(HINSTANCE hInstance, HWND parent, int width, int height);
	void Show(int nCmdShow);
	[[nodiscard]] HWND GetHandle() const;
	void UpdateUI();

	void SetOnNavigateToPurchase(NavigateToPurchaseCallback callback);
	void SetOnNavigateBack(NavigateBackCallback callback);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCreate();
	void OnCommand(WPARAM wParam);

	static constexpr const wchar_t* kClassName = L"ProductDetailWindow";
	static constexpr int kIdBackButton = 3001;
	static constexpr int kIdFavoriteButton = 3002;
	static constexpr int kIdPurchaseButton = 3003;
	static constexpr int kIdProductName = 3004;
	static constexpr int kIdProductPrice = 3005;
	static constexpr int kIdProductDesc = 3006;

	static HMENU ToHMenu(int id) { return reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)); }

	ws::viewmodels::ProductDetailViewModel& m_viewModel;
	HWND m_hwnd = nullptr;
	HWND m_nameLabel = nullptr;
	HWND m_priceLabel = nullptr;
	HWND m_descLabel = nullptr;
	HWND m_favoriteButton = nullptr;

	NavigateToPurchaseCallback m_onNavigateToPurchase;
	NavigateBackCallback m_onNavigateBack;
};

} // namespace ws::views
