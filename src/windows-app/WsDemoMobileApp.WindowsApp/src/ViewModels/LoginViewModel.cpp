#include "ViewModels/LoginViewModel.h"
#include "Services/IAuthService.h"
#include "Utils/ICredentialManager.h"

#include <thread>

namespace ws::viewmodels
{

LoginViewModel::LoginViewModel(
	ws::services::IAuthService& authService,
	ws::utils::ICredentialManager& credentialManager)
	: m_authService(authService)
	, m_credentialManager(credentialManager)
{
}

std::optional<ValidationError> LoginViewModel::Validate(
	const std::string& loginId,
	const std::string& password) const
{
	ValidationError error;
	bool hasError = false;

	if (loginId.empty())
	{
		error.loginIdError = "ログインIDを入力してください";
		hasError = true;
	}
	else if (static_cast<int>(loginId.length()) < kMinLoginIdLength)
	{
		error.loginIdError = "ログインIDは" + std::to_string(kMinLoginIdLength) + "文字以上で入力してください";
		hasError = true;
	}
	else if (static_cast<int>(loginId.length()) > kMaxLoginIdLength)
	{
		error.loginIdError = "ログインIDは" + std::to_string(kMaxLoginIdLength) + "文字以下で入力してください";
		hasError = true;
	}

	if (password.empty())
	{
		error.passwordError = "パスワードを入力してください";
		hasError = true;
	}
	else if (static_cast<int>(password.length()) < kMinPasswordLength)
	{
		error.passwordError = "パスワードは" + std::to_string(kMinPasswordLength) + "文字以上で入力してください";
		hasError = true;
	}
	else if (static_cast<int>(password.length()) > kMaxPasswordLength)
	{
		error.passwordError = "パスワードは" + std::to_string(kMaxPasswordLength) + "文字以下で入力してください";
		hasError = true;
	}

	if (hasError)
	{
		return error;
	}
	return std::nullopt;
}

void LoginViewModel::Login(const std::string& loginId, const std::string& password)
{
	auto validationResult = Validate(loginId, password);
	if (validationResult.has_value())
	{
		const auto& error = validationResult.value();
		std::string message;
		if (!error.loginIdError.empty())
		{
			message = error.loginIdError;
		}
		if (!error.passwordError.empty())
		{
			if (!message.empty())
			{
				message += "\n";
			}
			message += error.passwordError;
		}

		m_lastError = message;
		if (m_onLoginError)
		{
			m_onLoginError(ws::models::ApiError("VALIDATION_ERROR", message));
		}
		return;
	}

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
			auto expiryTs = m_authService.GetTokenExpiryTimestamp();
			(void)m_credentialManager.SaveTokenWithExpiry(loginResponse.token, expiryTs);

			if (m_onLoginSuccess)
			{
				m_onLoginSuccess(loginResponse.user);
			}
		}
		else
		{
			m_lastError = result.error().message;
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

const std::string& LoginViewModel::GetLastError() const
{
	return m_lastError;
}

} // namespace ws::viewmodels
