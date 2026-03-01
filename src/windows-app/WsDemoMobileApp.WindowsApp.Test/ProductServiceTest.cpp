#include "pch.h"
#include "CppUnitTest.h"

#include "Services/ProductService.h"
#include "Services/IHttpClient.h"
#include "Services/IAuthService.h"
#include "Models/Product.h"
#include "Models/Pagination.h"
#include "Models/ApiError.h"

#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <expected>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WsDemoMobileAppWindowsAppTest
{

// ---------------------------------------------------------------
// Mock: IHttpClient
// ---------------------------------------------------------------

class MockHttpClient : public ws::services::IHttpClient
{
public:
mutable std::string lastGetPath;
ws::services::HttpResult getResult;

MockHttpClient()
: getResult(std::unexpected(ws::models::ApiError{"MOCK", "not configured"}))
{
}

void SetGetResponse(int statusCode, const std::string& body)
{
ws::services::HttpResponse resp;
resp.statusCode = statusCode;
resp.body = body;
getResult = resp;
}

void SetGetError(const std::string& code, const std::string& message)
{
getResult = std::unexpected(ws::models::ApiError{code, message});
}

[[nodiscard]] ws::services::HttpResult Get(
const std::string& path,
const std::optional<std::string>& authToken) const override
{
lastGetPath = path;
return getResult;
}

[[nodiscard]] ws::services::HttpResult Post(
const std::string& path,
const std::string& jsonBody,
const std::optional<std::string>& authToken) const override
{
return std::unexpected(ws::models::ApiError{"MOCK", "not implemented"});
}

[[nodiscard]] ws::services::HttpResult Delete(
const std::string& path,
const std::optional<std::string>& authToken) const override
{
return std::unexpected(ws::models::ApiError{"MOCK", "not implemented"});
}
};

// ---------------------------------------------------------------
// Mock: IAuthService
// ---------------------------------------------------------------

class MockAuthService : public ws::services::IAuthService
{
public:
std::string m_token = "test-token-123";
ws::models::User m_user;

[[nodiscard]] std::expected<ws::services::LoginResponse, ws::models::ApiError> Login(
const std::string&, const std::string&) override
{
return std::unexpected(ws::models::ApiError{"MOCK", "not implemented"});
}

void Logout() override {}

[[nodiscard]] const std::string& GetToken() const override { return m_token; }
void SetToken(const std::string& token) override { m_token = token; }
void ClearToken() override { m_token.clear(); }

[[nodiscard]] bool IsAuthenticated() const override { return !m_token.empty(); }
[[nodiscard]] const ws::models::User& GetCurrentUser() const override { return m_user; }

[[nodiscard]] bool IsTokenExpired() const override { return false; }
void SetTokenExpiry(int) override {}
void SetTokenExpiryFromTimestamp(int64_t) override {}
[[nodiscard]] int64_t GetTokenExpiryTimestamp() const override { return 0; }
};

// ---------------------------------------------------------------
// JSON response helpers
// ---------------------------------------------------------------

static std::string MakeProductListJson(int count, int currentPage = 1,
int totalPages = 1, int totalCount = -1, int limit = 20)
{
nlohmann::json products = nlohmann::json::array();
for (int i = 1; i <= count; ++i)
{
products.push_back({
{"productId", i},
{"productName", "Product " + std::to_string(i)},
{"unitPrice", i * 1000},
{"description", "Description " + std::to_string(i)},
{"imageUrl", nullptr},
{"createdAt", "2024-01-01T00:00:00"},
{"updatedAt", "2024-01-01T00:00:00"}
});
}

nlohmann::json data;
data["products"] = products;
data["pagination"] = {
{"currentPage", currentPage},
{"totalPages", totalPages},
{"totalCount", totalCount >= 0 ? totalCount : count},
{"limit", limit}
};

nlohmann::json response;
response["data"] = data;
return response.dump();
}

static std::string MakeProductDetailJson(int64_t id, const std::string& name, int price)
{
nlohmann::json product = {
{"productId", id},
{"productName", name},
{"unitPrice", price},
{"description", "Detail of " + name},
{"imageUrl", nullptr},
{"createdAt", "2024-01-01T00:00:00"},
{"updatedAt", "2024-01-01T00:00:00"}
};

nlohmann::json response;
response["data"] = {{"product", product}};
return response.dump();
}

static std::string MakeErrorJson(const std::string& code, const std::string& message)
{
nlohmann::json response;
response["error"] = {{"code", code}, {"message", message}};
return response.dump();
}

// Valid JSON with no "error" field (triggers fallback error codes)
static std::string MakeNonErrorJson()
{
nlohmann::json response;
response["status"] = "fail";
return response.dump();
}

// ---------------------------------------------------------------
// ProductService tests
// ---------------------------------------------------------------

TEST_CLASS(ProductServiceTest)
{
private:
MockHttpClient m_mockHttp;
MockAuthService m_mockAuth;
std::unique_ptr<ws::services::ProductService> m_service;

public:
TEST_METHOD_INITIALIZE(Setup)
{
m_mockHttp = MockHttpClient();
m_mockAuth = MockAuthService();
m_service = std::make_unique<ws::services::ProductService>(m_mockHttp, m_mockAuth);
}

// ---------------------------------------------------------------
// FetchProducts
// ---------------------------------------------------------------

TEST_METHOD(FetchProducts_Success_ReturnsProducts)
{
m_mockHttp.SetGetResponse(200, MakeProductListJson(3));

auto result = m_service->FetchProducts(1, 20, "createdAt", "desc");

Assert::IsTrue(result.has_value(), L"FetchProducts should succeed");
Assert::AreEqual(static_cast<size_t>(3), result->products.size(),
L"Should return 3 products");
}

TEST_METHOD(FetchProducts_Success_CorrectProductFields)
{
m_mockHttp.SetGetResponse(200, MakeProductListJson(1));

auto result = m_service->FetchProducts();

Assert::IsTrue(result.has_value());
auto& product = result->products[0];
Assert::AreEqual(static_cast<int64_t>(1), product.productId);
Assert::AreEqual(std::string("Product 1"), product.productName);
Assert::AreEqual(1000, product.unitPrice);
Assert::IsTrue(product.description.has_value());
Assert::AreEqual(std::string("Description 1"), product.description.value());
}

TEST_METHOD(FetchProducts_Success_PaginationParsed)
{
m_mockHttp.SetGetResponse(200, MakeProductListJson(5, 2, 10, 100, 10));

auto result = m_service->FetchProducts(2, 10);

Assert::IsTrue(result.has_value());
Assert::AreEqual(2, result->pagination.currentPage);
Assert::AreEqual(10, result->pagination.totalPages);
Assert::AreEqual(100, result->pagination.totalCount);
Assert::AreEqual(10, result->pagination.limit);
}

TEST_METHOD(FetchProducts_Success_BuildsCorrectPath)
{
m_mockHttp.SetGetResponse(200, MakeProductListJson(0));

auto ignored = m_service->FetchProducts(3, 15, "unitPrice", "asc");

std::string expected = "/api/mobile/products?page=3&limit=15&sortBy=unitPrice&sortOrder=asc";
Assert::AreEqual(expected, m_mockHttp.lastGetPath,
L"Should build correct query path");
}

TEST_METHOD(FetchProducts_HttpError_ReturnsApiError)
{
m_mockHttp.SetGetError("NETWORK_ERROR", "Connection refused");

auto result = m_service->FetchProducts();

Assert::IsFalse(result.has_value(), L"Should return error");
Assert::AreEqual(std::string("NETWORK_ERROR"), result.error().code);
}

TEST_METHOD(FetchProducts_ServerError_WithErrorBody)
{
m_mockHttp.SetGetResponse(500, MakeErrorJson("SERVER_ERR", "Internal error"));

auto result = m_service->FetchProducts();

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("SERVER_ERR"), result.error().code);
}

TEST_METHOD(FetchProducts_ServerError_FallbackCode)
{
// Valid JSON without "error" field -> ParseApiError returns nullopt -> fallback
m_mockHttp.SetGetResponse(500, MakeNonErrorJson());

auto result = m_service->FetchProducts();

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("PRODUCT_001"), result.error().code);
}

