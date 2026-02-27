#pragma once

#include <httplib.h>

namespace ws::services { class AuthService; }

namespace ws::routes
{

class AuthRoutes
{
public:
	explicit AuthRoutes(ws::services::AuthService& authService);

	AuthRoutes(const AuthRoutes&) = delete;
	AuthRoutes& operator=(const AuthRoutes&) = delete;

	void Register(httplib::Server& server);

private:
	void HandleLogin(const httplib::Request& req, httplib::Response& res);
	void HandleLogout(const httplib::Request& req, httplib::Response& res);

	ws::services::AuthService& m_authService;
};

} // namespace ws::routes
