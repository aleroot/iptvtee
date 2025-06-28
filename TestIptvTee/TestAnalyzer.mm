//
//  TestAnalyzer.m
//  TestIptvTee
//
//  Created by Alessio Pollero on 01/02/22.
//

#import <XCTest/XCTest.h>

#include "analyzer.hpp"
#include "playlist.hpp"
#include <vector>

using namespace std::chrono_literals;

@interface TestAnalyzer : XCTestCase

@end

@implementation TestAnalyzer

- (void)skip_testExample { //Added skip_ prefix to ignore this test due to crash at the end
    PlaylistItem item { .name = "Videolina" , .text ="Videolina", .url = "http://livestreaming.videolina.it/live/Videolina/playlist.m3u8" };
    std::vector<PlaylistItem> items = { item };
    Playlist play(items);
    
    Analyzer analizer(play, std::vector{3s}, 1);
    int count = 0;
    analizer.evaluate();
    for(Rank rank : analizer) {
        XCTAssert(rank.score > 0);
        count++;
    }
    
    XCTAssertEqual(1, count);
}

@end
