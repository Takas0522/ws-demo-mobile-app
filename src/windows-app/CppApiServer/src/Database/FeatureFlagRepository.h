#pragma once

#include <string>
#include <map>
#include "Models/FeatureFlag.h"

namespace ws::database
{

class DatabaseManager;

class FeatureFlagRepository
{
public:
	explicit FeatureFlagRepository(DatabaseManager& dbManager);

	// Returns map of flagKey -> isEnabled (merging defaults with user overrides)
	[[nodiscard]] std::map<std::string, bool> FindFlagsByUserId(int64_t userId);

private:
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
