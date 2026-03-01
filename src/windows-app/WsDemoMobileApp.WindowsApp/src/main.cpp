#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "App.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPWSTR /*lpCmdLine*/,
	_In_ int nCmdShow)
{
	ws::App app;

	if (!app.Initialize(hInstance, nCmdShow))
	{
		MessageBoxW(nullptr,
			L"アプリケーションの初期化に失敗しました。",
			L"エラー",
			MB_OK | MB_ICONERROR);
		return 1;
	}

	return app.Run();
}
