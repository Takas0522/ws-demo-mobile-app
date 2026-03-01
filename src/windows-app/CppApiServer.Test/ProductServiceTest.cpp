#include "pch.h"
#include "CppUnitTest.h"

#include "Services/ProductService.h"
#include "Database/IProductRepository.h"
#include "Models/Product.h"

#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <vector>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CppApiServerTest
{

// ---------------------------------------------------------------
// MockProductRepository — IProductRepository のモック実装
// ---------------------------------------------------------------
class MockProductRepository : public ws::database::IProductRepository
{
public:
    std::vector<ws::models::Product> m_allProducts;

    static ws::models::Product MakeProduct(int64_t id, const std::string& name,
        int price, const std::string& description)
    {
        ws::models::Product p;
        p.productId = id;
        p.productName = name;
        p.unitPrice = price;
        p.description = description;
        p.imageUrl = std::nullopt;
        p.createdAt = "2025-01-01 00:00:00";
        p.updatedAt = "2025-01-01 00:00:00";
        return p;
    }

    void SeedDefaultProducts()
    {
        m_allProducts = {
            MakeProduct(1, "Green Tea", 1000, "Organic green tea"),
            MakeProduct(2, "Honey", 1500, "Natural honey"),
            MakeProduct(3, "Darjeeling Tea", 2000, "First flush darjeeling"),
            MakeProduct(4, "Flour", 2500, "Hokkaido wheat flour"),
            MakeProduct(5, "Olive Oil", 3000, "Extra virgin olive oil"),
        };
    }

    ws::database::ProductListResult FindAll(int page, int limit,
        const std::string& /*sortBy*/, const std::string& /*sortOrder*/) override
    {
        ws::database::ProductListResult result;
        result.totalCount = static_cast<int>(m_allProducts.size());

        int offset = (page - 1) * limit;
        for (int i = offset; i < static_cast<int>(m_allProducts.size()) && i < offset + limit; ++i)
        {
            result.products.push_back(m_allProducts[i]);
        }
        return result;
    }

    ws::database::ProductListResult SearchByKeyword(const std::string& keyword,
        int page, int limit) override
    {
        std::vector<ws::models::Product> matched;
        std::string lowerKeyword = keyword;
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

        for (const auto& p : m_allProducts)
        {
            std::string lowerName = p.productName;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::string lowerDesc = p.description.value_or("");
            std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);

            if (lowerName.find(lowerKeyword) != std::string::npos ||
                lowerDesc.find(lowerKeyword) != std::string::npos)
            {
                matched.push_back(p);
            }
        }

        ws::database::ProductListResult result;
        result.totalCount = static_cast<int>(matched.size());

        int offset = (page - 1) * limit;
        for (int i = offset; i < static_cast<int>(matched.size()) && i < offset + limit; ++i)
        {
            result.products.push_back(matched[i]);
        }
        return result;
    }

    std::optional<ws::models::Product> FindById(int64_t productId) override
    {
        for (const auto& p : m_allProducts)
        {
            if (p.productId == productId)
            {
                return p;
            }
        }
        return std::nullopt;
    }
};

