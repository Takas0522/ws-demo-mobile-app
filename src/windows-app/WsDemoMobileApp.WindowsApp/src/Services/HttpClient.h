#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winhttp.h>

#include "Services/IHttpClient.h"

namespace ws::services
{

class HttpClient : public IHttpClient
{
public:
	HttpClient();
	~HttpClient() override;

	HttpClient(const HttpClient&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;

	[[nodiscard]] HttpResult Get(
		const std::string& path,
		const std::optional<std::string>& authToken = std::nullopt) const override;

	[[nodiscard]] HttpResult Post(
		const std::string& path,
		const std::string& jsonBody,
		const std::optional<std::string>& authToken = std::nullopt) const override;

	[[nodiscard]] HttpResult Delete(
		const std::string& path,
		const std::optional<std::string>& authToken = std::nullopt) const override;

private:
	[[nodiscard]] HttpResult SendRequest(
		const std::wstring& method,
		const std::string& path,
		const std::optional<std::string>& body,
		const std::optional<std::string>& authToken) const;

	HINTERNET m_session = nullptr;
};

} // namespace ws::services
