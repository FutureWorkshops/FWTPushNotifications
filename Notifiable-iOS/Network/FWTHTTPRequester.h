//
//  FWTRequestManager.h
//  Notifiable-iOS
//  Copyright © 2016 Future Workshops. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^FWTRequestManagerSuccessBlock)(NSDictionary<NSString *, NSObject *>* _Nullable response);
typedef void(^FWTRequestManagerArraySuccessBlock)(NSArray* response);
typedef void(^FWTRequestManagerFailureBlock)(NSInteger responseCode, NSError * error);

@class FWTNotifiableAuthenticator;

@interface FWTHTTPRequester : NSObject

@property (nonatomic, readonly, strong) NSURL* baseUrl;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithBaseURL:(NSURL *)baseUrl
                        session:(NSURLSession *)session
               andAuthenticator:(FWTNotifiableAuthenticator*)authenticator NS_DESIGNATED_INITIALIZER;
- (void)registerDeviceWithParams:(NSDictionary *)params
                         success:(_Nullable FWTRequestManagerSuccessBlock)success
                         failure:(_Nullable FWTRequestManagerFailureBlock)failure;
- (void)updateDeviceWithTokenId:(NSNumber *)tokenId
                         params:(NSDictionary *)params
                        success:(_Nullable FWTRequestManagerSuccessBlock)success
                        failure:(_Nullable FWTRequestManagerFailureBlock)failure;
- (void)unregisterTokenId:(NSNumber *)tokenId
                  success:(FWTRequestManagerSuccessBlock)success
                  failure:(FWTRequestManagerFailureBlock)failure;
- (void)markNotificationAsOpenedWithId:(NSString *)notificationId
                         deviceTokenId:(NSString *)deviceTokenId
                                  user:(NSString *)user
                               success:(FWTRequestManagerSuccessBlock)success
                               failure:(FWTRequestManagerFailureBlock)failure;
- (void)markNotificationAsReceivedWithId:(NSString *)notificationId
                           deviceTokenId:(NSString *)deviceTokenId
                                 success:(FWTRequestManagerSuccessBlock)success
                                 failure:(FWTRequestManagerFailureBlock)failure;

@end

NS_ASSUME_NONNULL_END
