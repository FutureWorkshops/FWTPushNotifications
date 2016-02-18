//
//  FWTTestCase.h
//  Notifiable-iOS
//
//  Created by Igor Fereira on 21/01/2016.
//  Copyright © 2016 Future Workshops. All rights reserved.
//

#import <XCTest/XCTest.h>

@class FWTNotifiableManager;

@interface FWTTestCase : XCTestCase

- (void) assertDictionary:(NSDictionary *)origin withTarget:(NSDictionary *)target;

- (void) stubDeviceRegisterResponse:(NSNumber *)deviceTokenId onMock:(id)mock;
- (void) stubDeviceRegisterResponse:(NSNumber *)deviceTokenId andError:(NSError *)error onMock:(id)mock withBlock:(void(^)(void))block;

- (void) stubDeviceUpdateResponse:(NSNumber *)deviceTokenId onMock:(id)mock;

- (void) registerAnonymousDeviceWithTokenId:(NSNumber *)tokenId andError:(NSError *)error onManager:(FWTNotifiableManager *)manager andRquesterMock:(id)mock;
- (void) registerDeviceWithTokenId:(NSNumber *)tokenId error:(NSError *)error andUserAlias:(NSString *)userAlias onManager:(FWTNotifiableManager *)manager andRquesterMock:(id)mock;

@end
