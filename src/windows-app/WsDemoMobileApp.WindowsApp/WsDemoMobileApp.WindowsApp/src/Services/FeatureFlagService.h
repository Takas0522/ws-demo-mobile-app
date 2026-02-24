#pragma once

#include <string>
#include <vector>
#include <expected>
#include "Models/FeatureFlag.h"
#include "Models/ApiError.h"

namespace ws::services
{

class HttpClient;

class FeatureFlagService
{
public:
	explicit FeatureFlagService(const HttpClient& httpClient);
	~FeatureFlagService() = default;

	FeatureFlagService(const FeatureFlagService&) = delete;
	FeatureFlagService& operator=(const FeatureFlagService&) = delete;

	[[nodiscard]] std::expected<std::vector<ws::models::FeatureFlag>, ws::models::ApiError> FetchFeatureFlags(
		const std::string& token) const;

	[[nodiscard]] bool IsFavoriteEnabled() const;

private:
	const HttpClient& m_httpClient;
	mutable std::vector<ws::models::FeatureFlag> m_cachedFlags;
};

} // namespace ws::services
