#include "Services/AuthService.h"
#include "Services/HttpClient.h"
#include "Utils/Constants.h"
#include "Utils/CredentialManager.h"
#include "Utils/JsonHelper.h"

#include <nlohmann/json.hpp>

namespace ws::services
{

AuthService::AuthService(const HttpClient& httpClient, ws::utils::CredentialManager& credentialManager)
	: m_httpClient(httpClient)
	, m_credentialManager(credentialManager)
{
}

std::expected<LoginResponse, ws::models::ApiError> AuthService::Login(
	const std::string& loginId,
	const std::string& password)
{
	nlohmann::json requestBody = {
		{"loginId", loginId},
		{"password", password}
	};

	auto result = m_httpClient.Post(
		ws::utils::kLoginPath,
		requestBody.dump());

	if (!result.has_value())
	{
		return std::unexpected(result.error());
	}

	const auto& response = result.value();

	if (response.statusCode != 200)
	{
		auto error = ws::utils::ParseApiError(response.body);
		if (error.has_value())
		{
			return std::unexpected(std::move(error.value()));
		}
		return std::unexpected(ws::models::ApiError{
			"AUTH_001", "ログインに失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		LoginResponse loginResponse;
		data.at("token").get_to(loginResponse.token);
		data.at("tokenType").get_to(loginResponse.tokenType);
		data.at("expiresIn").get_to(loginResponse.expiresIn);
		loginResponse.user = data.at("user").get<ws::models::User>();

		m_currentUser = loginResponse.user;

		m_tokenExpiry = std::chrono::system_clock::now()
			+ std::chrono::seconds(loginResponse.expiresIn);

		return loginResponse;
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

void AuthService::Logout()
{
	// Fire-and-forget: POST to logout endpoint with current token
	if (!m_token.empty())
	{
		(void)m_httpClient.Post(
			ws::utils::kLogoutPath,
			"{}",
			m_token);
	}

	// Clear local state
	ClearToken();
	m_credentialManager.DeleteToken();
}

const std::string& AuthService::GetToken() const
{
	return m_token;
}

void AuthService::SetToken(const std::string& token)
{
	m_token = token;
}

void AuthService::ClearToken()
{
	m_token.clear();
	m_currentUser = ws::models::User{};
	m_tokenExpiry = {};
}

bool AuthService::IsAuthenticated() const
{
	return !m_token.empty();
}

const ws::models::User& AuthService::GetCurrentUser() const
{
	return m_currentUser;
}

bool AuthService::IsTokenExpired() const
{
	if (m_token.empty())
	{
		return true;
	}
	return std::chrono::system_clock::now() >= m_tokenExpiry;
}

void AuthService::SetTokenExpiry(int expiresInSeconds)
{
	m_tokenExpiry = std::chrono::system_clock::now()
		+ std::chrono::seconds(expiresInSeconds);
}

void AuthService::SetTokenExpiryFromTimestamp(int64_t timestamp)
{
	m_tokenExpiry = std::chrono::system_clock::from_time_t(
		static_cast<std::time_t>(timestamp));
}

int64_t AuthService::GetTokenExpiryTimestamp() const
{
	return static_cast<int64_t>(std::chrono::system_clock::to_time_t(m_tokenExpiry));
}

} // namespace ws::services
