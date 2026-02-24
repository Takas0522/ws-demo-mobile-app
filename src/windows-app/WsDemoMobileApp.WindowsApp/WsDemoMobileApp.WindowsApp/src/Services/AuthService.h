#pragma once

#include <string>
#include <expected>
#include <functional>
#include <chrono>
#include <cstdint>
#include "Models/User.h"
#include "Models/ApiError.h"

namespace ws::utils
{
class CredentialManager;
}

namespace ws::services
{

class HttpClient;

struct LoginResponse
{
	std::string token;
	std::string tokenType;
	int expiresIn = 0;
	ws::models::User user;
};

class AuthService
{
public:
	explicit AuthService(const HttpClient& httpClient, ws::utils::CredentialManager& credentialManager);
	~AuthService() = default;

	AuthService(const AuthService&) = delete;
	AuthService& operator=(const AuthService&) = delete;

	[[nodiscard]] std::expected<LoginResponse, ws::models::ApiError> Login(
		const std::string& loginId,
		const std::string& password);

	void Logout();

	[[nodiscard]] const std::string& GetToken() const;
	void SetToken(const std::string& token);
	void ClearToken();

	[[nodiscard]] bool IsAuthenticated() const;
	[[nodiscard]] const ws::models::User& GetCurrentUser() const;

	[[nodiscard]] bool IsTokenExpired() const;
	void SetTokenExpiry(int expiresInSeconds);
	void SetTokenExpiryFromTimestamp(int64_t timestamp);
	[[nodiscard]] int64_t GetTokenExpiryTimestamp() const;

private:
	const HttpClient& m_httpClient;
	ws::utils::CredentialManager& m_credentialManager;
	std::string m_token;
	ws::models::User m_currentUser;
	std::chrono::system_clock::time_point m_tokenExpiry{};
};

} // namespace ws::services
