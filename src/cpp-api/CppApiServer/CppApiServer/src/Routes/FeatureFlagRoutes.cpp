#include "Routes/FeatureFlagRoutes.h"

#include <nlohmann/json.hpp>

#include "Services/FeatureFlagService.h"
#include "Middleware/AuthMiddleware.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

namespace ws::routes
{

FeatureFlagRoutes::FeatureFlagRoutes(ws::services::FeatureFlagService& flagService)
	: m_flagService(flagService)
{
}

void FeatureFlagRoutes::Register(httplib::Server& server)
{
	server.Get(ws::utils::kFeatureFlagsPath,
		[this](const httplib::Request& req, httplib::Response& res)
		{
			HandleGetFlags(req, res);
		});
}

void FeatureFlagRoutes::HandleGetFlags(const httplib::Request& req, httplib::Response& res)
{
	// Authenticate
	auto payload = ws::middleware::Authenticate(req, res);
	if (!payload.has_value())
	{
		return;
	}

	int64_t userId = payload->userId;
	auto flags = m_flagService.GetFlags(userId);

	nlohmann::json flagsJson = nlohmann::json::object();
	for (const auto& [key, value] : flags)
	{
		flagsJson[key] = value;
	}

	nlohmann::json data = {
		{"flags", flagsJson}
	};

	res.status = 200;
	res.set_content(
		ws::utils::MakeSuccessResponse(data),
		"application/json; charset=utf-8");
}

} // namespace ws::routes
