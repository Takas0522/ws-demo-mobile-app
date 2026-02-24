#include "Utils/CredentialManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <wincred.h>
#include <vector>

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

	std::string token(
		reinterpret_cast<const char*>(pCred->CredentialBlob),
		pCred->CredentialBlobSize
	);

	CredFree(pCred);
	return token;
}

bool CredentialManager::DeleteToken()
{
	return CredDeleteW(kCredentialTarget, CRED_TYPE_GENERIC, 0) == TRUE;
}

} // namespace ws::utils
