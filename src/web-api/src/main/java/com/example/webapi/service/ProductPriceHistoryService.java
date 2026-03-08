package com.example.webapi.service;

import com.example.webapi.dto.PriceHistoryResponse;
import com.example.webapi.entity.ProductPriceHistory;
import com.example.webapi.entity.User;
import com.example.webapi.repository.ProductPriceHistoryRepository;
import com.example.webapi.repository.ProductRepository;
import com.example.webapi.repository.UserRepository;
import com.example.webapi.exception.ResourceNotFoundException;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

/**
 * 価格履歴サービス
 */
@Slf4j
@Service
@RequiredArgsConstructor
@Transactional
public class ProductPriceHistoryService {

    private final ProductPriceHistoryRepository priceHistoryRepository;
    private final ProductRepository productRepository;
    private final UserRepository userRepository;

    private static final DateTimeFormatter SQLITE_FORMAT = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");

    /**
     * 商品の価格履歴を取得
     *
     * @param productId 商品ID
     * @param startDate 検索開始日
     * @param endDate   検索終了日
     * @param page      ページ番号（0始まり）
     * @param limit     1ページあたりの件数
     * @return 価格履歴レスポンスのページ
     */
    @Transactional(readOnly = true)
    public Page<PriceHistoryResponse> getPriceHistory(Long productId, LocalDateTime startDate,
            LocalDateTime endDate, int page, int limit) {
        log.debug("価格履歴取得開始: productId={}, startDate={}, endDate={}, page={}, limit={}",
                productId, startDate, endDate, page, limit);

        // 商品の存在確認
        if (!productRepository.existsById(productId)) {
            throw new ResourceNotFoundException("PRODUCT_001", "商品が見つかりません");
        }

        Pageable pageable = PageRequest.of(page, limit);
        String startDateStr = startDate.format(SQLITE_FORMAT);
        String endDateStr = endDate.format(SQLITE_FORMAT);
        Page<ProductPriceHistory> historyPage = priceHistoryRepository
                .findByProductIdAndDateRange(productId, startDateStr, endDateStr, pageable);

        // 変更者のユーザー名を一括取得
        Set<Long> userIds = historyPage.getContent().stream()
                .map(ProductPriceHistory::getChangedBy)
                .collect(Collectors.toSet());
        Map<Long, String> userNameMap = userRepository.findAllById(userIds).stream()
                .collect(Collectors.toMap(User::getUserId, User::getUserName));

        log.info("価格履歴取得完了: productId={}, 件数={}", productId, historyPage.getTotalElements());

        return historyPage.map(entity -> {
            String userName = userNameMap.getOrDefault(entity.getChangedBy(), "不明");
            return PriceHistoryResponse.fromEntity(entity, userName);
        });
    }
}
