#pragma once

#include <string>
#include <optional>
#include <cstdint>

namespace ws::utils
{

struct TokenData
{
	std::string token;
	int64_t expiryTimestamp = 0;
};

class CredentialManager
{
public:
	CredentialManager() = default;
	~CredentialManager() = default;

	CredentialManager(const CredentialManager&) = delete;
	CredentialManager& operator=(const CredentialManager&) = delete;

	[[nodiscard]] bool SaveToken(const std::string& token);
	[[nodiscard]] std::optional<std::string> LoadToken() const;
	bool DeleteToken();

	[[nodiscard]] bool SaveTokenWithExpiry(const std::string& token, int64_t expiryTimestamp);
	[[nodiscard]] std::optional<TokenData> LoadTokenWithExpiry() const;

private:
	static constexpr const wchar_t* kCredentialTarget = L"WsDemoMobileApp/JWT";
};

} // namespace ws::utils
