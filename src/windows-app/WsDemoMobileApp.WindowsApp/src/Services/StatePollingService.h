#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <functional>
#include <atomic>
#include <thread>

namespace ws::services
{

class StatePollingService
{
public:
	using PollingCallback = std::function<void()>;
	using AuthCheckCallback = std::function<bool()>;

	StatePollingService();
	~StatePollingService();

	StatePollingService(const StatePollingService&) = delete;
	StatePollingService& operator=(const StatePollingService&) = delete;

	void Start(HWND targetWindow, PollingCallback callback, int intervalMs);
	void Stop();
	void SetAuthCheck(AuthCheckCallback callback);

	[[nodiscard]] bool IsRunning() const;

private:
	void PollingLoop();

	HWND m_targetWindow = nullptr;
	PollingCallback m_callback;
	AuthCheckCallback m_authCheck;
	int m_intervalMs = 0;
	std::atomic<bool> m_running{false};
	std::thread m_thread;
};

} // namespace ws::services
