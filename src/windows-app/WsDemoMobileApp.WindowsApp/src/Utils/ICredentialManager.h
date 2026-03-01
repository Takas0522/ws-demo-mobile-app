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

class ICredentialManager
{
public:
	virtual ~ICredentialManager() = default;

	[[nodiscard]] virtual bool SaveToken(const std::string& token) = 0;
	[[nodiscard]] virtual std::optional<std::string> LoadToken() const = 0;
	virtual bool DeleteToken() = 0;

	[[nodiscard]] virtual bool SaveTokenWithExpiry(const std::string& token, int64_t expiryTimestamp) = 0;
	[[nodiscard]] virtual std::optional<TokenData> LoadTokenWithExpiry() const = 0;

protected:
	ICredentialManager() = default;
	ICredentialManager(const ICredentialManager&) = default;
	ICredentialManager& operator=(const ICredentialManager&) = default;
};

} // namespace ws::utils
