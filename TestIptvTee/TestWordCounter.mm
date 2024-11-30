//
//  TestWordCounter.mm
//  TestIptvTee
//
//  Created by Alessio Pollero on 27/11/24.
//

#import <XCTest/XCTest.h>
#include "WordCounter.hpp"
#include "playlist.hpp"
#include <sstream>

@interface TestWordCounter : XCTestCase

@end

@implementation TestWordCounter

- (void)testBasicWordCount {
    WordCounter counter;
    counter.process("apple banana apple cherry banana apple date");
    XCTAssertEqual(counter.getTotalWords(), 7);
    XCTAssertEqual(counter.getUniqueWords(), 4);
}

- (void)testEmptyInput {
    WordCounter counter;
    counter.process("");
    XCTAssertEqual(counter.getTotalWords(), 0);
    XCTAssertEqual(counter.getUniqueWords(), 0);
    XCTAssertEqual(counter.top(3), "");
}

- (void)testTopWordsBasic {
    WordCounter counter;
    counter.process("apple banana apple cherry banana apple date");
    std::string result = counter.top(2);
    XCTAssertEqual(result, "APPLE BANANA");
}

- (void)testTopWordsWithRate {
    WordCounter counter;
    counter.process("apple apple apple apple apple apple apple banana apple cherry banana apple date");
    std::string result = counter.top(3, 40.0);
    XCTAssertEqual(result, "APPLE");
}

- (void)testCaseNormalization {
    WordCounter counter;
    counter.process("Apple APPLE apple bAnAnA BANANA");
    XCTAssertEqual(counter.getUniqueWords(), 2);
    std::string result = counter.top(2);
    XCTAssertEqual(result, "APPLE BANANA");
}

- (void)testPunctuation {
    WordCounter counter;
    counter.process("apple, apple! banana? cherry... banana, apple.");
    XCTAssertEqual(counter.getTotalWords(), 6);
    XCTAssertEqual(counter.getUniqueWords(), 3);
    std::string result = counter.top(2);
    XCTAssertEqual(result, "APPLE BANANA");
}

- (void)testClear {
    WordCounter counter;
    counter.process("apple banana apple");
    XCTAssertEqual(counter.getTotalWords(), 3);
    
    counter.clear();
    XCTAssertEqual(counter.getTotalWords(), 0);
    XCTAssertEqual(counter.getUniqueWords(), 0);
    XCTAssertEqual(counter.top(1), "");
}

- (void)testMultipleProcessCalls {
    WordCounter counter;
    counter.process("apple banana");
    counter.process("apple cherry");
    counter.process("apple date");
    
    XCTAssertEqual(counter.getTotalWords(), 6);
    XCTAssertEqual(counter.getUniqueWords(), 4);
    std::string result = counter.top(2);
    XCTAssertEqual(result, "APPLE BANANA");
}

- (void)testMinimumWordLength {
    WordCounter counter;
    counter.process("a a the apple banana");
    // Assuming minWordLength is set to filter out articles
    std::string result = counter.top(2);
    XCTAssertEqual(result, "APPLE BANANA");
}

- (void)testTopWithInvalidCount {
    WordCounter counter;
    counter.process("apple banana cherry");
    XCTAssertEqual(counter.top(0), "");
    XCTAssertEqual(counter.top(-1), "");
}

- (void)testTopWithExcessiveCount {
    WordCounter counter;
    counter.process("apple banana cherry");
    std::string result = counter.top(10); // Asked for more words than exist
    XCTAssertEqual(result, "APPLE BANANA CHERRY");
}

- (void)testAlphabeticalTieBreaking {
    WordCounter counter;
    counter.process("apple apple banana banana cherry cherry");
    // All words appear twice, should be ordered alphabetically
    std::string result = counter.top(3);
    XCTAssertEqual(result, "APPLE BANANA CHERRY");
}

- (void)testMutliCallsAndTop {
    WordCounter counter;
    counter.process("apple apple banana banana cherry cherry");
    for(int i = 1; i <= 20; i++) {
        counter.process("Sky Sport Sky Sport");
    }
    std::string result = counter.top(3, 50);
    XCTAssertEqual(result, "SKY SPORT");
}

- (void)testProcessPlaylistText {
    // Get the resource path and load the playlist
    NSString *filePath = [[[NSBundle bundleForClass:[self class]] resourcePath]
                         stringByAppendingPathComponent:@"it.txt"];
    XCTAssertNotNil(filePath);
    std::string m3u = std::string([filePath UTF8String]);
    
    std::ifstream testFile(m3u);
    XCTAssert(testFile.is_open());
    
    // Load the playlist
    Playlist pl = Playlist::fromM3U(testFile);
    XCTAssertEqual(142, pl.size());
    
    // Create word counter and process all playlist items
    WordCounter counter;
    for(auto i = 0; i < pl.size(); i++) {
        counter.process(pl[i].text);
    }
    
    // Verify we have processed some words
    XCTAssertGreaterThan(counter.getTotalWords(), 0);
    XCTAssertGreaterThan(counter.getUniqueWords(), 0);
    
    // Get top 5 most frequent words
    std::string topWords = counter.top(5);
    XCTAssertGreaterThan(topWords.length(), 0);
    
    // Get words that appear in more than 20% of entries
    std::string frequentWords = counter.top(10, 20.0);
    XCTAssertGreaterThan(frequentWords.length(), 0);
    
    std::string top3Words = counter.top(3, 25.0);
    XCTAssertEqual(top3Words, "TV RADIO");
    
    top3Words = counter.top(3, 50.0);
    XCTAssertEqual(top3Words, "TV");
    
    // Clear and verify
    counter.clear();
    XCTAssertEqual(counter.getTotalWords(), 0);
}

@end