TEST_CLASS(ProductServiceTest)
{
private:
    std::unique_ptr<MockProductRepository> m_mockRepo;
    std::unique_ptr<ws::services::ProductService> m_productService;

public:
    TEST_METHOD_INITIALIZE(Setup)
    {
        m_mockRepo = std::make_unique<MockProductRepository>();
        m_mockRepo->SeedDefaultProducts();
        m_productService = std::make_unique<ws::services::ProductService>(*m_mockRepo);
    }

    TEST_METHOD_CLEANUP(Teardown)
    {
        m_productService.reset();
        m_mockRepo.reset();
    }

    // ---------------------------------------------------------------
    // GetProducts
    // ---------------------------------------------------------------

    TEST_METHOD(GetProducts_ReturnsAllProducts)
    {
        auto data = m_productService->GetProducts(1, 10, "created_at", "DESC");

        Assert::IsTrue(data.contains("products"), L"Response should contain 'products' key");
        Assert::IsTrue(data.contains("pagination"), L"Response should contain 'pagination' key");

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(5), products.size(),
            L"Should return all 5 products");
    }

    TEST_METHOD(GetProducts_PaginationFirstPage)
    {
        auto data = m_productService->GetProducts(1, 2, "product_id", "ASC");

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(2), products.size(),
            L"First page should return 2 products");

        auto& pagination = data["pagination"];
        Assert::AreEqual(1, pagination["currentPage"].get<int>());
        Assert::AreEqual(2, pagination["pageSize"].get<int>());
        Assert::AreEqual(5, pagination["totalCount"].get<int>());
        Assert::AreEqual(3, pagination["totalPages"].get<int>());
    }

    TEST_METHOD(GetProducts_PaginationLastPage)
    {
        auto data = m_productService->GetProducts(3, 2, "product_id", "ASC");

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(1), products.size(),
            L"Last page should return 1 product");
    }

    TEST_METHOD(GetProducts_EmptyPageReturnsNoProducts)
    {
        auto data = m_productService->GetProducts(100, 10, "created_at", "DESC");

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(0), products.size(),
            L"Page beyond range should return 0 products");

        auto& pagination = data["pagination"];
        Assert::AreEqual(5, pagination["totalCount"].get<int>(),
            L"Total count should still be 5");
    }

    TEST_METHOD(GetProducts_JsonStructure)
    {
        auto data = m_productService->GetProducts(1, 1, "product_id", "ASC");

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(1), products.size());

        auto& product = products[0];
        Assert::IsTrue(product.contains("productId"), L"Should have productId");
        Assert::IsTrue(product.contains("productName"), L"Should have productName");
        Assert::IsTrue(product.contains("unitPrice"), L"Should have unitPrice");
        Assert::IsTrue(product.contains("description"), L"Should have description");
        Assert::IsTrue(product.contains("createdAt"), L"Should have createdAt");
        Assert::IsTrue(product.contains("updatedAt"), L"Should have updatedAt");
    }

    TEST_METHOD(GetProducts_EmptyRepository)
    {
        m_mockRepo->m_allProducts.clear();
        auto data = m_productService->GetProducts(1, 10, "created_at", "DESC");

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(0), products.size(),
            L"Empty repository should return 0 products");

        auto& pagination = data["pagination"];
        Assert::AreEqual(0, pagination["totalCount"].get<int>());
        Assert::AreEqual(0, pagination["totalPages"].get<int>());
    }

    // ---------------------------------------------------------------
    // SearchProducts
    // ---------------------------------------------------------------

    TEST_METHOD(SearchProducts_FindsByKeyword)
    {
        auto data = m_productService->SearchProducts("Tea", 1, 10);

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(2), products.size(),
            L"Should find 2 products containing 'Tea'");
        Assert::AreEqual(std::string("Tea"), data["keyword"].get<std::string>(),
            L"Response should echo the keyword");
    }

    TEST_METHOD(SearchProducts_CaseInsensitive)
    {
        auto data = m_productService->SearchProducts("tea", 1, 10);

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(2), products.size(),
            L"Search should be case-insensitive");
    }

    TEST_METHOD(SearchProducts_NoMatch)
    {
        auto data = m_productService->SearchProducts("NonExistentProduct", 1, 10);

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(0), products.size(),
            L"Should return 0 products for no match");

        auto& pagination = data["pagination"];
        Assert::AreEqual(0, pagination["totalCount"].get<int>());
        Assert::AreEqual(0, pagination["totalPages"].get<int>());
    }

    TEST_METHOD(SearchProducts_SearchByDescription)
    {
        auto data = m_productService->SearchProducts("olive", 1, 10);

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(1), products.size(),
            L"Should find product by description keyword");
    }

    TEST_METHOD(SearchProducts_PaginationWorks)
    {
        auto data = m_productService->SearchProducts("Tea", 1, 1);

        auto& products = data["products"];
        Assert::AreEqual(static_cast<size_t>(1), products.size(),
            L"Should return 1 product per page");

        auto& pagination = data["pagination"];
        Assert::AreEqual(2, pagination["totalCount"].get<int>());
        Assert::AreEqual(2, pagination["totalPages"].get<int>());
    }

    // ---------------------------------------------------------------
    // GetProductById
    // ---------------------------------------------------------------

    TEST_METHOD(GetProductById_ReturnsProduct)
    {
        auto result = m_productService->GetProductById(1);

        Assert::IsTrue(result.has_value(), L"Should return a product for valid ID");
        Assert::AreEqual(static_cast<int64_t>(1), result->productId);
        Assert::AreEqual(std::string("Green Tea"), result->productName);
        Assert::AreEqual(1000, result->unitPrice);
    }

    TEST_METHOD(GetProductById_ReturnsNulloptForInvalidId)
    {
        auto result = m_productService->GetProductById(9999);

        Assert::IsFalse(result.has_value(),
            L"Should return nullopt for non-existent product");
    }

    TEST_METHOD(GetProductById_CorrectDescription)
    {
        auto result = m_productService->GetProductById(3);

        Assert::IsTrue(result.has_value());
        Assert::AreEqual(std::string("Darjeeling Tea"), result->productName);
        Assert::IsTrue(result->description.has_value(),
            L"Description should have a value");
        Assert::AreEqual(std::string("First flush darjeeling"),
            result->description.value());
    }

    // ---------------------------------------------------------------
    // totalPages
    // ---------------------------------------------------------------

    TEST_METHOD(GetProducts_TotalPagesCalculation)
    {
        auto data = m_productService->GetProducts(1, 3, "product_id", "ASC");

        auto& pagination = data["pagination"];
        Assert::AreEqual(2, pagination["totalPages"].get<int>(),
            L"5 items / 3 per page should give 2 total pages");
    }
};

} // namespace CppApiServerTest
