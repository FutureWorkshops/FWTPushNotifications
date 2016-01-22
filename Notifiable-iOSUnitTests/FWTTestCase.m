//
//  FWTTestCase.m
//  Notifiable-iOS
//
//  Created by Igor Fereira on 21/01/2016.
//  Copyright © 2016 Future Workshops. All rights reserved.
//

#import "FWTTestCase.h"
#import "FWTRequesterManager.h"
#import <OCMock/OCMock.h>

@implementation FWTTestCase

- (void)setUp
{
    [super setUp];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"FWTUserInfoNotifiableCurrentDeviceKey"];;
}

- (void)tearDown
{
    [super setUp];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"FWTUserInfoNotifiableCurrentDeviceKey"];
}

- (void) assertDictionary:(NSDictionary *)origin withTarget:(NSDictionary *)target
{
    XCTAssertEqual(origin.count, target.count);
    XCTAssertTrue([origin isKindOfClass:[NSDictionary class]], @"The origin %@ is not a dictionary", origin);
    XCTAssertTrue([target isKindOfClass:[NSDictionary class]], @"The targer %@ is not a dictionary", target);
    
    for (NSString *key in origin.keyEnumerator) {
        id object = origin[key];
        id targetObject = target[key];
        
        XCTAssertNotNil(targetObject, @"The key %@ doesn't have an object on target.", key);
        if ([object isKindOfClass:[NSDictionary class]]) {
            [self assertDictionary:object withTarget:targetObject];
        } else {
            XCTAssertEqualObjects(object, targetObject, @"The objects for the key %@ are not the same", key);
        }
    }
}

- (void) mockDeviceRegisterResponse:(NSNumber *)deviceTokenId onMock:(id)mock
{
    [self mockDeviceRegisterResponse:deviceTokenId onMock:mock withBlock:nil];
}

- (void) mockDeviceRegisterResponse:(NSNumber *)deviceTokenId onMock:(id)mock withBlock:(void(^)(void))block
{
    void (^postProxyBlock)(NSInvocation *) = ^(NSInvocation *invocation) {
        FWTDeviceTokenIdResponse passedBlock;
        [invocation getArgument:&passedBlock atIndex:7];
        if (passedBlock) {
            passedBlock(deviceTokenId, nil);
        }
    };
    OCMStub([mock registerDeviceWithUserAlias:[OCMArg any]
                                        token:[OCMArg any]
                                         name:[OCMArg any]
                                       locale:[OCMArg any]
                            deviceInformation:[OCMArg any]
                            completionHandler:[OCMArg any]]).andDo(postProxyBlock);
    if (block) {
        block();
    }
}

- (void) stubDeviceUpdateResponse:(NSNumber *)deviceTokenId onMock:(id)mock
{
    [self stubDeviceUpdateResponse:deviceTokenId onMock:mock withBlock:nil];
}

- (void) stubDeviceUpdateResponse:(NSNumber *)deviceTokenId onMock:(id)mock withBlock:(void(^)(void))block
{
    void (^postProxyBlock)(NSInvocation *) = ^(NSInvocation *invocation) {
        FWTDeviceTokenIdResponse passedBlock;
        [invocation getArgument:&passedBlock atIndex:8];
        if (passedBlock) {
            passedBlock(deviceTokenId, nil);
        }
    };
    OCMStub([mock updateDevice:OCMOCK_ANY
                 withUserAlias:OCMOCK_ANY
                         token:OCMOCK_ANY
                          name:OCMOCK_ANY
                        locale:OCMOCK_ANY
             deviceInformation:OCMOCK_ANY
             completionHandler:OCMOCK_ANY]).andDo(postProxyBlock);
    
    if (block) {
        block();
    }
}

@end
