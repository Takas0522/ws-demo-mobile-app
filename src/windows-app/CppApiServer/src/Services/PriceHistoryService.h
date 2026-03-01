#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace ws::database { class PriceHistoryRepository; }

namespace ws::services
{

class PriceHistoryService
{
public:
	explicit PriceHistoryService(ws::database::PriceHistoryRepository& priceHistoryRepo);

	PriceHistoryService(const PriceHistoryService&) = delete;
	PriceHistoryService& operator=(const PriceHistoryService&) = delete;

	[[nodiscard]] nlohmann::json GetPriceHistory(
		int64_t productId,
		const std::string& startDate,
		const std::string& endDate,
		int page,
		int limit);

	[[nodiscard]] bool ProductExists(int64_t productId);

private:
	ws::database::PriceHistoryRepository& m_priceHistoryRepo;
};

} // namespace ws::services
