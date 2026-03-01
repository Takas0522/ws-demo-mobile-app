#pragma once

#include <string>
#include <expected>
#include <cstdint>
#include "Models/User.h"
#include "Models/ApiError.h"

namespace ws::services
{

struct LoginResponse
{
	std::string token;
	std::string tokenType;
	int expiresIn = 0;
	ws::models::User user;
};

class IAuthService
{
public:
	virtual ~IAuthService() = default;

	[[nodiscard]] virtual std::expected<LoginResponse, ws::models::ApiError> Login(
		const std::string& loginId,
		const std::string& password) = 0;

	virtual void Logout() = 0;

	[[nodiscard]] virtual const std::string& GetToken() const = 0;
	virtual void SetToken(const std::string& token) = 0;
	virtual void ClearToken() = 0;

	[[nodiscard]] virtual bool IsAuthenticated() const = 0;
	[[nodiscard]] virtual const ws::models::User& GetCurrentUser() const = 0;

	[[nodiscard]] virtual bool IsTokenExpired() const = 0;
	virtual void SetTokenExpiry(int expiresInSeconds) = 0;
	virtual void SetTokenExpiryFromTimestamp(int64_t timestamp) = 0;
	[[nodiscard]] virtual int64_t GetTokenExpiryTimestamp() const = 0;

protected:
	IAuthService() = default;
	IAuthService(const IAuthService&) = default;
	IAuthService& operator=(const IAuthService&) = default;
};

} // namespace ws::services
