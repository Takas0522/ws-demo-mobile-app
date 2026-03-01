#include "Services/FeatureFlagService.h"
#include "Services/IHttpClient.h"
#include "Utils/Constants.h"
#include "Utils/JsonHelper.h"

#include <nlohmann/json.hpp>
#include <algorithm>

namespace ws::services
{

FeatureFlagService::FeatureFlagService(const IHttpClient& httpClient)
	: m_httpClient(httpClient)
{
}

std::expected<std::vector<ws::models::FeatureFlag>, ws::models::ApiError> FeatureFlagService::FetchFeatureFlags(
	const std::string& token) const
{
	auto result = m_httpClient.Get(ws::utils::kFeatureFlagsPath, token);

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
		return std::unexpected(ws::models::ApiError{"FEATURE_FLAG_001", "フィーチャーフラグの取得に失敗しました"});
	}

	try
	{
		auto json = nlohmann::json::parse(response.body);
		auto data = json.contains("data") ? json.at("data") : json;

		auto flags = data.get<std::vector<ws::models::FeatureFlag>>();
		m_cachedFlags = flags;
		return flags;
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("レスポンスの解析に失敗しました: ") + e.what()});
	}
}

bool FeatureFlagService::IsFavoriteEnabled() const
{
	auto it = std::find_if(m_cachedFlags.begin(), m_cachedFlags.end(),
		[](const ws::models::FeatureFlag& flag)
		{
			return flag.flagKey == "enable_favorites" && flag.isEnabled;
		});
	return it != m_cachedFlags.end();
}

} // namespace ws::services
