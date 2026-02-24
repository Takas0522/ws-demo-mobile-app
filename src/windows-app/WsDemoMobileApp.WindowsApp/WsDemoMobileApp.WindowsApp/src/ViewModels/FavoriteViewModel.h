#pragma once

#include <vector>
#include <functional>
#include "Models/Favorite.h"
#include "Models/ApiError.h"

namespace ws::services
{
class FavoriteService;
}

namespace ws::viewmodels
{

class FavoriteViewModel
{
public:
	using FavoritesChangedCallback = std::function<void()>;
	using ErrorCallback = std::function<void(const ws::models::ApiError&)>;
	using LoadingChangedCallback = std::function<void(bool)>;

	explicit FavoriteViewModel(ws::services::FavoriteService& favoriteService);
	~FavoriteViewModel() = default;

	FavoriteViewModel(const FavoriteViewModel&) = delete;
	FavoriteViewModel& operator=(const FavoriteViewModel&) = delete;

	void FetchFavorites();
	void RemoveFavorite(int64_t productId);

	void SetOnFavoritesChanged(FavoritesChangedCallback callback);
	void SetOnError(ErrorCallback callback);
	void SetOnLoadingChanged(LoadingChangedCallback callback);

	[[nodiscard]] const std::vector<ws::models::Favorite>& GetFavorites() const;
	[[nodiscard]] bool IsLoading() const;
	[[nodiscard]] bool IsProductFavorited(int64_t productId) const;

private:
	ws::services::FavoriteService& m_favoriteService;

	std::vector<ws::models::Favorite> m_favorites;
	bool m_isLoading = false;

	FavoritesChangedCallback m_onFavoritesChanged;
	ErrorCallback m_onError;
	LoadingChangedCallback m_onLoadingChanged;
};

} // namespace ws::viewmodels
