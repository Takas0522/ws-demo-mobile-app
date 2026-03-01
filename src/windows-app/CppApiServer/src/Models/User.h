#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct User
{
	int64_t userId = 0;
	std::string userName;
	std::string loginId;
	std::string passwordHash;  // Only used server-side, not serialized to response
	std::string userType;       // "user" or "admin"
	std::string createdAt;
	std::string updatedAt;

	[[nodiscard]] bool IsAdmin() const { return userType == "admin"; }
};

// Serialize to JSON (for API response - exclude passwordHash)
inline void to_json(nlohmann::json& j, const User& u)
{
	j = nlohmann::json{
		{"userId", u.userId},
		{"userName", u.userName},
		{"loginId", u.loginId},
		{"userType", u.userType}
	};
}

} // namespace ws::models
