#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <functional>

namespace ws::viewmodels
{
class PurchaseViewModel;
}

namespace ws::views
{

class PurchaseWindow
{
public:
	using PurchaseCompleteCallback = std::function<void()>;
	using CancelCallback = std::function<void()>;

	explicit PurchaseWindow(ws::viewmodels::PurchaseViewModel& viewModel);
	~PurchaseWindow() = default;

	PurchaseWindow(const PurchaseWindow&) = delete;
	PurchaseWindow& operator=(const PurchaseWindow&) = delete;

	[[nodiscard]] bool Create(HINSTANCE hInstance, HWND parent, int width, int height);
	void Show(int nCmdShow);
	[[nodiscard]] HWND GetHandle() const;
	void UpdateUI();

	void SetOnPurchaseComplete(PurchaseCompleteCallback callback);
	void SetOnCancel(CancelCallback callback);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCreate();
	void OnCommand(WPARAM wParam);

	static constexpr const wchar_t* kClassName = L"PurchaseWindow";
	static constexpr UINT kWmShowError = WM_APP + 200;
	static constexpr int kIdQuantityLabel = 4001;
	static constexpr int kIdIncrementButton = 4002;
	static constexpr int kIdDecrementButton = 4003;
	static constexpr int kIdTotalLabel = 4004;
	static constexpr int kIdConfirmButton = 4005;
	static constexpr int kIdCancelButton = 4006;

	static HMENU ToHMenu(int id) { return reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)); }

	ws::viewmodels::PurchaseViewModel& m_viewModel;
	HWND m_hwnd = nullptr;
	HWND m_quantityLabel = nullptr;
	HWND m_totalLabel = nullptr;

	PurchaseCompleteCallback m_onPurchaseComplete;
	CancelCallback m_onCancel;
};

} // namespace ws::views
