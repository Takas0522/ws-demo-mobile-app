#include "ViewModels/ProductListViewModel.h"
#include "Services/ProductService.h"

#include <thread>

namespace ws::viewmodels
{

ProductListViewModel::ProductListViewModel(ws::services::ProductService& productService)
	: m_productService(productService)
{
}

void ProductListViewModel::FetchProducts(int page)
{
	m_isLoading = true;
	m_searchKeyword.clear();
	if (m_onLoadingChanged)
	{
		m_onLoadingChanged(true);
	}

	std::thread([this, page]()
	{
		auto result = m_productService.FetchProducts(page);

		m_isLoading = false;
		if (m_onLoadingChanged)
		{
			m_onLoadingChanged(false);
		}

		if (result.has_value())
		{
			auto& response = result.value();
			if (page == 1)
			{
				m_products = std::move(response.products);
			}
			else
			{
				m_products.insert(
					m_products.end(),
					std::make_move_iterator(response.products.begin()),
					std::make_move_iterator(response.products.end()));
			}
			m_pagination = response.pagination;

			if (m_onProductsChanged)
			{
				m_onProductsChanged();
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

void ProductListViewModel::SearchProducts(const std::string& keyword, int page)
{
	m_isLoading = true;
	m_searchKeyword = keyword;
	if (m_onLoadingChanged)
	{
		m_onLoadingChanged(true);
	}

	std::thread([this, keyword, page]()
	{
		auto result = m_productService.SearchProducts(keyword, page);

		m_isLoading = false;
		if (m_onLoadingChanged)
		{
			m_onLoadingChanged(false);
		}

		if (result.has_value())
		{
			auto& response = result.value();
			if (page == 1)
			{
				m_products = std::move(response.products);
			}
			else
			{
				m_products.insert(
					m_products.end(),
					std::make_move_iterator(response.products.begin()),
					std::make_move_iterator(response.products.end()));
			}
			m_pagination = response.pagination;

			if (m_onProductsChanged)
			{
				m_onProductsChanged();
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

void ProductListViewModel::LoadNextPage()
{
	if (m_isLoading || !m_pagination.HasNextPage())
	{
		return;
	}

	int nextPage = m_pagination.currentPage + 1;

	if (m_searchKeyword.empty())
	{
		FetchProducts(nextPage);
	}
	else
	{
		SearchProducts(m_searchKeyword, nextPage);
	}
}

void ProductListViewModel::SetOnProductsChanged(ProductsChangedCallback callback)
{
	m_onProductsChanged = std::move(callback);
}

void ProductListViewModel::SetOnError(ErrorCallback callback)
{
	m_onError = std::move(callback);
}

void ProductListViewModel::SetOnLoadingChanged(LoadingChangedCallback callback)
{
	m_onLoadingChanged = std::move(callback);
}

const std::vector<ws::models::Product>& ProductListViewModel::GetProducts() const
{
	return m_products;
}

const ws::models::Pagination& ProductListViewModel::GetPagination() const
{
	return m_pagination;
}

bool ProductListViewModel::IsLoading() const
{
	return m_isLoading;
}

const std::string& ProductListViewModel::GetSearchKeyword() const
{
	return m_searchKeyword;
}

} // namespace ws::viewmodels
