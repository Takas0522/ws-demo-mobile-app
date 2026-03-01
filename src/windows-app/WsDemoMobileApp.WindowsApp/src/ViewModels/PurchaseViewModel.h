#pragma once

#include <functional>
#include "Models/Product.h"
#include "Models/Purchase.h"
#include "Models/ApiError.h"
#include "Utils/Constants.h"

namespace ws::services
{
class PurchaseService;
}

namespace ws::viewmodels
{

class PurchaseViewModel
{
public:
	using PurchaseSuccessCallback = std::function<void(const ws::models::Purchase&)>;
	using ErrorCallback = std::function<void(const ws::models::ApiError&)>;
	using LoadingChangedCallback = std::function<void(bool)>;
	using QuantityChangedCallback = std::function<void(int, int)>;

	explicit PurchaseViewModel(ws::services::PurchaseService& purchaseService);
	~PurchaseViewModel() = default;

	PurchaseViewModel(const PurchaseViewModel&) = delete;
	PurchaseViewModel& operator=(const PurchaseViewModel&) = delete;

	void SetProduct(const ws::models::Product& product);
	void IncrementQuantity();
	void DecrementQuantity();
	void SetQuantity(int quantity);
	void ConfirmPurchase();

	void SetOnPurchaseSuccess(PurchaseSuccessCallback callback);
	void SetOnError(ErrorCallback callback);
	void SetOnLoadingChanged(LoadingChangedCallback callback);
	void SetOnQuantityChanged(QuantityChangedCallback callback);

	[[nodiscard]] int GetQuantity() const;
	[[nodiscard]] int GetTotalAmount() const;
	[[nodiscard]] const ws::models::Product& GetProduct() const;
	[[nodiscard]] bool IsLoading() const;

private:
	ws::services::PurchaseService& m_purchaseService;

	ws::models::Product m_product;
	int m_quantity = ws::utils::kMinQuantity;
	bool m_isLoading = false;

	PurchaseSuccessCallback m_onPurchaseSuccess;
	ErrorCallback m_onError;
	LoadingChangedCallback m_onLoadingChanged;
	QuantityChangedCallback m_onQuantityChanged;
};

} // namespace ws::viewmodels
