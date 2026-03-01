#include "Services/AuthService.h"

#include <bcrypt/bcrypt.h>

#include "Database/UserRepository.h"
#include "Utils/Constants.h"
#include "Utils/JwtHelper.h"

namespace ws::services
{

AuthService::AuthService(ws::database::UserRepository& userRepo)
	: m_userRepo(userRepo)
{
}

std::expected<LoginResult, std::string> AuthService::Login(
	const std::string& loginId, const std::string& password)
{
	if (loginId.empty() || password.empty())
	{
		return std::unexpected("Login ID and password are required");
	}

	// Look up user by login ID
	auto userOpt = m_userRepo.FindByLoginId(loginId);
	if (!userOpt.has_value())
	{
		return std::unexpected("Invalid login ID or password");
	}

	auto& user = userOpt.value();

	// Verify password with bcrypt
	int result = bcrypt_checkpw(password.c_str(), user.passwordHash.c_str());
	if (result != 0)
	{
		return std::unexpected("Invalid login ID or password");
	}

	// Generate JWT
	std::string token = ws::utils::CreateJwt(
		user.userId,
		user.loginId,
		user.userType,
		ws::utils::kJwtSecret,
		ws::utils::kJwtExpirationSeconds);

	LoginResult loginResult;
	loginResult.token = std::move(token);
	loginResult.tokenType = "Bearer";
	loginResult.expiresIn = ws::utils::kJwtExpirationSeconds;
	loginResult.user = std::move(user);
	// Clear passwordHash so it is never leaked in responses
	loginResult.user.passwordHash.clear();

	return loginResult;
}

} // namespace ws::services
