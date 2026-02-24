#pragma once

#include <string>
#include <optional>

namespace ws::utils
{

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

private:
	static constexpr const wchar_t* kCredentialTarget = L"WsDemoMobileApp/JWT";
};

} // namespace ws::utils
