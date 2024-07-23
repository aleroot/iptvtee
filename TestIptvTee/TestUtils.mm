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

PlaylistItem sportF1{0, "", "Sport F1", "", "", ""};

- (void)testFilter {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "F1";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(sportF1, std::numeric_limits<int>::max());
    XCTAssertTrue(r);
}

- (void)testFilterByField {
    PlaylistItem italia{0, "", "Sport F1", "Top Italia", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "group=italia";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(italia, std::numeric_limits<int>::max());
    XCTAssertTrue(r);
}

- (void)testFilterByFieldAnd {
    PlaylistItem italia{0, "", "Sport F1", "Top Italia", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "group=italia&text=F1";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(italia, std::numeric_limits<int>::max());
    XCTAssertTrue(r);
}

- (void)testFilterByFieldAndNoMatch {
    PlaylistItem italia{0, "", "Sport F1", "Top Italia", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "group=italia&text=F2";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(italia, std::numeric_limits<int>::max());
    XCTAssertFalse(r);
}

- (void)testFilterByFieldNoMatch {
    PlaylistItem italia{0, "", "Sport F1", "Top UK", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "group=italia";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(italia, std::numeric_limits<int>::max());
    XCTAssertFalse(r);
}

- (void)testFilterByFieldOrMatch {
    PlaylistItem italia{0, "", "Sport F1", "Top UK", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "group=italia|uk";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(italia, std::numeric_limits<int>::max());
    XCTAssertTrue(r);
}

- (void)testFilterAndMax {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "F1";
    p["max"] = "2";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(sportF1, 1);
    XCTAssertTrue(r);
}

- (void)testMaxNoFilter {
    std::unordered_map<std::string, std::string> p;
    p["max"] = "1";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    
    XCTAssertFalse(filter(sportF1, 3));
}

- (void)testFilterInsensitive {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "sport";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    bool r = filter(sportF1, std::numeric_limits<int>::max());
    XCTAssertTrue(r);
}

- (void)testAndFilter {
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "Shit & F1";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    PlaylistItem shitF1{0, "", "Shit Sport F1", "", "", ""};
    bool r = filter(shitF1, std::numeric_limits<int>::max());
    XCTAssertTrue(r);
}

- (void)testOrFilter {
    PlaylistItem euroSport{0, "", "Eurosport", "", "", ""};
    PlaylistItem one{0, "", "One", "", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "Eurosport | F1";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    XCTAssertTrue(filter(euroSport, 0));
    XCTAssertFalse(filter(one, 0));
    XCTAssertTrue(filter(sportF1, 0));
}

- (void)testAndOrFilter {
    PlaylistItem f1{0, "", "Sly Sport F1", "", "", ""};
    PlaylistItem cinema{0, "", "Sly Cinema 1", "", "", ""};
    std::unordered_map<std::string, std::string> p;
    p["filter"] = "Sly&Cinema|F1";
    p["max"] = "15000";
    std::function<bool (const PlaylistItem &, const int c)> filter = FilterUtils::extractFilter(p);
    int c = 0;
    XCTAssertTrue(filter(f1, c++));
    XCTAssertTrue(filter(cinema, c++));
    XCTAssertTrue(filter(sportF1, c++));
    PlaylistItem missing{0, "", "Cinema 1", "", "", ""};
    XCTAssertFalse(filter(missing, c++));

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
