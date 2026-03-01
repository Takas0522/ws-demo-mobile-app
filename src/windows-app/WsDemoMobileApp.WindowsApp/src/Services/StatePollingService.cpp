#include "Services/StatePollingService.h"
#include "Utils/Constants.h"

namespace ws::services
{

StatePollingService::StatePollingService() = default;

StatePollingService::~StatePollingService()
{
	Stop();
}

void StatePollingService::Start(HWND targetWindow, PollingCallback callback, int intervalMs)
{
	Stop();

	m_targetWindow = targetWindow;
	m_callback = std::move(callback);
	m_intervalMs = intervalMs;
	m_running.store(true);

	m_thread = std::thread(&StatePollingService::PollingLoop, this);
}

void StatePollingService::Stop()
{
	m_running.store(false);

	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

bool StatePollingService::IsRunning() const
{
	return m_running.load();
}

void StatePollingService::SetAuthCheck(AuthCheckCallback callback)
{
	m_authCheck = std::move(callback);
}

void StatePollingService::PollingLoop()
{
	while (m_running.load())
	{
		Sleep(static_cast<DWORD>(m_intervalMs));

		if (!m_running.load())
		{
			break;
		}

		// Check if authentication is still valid before polling
		if (m_authCheck && !m_authCheck())
		{
			m_running.store(false);
			if (m_targetWindow)
			{
				PostMessage(m_targetWindow, ws::utils::WM_AUTH_ERROR, 0, 0);
			}
			break;
		}

		if (m_callback)
		{
			m_callback();
		}

		if (m_targetWindow)
		{
			PostMessage(m_targetWindow, ws::utils::WM_HTTP_RESPONSE, 0, 0);
		}
	}
}

} // namespace ws::services
