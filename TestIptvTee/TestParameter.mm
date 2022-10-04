//
//  TestParameter.m
//  TestIptvTee
//
//  Created by Alessio Pollero on 26/08/22.
//

#import <XCTest/XCTest.h>
#include "parameter.hpp"

@interface TestParameter : XCTestCase

@end

@implementation TestParameter

- (void)testMultiParamters {
    const char * argv[] = { "", "-page=www.example1.com" };
    Parameters params = Parameters::from(2, argv);
    XCTAssertEqual("www.example1.com", params.get("page"));
    int counter = 0;
    while(params.contains("page")) {
        XCTAssertEqual("www.example1.com", params.pop("page"));
        counter++;
    }
    XCTAssertEqual(1, counter);
}


@end
