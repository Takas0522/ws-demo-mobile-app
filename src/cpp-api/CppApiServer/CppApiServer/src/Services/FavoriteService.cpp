#include "Services/FavoriteService.h"

#include "Database/FavoriteRepository.h"

namespace ws::services
{

FavoriteService::FavoriteService(ws::database::FavoriteRepository& favoriteRepo)
	: m_favoriteRepo(favoriteRepo)
{
}

std::expected<ws::models::Favorite, std::string> FavoriteService::AddFavorite(
	int64_t userId, int64_t productId)
{
	// Check for duplicate
	if (m_favoriteRepo.ExistsByUserAndProduct(userId, productId))
	{
		return std::unexpected("Product is already in favorites");
	}

	return m_favoriteRepo.Create(userId, productId);
}

std::expected<void, std::string> FavoriteService::RemoveFavorite(
	int64_t favoriteId, int64_t userId)
{
	return m_favoriteRepo.Delete(favoriteId, userId);
}

std::vector<ws::models::Favorite> FavoriteService::GetFavorites(int64_t userId)
{
	return m_favoriteRepo.FindByUserId(userId);
}

} // namespace ws::services
