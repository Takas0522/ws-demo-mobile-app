#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <memory>
#include <string>
#include <functional>

namespace ws::viewmodels
{
class LoginViewModel;
}

namespace ws::views
{

class LoginWindow
{
public:
	using LoginSuccessNavigateCallback = std::function<void()>;

	explicit LoginWindow(ws::viewmodels::LoginViewModel& viewModel);
	~LoginWindow() = default;

	LoginWindow(const LoginWindow&) = delete;
	LoginWindow& operator=(const LoginWindow&) = delete;

	[[nodiscard]] bool Create(HINSTANCE hInstance, int width, int height);
	void Show(int nCmdShow);
	[[nodiscard]] HWND GetHandle() const;

	void SetOnLoginSuccessNavigate(LoginSuccessNavigateCallback callback);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCreate();
	void OnCommand(WPARAM wParam);
	void OnLogin();
	void UpdateUI();
	void ShowError(const std::string& message);

	static constexpr const wchar_t* kClassName = L"LoginWindow";
	static constexpr UINT kWmShowError = WM_APP + 200;
	static constexpr int kIdLoginIdEdit = 1001;
	static constexpr int kIdPasswordEdit = 1002;
	static constexpr int kIdLoginButton = 1003;
	static constexpr int kIdErrorLabel = 1004;

	static HMENU ToHMenu(int id) { return reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)); }

	ws::viewmodels::LoginViewModel& m_viewModel;
	HWND m_hwnd = nullptr;
	HWND m_loginIdEdit = nullptr;
	HWND m_passwordEdit = nullptr;
	HWND m_loginButton = nullptr;
	HWND m_errorLabel = nullptr;
	LoginSuccessNavigateCallback m_onLoginSuccessNavigate;
};

} // namespace ws::views
