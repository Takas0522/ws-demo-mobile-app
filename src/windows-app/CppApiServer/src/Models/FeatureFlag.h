#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct FeatureFlag
{
	int64_t flagId = 0;
	std::string flagKey;
	std::string flagName;
	bool defaultValue = false;
	bool isEnabled = false;  // User-specific or default
};

} // namespace ws::models
