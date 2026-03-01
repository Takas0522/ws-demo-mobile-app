#pragma once

#include <functional>
#include <optional>
#include "Models/Product.h"
#include "Models/ApiError.h"

namespace ws::services
{
class ProductService;
class FavoriteService;
}

namespace ws::viewmodels
{

class ProductDetailViewModel
{
public:
	using ProductChangedCallback = std::function<void()>;
	using ErrorCallback = std::function<void(const ws::models::ApiError&)>;
	using LoadingChangedCallback = std::function<void(bool)>;
	using FavoriteChangedCallback = std::function<void(bool)>;

	ProductDetailViewModel(
		ws::services::ProductService& productService,
		ws::services::FavoriteService& favoriteService);
	~ProductDetailViewModel() = default;

	ProductDetailViewModel(const ProductDetailViewModel&) = delete;
	ProductDetailViewModel& operator=(const ProductDetailViewModel&) = delete;

	void FetchProductDetail(int64_t productId);
	void ToggleFavorite();

	void SetOnProductChanged(ProductChangedCallback callback);
	void SetOnError(ErrorCallback callback);
	void SetOnLoadingChanged(LoadingChangedCallback callback);
	void SetOnFavoriteChanged(FavoriteChangedCallback callback);

	[[nodiscard]] const std::optional<ws::models::Product>& GetProduct() const;
	[[nodiscard]] bool IsFavorite() const;
	[[nodiscard]] bool IsLoading() const;

	void SetIsFavorite(bool isFavorite);

private:
	ws::services::ProductService& m_productService;
	ws::services::FavoriteService& m_favoriteService;

	std::optional<ws::models::Product> m_product;
	bool m_isFavorite = false;
	bool m_isLoading = false;

	ProductChangedCallback m_onProductChanged;
	ErrorCallback m_onError;
	LoadingChangedCallback m_onLoadingChanged;
	FavoriteChangedCallback m_onFavoriteChanged;
};

} // namespace ws::viewmodels
