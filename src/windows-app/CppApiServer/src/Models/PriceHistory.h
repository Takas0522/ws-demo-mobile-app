#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace ws::models
{

struct ChangedBy
{
	int64_t userId = 0;
	std::string userName;
};

struct PriceHistory
{
	int64_t priceHistoryId = 0;
	int64_t productId = 0;
	int oldPrice = 0;
	int newPrice = 0;
	std::string changedAt;
	ChangedBy changedBy;
	std::optional<std::string> changeReason;
};

inline void to_json(nlohmann::json& j, const ChangedBy& c)
{
	j = nlohmann::json{
		{"userId", c.userId},
		{"userName", c.userName}
	};
}

inline void to_json(nlohmann::json& j, const PriceHistory& ph)
{
	j = nlohmann::json{
		{"priceHistoryId", ph.priceHistoryId},
		{"productId", ph.productId},
		{"oldPrice", ph.oldPrice},
		{"newPrice", ph.newPrice},
		{"changedAt", ph.changedAt},
		{"changedBy", ph.changedBy}
	};
	j["changeReason"] = ph.changeReason.has_value()
		? nlohmann::json(ph.changeReason.value())
		: nlohmann::json(nullptr);
}

} // namespace ws::models
