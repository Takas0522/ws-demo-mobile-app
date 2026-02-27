#include "Database/FeatureFlagRepository.h"
#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <mutex>

namespace ws::database
{

FeatureFlagRepository::FeatureFlagRepository(DatabaseManager& dbManager)
	: m_dbManager(dbManager)
{
}

std::map<std::string, bool> FeatureFlagRepository::FindFlagsByUserId(int64_t userId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	// LEFT JOIN user_feature_flags to merge default values with user-specific overrides.
	// COALESCE picks the user override if present, otherwise falls back to default_value.
	const char* sql =
		"SELECT ff.flag_key, "
		"COALESCE(uff.is_enabled, ff.default_value) AS is_enabled "
		"FROM feature_flags ff "
		"LEFT JOIN user_feature_flags uff "
		"ON ff.flag_id = uff.flag_id AND uff.user_id = ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return {};
	}

	sqlite3_bind_int64(stmt, 1, userId);

	std::map<std::string, bool> flags;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::string flagKey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		bool isEnabled = (sqlite3_column_int(stmt, 1) != 0);
		flags[flagKey] = isEnabled;
	}

	sqlite3_finalize(stmt);
	return flags;
}

} // namespace ws::database
