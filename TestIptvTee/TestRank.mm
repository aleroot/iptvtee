//
//  TestRank.mm
//  TestIptvTee
//
//  Created by Alessio Pollero on 06/06/21.
//

#import <XCTest/XCTest.h>

#import <sstream>

#import "Evaluable.hpp"

@interface TestRank : XCTestCase

@end

@implementation TestRank

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
    Rank rank;
    rank.max_value = 60000;
    rank.value = 22000;
    rank.score = rank.value / static_cast<float>(rank.max_value);
    
    Rank rank2;
    rank2.max_value = 60000;
    rank2.value = 44000;
    rank2.score = rank2.value / static_cast<float>(rank2.max_value);
    
    rank += rank2;
    
    XCTAssert(rank.score < rank2.score);
    XCTAssert(rank.value == 33000);
}

- (void)testPlusOperator {
    Rank rank;
    rank.elements = 1;
    rank.max_value = 60000;
    rank.value = 22000;
    rank.score = rank.value / static_cast<float>(rank.max_value);
    
    Rank rank2;
    rank2.elements = 1;
    rank2.max_value = 65000;
    rank2.value = 55000;
    rank2.score = rank2.value / static_cast<float>(rank2.max_value);
    
    Rank sum = rank + rank2;
    XCTAssert(rank.max_value == 62500);
    XCTAssert(rank2.max_value == 65000);
    XCTAssert(sum.max_value == rank.max_value);
    XCTAssert(sum.max_value < rank2.max_value);
    XCTAssert(sum.max_value == 62500);
}

- (void)testPlusOperatorEmptyCount {
    Rank rank;
    rank.max_value = 60000;
    rank.value = 22000;
    rank.score = rank.value / static_cast<float>(rank.max_value);
    
    Rank rank2;
    rank2.max_value = 65000;
    rank2.value = 55000;
    rank2.score = rank2.value / static_cast<float>(rank2.max_value);
    
    Rank sum = rank + rank2;
    XCTAssert(rank.max_value == 62500);
    XCTAssert(rank2.max_value == 65000);
    XCTAssert(sum.max_value == rank.max_value);
    XCTAssert(sum.max_value < rank2.max_value);
    XCTAssert(sum.max_value == 62500);
}

- (void)testToString {
    Rank rank;
    rank.max_value = 60000;
    rank.value = 22563;
    rank.score = rank.value / static_cast<float>(rank.max_value);
    rank.elements = 1;
    
    std::ostringstream sstring;
    sstring << rank;
    
    XCTAssert("Rank{ score = 37.60%, value = 22563, max = 60000, count = 1 }" == sstring.str());
}


@end
