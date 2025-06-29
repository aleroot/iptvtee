//
//  TestDownloader.mm
//  iptvtee
//
//  Created by Alessio Pollero on 29/06/25.
//

#import <XCTest/XCTest.h>
#include "downloader.hpp"
#include <sstream>

@interface TestDownloader : XCTestCase
@end

@implementation TestDownloader

- (void)testIsUrlValidation {
    // Test with valid URLs
    XCTAssertTrue(HTTPDownloader::isUrl("http://example.com"));
    XCTAssertTrue(HTTPDownloader::isUrl("https://www.example.com/path/to/resource?query=value"));
    XCTAssertTrue(HTTPDownloader::isUrl("https://192.168.1.1:8080"));

    // Test with invalid strings
    XCTAssertFalse(HTTPDownloader::isUrl("just some text"));
    XCTAssertFalse(HTTPDownloader::isUrl(""));
    XCTAssertFalse(HTTPDownloader::isUrl("htp://invalid.url"));
}

- (void)testDownloadSuccessFromWeb {
    HTTPDownloader downloader;
    // Use a reliable and simple endpoint for testing.
    auto result = downloader.download("https://example.com");
    
    XCTAssertTrue(result.has_value(), "Download from example.com should succeed.");
    
    if (result) {
        std::string content = result->str();
        XCTAssertTrue(content.find("Example Domain") != std::string::npos, "The downloaded content should contain 'Example Domain'");
    }
}

- (void)testDownloadFailureWithInvalidDomain {
    HTTPDownloader downloader;
    // This domain is reserved for documentation and should not resolve.
    auto result = downloader.download("http://invalid.example.com");
    
    XCTAssertFalse(result.has_value(), "Download from a non-existent domain should fail and return nullopt.");
}

- (void)testDownloadLocalFileWithFileProtocol {
    HTTPDownloader downloader;
    
    // Get the path to the test resource file
    NSString *filePath = [[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"it.txt"];
    XCTAssertNotNil(filePath, "Test resource 'it.txt' should exist.");
    
    // Construct a file:// URL
    std::string fileUrl = "file://" + std::string([filePath UTF8String]);
    
    auto result = downloader.download(fileUrl);
    XCTAssertTrue(result.has_value(), "Download of a local file via file:// URL should succeed.");
    
    if (result) {
        std::string content = result->str();
        // Check if the content starts with #EXTM3U, which is expected for this file
        XCTAssertTrue(content.rfind("#EXTM3U", 0) == 0, "Content of local file should be read correctly.");
    }
}

@end
