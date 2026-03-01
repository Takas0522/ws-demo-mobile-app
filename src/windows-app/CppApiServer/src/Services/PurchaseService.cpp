#include "Services/PurchaseService.h"

#include "Database/PurchaseRepository.h"
#include "Database/ProductRepository.h"
#include "Utils/Constants.h"
#include "Utils/UuidGenerator.h"

namespace ws::services
{

PurchaseService::PurchaseService(
	ws::database::PurchaseRepository& purchaseRepo,
	ws::database::ProductRepository& productRepo)
	: m_purchaseRepo(purchaseRepo)
	, m_productRepo(productRepo)
{
}

std::expected<ws::models::Purchase, std::string> PurchaseService::CreatePurchase(
	int64_t userId, int64_t productId, int quantity)
{
	// Validate quantity: must be positive
	if (quantity <= 0)
	{
		return std::unexpected("Quantity must be a positive number");
	}

	// Validate quantity: must be a multiple of kQuantityStep (100)
	if (quantity % ws::utils::kQuantityStep != 0)
	{
		return std::unexpected(
			"Quantity must be a multiple of " + std::to_string(ws::utils::kQuantityStep));
	}

	// Validate quantity: must not exceed kMaxQuantity (9900)
	if (quantity > ws::utils::kMaxQuantity)
	{
		return std::unexpected(
			"Quantity must not exceed " + std::to_string(ws::utils::kMaxQuantity));
	}

	// Look up the product
	auto productOpt = m_productRepo.FindById(productId);
	if (!productOpt.has_value())
	{
		return std::unexpected("Product not found");
	}

	const auto& product = productOpt.value();
	int totalAmount = product.unitPrice * quantity;
	std::string purchaseId = ws::utils::GenerateUuid();

	return m_purchaseRepo.Create(
		purchaseId, userId, productId,
		quantity, product.unitPrice, totalAmount);
}

std::vector<ws::models::Purchase> PurchaseService::GetPurchasesByUserId(int64_t userId)
{
	return m_purchaseRepo.FindByUserId(userId);
}

} // namespace ws::services
