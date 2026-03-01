#pragma once

#include <chrono>
#include "Services/IAuthService.h"

namespace ws::services
{
class IHttpClient;
}

namespace ws::utils
{
class ICredentialManager;
}

namespace ws::services
{

class AuthService : public IAuthService
{
public:
	explicit AuthService(const IHttpClient& httpClient, ws::utils::ICredentialManager& credentialManager);
	~AuthService() override = default;

	AuthService(const AuthService&) = delete;
	AuthService& operator=(const AuthService&) = delete;

	[[nodiscard]] std::expected<LoginResponse, ws::models::ApiError> Login(
		const std::string& loginId,
		const std::string& password) override;

	void Logout() override;

	[[nodiscard]] const std::string& GetToken() const override;
	void SetToken(const std::string& token) override;
	void ClearToken() override;

	[[nodiscard]] bool IsAuthenticated() const override;
	[[nodiscard]] const ws::models::User& GetCurrentUser() const override;

	[[nodiscard]] bool IsTokenExpired() const override;
	void SetTokenExpiry(int expiresInSeconds) override;
	void SetTokenExpiryFromTimestamp(int64_t timestamp) override;
	[[nodiscard]] int64_t GetTokenExpiryTimestamp() const override;

private:
	const IHttpClient& m_httpClient;
	ws::utils::ICredentialManager& m_credentialManager;
	std::string m_token;
	ws::models::User m_currentUser;
	std::chrono::system_clock::time_point m_tokenExpiry{};
};

} // namespace ws::services
