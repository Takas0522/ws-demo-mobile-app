package com.example.mobileapp.models;

import java.util.List;

public class ProductListData {
    private List<Product> products;
    private Object pagination;

    public List<Product> getProducts() {
        return products;
    }

    public void setProducts(List<Product> products) {
        this.products = products;
    }
}
