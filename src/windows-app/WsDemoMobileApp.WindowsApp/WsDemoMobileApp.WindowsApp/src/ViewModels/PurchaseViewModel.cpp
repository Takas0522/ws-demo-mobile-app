#include "ViewModels/PurchaseViewModel.h"
#include "Services/PurchaseService.h"

#include <thread>
#include <algorithm>

namespace ws::viewmodels
{

PurchaseViewModel::PurchaseViewModel(ws::services::PurchaseService& purchaseService)
	: m_purchaseService(purchaseService)
{
}

void PurchaseViewModel::SetProduct(const ws::models::Product& product)
{
	m_product = product;
	m_quantity = ws::utils::kMinQuantity;
}

void PurchaseViewModel::IncrementQuantity()
{
	int newQuantity = std::min(m_quantity + ws::utils::kQuantityStep, ws::utils::kMaxQuantity);
	if (newQuantity != m_quantity)
	{
		m_quantity = newQuantity;
		if (m_onQuantityChanged)
		{
			m_onQuantityChanged(m_quantity, GetTotalAmount());
		}
	}
}

void PurchaseViewModel::DecrementQuantity()
{
	int newQuantity = std::max(m_quantity - ws::utils::kQuantityStep, ws::utils::kMinQuantity);
	if (newQuantity != m_quantity)
	{
		m_quantity = newQuantity;
		if (m_onQuantityChanged)
		{
			m_onQuantityChanged(m_quantity, GetTotalAmount());
		}
	}
}

void PurchaseViewModel::SetQuantity(int quantity)
{
	quantity = std::clamp(quantity, ws::utils::kMinQuantity, ws::utils::kMaxQuantity);
	quantity = ((quantity + ws::utils::kQuantityStep / 2) / ws::utils::kQuantityStep) * ws::utils::kQuantityStep;
	quantity = std::clamp(quantity, ws::utils::kMinQuantity, ws::utils::kMaxQuantity);

	if (quantity != m_quantity)
	{
		m_quantity = quantity;
		if (m_onQuantityChanged)
		{
			m_onQuantityChanged(m_quantity, GetTotalAmount());
		}
	}
}

void PurchaseViewModel::ConfirmPurchase()
{
	m_isLoading = true;
	if (m_onLoadingChanged)
	{
		m_onLoadingChanged(true);
	}

	std::thread([this]()
	{
		auto result = m_purchaseService.CreatePurchase(m_product.productId, m_quantity);

		m_isLoading = false;
		if (m_onLoadingChanged)
		{
			m_onLoadingChanged(false);
		}

		if (result.has_value())
		{
			if (m_onPurchaseSuccess)
			{
				m_onPurchaseSuccess(result.value());
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

void PurchaseViewModel::SetOnPurchaseSuccess(PurchaseSuccessCallback callback)
{
	m_onPurchaseSuccess = std::move(callback);
}

void PurchaseViewModel::SetOnError(ErrorCallback callback)
{
	m_onError = std::move(callback);
}

void PurchaseViewModel::SetOnLoadingChanged(LoadingChangedCallback callback)
{
	m_onLoadingChanged = std::move(callback);
}

void PurchaseViewModel::SetOnQuantityChanged(QuantityChangedCallback callback)
{
	m_onQuantityChanged = std::move(callback);
}

int PurchaseViewModel::GetQuantity() const
{
	return m_quantity;
}

int PurchaseViewModel::GetTotalAmount() const
{
	return m_product.unitPrice * m_quantity;
}

const ws::models::Product& PurchaseViewModel::GetProduct() const
{
	return m_product;
}

bool PurchaseViewModel::IsLoading() const
{
	return m_isLoading;
}

} // namespace ws::viewmodels
