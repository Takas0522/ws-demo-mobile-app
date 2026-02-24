#include "ViewModels/LoginViewModel.h"
#include "Services/AuthService.h"
#include "Utils/CredentialManager.h"

#include <thread>

namespace ws::viewmodels
{

LoginViewModel::LoginViewModel(
	ws::services::AuthService& authService,
	ws::utils::CredentialManager& credentialManager)
	: m_authService(authService)
	, m_credentialManager(credentialManager)
{
}

void LoginViewModel::Login(const std::string& loginId, const std::string& password)
{
	m_isLoading = true;
	if (m_onLoadingChanged)
	{
		m_onLoadingChanged(true);
	}

	// HTTP on background thread
	std::thread([this, loginId, password]()
	{
		auto result = m_authService.Login(loginId, password);

		m_isLoading = false;
		if (m_onLoadingChanged)
		{
			m_onLoadingChanged(false);
		}

		if (result.has_value())
		{
			auto& loginResponse = result.value();
			m_authService.SetToken(loginResponse.token);
			m_credentialManager.SaveToken(loginResponse.token);

			if (m_onLoginSuccess)
			{
				m_onLoginSuccess(loginResponse.user);
			}
		}
		else
		{
			if (m_onLoginError)
			{
				m_onLoginError(result.error());
			}
		}
	}).detach();
}

void LoginViewModel::SetOnLoginSuccess(LoginSuccessCallback callback)
{
	m_onLoginSuccess = std::move(callback);
}

void LoginViewModel::SetOnLoginError(LoginErrorCallback callback)
{
	m_onLoginError = std::move(callback);
}

void LoginViewModel::SetOnLoadingChanged(LoadingChangedCallback callback)
{
	m_onLoadingChanged = std::move(callback);
}

bool LoginViewModel::IsLoading() const
{
	return m_isLoading;
}

const std::string& LoginViewModel::GetLoginId() const
{
	return m_loginId;
}

void LoginViewModel::SetLoginId(const std::string& loginId)
{
	m_loginId = loginId;
}

void LoginViewModel::SetPassword(const std::string& password)
{
	m_password = password;
}

} // namespace ws::viewmodels
