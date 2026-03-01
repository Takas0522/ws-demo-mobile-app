#pragma once

#include <httplib.h>

namespace ws::services { class ProductService; }

namespace ws::routes
{

class ProductRoutes
{
public:
	explicit ProductRoutes(ws::services::ProductService& productService);

	ProductRoutes(const ProductRoutes&) = delete;
	ProductRoutes& operator=(const ProductRoutes&) = delete;

	void Register(httplib::Server& server);

private:
	void HandleGetProducts(const httplib::Request& req, httplib::Response& res);
	void HandleSearchProducts(const httplib::Request& req, httplib::Response& res);
	void HandleGetProductDetail(const httplib::Request& req, httplib::Response& res);

	ws::services::ProductService& m_productService;
};

} // namespace ws::routes
