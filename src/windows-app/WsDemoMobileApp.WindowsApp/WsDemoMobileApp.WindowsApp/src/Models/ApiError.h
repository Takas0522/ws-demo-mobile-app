#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct ApiError
{
	std::string code;
	std::string message;
	std::optional<std::string> details;

	ApiError() = default;
	ApiError(std::string code, std::string message)
		: code(std::move(code))
		, message(std::move(message))
	{
	}
};

inline void from_json(const nlohmann::json& j, ApiError& e)
{
	j.at("code").get_to(e.code);
	j.at("message").get_to(e.message);

	if (j.contains("details") && !j.at("details").is_null())
	{
		e.details = j.at("details").get<std::string>();
	}
}

inline void to_json(nlohmann::json& j, const ApiError& error)
{
	j = nlohmann::json{{"code", error.code}, {"message", error.message}};
	if (error.details.has_value())
	{
		j["details"] = error.details.value();
	}
}

} // namespace ws::models
