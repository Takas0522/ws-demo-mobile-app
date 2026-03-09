package com.example.admin.action.product;

import com.example.admin.entity.Product;
import com.example.admin.service.ProductService;
import com.opensymphony.xwork2.ActionSupport;

/**
 * 商品更新 Action。
 * フォーム POST で商品情報を更新する。
 */
public class ProductUpdateAction extends ActionSupport {

    private static final long serialVersionUID = 1L;

    private ProductService productService;

    // Input
    private Long productId;
    private String productName;
    private Integer unitPrice;
    private String description;
    private String imageUrl;

    // Output
    private Product product;

    public void setProductService(ProductService productService) {
        this.productService = productService;
    }

    @Override
    public String execute() {
        // バリデーション
        if (productId == null) {
            addActionError("商品IDが指定されていません");
            return ERROR;
        }
        if (productName == null || productName.trim().isEmpty()) {
            addFieldError("productName", "商品名を入力してください");
        }
        if (unitPrice == null || unitPrice < 1) {
            addFieldError("unitPrice", "単価は1以上の整数を入力してください");
        }
        if (productName != null && productName.length() > 100) {
            addFieldError("productName", "商品名は100文字以内で入力してください");
        }
        if (description != null && description.length() > 500) {
            addFieldError("description", "説明は500文字以内で入力してください");
        }

        if (hasFieldErrors()) {
            // バリデーションエラー時は現在の商品情報をロードして編集フォームを再表示
            product = productService.getProductById(productId);
            return INPUT;
        }

        // 更新
        Product updateProduct = new Product();
        updateProduct.setProductId(productId);
        updateProduct.setProductName(productName.trim());
        updateProduct.setUnitPrice(unitPrice);
        updateProduct.setDescription(description != null ? description.trim() : null);
        updateProduct.setImageUrl(imageUrl != null ? imageUrl.trim() : null);

        productService.updateProduct(updateProduct);

        return SUCCESS;
    }

    // Getter/Setter
    public Long getProductId() {
        return productId;
    }

    public void setProductId(Long productId) {
        this.productId = productId;
    }

    public String getProductName() {
        return productName;
    }

    public void setProductName(String productName) {
        this.productName = productName;
    }

    public Integer getUnitPrice() {
        return unitPrice;
    }

    public void setUnitPrice(Integer unitPrice) {
        this.unitPrice = unitPrice;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getImageUrl() {
        return imageUrl;
    }

    public void setImageUrl(String imageUrl) {
        this.imageUrl = imageUrl;
    }

    public Product getProduct() {
        return product;
    }
}
