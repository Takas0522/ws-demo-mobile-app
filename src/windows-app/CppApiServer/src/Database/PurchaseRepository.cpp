#include "Database/PurchaseRepository.h"
#include "Database/DatabaseManager.h"
#include <sqlite3.h>
#include <mutex>

namespace ws::database
{

PurchaseRepository::PurchaseRepository(DatabaseManager& dbManager)
	: m_dbManager(dbManager)
{
}

std::expected<ws::models::Purchase, std::string> PurchaseRepository::Create(
	const std::string& purchaseId, int64_t userId, int64_t productId,
	int quantity, int unitPrice, int totalAmount)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"INSERT INTO purchases (purchase_id, user_id, product_id, quantity, unit_price_at_purchase, total_amount) "
		"VALUES (?, ?, ?, ?, ?, ?);";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::unexpected(std::string("Failed to prepare INSERT: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	sqlite3_bind_text(stmt, 1, purchaseId.c_str(), static_cast<int>(purchaseId.size()), SQLITE_STATIC);
	sqlite3_bind_int64(stmt, 2, userId);
	sqlite3_bind_int64(stmt, 3, productId);
	sqlite3_bind_int(stmt, 4, quantity);
	sqlite3_bind_int(stmt, 5, unitPrice);
	sqlite3_bind_int(stmt, 6, totalAmount);

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE)
	{
		return std::unexpected(std::string("Failed to insert purchase: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	// Retrieve the created purchase with product name
	const char* selectSql =
		"SELECT p.purchase_id, p.user_id, p.product_id, pr.product_name, "
		"p.quantity, p.unit_price_at_purchase, p.total_amount, p.purchased_at "
		"FROM purchases p "
		"INNER JOIN products pr ON p.product_id = pr.product_id "
		"WHERE p.purchase_id = ?;";

	sqlite3_stmt* selectStmt = nullptr;
	rc = sqlite3_prepare_v2(m_dbManager.GetDb(), selectSql, -1, &selectStmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return std::unexpected(std::string("Failed to retrieve created purchase: ") + sqlite3_errmsg(m_dbManager.GetDb()));
	}

	sqlite3_bind_text(selectStmt, 1, purchaseId.c_str(), static_cast<int>(purchaseId.size()), SQLITE_STATIC);

	if (sqlite3_step(selectStmt) != SQLITE_ROW)
	{
		sqlite3_finalize(selectStmt);
		return std::unexpected("Created purchase not found after insert");
	}

	ws::models::Purchase purchase;
	purchase.purchaseId = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 0));
	purchase.userId = sqlite3_column_int64(selectStmt, 1);
	purchase.productId = sqlite3_column_int64(selectStmt, 2);
	purchase.productName = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 3));
	purchase.quantity = sqlite3_column_int(selectStmt, 4);
	purchase.unitPriceAtPurchase = sqlite3_column_int(selectStmt, 5);
	purchase.totalAmount = sqlite3_column_int(selectStmt, 6);
	purchase.purchasedAt = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 7));

	sqlite3_finalize(selectStmt);
	return purchase;
}

std::vector<ws::models::Purchase> PurchaseRepository::FindByUserId(int64_t userId)
{
	std::lock_guard<std::mutex> lock(m_dbManager.GetMutex());

	const char* sql =
		"SELECT p.purchase_id, p.user_id, p.product_id, pr.product_name, "
		"p.quantity, p.unit_price_at_purchase, p.total_amount, p.purchased_at "
		"FROM purchases p "
		"INNER JOIN products pr ON p.product_id = pr.product_id "
		"WHERE p.user_id = ? "
		"ORDER BY p.purchased_at DESC;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_dbManager.GetDb(), sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return {};
	}

	sqlite3_bind_int64(stmt, 1, userId);

	std::vector<ws::models::Purchase> purchases;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ws::models::Purchase purchase;
		purchase.purchaseId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		purchase.userId = sqlite3_column_int64(stmt, 1);
		purchase.productId = sqlite3_column_int64(stmt, 2);
		purchase.productName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		purchase.quantity = sqlite3_column_int(stmt, 4);
		purchase.unitPriceAtPurchase = sqlite3_column_int(stmt, 5);
		purchase.totalAmount = sqlite3_column_int(stmt, 6);
		purchase.purchasedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
		purchases.push_back(purchase);
	}

	sqlite3_finalize(stmt);
	return purchases;
}

} // namespace ws::database
