#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winhttp.h>

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

class HttpClient
{
public:
	HttpClient();
	~HttpClient();

	HttpClient(const HttpClient&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;

	[[nodiscard]] HttpResult Get(
		const std::string& path,
		const std::optional<std::string>& authToken = std::nullopt) const;

	[[nodiscard]] HttpResult Post(
		const std::string& path,
		const std::string& jsonBody,
		const std::optional<std::string>& authToken = std::nullopt) const;

	[[nodiscard]] HttpResult Delete(
		const std::string& path,
		const std::optional<std::string>& authToken = std::nullopt) const;

private:
	[[nodiscard]] HttpResult SendRequest(
		const std::wstring& method,
		const std::string& path,
		const std::optional<std::string>& body,
		const std::optional<std::string>& authToken) const;

	HINTERNET m_session = nullptr;
};

} // namespace ws::services
