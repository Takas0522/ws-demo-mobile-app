#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Models/Product.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

namespace ws::services
{
class ProductService;
}

namespace ws::viewmodels
{

class ProductListViewModel
{
public:
	using ProductsChangedCallback = std::function<void()>;
	using ErrorCallback = std::function<void(const ws::models::ApiError&)>;
	using LoadingChangedCallback = std::function<void(bool)>;

	explicit ProductListViewModel(ws::services::ProductService& productService);
	~ProductListViewModel() = default;

	ProductListViewModel(const ProductListViewModel&) = delete;
	ProductListViewModel& operator=(const ProductListViewModel&) = delete;

	void FetchProducts(int page = 1);
	void SearchProducts(const std::string& keyword, int page = 1);
	void LoadNextPage();

	void SetOnProductsChanged(ProductsChangedCallback callback);
	void SetOnError(ErrorCallback callback);
	void SetOnLoadingChanged(LoadingChangedCallback callback);

	[[nodiscard]] const std::vector<ws::models::Product>& GetProducts() const;
	[[nodiscard]] const ws::models::Pagination& GetPagination() const;
	[[nodiscard]] bool IsLoading() const;
	[[nodiscard]] const std::string& GetSearchKeyword() const;

private:
	ws::services::ProductService& m_productService;

	std::vector<ws::models::Product> m_products;
	ws::models::Pagination m_pagination;
	std::string m_searchKeyword;
	bool m_isLoading = false;

	ProductsChangedCallback m_onProductsChanged;
	ErrorCallback m_onError;
	LoadingChangedCallback m_onLoadingChanged;
};

} // namespace ws::viewmodels
