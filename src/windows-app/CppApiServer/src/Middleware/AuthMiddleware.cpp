#include "Middleware/AuthMiddleware.h"

#include <string>

#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws::middleware
{

std::optional<ws::utils::JwtPayload> Authenticate(
	const httplib::Request& req, httplib::Response& res)
{
	// Look for the Authorization header
	auto it = req.headers.find("Authorization");
	if (it == req.headers.end())
	{
		res.status = 401;
		res.set_content(
			ws::utils::MakeErrorResponse("UNAUTHORIZED", "Authorization header is required"),
			"application/json");
		return std::nullopt;
	}

	const std::string& authHeader = it->second;
	const std::string kBearerPrefix = "Bearer ";

	// Ensure the header starts with "Bearer "
	if (authHeader.size() <= kBearerPrefix.size() ||
		authHeader.substr(0, kBearerPrefix.size()) != kBearerPrefix)
	{
		res.status = 401;
		res.set_content(
			ws::utils::MakeErrorResponse("UNAUTHORIZED", "Invalid authorization format"),
			"application/json");
		return std::nullopt;
	}

	std::string token = authHeader.substr(kBearerPrefix.size());

	// Verify the JWT
	auto result = ws::utils::VerifyJwt(token, ws::utils::kJwtSecret);
	if (!result.has_value())
	{
		res.status = 401;
		res.set_content(
			ws::utils::MakeErrorResponse("UNAUTHORIZED", result.error()),
			"application/json");
		return std::nullopt;
	}

	return result.value();
}

} // namespace ws::middleware
