#pragma once

#include "Utils/ICredentialManager.h"

namespace ws::utils
{

class CredentialManager : public ICredentialManager
{
public:
	CredentialManager() = default;
	~CredentialManager() override = default;

	CredentialManager(const CredentialManager&) = delete;
	CredentialManager& operator=(const CredentialManager&) = delete;

	[[nodiscard]] bool SaveToken(const std::string& token) override;
	[[nodiscard]] std::optional<std::string> LoadToken() const override;
	bool DeleteToken() override;

	[[nodiscard]] bool SaveTokenWithExpiry(const std::string& token, int64_t expiryTimestamp) override;
	[[nodiscard]] std::optional<TokenData> LoadTokenWithExpiry() const override;

private:
	static constexpr const wchar_t* kCredentialTarget = L"WsDemoMobileApp/JWT";
};

} // namespace ws::utils