TEST_METHOD(FetchProducts_InvalidJson_ReturnsParseError)
{
m_mockHttp.SetGetResponse(200, "{invalid json");

auto result = m_service->FetchProducts();

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("JSON_PARSE_ERROR"), result.error().code);
}

// ---------------------------------------------------------------
// SearchProducts
// ---------------------------------------------------------------

TEST_METHOD(SearchProducts_Success_ReturnsProducts)
{
m_mockHttp.SetGetResponse(200, MakeProductListJson(2));

auto result = m_service->SearchProducts("Tea", 1, 10);

Assert::IsTrue(result.has_value());
Assert::AreEqual(static_cast<size_t>(2), result->products.size());
}

TEST_METHOD(SearchProducts_Success_BuildsCorrectPath)
{
m_mockHttp.SetGetResponse(200, MakeProductListJson(0));

auto ignored = m_service->SearchProducts("green tea", 2, 5);

std::string expected = "/api/mobile/products/search?keyword=green tea&page=2&limit=5";
Assert::AreEqual(expected, m_mockHttp.lastGetPath);
}

TEST_METHOD(SearchProducts_HttpError_ReturnsApiError)
{
m_mockHttp.SetGetError("TIMEOUT", "Request timed out");

auto result = m_service->SearchProducts("test");

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("TIMEOUT"), result.error().code);
}

