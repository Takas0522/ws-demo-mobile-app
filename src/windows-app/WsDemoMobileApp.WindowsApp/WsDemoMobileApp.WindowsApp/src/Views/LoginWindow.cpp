#include "Views/LoginWindow.h"
#include "ViewModels/LoginViewModel.h"
#include "Utils/Constants.h"

#include <string>

namespace ws::views
{

LoginWindow::LoginWindow(ws::viewmodels::LoginViewModel& viewModel)
	: m_viewModel(viewModel)
{
}

bool LoginWindow::Create(HINSTANCE hInstance, int width, int height)
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
		0,
		kClassName,
		ws::utils::kAppTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		nullptr, nullptr, hInstance, this);

	return m_hwnd != nullptr;
}

void LoginWindow::Show(int nCmdShow)
{
	ShowWindow(m_hwnd, nCmdShow);
	UpdateWindow(m_hwnd);
}

HWND LoginWindow::GetHandle() const
{
	return m_hwnd;
}

LRESULT CALLBACK LoginWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LoginWindow* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		self = static_cast<LoginWindow*>(cs->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->m_hwnd = hwnd;
	}
	else
	{
		self = reinterpret_cast<LoginWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (self)
	{
		return self->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT LoginWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_COMMAND:
		OnCommand(wParam);
		return 0;

	case WM_APP + 100:
		if (m_onLoginSuccessNavigate)
		{
			m_onLoginSuccessNavigate();
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(m_hwnd, msg, wParam, lParam);
	}
}

void LoginWindow::OnCreate()
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(m_hwnd, GWLP_HINSTANCE));

	int centerX = 60;
	int startY = 200;

	// Title label
	CreateWindowExW(0, L"STATIC", L"WS Demo Mobile App",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		centerX, 80, 300, 40,
		m_hwnd, nullptr, hInstance, nullptr);

	// Login ID label
	CreateWindowExW(0, L"STATIC", L"ログインID:",
		WS_CHILD | WS_VISIBLE,
		centerX, startY, 300, 20,
		m_hwnd, nullptr, hInstance, nullptr);

	// Login ID edit
	m_loginIdEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
		centerX, startY + 25, 300, 28,
		m_hwnd, ToHMenu(kIdLoginIdEdit), hInstance, nullptr);

	// Password label
	CreateWindowExW(0, L"STATIC", L"パスワード:",
		WS_CHILD | WS_VISIBLE,
		centerX, startY + 70, 300, 20,
		m_hwnd, nullptr, hInstance, nullptr);

	// Password edit
	m_passwordEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL,
		centerX, startY + 95, 300, 28,
		m_hwnd, ToHMenu(kIdPasswordEdit), hInstance, nullptr);

	// Login button
	m_loginButton = CreateWindowExW(0, L"BUTTON", L"ログイン",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
		centerX, startY + 150, 300, 40,
		m_hwnd, ToHMenu(kIdLoginButton), hInstance, nullptr);

	// Error label
	m_errorLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | SS_CENTER,
		centerX, startY + 200, 300, 40,
		m_hwnd, ToHMenu(kIdErrorLabel), hInstance, nullptr);
}

void LoginWindow::OnCommand(WPARAM wParam)
{
	if (LOWORD(wParam) == kIdLoginButton)
	{
		OnLogin();
	}
}

void LoginWindow::OnLogin()
{
	wchar_t loginIdBuf[256] = {};
	wchar_t passwordBuf[256] = {};

	GetWindowTextW(m_loginIdEdit, loginIdBuf, 256);
	GetWindowTextW(m_passwordEdit, passwordBuf, 256);

	int loginIdLen = WideCharToMultiByte(CP_UTF8, 0, loginIdBuf, -1, nullptr, 0, nullptr, nullptr);
	int passwordLen = WideCharToMultiByte(CP_UTF8, 0, passwordBuf, -1, nullptr, 0, nullptr, nullptr);

	std::string loginId(loginIdLen - 1, '\0');
	std::string password(passwordLen - 1, '\0');

	WideCharToMultiByte(CP_UTF8, 0, loginIdBuf, -1, loginId.data(), loginIdLen, nullptr, nullptr);
	WideCharToMultiByte(CP_UTF8, 0, passwordBuf, -1, password.data(), passwordLen, nullptr, nullptr);

	// Hide error
	ShowWindow(m_errorLabel, SW_HIDE);

	m_viewModel.Login(loginId, password);
}

void LoginWindow::UpdateUI()
{
	EnableWindow(m_loginButton, !m_viewModel.IsLoading());
}

void LoginWindow::ShowError(const std::string& message)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
	std::wstring wideMessage(len - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, wideMessage.data(), len);

	SetWindowTextW(m_errorLabel, wideMessage.c_str());
	ShowWindow(m_errorLabel, SW_SHOW);
}

void LoginWindow::SetOnLoginSuccessNavigate(LoginSuccessNavigateCallback callback)
{
	m_onLoginSuccessNavigate = std::move(callback);
}

} // namespace ws::views
