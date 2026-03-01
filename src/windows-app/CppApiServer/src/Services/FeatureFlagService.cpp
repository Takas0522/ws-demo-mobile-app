#include "Services/FeatureFlagService.h"

#include "Database/FeatureFlagRepository.h"

namespace ws::services
{

FeatureFlagService::FeatureFlagService(ws::database::FeatureFlagRepository& flagRepo)
	: m_flagRepo(flagRepo)
{
}

std::map<std::string, bool> FeatureFlagService::GetFlags(int64_t userId)
{
	return m_flagRepo.FindFlagsByUserId(userId);
}

} // namespace ws::services
