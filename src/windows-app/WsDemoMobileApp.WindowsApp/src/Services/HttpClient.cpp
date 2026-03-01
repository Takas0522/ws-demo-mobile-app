#include "Services/HttpClient.h"
#include "Utils/Constants.h"

#include <vector>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

namespace
{

std::string FormatWinHttpError(DWORD errorCode)
{
	switch (errorCode)
	{
	case ERROR_WINHTTP_CANNOT_CONNECT:
		return "サーバーに接続できません。BFFサーバー(localhost:"
			+ std::to_string(ws::utils::kBffPort)
			+ ")が起動しているか確認してください";
	case ERROR_WINHTTP_TIMEOUT:
		return "サーバーへの接続がタイムアウトしました";
	case ERROR_WINHTTP_NAME_NOT_RESOLVED:
		return "ホスト名を解決できません";
	case ERROR_WINHTTP_CONNECTION_ERROR:
		return "接続中にエラーが発生しました";
	default:
		return "ネットワークエラー (コード: " + std::to_string(errorCode) + ")";
	}
}

} // anonymous namespace

namespace ws::services
{

HttpClient::HttpClient()
{
	m_session = WinHttpOpen(
		L"WsDemoMobileApp/1.0",
		WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);
}

HttpClient::~HttpClient()
{
	if (m_session)
	{
		WinHttpCloseHandle(m_session);
	}
}

HttpResult HttpClient::Get(
	const std::string& path,
	const std::optional<std::string>& authToken) const
{
	return SendRequest(L"GET", path, std::nullopt, authToken);
}

HttpResult HttpClient::Post(
	const std::string& path,
	const std::string& jsonBody,
	const std::optional<std::string>& authToken) const
{
	return SendRequest(L"POST", path, jsonBody, authToken);
}

HttpResult HttpClient::Delete(
	const std::string& path,
	const std::optional<std::string>& authToken) const
{
	return SendRequest(L"DELETE", path, std::nullopt, authToken);
}

HttpResult HttpClient::SendRequest(
	const std::wstring& method,
	const std::string& path,
	const std::optional<std::string>& body,
	const std::optional<std::string>& authToken) const
{
	if (!m_session)
	{
		return std::unexpected(ws::models::ApiError{
			"NETWORK_001", "HTTPセッションの初期化に失敗しました"});
	}

	HINTERNET connection = WinHttpConnect(
		m_session,
		ws::utils::kBffHost,
		static_cast<INTERNET_PORT>(ws::utils::kBffPort),
		0);

	if (!connection)
	{
		return std::unexpected(ws::models::ApiError{
			"NETWORK_001", "サーバーへの接続に失敗しました"});
	}

	std::wstring widePath(path.begin(), path.end());

	HINTERNET request = WinHttpOpenRequest(
		connection,
		method.c_str(),
		widePath.c_str(),
		nullptr,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		0);

	if (!request)
	{
		WinHttpCloseHandle(connection);
		return std::unexpected(ws::models::ApiError{
			"NETWORK_001", "リクエストの作成に失敗しました"});
	}

	// Set timeout
	DWORD timeout = ws::utils::kHttpTimeoutMs;
	WinHttpSetTimeouts(request, timeout, timeout, timeout, timeout);

	// Add headers
	std::wstring headers = L"Content-Type: application/json\r\n";
	if (authToken.has_value())
	{
		std::wstring token(authToken.value().begin(), authToken.value().end());
		headers += L"Authorization: Bearer " + token + L"\r\n";
	}

	LPVOID bodyData = WINHTTP_NO_REQUEST_DATA;
	DWORD bodyLength = 0;

	if (body.has_value())
	{
		bodyData = const_cast<char*>(body.value().c_str());
		bodyLength = static_cast<DWORD>(body.value().size());
	}

	BOOL result = WinHttpSendRequest(
		request,
		headers.c_str(),
		static_cast<DWORD>(headers.size()),
		bodyData,
		bodyLength,
		bodyLength,
		0);

	if (!result)
	{
		DWORD errorCode = GetLastError();
		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		return std::unexpected(ws::models::ApiError{
			"NETWORK_002", FormatWinHttpError(errorCode)});
	}

	result = WinHttpReceiveResponse(request, nullptr);
	if (!result)
	{
		DWORD errorCode = GetLastError();
		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		return std::unexpected(ws::models::ApiError{
			"NETWORK_003", FormatWinHttpError(errorCode)});
	}

	// Get status code
	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);
	WinHttpQueryHeaders(
		request,
		WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&statusCode,
		&statusCodeSize,
		WINHTTP_NO_HEADER_INDEX);

	// Read response body
	std::string responseBody;
	DWORD bytesAvailable = 0;
	DWORD bytesRead = 0;

	while (WinHttpQueryDataAvailable(request, &bytesAvailable) && bytesAvailable > 0)
	{
		std::vector<char> buffer(bytesAvailable);
		if (WinHttpReadData(request, buffer.data(), bytesAvailable, &bytesRead))
		{
			responseBody.append(buffer.data(), bytesRead);
		}
	}

	WinHttpCloseHandle(request);
	WinHttpCloseHandle(connection);

	return HttpResponse{
		static_cast<int>(statusCode),
		std::move(responseBody),
		{}
	};
}

} // namespace ws::services
