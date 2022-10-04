//
//  TestUtils.m
//  TestIptvTee
//
//  Created by Alessio Pollero on 20/06/22.
//

#import <XCTest/XCTest.h>
#include "Utils.hpp"

@interface TestUtils : XCTestCase

@end

@implementation TestUtils

- (void)testFilter {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "F1";
    std::function<bool (const std::string &)> filter = FilterUtils::extractFilter(p);
    bool r = filter("Sport F1");
    XCTAssertTrue(r);
}

- (void)testFilterInsensitive {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "sport";
    std::function<bool (const std::string &)> filter = FilterUtils::extractFilter(p);
    bool r = filter("Sport F1");
    XCTAssertTrue(r);
}

- (void)testAndFilter {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "Shit & F1";
    std::function<bool (const std::string &)> filter = FilterUtils::extractFilter(p);
    bool r = filter("Shit Sport F1");
    XCTAssertTrue(r);
}

- (void)testOrFilter {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "Eurosport | F1";
    std::function<bool (const std::string &)> filter = FilterUtils::extractFilter(p);
    XCTAssertTrue(filter("Eurosport"));
    XCTAssertFalse(filter("One"));
    XCTAssertTrue(filter("Sport F1"));
}

- (void)testFormat {
    XCTAssertEqual(Format::CSV, ReportUtils::toFormat("CSV"));
    XCTAssertEqual(Format::JSON, ReportUtils::toFormat("JSON"));
    XCTAssertEqual(Format::M3U, ReportUtils::toFormat("M3U"));
    
    XCTAssertEqual(Format::M3U, ReportUtils::toFormat("  M3U   "));
}

- (void)testInvalidFormat {
    XCTAssertEqual(Format::M3U, ReportUtils::toFormat(""));
    XCTAssertEqual(Format::M3U, ReportUtils::toFormat("aaa"));
}

@end
