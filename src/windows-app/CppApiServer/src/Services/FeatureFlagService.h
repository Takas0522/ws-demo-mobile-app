#pragma once

#include <string>
#include <map>

namespace ws::database { class FeatureFlagRepository; }

namespace ws::services
{

class FeatureFlagService
{
public:
	explicit FeatureFlagService(ws::database::FeatureFlagRepository& flagRepo);

	FeatureFlagService(const FeatureFlagService&) = delete;
	FeatureFlagService& operator=(const FeatureFlagService&) = delete;

	[[nodiscard]] std::map<std::string, bool> GetFlags(int64_t userId);

private:
	ws::database::FeatureFlagRepository& m_flagRepo;
};

} // namespace ws::services
