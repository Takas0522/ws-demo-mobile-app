#pragma once

#include <string>
#include <vector>
#include <expected>
#include "Models/FeatureFlag.h"
#include "Models/ApiError.h"

namespace ws::services
{

class IFeatureFlagService
{
public:
	virtual ~IFeatureFlagService() = default;

	[[nodiscard]] virtual std::expected<std::vector<ws::models::FeatureFlag>, ws::models::ApiError> FetchFeatureFlags(
		const std::string& token) const = 0;

	[[nodiscard]] virtual bool IsFavoriteEnabled() const = 0;

protected:
	IFeatureFlagService() = default;
	IFeatureFlagService(const IFeatureFlagService&) = default;
	IFeatureFlagService& operator=(const IFeatureFlagService&) = default;
};

} // namespace ws::services
