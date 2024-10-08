//
//  TestStringUtils.m
//  TestIptvTee
//
//  Created by Alessio Pollero on 18/06/22.
//

#import <XCTest/XCTest.h>
#include "StringUtils.hpp"

using namespace StringUtils;
using namespace std;

@interface TestStringUtils : XCTestCase

@end

@implementation TestStringUtils

- (void)testQuote {
    string input = "Alessio";
    string str = quote(input);
    
    XCTAssertTrue(str.length() > input.length());
    XCTAssertTrue(str[0] == '"');
    XCTAssertTrue(str[str.length() - 1] == '"');
}

- (void)testSplit {
    std::vector<std::string> andParts = StringUtils::split("F1", std::regex{"[\\&\\s]+"});
    XCTAssertFalse(andParts.empty());
    XCTAssertEqual(1, andParts.size());
}

- (void)testSplitMultiple {
    std::vector<std::string> andParts = StringUtils::split("F1 & F2 & F3", std::regex{"[\\&\\s]+"});
    XCTAssertFalse(andParts.empty());
    XCTAssertEqual(3, andParts.size());
    for(int i = 1; i <= 3; i++) {
        auto s = "F" + std::to_string(i);
        XCTAssertEqual(s, andParts[i-1]);
    }
}

- (void)testSplitExtended {
    const std::string s = "F1";
    const std::regex sep_regex = std::regex{"[\\&\\s]+"};
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> v = {iter, end};
    XCTAssertFalse(v.empty());
    std::vector<std::string> andParts = StringUtils::split("F1", std::regex{"[\\&\\s]+"});
    XCTAssertEqual(v.size(), andParts.size());
}

- (void)testExtractPrefixAndStripBasic {
    std::string input = "key=value";
    std::string prefix;
    bool result = StringUtils::extractPrefixAndStrip(input, prefix);
    
    XCTAssertTrue(result);
    XCTAssertEqual("key", prefix);
    XCTAssertEqual("value", input);
}

- (void)testExtractPrefixAndStripNoEquals {
    std::string input = "justAString";
    std::string prefix;
    bool result = StringUtils::extractPrefixAndStrip(input, prefix);
    
    XCTAssertFalse(result);
    XCTAssertEqual("", prefix);
    XCTAssertEqual("justAString", input);
}

- (void)testExtractPrefixAndStripEmptyPrefix {
    std::string input = "=value";
    std::string prefix;
    bool result = StringUtils::extractPrefixAndStrip(input, prefix);
    
    XCTAssertTrue(result);
    XCTAssertEqual("", prefix);
    XCTAssertEqual("value", input);
}

- (void)testExtractPrefixAndStripEmptyValue {
    std::string input = "key=";
    std::string prefix;
    bool result = StringUtils::extractPrefixAndStrip(input, prefix);
    
    XCTAssertTrue(result);
    XCTAssertEqual("key", prefix);
    XCTAssertEqual("", input);
}

- (void)testExtractPrefixAndStripMultipleEquals {
    std::string input = "key=value=more";
    std::string prefix;
    bool result = StringUtils::extractPrefixAndStrip(input, prefix);
    
    XCTAssertTrue(result);
    XCTAssertEqual("key", prefix);
    XCTAssertEqual("value=more", input);
}

@end