TEST_METHOD(SearchProducts_ServerError_FallbackCode)
{
m_mockHttp.SetGetResponse(403, MakeNonErrorJson());

auto result = m_service->SearchProducts("test");

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("PRODUCT_002"), result.error().code);
}

// ---------------------------------------------------------------
// FetchProductDetail
// ---------------------------------------------------------------

TEST_METHOD(FetchProductDetail_Success_ReturnsProduct)
{
m_mockHttp.SetGetResponse(200, MakeProductDetailJson(42, "Matcha", 5500));

auto result = m_service->FetchProductDetail(42);

Assert::IsTrue(result.has_value());
Assert::AreEqual(static_cast<int64_t>(42), result->productId);
Assert::AreEqual(std::string("Matcha"), result->productName);
Assert::AreEqual(5500, result->unitPrice);
}

TEST_METHOD(FetchProductDetail_Success_BuildsCorrectPath)
{
m_mockHttp.SetGetResponse(200, MakeProductDetailJson(99, "Test", 100));

auto ignored = m_service->FetchProductDetail(99);

Assert::AreEqual(std::string("/api/mobile/products/99"), m_mockHttp.lastGetPath);
}

TEST_METHOD(FetchProductDetail_NotFound_ReturnsApiError)
{
m_mockHttp.SetGetResponse(404, MakeErrorJson("NOT_FOUND", "Product not found"));

auto result = m_service->FetchProductDetail(9999);

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("NOT_FOUND"), result.error().code);
}

TEST_METHOD(FetchProductDetail_ServerError_FallbackCode)
{
m_mockHttp.SetGetResponse(500, MakeNonErrorJson());

auto result = m_service->FetchProductDetail(1);

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("PRODUCT_003"), result.error().code);
}

TEST_METHOD(FetchProductDetail_InvalidJson_ReturnsParseError)
{
m_mockHttp.SetGetResponse(200, "{bad}");

auto result = m_service->FetchProductDetail(1);

Assert::IsFalse(result.has_value());
Assert::AreEqual(std::string("JSON_PARSE_ERROR"), result.error().code);
}

// ---------------------------------------------------------------
// Token propagation
// ---------------------------------------------------------------

TEST_METHOD(FetchProducts_EmptyToken_StillCallsHttp)
{
m_mockAuth.m_token = "";
m_mockHttp.SetGetResponse(200, MakeProductListJson(0));

auto result = m_service->FetchProducts();

Assert::IsTrue(result.has_value(),
L"Should succeed even with empty token (auth is server-side concern)");
}
};

} // namespace WsDemoMobileAppWindowsAppTest
