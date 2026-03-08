import XCTest
@testable import MobileApp

// =============================================================================
// モックプロトコルの定義（テスト対象クラスの依存コンポーネントごとに作成）
// =============================================================================

protocol {DependencyProtocol} {
    func {dependencyMethod}() async throws -> {ReturnType}
}

final class Mock{Dependency}: {DependencyProtocol} {
    // テスト制御フラグ
    var shouldThrowError = false
    var {returnValueProperty}: {ReturnType}?

    // 呼び出し記録
    var {methodName}CallCount = 0

    func {dependencyMethod}() async throws -> {ReturnType} {
        {methodName}CallCount += 1
        if shouldThrowError {
            throw {ErrorType}.{errorCase}
        }
        return {returnValueProperty}!
    }
}

// =============================================================================
// テストクラス
// テスト命名規則: test_{methodName}_when{Condition}_expect{Result}
// =============================================================================

@MainActor
final class {SubjectUnderTest}Tests: XCTestCase {

    // テスト対象クラス（SUT: System Under Test）
    var sut: {SubjectUnderTest}!
    var mock{Dependency}: Mock{Dependency}!

    override func setUpWithError() throws {
        try super.setUpWithError()
        mock{Dependency} = Mock{Dependency}()
        sut = {SubjectUnderTest}({dependencyParam}: mock{Dependency})
    }

    override func tearDownWithError() throws {
        sut = nil
        mock{Dependency} = nil
        try super.tearDownWithError()
    }

    // =========================================================================
    // 正常系テスト
    // =========================================================================

    func test_{methodName}_whenValidInput_expectSuccess() async throws {
        // Arrange（準備）
        mock{Dependency}.{returnValueProperty} = {expectedValue}

        // Act（実行）
        try await sut.{methodName}()

        // Assert（検証）
        XCTAssertEqual(sut.{stateProperty}, {expectedState})
        XCTAssertFalse(sut.isLoading)
        XCTAssertNil(sut.errorMessage)
        XCTAssertEqual(mock{Dependency}.{methodName}CallCount, 1)
    }

    // =========================================================================
    // 異常系テスト
    // =========================================================================

    func test_{methodName}_whenNetworkError_expectErrorState() async throws {
        // Arrange
        mock{Dependency}.shouldThrowError = true

        // Act
        try await sut.{methodName}()

        // Assert
        XCTAssertFalse(sut.isLoading)
        XCTAssertNotNil(sut.errorMessage)
        XCTAssertTrue(sut.{stateProperty}.isEmpty)
    }

    // =========================================================================
    // 境界値テスト
    // =========================================================================

    func test_{methodName}_whenEmptyInput_expectValidationFailure() throws {
        // Arrange
        let emptyInput = ""

        // Act
        let isValid = sut.validate(emptyInput)

        // Assert
        XCTAssertFalse(isValid)
        XCTAssertEqual(mock{Dependency}.{methodName}CallCount, 0)
    }

    // =========================================================================
    // 状態遷移テスト
    // =========================================================================

    func test_{methodName}_duringLoading_isLoadingIsTrue() async throws {
        // Arrange
        let expectation = XCTestExpectation(description: "Loading state")
        mock{Dependency}.{returnValueProperty} = {expectedValue}

        // Act
        let task = Task {
            try await sut.{methodName}()
        }

        // Assert: ロード開始直後は isLoading = true
        XCTAssertTrue(sut.isLoading)

        // 完了を待つ
        await task.value
        expectation.fulfill()

        await fulfillment(of: [expectation], timeout: 2.0)
        XCTAssertFalse(sut.isLoading)
    }
}
