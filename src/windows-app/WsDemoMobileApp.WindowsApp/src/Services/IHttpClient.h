#pragma once

#include <string>
#include <expected>
#include <optional>
#include <map>
#include "Models/ApiError.h"

namespace ws::services
{

struct HttpResponse
{
	int statusCode = 0;
	std::string body;
	std::map<std::string, std::string> headers;
};

using HttpResult = std::expected<HttpResponse, ws::models::ApiError>;

class IHttpClient
{
public:
	virtual ~IHttpClient() = default;

	[[nodiscard]] virtual HttpResult Get(
		const std::string& path,
		const std::optional<std::string>& authToken = std::nullopt) const = 0;

	[[nodiscard]] virtual HttpResult Post(
		const std::string& path,
		const std::string& jsonBody,
		const std::optional<std::string>& authToken = std::nullopt) const = 0;

	[[nodiscard]] virtual HttpResult Delete(
		const std::string& path,
		const std::optional<std::string>& authToken = std::nullopt) const = 0;

protected:
	IHttpClient() = default;
	IHttpClient(const IHttpClient&) = default;
	IHttpClient& operator=(const IHttpClient&) = default;
};

} // namespace ws::services
