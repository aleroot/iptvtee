//
//  TestPlaylist.m
//  TestIptvTee
//
//  Created by Alessio Pollero on 31/01/22.
//

#import <XCTest/XCTest.h>
#include <sstream>
#include <iostream>
#include "playlist.hpp"


@interface TestPlaylist : XCTestCase

@end

@implementation TestPlaylist

- (void)testReadM3U {
    NSString *filePath = [[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"it.txt"];
    XCTAssertNotNil(filePath);
    std::string m3u = std::string([filePath UTF8String]);
    
    std::ifstream testFile (m3u);
    XCTAssert(testFile.is_open());
    Playlist pl = Playlist::fromM3U(testFile);
    XCTAssert(142 == pl.size());
    auto lastItem = pl[(int)pl.size()-1];
    XCTAssert(lastItem.text.starts_with("Videolina"));
    XCTAssert(lastItem.url.find("Videolina") != std::string::npos);
    for(auto i =0; i < pl.size(); i++) {
        XCTAssertTrue(pl[i].url.length() > 5);
        XCTAssertTrue(pl[i].text.length() > 0);
    }
}

- (void)testReadM3UFolder {
    NSString *filePath = [[NSBundle bundleForClass:[self class]] resourcePath];
    XCTAssertNotNil(filePath);
    std::string folder = std::string([filePath UTF8String]);
    
    Playlist pl = Playlist::fromM3U(folder);
    XCTAssert(pl.size() > 100);
    for(auto i =0; i < pl.size(); i++) {
        XCTAssertTrue(pl[i].url.length() > 5);
        XCTAssertTrue(pl[i].text.length() > 0);
    }
}

- (void)testReadURLs {
    std::stringstream str_strm("https://d3749synfikwkv.cloudfront.net/v1/master/3722c60a815c199d9c0ef36c5b73da68a62b09d1/cc-74ylxpgd78bpb/Live.m3u8");
    Playlist pl = Playlist::fromM3U(str_strm);
    XCTAssertEqual(1, pl.size());
}

- (void)testReadFileNotExists {
    std::stringstream str_strm("M3u-Server.m3u8");
    Playlist pl = Playlist::fromM3U(str_strm);
    XCTAssertEqual(0, pl.size());
}

- (void)testNonM3UContentURLAsSingleItem {
    // This URL points to a valid web page (HTML), not an M3U file.
    std::string nonM3uUrl = "https://example.com";
    Playlist pl = Playlist::fromM3U(nonM3uUrl);
    
    XCTAssertEqual(1, pl.size(), "A URL with non-M3U content should be treated as a single item playlist.");
    XCTAssertEqual(nonM3uUrl, pl[0].url, "The item's URL should be the original URL.");
}

- (void)testRemoteM3UDownloadAndParse {
    std::string url = "https://raw.githubusercontent.com/iptv-org/iptv/master/streams/us_ny.m3u";
    Playlist pl = Playlist::fromM3U(url);
    XCTAssertGreaterThan(pl.size(), 0, "Should successfully download and parse the remote M3U playlist.");
}

- (void)testInvalidUrlThrowsException {
    std::string invalidUrl = "this-is-not-a-valid-url-or-path";
    XCTAssertThrows(Playlist::fromM3U(invalidUrl), "Passing a completely invalid string should throw an exception.");
}

- (void)testEmptyUrlReturnsEmptyPlaylist {
    XCTAssertThrows(Playlist::fromM3U(""), "Passing an empty string should throw an exception.");
}

@end
