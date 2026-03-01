#include "ViewModels/ProductDetailViewModel.h"
#include "Services/IProductService.h"
#include "Services/IFavoriteService.h"

#include <thread>

namespace ws::viewmodels
{

ProductDetailViewModel::ProductDetailViewModel(
	ws::services::IProductService& productService,
	ws::services::IFavoriteService& favoriteService)
	: m_productService(productService)
	, m_favoriteService(favoriteService)
{
}

void ProductDetailViewModel::FetchProductDetail(int64_t productId)
{
	m_isLoading = true;
	if (m_onLoadingChanged)
	{
		m_onLoadingChanged(true);
	}

	std::thread([this, productId]()
	{
		auto result = m_productService.FetchProductDetail(productId);

		m_isLoading = false;
		if (m_onLoadingChanged)
		{
			m_onLoadingChanged(false);
		}

		if (result.has_value())
		{
			m_product = std::move(result.value());
			if (m_onProductChanged)
			{
				m_onProductChanged();
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

void ProductDetailViewModel::ToggleFavorite()
{
	if (!m_product.has_value())
	{
		return;
	}

	auto productId = m_product.value().productId;

	std::thread([this, productId]()
	{
		if (m_isFavorite)
		{
			auto result = m_favoriteService.RemoveFavorite(productId);
			if (result.has_value())
			{
				m_isFavorite = false;
				if (m_onFavoriteChanged)
				{
					m_onFavoriteChanged(false);
				}
			}
			else
			{
				if (m_onError)
				{
					m_onError(result.error());
				}
			}
		}
		else
		{
			auto result = m_favoriteService.AddFavorite(productId);
			if (result.has_value())
			{
				m_isFavorite = true;
				if (m_onFavoriteChanged)
				{
					m_onFavoriteChanged(true);
				}
			}
			else
			{
				if (m_onError)
				{
					m_onError(result.error());
				}
			}
		}
	}).detach();
}

void ProductDetailViewModel::SetOnProductChanged(ProductChangedCallback callback)
{
	m_onProductChanged = std::move(callback);
}

void ProductDetailViewModel::SetOnError(ErrorCallback callback)
{
	m_onError = std::move(callback);
}

void ProductDetailViewModel::SetOnLoadingChanged(LoadingChangedCallback callback)
{
	m_onLoadingChanged = std::move(callback);
}

void ProductDetailViewModel::SetOnFavoriteChanged(FavoriteChangedCallback callback)
{
	m_onFavoriteChanged = std::move(callback);
}

const std::optional<ws::models::Product>& ProductDetailViewModel::GetProduct() const
{
	return m_product;
}

bool ProductDetailViewModel::IsFavorite() const
{
	return m_isFavorite;
}

bool ProductDetailViewModel::IsLoading() const
{
	return m_isLoading;
}

void ProductDetailViewModel::SetIsFavorite(bool isFavorite)
{
	m_isFavorite = isFavorite;
}

} // namespace ws::viewmodels
