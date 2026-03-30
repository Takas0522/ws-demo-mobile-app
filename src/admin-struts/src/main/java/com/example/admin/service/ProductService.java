package com.example.admin.service;

import com.example.admin.dao.ProductDao;
import com.example.admin.dao.ProductPriceHistoryDao;
import com.example.admin.dao.UserDao;
import com.example.admin.dto.PaginationDto;
import com.example.admin.entity.Product;
import com.example.admin.entity.ProductPriceHistory;
import com.example.admin.entity.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class ProductService {

    private static final Logger logger = LoggerFactory.getLogger(ProductService.class);

    private ProductDao productDao;
    private ProductPriceHistoryDao priceHistoryDao;
    private UserDao userDao;

    @Autowired
    public void setProductDao(ProductDao productDao) {
        this.productDao = productDao;
    }

    @Autowired
    public void setProductPriceHistoryDao(ProductPriceHistoryDao priceHistoryDao) {
        this.priceHistoryDao = priceHistoryDao;
    }

    @Autowired
    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    /**
     * 全商品を取得する。
     * @return 商品リスト
     */
    public List<Product> getProducts() {
        try {
            return productDao.findAll();
        } catch (SQLException e) {
            logger.error("Failed to get products", e);
            throw new RuntimeException("商品一覧の取得に失敗しました", e);
        }
    }

    /**
     * キーワードで商品を検索する。
     * @param keyword 検索キーワード
     * @return 検索結果の商品リスト
     */
    public List<Product> searchProducts(String keyword) {
        try {
            return productDao.searchByKeyword(keyword);
        } catch (SQLException e) {
            logger.error("Failed to search products with keyword: {}", keyword, e);
            throw new RuntimeException("商品検索に失敗しました", e);
        }
    }

    /**
     * 商品IDで商品を取得する。
     * @param productId 商品ID
     * @return 商品、見つからない場合は null
     */
    public Product getProductById(Long productId) {
        try {
            return productDao.findById(productId);
        } catch (SQLException e) {
            logger.error("Failed to get product by id: {}", productId, e);
            throw new RuntimeException("商品の取得に失敗しました", e);
        }
    }

    /**
     * 商品を更新する。
     * 価格変更時は DB トリガー (trg_record_price_history) が自動的に履歴を記録する。
     * ただし products テーブルに updated_by カラムが存在しないため、
     * トリガーの changed_by は NULL になる可能性がある点に注意。
     * @param product 更新する商品情報
     */
    public void updateProduct(Product product) {
        try {
            productDao.update(product);
        } catch (SQLException e) {
            logger.error("Failed to update product: {}", product.getProductId(), e);
            throw new RuntimeException("商品の更新に失敗しました", e);
        }
    }

    /**
     * API 用商品更新。個別パラメータで更新し、更新後の商品を返す。
     *
     * @param productId   商品ID
     * @param productName 商品名（null の場合は更新しない）
     * @param unitPrice   単価（null の場合は更新しない）
     * @param description 説明（null の場合は更新しない）
     * @param imageUrl    画像URL（null の場合は更新しない）
     * @return 更新後の商品
     * @throws IllegalArgumentException 商品が見つからない場合
     */
    public Product updateProductForApi(Long productId, String productName, Integer unitPrice,
                                       String description, String imageUrl) {
        Product product = getProductById(productId);
        if (product == null) {
            throw new IllegalArgumentException("Product not found: " + productId);
        }

        if (productName != null) {
            product.setProductName(productName);
        }
        if (unitPrice != null) {
            product.setUnitPrice(unitPrice);
        }
        if (description != null) {
            product.setDescription(description);
        }
        if (imageUrl != null) {
            product.setImageUrl(imageUrl);
        }

        updateProduct(product);

        // 更新後の商品を返す
        return getProductById(productId);
    }

    /**
     * 商品の価格変更履歴を取得する。
     * @param productId 商品ID
     * @return 価格変更履歴リスト
     */
    public List<ProductPriceHistory> getPriceHistory(Long productId) {
        try {
            return priceHistoryDao.findByProductId(productId);
        } catch (SQLException e) {
            logger.error("Failed to get price history for product: {}", productId, e);
            throw new RuntimeException("価格変更履歴の取得に失敗しました", e);
        }
    }

    private static final String DATE_FORMAT = "yyyy-MM-dd'T'HH:mm:ss";
    private static final int DEFAULT_LIMIT = 20;
    private static final int MAX_LIMIT = 100;
    private static final int DATA_RETENTION_YEARS = 5;

    /**
     * ページネーション・日付フィルタ対応の価格変更履歴を取得する。
     * changedBy をオブジェクト（userId + userName）に変換し、pagination 情報を付与する。
     *
     * @param productId 商品ID
     * @param startDate 検索開始日（ISO 8601、null の場合は5年前）
     * @param endDate   検索終了日（ISO 8601、null の場合は現在日時）
     * @param page      ページ番号（1以上、null の場合は 1）
     * @param limit     1ページあたり件数（1〜100、null の場合は 20）
     * @return priceHistory リストと pagination 情報を含む Map
     */
    public Map<String, Object> getPriceHistoryWithPagination(
            Long productId, String startDate, String endDate, Integer page, Integer limit) {
        try {
            SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT);

            // デフォルト値設定
            if (startDate == null || startDate.isEmpty()) {
                Calendar cal = Calendar.getInstance();
                cal.add(Calendar.YEAR, -DATA_RETENTION_YEARS);
                startDate = sdf.format(cal.getTime());
            }
            if (endDate == null || endDate.isEmpty()) {
                endDate = sdf.format(new Date());
            }

            int pageNum = (page != null && page >= 1) ? page : 1;
            int limitNum = (limit != null) ? limit : DEFAULT_LIMIT;
            if (limitNum < 1) {
                limitNum = DEFAULT_LIMIT;
            }
            if (limitNum > MAX_LIMIT) {
                limitNum = MAX_LIMIT;
            }

            // 総件数カウント
            int totalItems = priceHistoryDao.countByProductIdWithFilter(productId, startDate, endDate);

            // ページネーション計算
            int totalPages = (totalItems > 0) ? (int) Math.ceil((double) totalItems / limitNum) : 0;
            if (pageNum > totalPages && totalPages > 0) {
                pageNum = totalPages;
            }
            int offset = (pageNum - 1) * limitNum;

            // データ取得
            List<ProductPriceHistory> historyList =
                    priceHistoryDao.findByProductIdWithFilter(productId, startDate, endDate, limitNum, offset);

            // changedBy をオブジェクト化（ユーザーキャッシュで N+1 軽減）
            Map<Long, String> userNameCache = new HashMap<Long, String>();
            List<Map<String, Object>> enrichedHistory = new ArrayList<Map<String, Object>>();
            for (ProductPriceHistory h : historyList) {
                Map<String, Object> item = new HashMap<String, Object>();
                item.put("priceHistoryId", h.getPriceHistoryId());
                item.put("productId", h.getProductId());
                item.put("oldPrice", h.getOldPrice());
                item.put("newPrice", h.getNewPrice());
                item.put("changedAt", normalizeToIso8601(h.getChangedAt()));
                item.put("changeReason", h.getChangeReason());

                // changedBy オブジェクト化
                Map<String, Object> changedByObj = new HashMap<String, Object>();
                Long userId = h.getChangedBy();
                changedByObj.put("userId", userId);
                changedByObj.put("userName", resolveUserName(userId, userNameCache));
                item.put("changedBy", changedByObj);

                enrichedHistory.add(item);
            }

            // レスポンス構築
            PaginationDto pagination = new PaginationDto(pageNum, totalPages, totalItems, limitNum);

            Map<String, Object> result = new HashMap<String, Object>();
            result.put("priceHistory", enrichedHistory);
            result.put("pagination", pagination);

            return result;
        } catch (SQLException e) {
            logger.error("Failed to get price history with pagination for product: {}", productId, e);
            throw new RuntimeException("価格変更履歴の取得に失敗しました", e);
        }
    }

    /**
     * ユーザーIDからユーザー名を解決する。キャッシュを使用して N+1 を軽減する。
     */
    private String resolveUserName(Long userId, Map<Long, String> cache) {
        if (userId == null) {
            return "不明";
        }
        if (cache.containsKey(userId)) {
            return cache.get(userId);
        }
        try {
            User user = userDao.findById(userId);
            String userName = (user != null) ? user.getUserName() : "不明";
            cache.put(userId, userName);
            return userName;
        } catch (SQLException e) {
            logger.warn("Failed to resolve user name for userId: {}", userId, e);
            cache.put(userId, "不明");
            return "不明";
        }
    }

    /**
     * changedAt の形式を ISO 8601 に正規化する。
     * "yyyy-MM-dd HH:mm:ss" → "yyyy-MM-ddTHH:mm:ss"
     */
    private String normalizeToIso8601(String dateTime) {
        if (dateTime == null) {
            return null;
        }
        // スペース区切りを T 区切りに変換
        return dateTime.replace(" ", "T");
    }
}
