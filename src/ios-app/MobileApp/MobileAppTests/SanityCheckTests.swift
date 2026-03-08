//
//  SanityCheckTests.swift
//  MobileAppTests
//

import XCTest

final class SanityCheckTests: XCTestCase {

    func testTrueIsTrue() {
        XCTAssertTrue(true)
    }

    func testOneEqualsOne() {
        XCTAssertEqual(1, 1)
    }

    func testStringIsNotEmpty() {
        let text = "Hello"
        XCTAssertFalse(text.isEmpty)
    }
}
