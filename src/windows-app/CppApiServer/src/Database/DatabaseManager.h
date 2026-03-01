#pragma once

#include <string>
#include <mutex>
#include <expected>

struct sqlite3;

namespace ws::database
{

class DatabaseManager
{
public:
	DatabaseManager();
	~DatabaseManager();

	DatabaseManager(const DatabaseManager&) = delete;
	DatabaseManager& operator=(const DatabaseManager&) = delete;

	[[nodiscard]] std::expected<void, std::string> Initialize(const std::string& dbPath);
	[[nodiscard]] sqlite3* GetDb() const;
	std::mutex& GetMutex();

private:
	[[nodiscard]] std::expected<void, std::string> ExecuteSql(const std::string& sql);
	[[nodiscard]] std::expected<void, std::string> InitializeSchema();
	[[nodiscard]] bool IsSchemaInitialized();

	sqlite3* m_db = nullptr;
	std::mutex m_mutex;
	std::string m_dbPath;
};

} // namespace ws::database
