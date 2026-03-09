package com.example.admin.action.product;

import com.example.admin.entity.Product;
import com.example.admin.service.ProductService;
import com.opensymphony.xwork2.ActionSupport;

/**
 * 商品詳細（編集フォーム表示） Action。
 * 商品IDを受け取り、編集フォームに商品情報を表示する。
 */
public class ProductDetailAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private ProductService productService;

    // Input
    private Long productId;

    // Output
    private Product product;
    private String successMessage;

    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    @Override
    public String execute() {
        if (productId == null) {
            addActionError("商品IDが指定されていません");
            return ERROR;
        }
        product = productService.getProductById(productId);
        if (product == null) {
            addActionError("商品が見つかりません");
            return ERROR;
        }
        return SUCCESS;
    }

    // Getter/Setter
    public Long getProductId() {
        return productId;
    }

    public void setProductId(Long productId) {
        this.productId = productId;
    }

    public Product getProduct() {
        return product;
    }

    public String getSuccessMessage() {
        return successMessage;
    }

    public void setSuccessMessage(String successMessage) {
        this.successMessage = successMessage;
    }
}
