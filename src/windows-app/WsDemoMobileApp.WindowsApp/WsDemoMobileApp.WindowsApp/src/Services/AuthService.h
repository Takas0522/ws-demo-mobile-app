#pragma once

#include <string>
#include <expected>
#include <functional>
#include "Models/User.h"
#include "Models/ApiError.h"

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
	explicit AuthService(const HttpClient& httpClient);
	~AuthService() = default;

	AuthService(const AuthService&) = delete;
	AuthService& operator=(const AuthService&) = delete;

	[[nodiscard]] std::expected<LoginResponse, ws::models::ApiError> Login(
		const std::string& loginId,
		const std::string& password);

	[[nodiscard]] const std::string& GetToken() const;
	void SetToken(const std::string& token);
	void ClearToken();

	[[nodiscard]] bool IsAuthenticated() const;
	[[nodiscard]] const ws::models::User& GetCurrentUser() const;

private:
	const HttpClient& m_httpClient;
	std::string m_token;
	ws::models::User m_currentUser;
};

} // namespace ws::services
