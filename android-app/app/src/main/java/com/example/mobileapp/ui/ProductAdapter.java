package com.example.mobileapp.ui;

import android.view.LayoutInflater;
import android.view.ViewGroup;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;
import com.example.mobileapp.databinding.ItemProductBinding;
import com.example.mobileapp.models.Product;

public class ProductAdapter extends ListAdapter<Product, ProductAdapter.ProductViewHolder> {
    
    public interface OnItemClickListener {
        void onItemClick(Product product);
    }
    
    private final OnItemClickListener onItemClickListener;
    
    public ProductAdapter(OnItemClickListener onItemClickListener) {
        super(new ProductDiffCallback());
        this.onItemClickListener = onItemClickListener;
    }
    
    @NonNull
    @Override
    public ProductViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        ItemProductBinding binding = ItemProductBinding.inflate(
            LayoutInflater.from(parent.getContext()),
            parent,
            false
        );
        return new ProductViewHolder(binding, onItemClickListener);
    }
    
    @Override
    public void onBindViewHolder(@NonNull ProductViewHolder holder, int position) {
        holder.bind(getItem(position));
    }
    
    static class ProductViewHolder extends RecyclerView.ViewHolder {
        private final ItemProductBinding binding;
        private final OnItemClickListener onItemClickListener;
        
        ProductViewHolder(ItemProductBinding binding, OnItemClickListener onItemClickListener) {
            super(binding.getRoot());
            this.binding = binding;
            this.onItemClickListener = onItemClickListener;
        }
        
        void bind(Product product) {
            binding.tvProductName.setText(product.getProductName());
            binding.tvUnitPrice.setText("¥" + product.getUnitPrice());
            
            binding.getRoot().setOnClickListener(v -> {
                if (onItemClickListener != null) {
                    onItemClickListener.onItemClick(product);
                }
            });
        }
    }
    
    static class ProductDiffCallback extends DiffUtil.ItemCallback<Product> {
        @Override
        public boolean areItemsTheSame(@NonNull Product oldItem, @NonNull Product newItem) {
            return oldItem.getProductId() == newItem.getProductId();
        }
        
        @Override
        public boolean areContentsTheSame(@NonNull Product oldItem, @NonNull Product newItem) {
            return oldItem.getProductId() == newItem.getProductId() &&
                   oldItem.getProductName().equals(newItem.getProductName()) &&
                   oldItem.getUnitPrice() == newItem.getUnitPrice();
        }
    }
}
