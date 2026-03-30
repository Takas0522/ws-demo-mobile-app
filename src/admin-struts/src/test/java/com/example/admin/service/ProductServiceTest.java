package com.example.admin.service;

import com.example.admin.dao.ProductDao;
import com.example.admin.dao.ProductPriceHistoryDao;
import com.example.admin.dao.UserDao;
import com.example.admin.dto.PaginationDto;
import com.example.admin.entity.ProductPriceHistory;
import com.example.admin.entity.User;

import org.junit.Before;
import org.junit.Test;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import static org.junit.Assert.*;
import static org.mockito.Mockito.*;

@SuppressWarnings("unchecked")
public class ProductServiceTest {

    private ProductService productService;
    private ProductDao mockProductDao;
    private ProductPriceHistoryDao mockPriceHistoryDao;
    private UserDao mockUserDao;

    @Before
    public void setUp() {
        productService = new ProductService();
        mockProductDao = mock(ProductDao.class);
        mockPriceHistoryDao = mock(ProductPriceHistoryDao.class);
        mockUserDao = mock(UserDao.class);
        productService.setProductDao(mockProductDao);
        productService.setProductPriceHistoryDao(mockPriceHistoryDao);
        productService.setUserDao(mockUserDao);
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_ページネーション情報が正しい() throws SQLException {
        // 総件数 5 件、limit 2 → 3 ページ
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(5);
        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), eq(2), eq(0)))
                .thenReturn(createHistoryList(2));
        when(mockUserDao.findById(anyLong())).thenReturn(createUser(1L, "管理者"));

        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, 1, 2);

        PaginationDto pagination = (PaginationDto) result.get("pagination");
        assertEquals(1, pagination.getCurrentPage());
        assertEquals(3, pagination.getTotalPages());
        assertEquals(5, pagination.getTotalItems());
        assertEquals(2, pagination.getItemsPerPage());
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_changedByがオブジェクト化される() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(1);
        List<ProductPriceHistory> historyList = new ArrayList<ProductPriceHistory>();
        ProductPriceHistory h = new ProductPriceHistory();
        h.setPriceHistoryId(1L);
        h.setProductId(1L);
        h.setOldPrice(1000);
        h.setNewPrice(1200);
        h.setChangedAt("2025-06-15 10:30:00");
        h.setChangedBy(3L);
        h.setChangeReason("値上げ");
        historyList.add(h);

        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), anyInt(), anyInt()))
                .thenReturn(historyList);

        User user = new User();
        user.setUserId(3L);
        user.setUserName("管理者");
        when(mockUserDao.findById(3L)).thenReturn(user);

        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, 1, 20);

        List<Map<String, Object>> priceHistory = (List<Map<String, Object>>) result.get("priceHistory");
        assertEquals(1, priceHistory.size());

        Map<String, Object> item = priceHistory.get(0);
        Map<String, Object> changedBy = (Map<String, Object>) item.get("changedBy");
        assertNotNull(changedBy);
        assertEquals(3L, changedBy.get("userId"));
        assertEquals("管理者", changedBy.get("userName"));
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_changedAtがISO8601形式に変換される() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(1);

        ProductPriceHistory h = new ProductPriceHistory();
        h.setPriceHistoryId(1L);
        h.setProductId(1L);
        h.setOldPrice(1000);
        h.setNewPrice(1200);
        h.setChangedAt("2025-06-15 10:30:00");
        h.setChangedBy(1L);

        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), anyInt(), anyInt()))
                .thenReturn(Collections.singletonList(h));
        when(mockUserDao.findById(1L)).thenReturn(createUser(1L, "管理者"));

        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, 1, 20);

        List<Map<String, Object>> priceHistory = (List<Map<String, Object>>) result.get("priceHistory");
        assertEquals("2025-06-15T10:30:00", priceHistory.get(0).get("changedAt"));
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_デフォルト値が適用される() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(0);
        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), eq(20), eq(0)))
                .thenReturn(Collections.<ProductPriceHistory>emptyList());

        // page=null, limit=null → デフォルト page=1, limit=20
        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, null, null);

        PaginationDto pagination = (PaginationDto) result.get("pagination");
        assertEquals(1, pagination.getCurrentPage());
        assertEquals(20, pagination.getItemsPerPage());

        // limit=20 で呼ばれることを検証
        verify(mockPriceHistoryDao).findByProductIdWithFilter(eq(1L), anyString(), anyString(), eq(20), eq(0));
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_結果0件で空配列とページネーションが返る() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(0);
        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), anyInt(), anyInt()))
                .thenReturn(Collections.<ProductPriceHistory>emptyList());

        Map<String, Object> result = productService.getPriceHistoryWithPagination(
                1L, "2025-01-01", "2025-12-31", 1, 20);

        List<Map<String, Object>> priceHistory = (List<Map<String, Object>>) result.get("priceHistory");
        assertNotNull(priceHistory);
        assertTrue(priceHistory.isEmpty());

        PaginationDto pagination = (PaginationDto) result.get("pagination");
        assertEquals(1, pagination.getCurrentPage());
        assertEquals(0, pagination.getTotalPages());
        assertEquals(0, pagination.getTotalItems());
        assertEquals(20, pagination.getItemsPerPage());
    }

    @Test(expected = RuntimeException.class)
    public void testGetPriceHistoryWithPagination_異常系_DAOでSQLException発生時にRuntimeExceptionをスロー() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenThrow(new SQLException("DB error"));

        productService.getPriceHistoryWithPagination(1L, null, null, 1, 20);
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_ユーザー未存在時にuserNameが不明になる() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(1);

        ProductPriceHistory h = new ProductPriceHistory();
        h.setPriceHistoryId(1L);
        h.setProductId(1L);
        h.setOldPrice(1000);
        h.setNewPrice(1200);
        h.setChangedAt("2025-06-15T10:30:00");
        h.setChangedBy(999L);

        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), anyInt(), anyInt()))
                .thenReturn(Collections.singletonList(h));
        when(mockUserDao.findById(999L)).thenReturn(null);

        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, 1, 20);

        List<Map<String, Object>> priceHistory = (List<Map<String, Object>>) result.get("priceHistory");
        Map<String, Object> changedBy = (Map<String, Object>) priceHistory.get(0).get("changedBy");
        assertEquals("不明", changedBy.get("userName"));
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_limitが100を超える場合100にクランプされる() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(0);
        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), eq(100), eq(0)))
                .thenReturn(Collections.<ProductPriceHistory>emptyList());

        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, 1, 200);

        PaginationDto pagination = (PaginationDto) result.get("pagination");
        assertEquals(100, pagination.getItemsPerPage());
        verify(mockPriceHistoryDao).findByProductIdWithFilter(eq(1L), anyString(), anyString(), eq(100), eq(0));
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_pageが1未満の場合1にクランプされる() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(10);
        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), eq(20), eq(0)))
                .thenReturn(Collections.<ProductPriceHistory>emptyList());

        Map<String, Object> result = productService.getPriceHistoryWithPagination(1L, null, null, -1, 20);

        PaginationDto pagination = (PaginationDto) result.get("pagination");
        assertEquals(1, pagination.getCurrentPage());
    }

    @Test
    public void testGetPriceHistoryWithPagination_正常系_同一ユーザーが複数回参照されてもキャッシュが効く() throws SQLException {
        when(mockPriceHistoryDao.countByProductIdWithFilter(eq(1L), anyString(), anyString()))
                .thenReturn(3);
        when(mockPriceHistoryDao.findByProductIdWithFilter(eq(1L), anyString(), anyString(), anyInt(), anyInt()))
                .thenReturn(createHistoryList(3));
        when(mockUserDao.findById(1L)).thenReturn(createUser(1L, "管理者"));

        productService.getPriceHistoryWithPagination(1L, null, null, 1, 20);

        // 同一ユーザーID(1L)が3件あっても findById は1回のみ呼ばれる
        verify(mockUserDao, times(1)).findById(1L);
    }

    // --- Helper methods ---

    private List<ProductPriceHistory> createHistoryList(int count) {
        List<ProductPriceHistory> list = new ArrayList<ProductPriceHistory>();
        for (int i = 0; i < count; i++) {
            ProductPriceHistory h = new ProductPriceHistory();
            h.setPriceHistoryId((long) (i + 1));
            h.setProductId(1L);
            h.setOldPrice(1000 + i * 100);
            h.setNewPrice(1100 + i * 100);
            h.setChangedAt("2025-01-" + String.format("%02d", i + 1) + "T10:00:00");
            h.setChangedBy(1L);
            h.setChangeReason("テスト変更" + (i + 1));
            list.add(h);
        }
        return list;
    }

    private User createUser(Long userId, String userName) {
        User user = new User();
        user.setUserId(userId);
        user.setUserName(userName);
        return user;
    }
}
