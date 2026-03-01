#include "Services/PriceHistoryService.h"
#include "Database/PriceHistoryRepository.h"

namespace ws::services
{

PriceHistoryService::PriceHistoryService(ws::database::PriceHistoryRepository& priceHistoryRepo)
	: m_priceHistoryRepo(priceHistoryRepo)
{
}

nlohmann::json PriceHistoryService::GetPriceHistory(
	int64_t productId,
	const std::string& startDate,
	const std::string& endDate,
	int page,
	int limit)
{
	auto result = m_priceHistoryRepo.FindByProductId(productId, startDate, endDate, page, limit);

	int totalPages = (result.totalCount > 0)
		? (result.totalCount + limit - 1) / limit
		: 0;

	nlohmann::json historyJson = nlohmann::json::array();
	for (const auto& ph : result.items)
	{
		historyJson.push_back(ph);
	}

	nlohmann::json data;
	data["priceHistory"] = std::move(historyJson);
	data["pagination"] = {
		{"currentPage", page},
		{"totalPages", totalPages},
		{"totalItems", result.totalCount},
		{"itemsPerPage", limit}
	};

	return data;
}

bool PriceHistoryService::ProductExists(int64_t productId)
{
	return m_priceHistoryRepo.ProductExists(productId);
}

} // namespace ws::services
