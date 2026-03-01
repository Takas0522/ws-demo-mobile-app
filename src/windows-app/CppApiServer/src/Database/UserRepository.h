#pragma once

#include <string>
#include <optional>
#include <vector>
#include "Models/User.h"

struct sqlite3;

namespace ws::database
{

class DatabaseManager;

class UserRepository
{
public:
	explicit UserRepository(DatabaseManager& dbManager);

	[[nodiscard]] std::optional<ws::models::User> FindByLoginId(const std::string& loginId);
	[[nodiscard]] std::optional<ws::models::User> FindById(int64_t userId);
	[[nodiscard]] std::vector<ws::models::User> FindByUserType(const std::string& userType);

private:
	DatabaseManager& m_dbManager;
};

} // namespace ws::database
