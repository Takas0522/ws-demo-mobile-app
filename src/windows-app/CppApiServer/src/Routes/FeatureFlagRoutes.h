#pragma once

#include <httplib.h>

namespace ws::services { class FeatureFlagService; }

namespace ws::routes
{

class FeatureFlagRoutes
{
public:
	explicit FeatureFlagRoutes(ws::services::FeatureFlagService& flagService);

	FeatureFlagRoutes(const FeatureFlagRoutes&) = delete;
	FeatureFlagRoutes& operator=(const FeatureFlagRoutes&) = delete;

	void Register(httplib::Server& server);

private:
	void HandleGetFlags(const httplib::Request& req, httplib::Response& res);

	ws::services::FeatureFlagService& m_flagService;
};

} // namespace ws::routes
