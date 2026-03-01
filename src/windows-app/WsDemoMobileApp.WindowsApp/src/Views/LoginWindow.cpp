#include "Views/LoginWindow.h"
#include "ViewModels/LoginViewModel.h"
#include "Utils/Constants.h"
#include "Utils/ModernTheme.h"

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
	wc.hbrBackground = ws::utils::ModernTheme::Instance().GetBackgroundBrush();
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
		if (hCtrl == m_errorLabel)
		{
			SetTextColor(hdc, ws::utils::colors::kDanger);
		}
		else
		{
			SetTextColor(hdc, ws::utils::colors::kTextPrimary);
		}
		return reinterpret_cast<LRESULT>(theme.GetBackgroundBrush());
	}

	case WM_CTLCOLOREDIT:
	{
		HDC hdc = reinterpret_cast<HDC>(wParam);
		SetTextColor(hdc, ws::utils::colors::kTextPrimary);
		SetBkColor(hdc, ws::utils::colors::kEditBg);
		return reinterpret_cast<LRESULT>(theme.GetEditBrush());
	}

	case WM_APP + 100:
		if (m_onLoginSuccessNavigate)
		{
			m_onLoginSuccessNavigate();
		}
		return 0;

	case kWmShowError:
	{
		auto* errorMsg = reinterpret_cast<std::wstring*>(wParam);
		EnableWindow(m_loginButton, TRUE);
		SetWindowTextW(m_errorLabel, errorMsg->c_str());
		ShowWindow(m_errorLabel, SW_SHOW);
		InvalidateRect(m_hwnd, nullptr, TRUE);
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

void LoginWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);
	auto& theme = ws::utils::ModernTheme::Instance();

	RECT clientRc;
	GetClientRect(m_hwnd, &clientRc);
	int centerX = (clientRc.right - 320) / 2;

	// Card background
	RECT cardRc = { centerX - 20, 60, centerX + 340, 520 };
	ws::utils::ModernTheme::FillRoundRect(hdc, cardRc,
		ws::utils::colors::kSurface, 12);

	// App icon circle
	int iconCx = clientRc.right / 2;
	int iconCy = 120;
	HBRUSH accentBrush = CreateSolidBrush(ws::utils::colors::kAccent);
	HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, accentBrush));
	HPEN pen = CreatePen(PS_SOLID, 1, ws::utils::colors::kAccent);
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
	Ellipse(hdc, iconCx - 30, iconCy - 30, iconCx + 30, iconCy + 30);
	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);
	DeleteObject(pen);
	DeleteObject(accentBrush);

	// Icon text "WS" inside circle
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, ws::utils::colors::kTextOnAccent);
	SelectObject(hdc, theme.GetFontLarge());
	RECT iconTextRc = { iconCx - 30, iconCy - 30, iconCx + 30, iconCy + 30 };
	DrawTextW(hdc, L"WS", -1, &iconTextRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Title
	SetTextColor(hdc, ws::utils::colors::kTextPrimary);
	SelectObject(hdc, theme.GetFontLarge());
	RECT titleRc = { centerX, 165, centerX + 320, 195 };
	DrawTextW(hdc, L"WS Demo Mobile App", -1, &titleRc, DT_CENTER | DT_SINGLELINE);

	// Subtitle
	SetTextColor(hdc, ws::utils::colors::kTextSecondary);
	SelectObject(hdc, theme.GetFontSmall());
	RECT subRc = { centerX, 198, centerX + 320, 218 };
	DrawTextW(hdc, L"アカウントにログインしてください", -1, &subRc, DT_CENTER | DT_SINGLELINE);

	EndPaint(m_hwnd, &ps);
}

void LoginWindow::OnDrawItem(DRAWITEMSTRUCT* dis)
{
	if (dis->CtlID == kIdLoginButton)
	{
		ws::utils::ModernTheme::DrawButton(dis,
			ws::utils::colors::kAccent,
			ws::utils::colors::kAccentHover,
			ws::utils::colors::kTextOnAccent, 8);
	}
}

void LoginWindow::OnCreate()
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(m_hwnd, GWLP_HINSTANCE));
	auto& theme = ws::utils::ModernTheme::Instance();

	RECT clientRc;
	GetClientRect(m_hwnd, &clientRc);
	int centerX = (clientRc.right - 320) / 2;
	if (centerX < 20) centerX = 20;
	int startY = 240;

	// Login ID label
	HWND loginIdLabel = CreateWindowExW(0, L"STATIC", L"ログインID",
		WS_CHILD | WS_VISIBLE,
		centerX, startY, 320, 20,
		m_hwnd, nullptr, hInstance, nullptr);
	SendMessageW(loginIdLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontBold()), TRUE);

	// Login ID edit
	m_loginIdEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
		centerX, startY + 24, 320, 32,
		m_hwnd, ToHMenu(kIdLoginIdEdit), hInstance, nullptr);

	// Password label
	HWND passwordLabel = CreateWindowExW(0, L"STATIC", L"パスワード",
		WS_CHILD | WS_VISIBLE,
		centerX, startY + 72, 320, 20,
		m_hwnd, nullptr, hInstance, nullptr);
	SendMessageW(passwordLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontBold()), TRUE);

	// Password edit
	m_passwordEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL,
		centerX, startY + 96, 320, 32,
		m_hwnd, ToHMenu(kIdPasswordEdit), hInstance, nullptr);

	// Login button (owner-drawn)
	m_loginButton = CreateWindowExW(0, L"BUTTON", L"ログイン",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
		centerX, startY + 152, 320, 44,
		m_hwnd, ToHMenu(kIdLoginButton), hInstance, nullptr);

	// Error label
	m_errorLabel = CreateWindowExW(0, L"STATIC", L"",
		WS_CHILD | SS_CENTER,
		centerX, startY + 210, 320, 40,
		m_hwnd, ToHMenu(kIdErrorLabel), hInstance, nullptr);

	// Apply font to all controls
	ws::utils::ModernTheme::ApplyFont(m_hwnd, theme.GetFont());

	// Restore bold font for labels
	SendMessageW(loginIdLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontBold()), TRUE);
	SendMessageW(passwordLabel, WM_SETFONT, reinterpret_cast<WPARAM>(theme.GetFontBold()), TRUE);

	// Register ViewModel error callback to show error in label
	m_viewModel.SetOnLoginError([hwnd = m_hwnd](const ws::models::ApiError& error)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, nullptr, 0);
		auto* msg = new std::wstring(len - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, error.message.c_str(), -1, msg->data(), len);
		PostMessage(hwnd, kWmShowError, reinterpret_cast<WPARAM>(msg), 0);
	});
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

	// Disable button while processing
	EnableWindow(m_loginButton, FALSE);

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
