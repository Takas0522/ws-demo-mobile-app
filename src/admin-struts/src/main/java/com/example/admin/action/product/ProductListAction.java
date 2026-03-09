package com.example.admin.action.product;

import com.example.admin.entity.Product;
import com.example.admin.service.ProductService;
import com.opensymphony.xwork2.ActionSupport;

import java.util.List;

/**
 * 商品一覧 Action。
 * 全商品を取得して一覧画面を表示する。
 */
public class ProductListAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private ProductService productService;

    // Output
    private List<Product> products;

    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    @Override
    public String execute() {
        products = productService.getProducts();
        return SUCCESS;
    }

    public List<Product> getProducts() {
        return products;
    }
}
