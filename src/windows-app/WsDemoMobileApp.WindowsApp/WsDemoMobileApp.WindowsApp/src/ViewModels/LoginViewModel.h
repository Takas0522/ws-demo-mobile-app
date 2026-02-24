#pragma once

#include <string>
#include <functional>
#include "Models/User.h"
#include "Models/ApiError.h"

namespace ws::services
{
class AuthService;
}

namespace ws::utils
{
class CredentialManager;
}

namespace ws::viewmodels
{

class LoginViewModel
{
public:
	using LoginSuccessCallback = std::function<void(const ws::models::User&)>;
	using LoginErrorCallback = std::function<void(const ws::models::ApiError&)>;
	using LoadingChangedCallback = std::function<void(bool)>;

	LoginViewModel(
		ws::services::AuthService& authService,
		ws::utils::CredentialManager& credentialManager);
	~LoginViewModel() = default;

	LoginViewModel(const LoginViewModel&) = delete;
	LoginViewModel& operator=(const LoginViewModel&) = delete;

	void Login(const std::string& loginId, const std::string& password);

	void SetOnLoginSuccess(LoginSuccessCallback callback);
	void SetOnLoginError(LoginErrorCallback callback);
	void SetOnLoadingChanged(LoadingChangedCallback callback);

	[[nodiscard]] bool IsLoading() const;
	[[nodiscard]] const std::string& GetLoginId() const;
	void SetLoginId(const std::string& loginId);
	void SetPassword(const std::string& password);

private:
	ws::services::AuthService& m_authService;
	ws::utils::CredentialManager& m_credentialManager;

	std::string m_loginId;
	std::string m_password;
	bool m_isLoading = false;

	LoginSuccessCallback m_onLoginSuccess;
	LoginErrorCallback m_onLoginError;
	LoadingChangedCallback m_onLoadingChanged;
};

} // namespace ws::viewmodels
