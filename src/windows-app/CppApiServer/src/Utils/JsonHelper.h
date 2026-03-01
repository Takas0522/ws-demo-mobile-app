#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace ws::utils
{

inline std::string GetTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	gmtime_s(&tm, &time);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
	return oss.str();
}

inline std::string MakeSuccessResponse(const nlohmann::json& data)
{
	nlohmann::json response = {
		{"data", data},
		{"timestamp", GetTimestamp()}
	};
	return response.dump();
}

inline std::string MakeErrorResponse(
	const std::string& code,
	const std::string& message,
	const std::string& details = "")
{
	nlohmann::json error = {
		{"code", code},
		{"message", message}
	};
	if (!details.empty())
	{
		error["details"] = details;
	}
	nlohmann::json response = {
		{"error", error},
		{"timestamp", GetTimestamp()}
	};
	return response.dump();
}

} // namespace ws::utils
