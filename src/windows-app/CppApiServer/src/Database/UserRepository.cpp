#include "Database/UserRepository.h"
#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <mutex>

namespace ws::database
{

UserRepository::UserRepository(DatabaseManager& dbManager)
	: m_dbManager(dbManager)
{
}

std::optional<ws::models::User> UserRepository::FindByLoginId(const std::string& loginId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT user_id, user_name, login_id, password_hash, user_type, created_at, updated_at "
		"FROM users WHERE login_id = ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::nullopt;
	}

	sqlite3_bind_text(stmt, 1, loginId.c_str(), static_cast<int>(loginId.size()), SQLITE_STATIC);

	std::optional<ws::models::User> result;
	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ws::models::User user;
		user.userId = sqlite3_column_int64(stmt, 0);
		user.userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		user.loginId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		user.passwordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		user.userType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		user.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
		user.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
		result = user;
	}

	sqlite3_finalize(stmt);
	return result;
}

std::optional<ws::models::User> UserRepository::FindById(int64_t userId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT user_id, user_name, login_id, password_hash, user_type, created_at, updated_at "
		"FROM users WHERE user_id = ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::nullopt;
	}

	sqlite3_bind_int64(stmt, 1, userId);

	std::optional<ws::models::User> result;
	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ws::models::User user;
		user.userId = sqlite3_column_int64(stmt, 0);
		user.userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		user.loginId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		user.passwordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		user.userType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		user.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
		user.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
		result = user;
	}

	sqlite3_finalize(stmt);
	return result;
}

std::vector<ws::models::User> UserRepository::FindByUserType(const std::string& userType)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT user_id, user_name, login_id, password_hash, user_type, created_at, updated_at "
		"FROM users WHERE user_type = ? ORDER BY user_id;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return {};
	}

	sqlite3_bind_text(stmt, 1, userType.c_str(), static_cast<int>(userType.size()), SQLITE_STATIC);

	std::vector<ws::models::User> users;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ws::models::User user;
		user.userId = sqlite3_column_int64(stmt, 0);
		user.userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		user.loginId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		user.passwordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		user.userType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		user.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
		user.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
		users.push_back(user);
	}

	sqlite3_finalize(stmt);
	return users;
}

} // namespace ws::database
