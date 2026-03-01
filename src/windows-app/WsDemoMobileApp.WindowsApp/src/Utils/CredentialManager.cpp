#include "Utils/CredentialManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <wincred.h>
#include <vector>
#include <cstdlib>

#pragma comment(lib, "advapi32.lib")

namespace ws::utils
{

bool CredentialManager::SaveToken(const std::string& token)
{
	CREDENTIALW cred = {};
	cred.Type = CRED_TYPE_GENERIC;
	cred.TargetName = const_cast<LPWSTR>(kCredentialTarget);
	cred.CredentialBlobSize = static_cast<DWORD>(token.size());
	cred.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<char*>(token.data()));
	cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

	return CredWriteW(&cred, 0) == TRUE;
}

std::optional<std::string> CredentialManager::LoadToken() const
{
	PCREDENTIALW pCred = nullptr;

	if (CredReadW(kCredentialTarget, CRED_TYPE_GENERIC, 0, &pCred) != TRUE)
	{
		return std::nullopt;
	}

	std::string raw(
		reinterpret_cast<const char*>(pCred->CredentialBlob),
		pCred->CredentialBlobSize
	);

	CredFree(pCred);

	// Handle new format "token|timestamp" - strip timestamp for backward compat
	auto pos = raw.rfind('|');
	if (pos != std::string::npos)
	{
		return raw.substr(0, pos);
	}

	return raw;
}

bool CredentialManager::DeleteToken()
{
	return CredDeleteW(kCredentialTarget, CRED_TYPE_GENERIC, 0) == TRUE;
}

bool CredentialManager::SaveTokenWithExpiry(const std::string& token, int64_t expiryTimestamp)
{
	// Store as "token|expiryTimestamp" in a single credential
	std::string combined = token + "|" + std::to_string(expiryTimestamp);

	CREDENTIALW cred = {};
	cred.Type = CRED_TYPE_GENERIC;
	cred.TargetName = const_cast<LPWSTR>(kCredentialTarget);
	cred.CredentialBlobSize = static_cast<DWORD>(combined.size());
	cred.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<char*>(combined.data()));
	cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

	return CredWriteW(&cred, 0) == TRUE;
}

std::optional<TokenData> CredentialManager::LoadTokenWithExpiry() const
{
	PCREDENTIALW pCred = nullptr;

	if (CredReadW(kCredentialTarget, CRED_TYPE_GENERIC, 0, &pCred) != TRUE)
	{
		return std::nullopt;
	}

	std::string raw(
		reinterpret_cast<const char*>(pCred->CredentialBlob),
		pCred->CredentialBlobSize
	);

	CredFree(pCred);

	auto pos = raw.rfind('|');
	if (pos == std::string::npos)
	{
		// Old format: token only, no expiry
		return TokenData{raw, 0};
	}

	TokenData data;
	data.token = raw.substr(0, pos);

	auto expiryStr = raw.substr(pos + 1);
	char* end = nullptr;
	data.expiryTimestamp = std::strtoll(expiryStr.c_str(), &end, 10);
	if (end == expiryStr.c_str())
	{
		data.expiryTimestamp = 0;
	}

	return data;
}

} // namespace ws::utils
