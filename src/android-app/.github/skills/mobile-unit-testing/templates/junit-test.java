package com.example.mobileapp.{package_name};

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnitRunner;

/**
 * {SubjectUnderTest} のユニットテスト。
 *
 * テスト命名規則: {methodName}_when{Condition}_expect{Result}
 */
@RunWith(MockitoJUnitRunner.class)
public class {SubjectUnderTest}Test {

    // モック依存コンポーネント
    @Mock
    private {DependencyClass} mock{Dependency};

    // テスト対象クラス（SUT: System Under Test）
    private {SubjectUnderTest} sut;

    @Before
    public void setUp() {
        sut = new {SubjectUnderTest}(mock{Dependency});
    }

    // =========================================================================
    // 正常系テスト
    // =========================================================================

    @Test
    public void {methodName}_whenValidInput_expectSuccess() {
        // Arrange（準備）
        when(mock{Dependency}.{dependencyMethod}()).thenReturn({expectedValue});

        // Act（実行）
        {ReturnType} result = sut.{methodName}({validInput});

        // Assert（検証）
        assertNotNull(result);
        assertEquals({expectedValue}, result.{getter}());
        verify(mock{Dependency}).{dependencyMethod}();
    }

    // =========================================================================
    // 異常系テスト
    // =========================================================================

    @Test
    public void {methodName}_whenNullInput_expectException() {
        // Arrange
        // （特になし）

        // Act & Assert
        try {
            sut.{methodName}(null);
            // 例外が発生しなかった場合はテスト失敗
            assertTrue("Expected exception was not thrown", false);
        } catch ({ExpectedException} e) {
            assertNotNull(e.getMessage());
        }
    }

    @Test
    public void {methodName}_whenNetworkError_expectErrorState() {
        // Arrange
        when(mock{Dependency}.{dependencyMethod}()).thenThrow(new {NetworkException}("Network error"));

        // Act
        {ReturnType} result = sut.{methodName}({validInput});

        // Assert
        assertNull(result);
        verify(mock{Dependency}).{dependencyMethod}();
    }

    // =========================================================================
    // 境界値テスト
    // =========================================================================

    @Test
    public void {methodName}_whenEmptyString_expectValidationError() {
        // Arrange
        String emptyInput = "";

        // Act
        boolean isValid = sut.validate(emptyInput);

        // Assert
        assertFalse(isValid);
        verify(mock{Dependency}, never()).{dependencyMethod}();
    }
}
