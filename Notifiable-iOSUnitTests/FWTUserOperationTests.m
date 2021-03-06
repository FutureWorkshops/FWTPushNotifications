//
//  FWTUserOperationTests.m
//  Notifiable-iOS
//
//  Created by Igor Fereira on 22/01/2016.
//  Copyright © 2016 Future Workshops. All rights reserved.
//

#import "FWTTestCase.h"
#import "FWTNotifiableDevice.h"
#import "FWTRequesterManager.h"
#import "FWTNotifiableManager.h"
#import "FWTHTTPRequester.h"
#import <OCMock/OCMock.h>

@interface FWTUserOperationTests : FWTTestCase

@property (nonatomic, strong) id requesterManagerMock;
@property (nonatomic, strong) id httpRequestMock;
@property (nonatomic, strong) FWTNotifiableManager *manager;
@property (nonatomic, strong) NSNumber *deviceTokenId;
@property (nonatomic, strong) NSData *deviceToken;

@end

@implementation FWTUserOperationTests

- (NSNumber *)deviceTokenId
{
    if (self->_deviceTokenId == nil) {
        self->_deviceTokenId = @42;
    }
    return self->_deviceTokenId;
}

- (FWTNotifiableManager *)manager
{
    if (self->_manager == nil) {
        [FWTNotifiableManager application:OCMOCK_ANY didRegisterForRemoteNotificationsWithDeviceToken:self.deviceToken];
        self->_manager = [[FWTNotifiableManager alloc] initWithURL:OCMOCK_ANY
                                                          accessId:OCMOCK_ANY
                                                         secretKey:OCMOCK_ANY
                                                  didRegisterBlock:nil
                                              andNotificationBlock:nil];
    }
    return self->_manager;
}

- (NSData *)deviceToken
{
    if (self->_deviceToken == nil) {
        self->_deviceToken = [@"test" dataUsingEncoding:NSUTF8StringEncoding];
    }
    return self->_deviceToken;
}

- (void)mockRequester
{
    if (self->_requesterManagerMock == nil) {
        id requesterManagerMock = OCMClassMock([FWTRequesterManager class]);
        OCMStub([requesterManagerMock alloc]).andReturn(requesterManagerMock);
        OCMStub([requesterManagerMock initWithRequester:[OCMArg any]]).andReturn(requesterManagerMock);
        self->_requesterManagerMock = requesterManagerMock;
    }
}

- (void)mockHttpRequest
{
    if (self->_httpRequestMock == nil) {
        id httpMock = OCMClassMock([FWTHTTPRequester class]);
        OCMStub([httpMock alloc]).andReturn(httpMock);
        OCMStub([httpMock initWithBaseURL:[OCMArg any] andAuthenticator:[OCMArg any]]).andReturn(httpMock);
    }
}

- (void)setUp {
    [super setUp];
    [self mockRequester];
    [self mockHttpRequest];
}

- (void)tearDown {
    [self->_requesterManagerMock stopMocking];
    self->_requesterManagerMock = nil;
    
    [self->_httpRequestMock stopMocking];
    self->_httpRequestMock = nil;
    
    [super tearDown];
}

- (void)testAssociateUserWithAnonymous {
    [self stubDeviceUpdateResponse:self.deviceTokenId onMock:self.requesterManagerMock];
    [self registerAnonymousDeviceWithTokenId:self.deviceTokenId andError:nil onManager:self.manager andRquesterMock:self.requesterManagerMock];
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"associate"];
    [self.manager associateDeviceToUser:@"user" completionHandler:^(FWTNotifiableDevice *device, NSError * _Nullable error) {
        XCTAssertNotNil(device);
        XCTAssertNil(error);
        [expectation fulfill];
    }];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    XCTAssertEqualObjects(self.manager.currentDevice.user, @"user");
}

- (void)testAssociateUserWithPreviousUser {
    [self stubDeviceUpdateResponse:self.deviceTokenId onMock:self.requesterManagerMock];
    [self registerDeviceWithTokenId:self.deviceTokenId error:nil andUserAlias:@"user" onManager:self.manager andRquesterMock:self.requesterManagerMock];
    
    XCTAssertEqualObjects(self.manager.currentDevice.user, @"user");
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"associate"];
    [self.manager associateDeviceToUser:@"test" completionHandler:^(FWTNotifiableDevice *device, NSError * _Nullable error) {
        XCTAssertNotNil(device);
        XCTAssertNil(error);
        [expectation fulfill];
    }];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    XCTAssertEqualObjects(self.manager.currentDevice.user, @"test");
    
}

- (void) testAnonymiseUser {
    [self stubDeviceRegisterResponse:self.deviceTokenId onMock:self.requesterManagerMock];
    [self registerDeviceWithTokenId:self.deviceTokenId error:nil andUserAlias:@"user" onManager:self.manager andRquesterMock:self.requesterManagerMock];
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"associate"];
    [self.manager anonymiseTokenWithCompletionHandler:^(FWTNotifiableDevice *device, NSError * _Nullable error) {
        XCTAssertNotNil(device);
        XCTAssertNil(error);
        [expectation fulfill];
    }];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    XCTAssertNotNil(self.manager.currentDevice);
    XCTAssertNil(self.manager.currentDevice.user);
}

@end
