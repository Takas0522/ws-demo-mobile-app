#pragma once

#include <string>
#include <optional>
#include <httplib.h>
#include "Utils/JwtHelper.h"

namespace ws::middleware
{

// Extract and verify JWT from the Authorization header.
// Returns JwtPayload on success, sets error response on failure.
[[nodiscard]] std::optional<ws::utils::JwtPayload> Authenticate(
	const httplib::Request& req, httplib::Response& res);

} // namespace ws::middleware
