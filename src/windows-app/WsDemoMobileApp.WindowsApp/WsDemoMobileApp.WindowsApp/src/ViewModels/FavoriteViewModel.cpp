#include "ViewModels/FavoriteViewModel.h"
#include "Services/FavoriteService.h"

#include <thread>
#include <algorithm>

namespace ws::viewmodels
{

FavoriteViewModel::FavoriteViewModel(ws::services::FavoriteService& favoriteService)
	: m_favoriteService(favoriteService)
{
}

void FavoriteViewModel::FetchFavorites()
{
	m_isLoading = true;
	if (m_onLoadingChanged)
	{
		m_onLoadingChanged(true);
	}

	std::thread([this]()
	{
		auto result = m_favoriteService.FetchFavorites();

		m_isLoading = false;
		if (m_onLoadingChanged)
		{
			m_onLoadingChanged(false);
		}

		if (result.has_value())
		{
			m_favorites = std::move(result.value());
			if (m_onFavoritesChanged)
			{
				m_onFavoritesChanged();
			}
		}
		else
		{
			if (m_onError)
			{
				m_onError(result.error());
			}
		}
	}).detach();
}

void FavoriteViewModel::RemoveFavorite(int64_t productId)
{
	std::thread([this, productId]()
	{
		auto result = m_favoriteService.RemoveFavorite(productId);

		if (result.has_value())
		{
			auto it = std::remove_if(m_favorites.begin(), m_favorites.end(),
				[productId](const ws::models::Favorite& f)
				{
					return f.productId == productId;
				});
			m_favorites.erase(it, m_favorites.end());

			if (m_onFavoritesChanged)
			{
				m_onFavoritesChanged();
			}
		}
		else
		{
			if (m_onError)
			{
				m_onError(result.error());
			}
		}
	}).detach();
}

void FavoriteViewModel::SetOnFavoritesChanged(FavoritesChangedCallback callback)
{
	m_onFavoritesChanged = std::move(callback);
}

void FavoriteViewModel::SetOnError(ErrorCallback callback)
{
	m_onError = std::move(callback);
}

void FavoriteViewModel::SetOnLoadingChanged(LoadingChangedCallback callback)
{
	m_onLoadingChanged = std::move(callback);
}

const std::vector<ws::models::Favorite>& FavoriteViewModel::GetFavorites() const
{
	return m_favorites;
}

bool FavoriteViewModel::IsLoading() const
{
	return m_isLoading;
}

bool FavoriteViewModel::IsProductFavorited(int64_t productId) const
{
	return std::any_of(m_favorites.begin(), m_favorites.end(),
		[productId](const ws::models::Favorite& f)
		{
			return f.productId == productId;
		});
}

} // namespace ws::viewmodels
