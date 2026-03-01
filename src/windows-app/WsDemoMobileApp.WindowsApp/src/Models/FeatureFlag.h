#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct FeatureFlag
{
	std::string flagKey;
	std::string description;
	bool isEnabled = false;
};

inline void from_json(const nlohmann::json& j, FeatureFlag& f)
{
	j.at("flagKey").get_to(f.flagKey);
	if (j.contains("description"))
	{
		j.at("description").get_to(f.description);
	}
	j.at("isEnabled").get_to(f.isEnabled);
}

inline void to_json(nlohmann::json& j, const FeatureFlag& f)
{
	j = nlohmann::json{
		{"flagKey", f.flagKey},
		{"description", f.description},
		{"isEnabled", f.isEnabled}
	};
}

} // namespace ws::models
