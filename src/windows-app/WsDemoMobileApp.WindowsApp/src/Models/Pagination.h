#pragma once

#include <nlohmann/json.hpp>

namespace ws::models
{

struct Pagination
{
	int currentPage = 1;
	int totalPages = 1;
	int totalCount = 0;
	int limit = 20;

	Pagination() = default;

	[[nodiscard]] bool HasNextPage() const
	{
		return currentPage < totalPages;
	}
};

inline void from_json(const nlohmann::json& j, Pagination& p)
{
	if (j.contains("currentPage"))
	{
		j.at("currentPage").get_to(p.currentPage);
	}
	if (j.contains("totalPages"))
	{
		j.at("totalPages").get_to(p.totalPages);
	}
	if (j.contains("totalCount"))
	{
		j.at("totalCount").get_to(p.totalCount);
	}
	if (j.contains("limit"))
	{
		j.at("limit").get_to(p.limit);
	}
}

} // namespace ws::models
