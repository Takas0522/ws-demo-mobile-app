#include "Utils/ModernTheme.h"

#include <windowsx.h>

namespace ws::utils
{

ModernTheme& ModernTheme::Instance()
{
	static ModernTheme instance;
	return instance;
}

void ModernTheme::Initialize()
{
	int dpi = GetDpiForSystem();
	int scale = MulDiv(1, dpi, 96);

	auto createFont = [&](int size, int weight) -> HFONT
	{
		return CreateFontW(
			-MulDiv(size, dpi, 72),
			0, 0, 0, weight,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			L"Segoe UI");
	};

	m_font = createFont(10, FW_REGULAR);
	m_fontBold = createFont(10, FW_SEMIBOLD);
	m_fontLarge = createFont(13, FW_SEMIBOLD);
	m_fontTitle = createFont(20, FW_BOLD);
	m_fontSmall = createFont(9, FW_REGULAR);

	m_bgBrush = CreateSolidBrush(colors::kBackground);
	m_surfaceBrush = CreateSolidBrush(colors::kSurface);
	m_editBrush = CreateSolidBrush(colors::kEditBg);
	m_headerBrush = CreateSolidBrush(colors::kHeaderBg);
}

void ModernTheme::Shutdown()
{
	auto safeDelete = [](HGDIOBJ& obj)
	{
		if (obj)
		{
			DeleteObject(obj);
			obj = nullptr;
		}
	};

	safeDelete(reinterpret_cast<HGDIOBJ&>(m_font));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_fontBold));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_fontLarge));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_fontTitle));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_fontSmall));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_bgBrush));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_surfaceBrush));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_editBrush));
	safeDelete(reinterpret_cast<HGDIOBJ&>(m_headerBrush));
}

void ModernTheme::ApplyFont(HWND hwnd, HFONT font)
{
	SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

	EnumChildWindows(hwnd, [](HWND child, LPARAM lParam) -> BOOL
	{
		SendMessageW(child, WM_SETFONT, static_cast<WPARAM>(lParam), TRUE);
		return TRUE;
	}, reinterpret_cast<LPARAM>(font));
}

void ModernTheme::DrawButton(DRAWITEMSTRUCT* dis, COLORREF bgNormal, COLORREF bgHover,
	COLORREF textColor, int cornerRadius)
{
	HDC hdc = dis->hDC;
	RECT rc = dis->rcItem;

	COLORREF bgColor = bgNormal;
	if (dis->itemState & ODS_SELECTED)
	{
		bgColor = bgHover;
	}

	// Rounded rectangle background
	FillRoundRect(hdc, rc, bgColor, cornerRadius);

	// Text
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textColor);

	auto& theme = Instance();
	SelectObject(hdc, theme.GetFontBold());

	wchar_t text[256] = {};
	GetWindowTextW(dis->hwndItem, text, 256);
	DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Focus rect (subtle)
	if (dis->itemState & ODS_FOCUS)
	{
		RECT focusRc = rc;
		InflateRect(&focusRc, -3, -3);
		HPEN pen = CreatePen(PS_DOT, 1, textColor);
		HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
		HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH)));
		RoundRect(hdc, focusRc.left, focusRc.top, focusRc.right, focusRc.bottom,
			cornerRadius, cornerRadius);
		SelectObject(hdc, oldBrush);
		SelectObject(hdc, oldPen);
		DeleteObject(pen);
	}
}

void ModernTheme::FillRoundRect(HDC hdc, const RECT& rc, COLORREF color, int radius)
{
	HBRUSH brush = CreateSolidBrush(color);
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, brush));
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
	RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);
	DeleteObject(pen);
	DeleteObject(brush);
}

void ModernTheme::DrawDivider(HDC hdc, int x, int y, int width)
{
	HPEN pen = CreatePen(PS_SOLID, 1, colors::kDivider);
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
	MoveToEx(hdc, x, y, nullptr);
	LineTo(hdc, x + width, y);
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}

} // namespace ws::utils
