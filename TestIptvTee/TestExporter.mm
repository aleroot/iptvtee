//
//  TestExporter.m
//  TestIptvTee
//
//  Created by Alessio Pollero on 17/06/22.
//

#import <XCTest/XCTest.h>
#include "exporter.hpp"
#include "StringUtils.hpp"
#include <iostream>
#include <sstream>
@interface TestExporter : XCTestCase

@end

@implementation TestExporter

- (void)testEnquote {
    Exporters::CSVWriter csv(std::cout);
    std::vector<std::string> row = { "Alessio" , "Tes,tgg", "Inter"};
    csv.enquoteIfNecessary(row);
    std::string str = row[1];
    XCTAssertTrue(str[0] == '"');
    XCTAssertTrue(str[str.length() - 1] == '"');
}

- (void)testAddRow {
    Exporters::CSVWriter csv(std::cout);
    std::vector<std::string> row = { "Alessio" , "Tes,tgg", "Inter"};
    XCTAssertTrue(csv.addRow(row));
}

- (void)testAddM3URow {
    std::stringstream str_strm;
    Exporters::M3UWriter m3u(str_strm);
    PlaylistItem pl { .url = "http://iptvserver.tv/kkkr/1", .text = "BBC" };
    ExportableItem itm { .item = pl };
    XCTAssertTrue(m3u.addRow(itm));
    const std::string line = str_strm.str();
    XCTAssertTrue(line.find(",BBC") != std::string::npos);
    XCTAssertTrue(line.find(pl.url) != std::string::npos);
    std::vector<std::string> lines = StringUtils::split(line, std::regex { "\\n" });
    XCTAssertEqual(3, lines.size());
    XCTAssertEqual("#EXTM3U", lines[0]);
}

@end
