#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace ws::utils
{

// Modern flat color palette
namespace colors
{
	constexpr COLORREF kBackground = RGB(245, 247, 250);
	constexpr COLORREF kSurface = RGB(255, 255, 255);
	constexpr COLORREF kTextPrimary = RGB(30, 41, 59);
	constexpr COLORREF kTextSecondary = RGB(100, 116, 139);
	constexpr COLORREF kTextOnAccent = RGB(255, 255, 255);
	constexpr COLORREF kAccent = RGB(59, 130, 246);
	constexpr COLORREF kAccentHover = RGB(37, 99, 235);
	constexpr COLORREF kAccentPressed = RGB(29, 78, 216);
	constexpr COLORREF kDanger = RGB(239, 68, 68);
	constexpr COLORREF kDangerHover = RGB(220, 38, 38);
	constexpr COLORREF kSecondary = RGB(226, 232, 240);
	constexpr COLORREF kSecondaryHover = RGB(203, 213, 225);
	constexpr COLORREF kBorder = RGB(203, 213, 225);
	constexpr COLORREF kEditBg = RGB(255, 255, 255);
	constexpr COLORREF kHeaderBg = RGB(255, 255, 255);
	constexpr COLORREF kDivider = RGB(226, 232, 240);
} // namespace colors

class ModernTheme
{
public:
	static ModernTheme& Instance();

	ModernTheme(const ModernTheme&) = delete;
	ModernTheme& operator=(const ModernTheme&) = delete;

	void Initialize();
	void Shutdown();

	// Fonts
	[[nodiscard]] HFONT GetFont() const noexcept { return m_font; }
	[[nodiscard]] HFONT GetFontBold() const noexcept { return m_fontBold; }
	[[nodiscard]] HFONT GetFontLarge() const noexcept { return m_fontLarge; }
	[[nodiscard]] HFONT GetFontTitle() const noexcept { return m_fontTitle; }
	[[nodiscard]] HFONT GetFontSmall() const noexcept { return m_fontSmall; }

	// Brushes
	[[nodiscard]] HBRUSH GetBackgroundBrush() const noexcept { return m_bgBrush; }
	[[nodiscard]] HBRUSH GetSurfaceBrush() const noexcept { return m_surfaceBrush; }
	[[nodiscard]] HBRUSH GetEditBrush() const noexcept { return m_editBrush; }
	[[nodiscard]] HBRUSH GetHeaderBrush() const noexcept { return m_headerBrush; }

	// Apply font to window and its children
	static void ApplyFont(HWND hwnd, HFONT font);

	// Owner-draw button painting
	static void DrawButton(DRAWITEMSTRUCT* dis, COLORREF bgNormal, COLORREF bgHover,
		COLORREF textColor, int cornerRadius = 6);

	// Draw a rounded rectangle filled with color
	static void FillRoundRect(HDC hdc, const RECT& rc, COLORREF color, int radius);

	// Draw horizontal divider line
	static void DrawDivider(HDC hdc, int x, int y, int width);

private:
	ModernTheme() = default;
	~ModernTheme() = default;

	HFONT m_font = nullptr;
	HFONT m_fontBold = nullptr;
	HFONT m_fontLarge = nullptr;
	HFONT m_fontTitle = nullptr;
	HFONT m_fontSmall = nullptr;

	HBRUSH m_bgBrush = nullptr;
	HBRUSH m_surfaceBrush = nullptr;
	HBRUSH m_editBrush = nullptr;
	HBRUSH m_headerBrush = nullptr;
};

} // namespace ws::utils
