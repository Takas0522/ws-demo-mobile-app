#pragma once

#include <vector>
#include "Services/IFeatureFlagService.h"

namespace ws::services
{

class IHttpClient;

class FeatureFlagService : public IFeatureFlagService
{
public:
	explicit FeatureFlagService(const IHttpClient& httpClient);
	~FeatureFlagService() override = default;

	FeatureFlagService(const FeatureFlagService&) = delete;
	FeatureFlagService& operator=(const FeatureFlagService&) = delete;

	[[nodiscard]] std::expected<std::vector<ws::models::FeatureFlag>, ws::models::ApiError> FetchFeatureFlags(
		const std::string& token) const override;

	[[nodiscard]] bool IsFavoriteEnabled() const override;

private:
	const IHttpClient& m_httpClient;
	mutable std::vector<ws::models::FeatureFlag> m_cachedFlags;
};

} // namespace ws::services
