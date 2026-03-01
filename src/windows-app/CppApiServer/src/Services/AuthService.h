#pragma once

#include <string>
#include <expected>
#include "Models/User.h"

namespace ws::database { class UserRepository; }

namespace ws::services
{

struct LoginResult
{
	std::string token;
	std::string tokenType = "Bearer";
	int expiresIn = 0;
	ws::models::User user;
};

class AuthService
{
public:
	explicit AuthService(ws::database::UserRepository& userRepo);

	AuthService(const AuthService&) = delete;
	AuthService& operator=(const AuthService&) = delete;

	[[nodiscard]] std::expected<LoginResult, std::string> Login(
		const std::string& loginId, const std::string& password);

private:
	ws::database::UserRepository& m_userRepo;
};

} // namespace ws::services
