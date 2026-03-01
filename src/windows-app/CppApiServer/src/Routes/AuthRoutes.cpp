#include "Routes/AuthRoutes.h"

#include <nlohmann/json.hpp>

#include "Services/AuthService.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws::routes
{

AuthRoutes::AuthRoutes(ws::services::AuthService& authService)
	: m_authService(authService)
{
}

void AuthRoutes::Register(httplib::Server& server)
{
	server.Post(ws::utils::kLoginPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleLogin(req, res);
		});

	server.Post(ws::utils::kLogoutPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleLogout(req, res);
		});
}

void AuthRoutes::HandleLogin(const httplib::Request& req, httplib::Response& res)
{
	// Parse request body
	nlohmann::json body;
	try
	{
		body = nlohmann::json::parse(req.body);
	}
	catch (const nlohmann::json::exception&)
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "Invalid JSON in request body"),
			"application/json; charset=utf-8");
		return;
	}

	// Extract fields
	std::string loginId;
	std::string password;

	if (body.contains("loginId") && body["loginId"].is_string())
	{
		loginId = body["loginId"].get<std::string>();
	}
	if (body.contains("password") && body["password"].is_string())
	{
		password = body["password"].get<std::string>();
	}

	if (loginId.empty() || password.empty())
	{
		res.status = 400;
		res.set_content(
			ws::utils::MakeErrorResponse("BAD_REQUEST", "loginId and password are required"),
			"application/json; charset=utf-8");
		return;
	}

	// Call auth service
	auto result = m_authService.Login(loginId, password);

	if (!result.has_value())
	{
		res.status = 401;
		res.set_content(
			ws::utils::MakeErrorResponse("UNAUTHORIZED", result.error()),
			"application/json; charset=utf-8");
		return;
	}

	const auto& loginResult = result.value();

	// Build response
	nlohmann::json userData = loginResult.user;

	nlohmann::json data = {
		{"token", loginResult.token},
		{"tokenType", loginResult.tokenType},
		{"expiresIn", loginResult.expiresIn},
		{"user", userData}
	};

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

void AuthRoutes::HandleLogout(const httplib::Request& /*req*/, httplib::Response& res)
{
	// Stateless JWT — no server-side invalidation needed for demo
	nlohmann::json data = {
		{"message", "Logged out successfully"}
	};

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

} // namespace ws::routes
