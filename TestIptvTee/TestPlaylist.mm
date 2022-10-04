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
    std::stringstream str_strm("http://livestreaming.videolina.it/live/Videolina/playlist.m3u8");
    Playlist pl = Playlist::fromM3U(str_strm);
    XCTAssertEqual(1, pl.size());
}

- (void)testReadFileNotExists {
    std::stringstream str_strm("M3u-Server.m3u8");
    try {
        Playlist::fromM3U(str_strm);
        XCTFail("An Exception is expected!");
    } catch (std::invalid_argument const &ex) {
        //Success
        std::stringstream ex_msg;
        ex_msg << ex.what();
        XCTAssertEqual("Given url:M3u-Server.m3u8 not parsable!", ex_msg.str());
    }
}


@end
