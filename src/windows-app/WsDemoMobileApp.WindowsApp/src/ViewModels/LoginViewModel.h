#pragma once

#include <string>
#include <functional>
#include <optional>
#include "Models/User.h"
#include "Models/ApiError.h"

namespace ws::services
{
class IAuthService;
}

namespace ws::utils
{
class ICredentialManager;
}

namespace ws::viewmodels
{

constexpr int kMinLoginIdLength = 4;
constexpr int kMaxLoginIdLength = 20;
constexpr int kMinPasswordLength = 8;
constexpr int kMaxPasswordLength = 50;

struct ValidationError
{
	std::string loginIdError;
	std::string passwordError;
};

class LoginViewModel
{
public:
	using LoginSuccessCallback = std::function<void(const ws::models::User&)>;
	using LoginErrorCallback = std::function<void(const ws::models::ApiError&)>;
	using LoadingChangedCallback = std::function<void(bool)>;

	LoginViewModel(
		ws::services::IAuthService& authService,
		ws::utils::ICredentialManager& credentialManager);
	~LoginViewModel() = default;

	LoginViewModel(const LoginViewModel&) = delete;
	LoginViewModel& operator=(const LoginViewModel&) = delete;

	void Login(const std::string& loginId, const std::string& password);

	[[nodiscard]] std::optional<ValidationError> Validate(
		const std::string& loginId,
		const std::string& password) const;

	void SetOnLoginSuccess(LoginSuccessCallback callback);
	void SetOnLoginError(LoginErrorCallback callback);
	void SetOnLoadingChanged(LoadingChangedCallback callback);

	[[nodiscard]] bool IsLoading() const;
	[[nodiscard]] const std::string& GetLoginId() const;
	[[nodiscard]] const std::string& GetLastError() const;
	void SetLoginId(const std::string& loginId);
	void SetPassword(const std::string& password);

private:
	ws::services::IAuthService& m_authService;
	ws::utils::ICredentialManager& m_credentialManager;

	std::string m_loginId;
	std::string m_password;
	std::string m_lastError;
	bool m_isLoading = false;

	LoginSuccessCallback m_onLoginSuccess;
	LoginErrorCallback m_onLoginError;
	LoadingChangedCallback m_onLoadingChanged;
};

} // namespace ws::viewmodels
