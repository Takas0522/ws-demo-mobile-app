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
	std::string userType;  // "user" or "admin"

	User() = default;

	[[nodiscard]] bool IsAdmin() const
	{
		return userType == "admin";
	}
};

inline void from_json(const nlohmann::json& j, User& u)
{
	j.at("userId").get_to(u.userId);
	j.at("userName").get_to(u.userName);
	j.at("loginId").get_to(u.loginId);
	j.at("userType").get_to(u.userType);
}

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
