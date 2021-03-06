//
//  FWTRequestManager.m
//  Notifiable-iOS
//  Copyright © 2016 Future Workshops. All rights reserved.
//

#import "FWTHTTPRequester.h"
#import "FWTNotifiableAuthenticator.h"
#import "NSError+FWTNotifiable.h"
#import "FWTHTTPSessionManager.h"

typedef void(^FWTAFNetworkingSuccessBlock)(id  _Nullable responseObject);
typedef void(^FWTAFNetworkingFailureBlock)(NSInteger responseCode, NSError * _Nonnull error);

NSString * const FWTDeviceTokensPath = @"api/v1/device_tokens";
NSString * const FWTNotificationOpenPath = @"api/v1/notifications/%@/opened";
NSString * const FWTNotificationReceivedPath = @"api/v1/notifications/%@/delivered";
NSString * const FWTListDevicesPath = @"api/v1/device_tokens.json";

@interface FWTHTTPRequester ()

@property (nonatomic, strong) FWTHTTPSessionManager *httpSessionManager;
@property (nonatomic, strong) FWTNotifiableAuthenticator *authenticator;
@property (nonatomic, strong) NSURLSession *urlSession;

@end

@implementation FWTHTTPRequester

- (instancetype)initWithBaseURL:(NSURL *)baseUrl
                        session:(NSURLSession *)session
               andAuthenticator:(FWTNotifiableAuthenticator*)authenticator {
    self = [super init];
    if (self) {
        self->_baseUrl = baseUrl;
        self->_authenticator = authenticator;
        self->_urlSession = session;
    }
    return self;
}

- (FWTHTTPSessionManager *)httpSessionManager
{
    if (!self->_httpSessionManager) {
        self->_httpSessionManager = [[FWTHTTPSessionManager alloc] initWithBaseURL:self.baseUrl
                                                                           session:self.urlSession];
    }
    return self->_httpSessionManager;
}

- (void)registerDeviceWithParams:(NSDictionary *)params
                         success:(FWTRequestManagerSuccessBlock)success
                         failure:(FWTRequestManagerFailureBlock)failure
{
    NSAssert(params != nil, @"You need provide, at least, the device token that will be registered");
    
    [self _updateAuthenticationForPath:FWTDeviceTokensPath httpMethod:@"POST"];
    
    [self.httpSessionManager POST:FWTDeviceTokensPath
                       parameters:params
                          success:[self _defaultSuccessHandler:success]
                          failure:[self _defaultFailureHandler:failure success:success]];
}

- (void)updateDeviceWithTokenId:(NSNumber *)tokenId
                         params:(NSDictionary *)params
                        success:(FWTRequestManagerSuccessBlock)success
                        failure:(FWTRequestManagerFailureBlock)failure
{
    NSAssert(params != nil, @"You need provide some information to update");
    NSAssert(tokenId != nil, @"Device token id missing");
    
    NSString *path = [NSString stringWithFormat:@"%@/%@",FWTDeviceTokensPath, [tokenId stringValue]];
    [self _updateAuthenticationForPath:path httpMethod:@"PATCH"];
    [self.httpSessionManager PATCH:path
                        parameters:params
                           success:[self _defaultSuccessHandler:success]
                           failure:[self _defaultFailureHandler:failure success:success]];
}

- (void)unregisterTokenId:(NSNumber *)tokenId
                  success:(FWTRequestManagerSuccessBlock)success
                  failure:(FWTRequestManagerFailureBlock)failure
{
    [self updateDeviceWithTokenId:tokenId
                           params:@{@"device_token": @{@"user_alias": @""}}
                          success:success
                          failure:failure];
}

- (void)markNotificationAsOpenedWithId:(NSString *)notificationId
                         deviceTokenId:(NSString *)deviceTokenId
                                  user:(NSString *)user
                               success:(FWTRequestManagerSuccessBlock)success
                               failure:(FWTRequestManagerFailureBlock)failure
{
    NSAssert(deviceTokenId != nil, @"Device token id missing");
    NSAssert(notificationId != nil, @"Notification id missing");
    NSAssert(user != nil, @"User name is missing");
    
    if (user.length == 0 || deviceTokenId.length == 0) {
        if (failure) {
            failure(404,[NSError fwt_invalidOperationErrorWithUnderlyingError:nil]);
        }
        return;
    }
    
    NSString *path = [NSString stringWithFormat:FWTNotificationOpenPath, notificationId];
    [self _updateAuthenticationForPath:path httpMethod:@"POST"];
    [self.httpSessionManager POST:path
                      parameters:@{@"device_token_id": deviceTokenId,
                                   @"user": @{@"alias":user}}
                         success:[self _defaultSuccessHandler:success]
                         failure:[self _defaultFailureHandler:failure success:success]];
}

- (void)markNotificationAsReceivedWithId:(NSString *)notificationId
                           deviceTokenId:(NSString *)deviceTokenId
                                 success:(FWTRequestManagerSuccessBlock)success
                                 failure:(FWTRequestManagerFailureBlock)failure {
    NSAssert(deviceTokenId != nil, @"Device token id missing");
    NSAssert(notificationId != nil, @"Notification id missing");
    
    if (notificationId.length == 0 || deviceTokenId.length == 0) {
        if (failure) {
            failure(404,[NSError fwt_invalidOperationErrorWithUnderlyingError:nil]);
        }
        return;
    }
    
    NSString *path = [NSString stringWithFormat:FWTNotificationReceivedPath, notificationId];
    [self _updateAuthenticationForPath:path httpMethod:@"POST"];
    [self.httpSessionManager POST:path
                       parameters:@{@"device_token_id": deviceTokenId}
                          success:[self _defaultSuccessHandler:success]
                          failure:[self _defaultFailureHandler:failure success:success]];
}

#pragma mark - Private Methods
- (FWTAFNetworkingSuccessBlock) _defaultSuccessHandler:(FWTRequestManagerSuccessBlock)success
{
    return ^(id  _Nullable responseObject) {
        if ([responseObject isKindOfClass:[NSDictionary class]]) {
            success(responseObject);
        } else {
            success(nil);
        }
    };
}

- (FWTAFNetworkingFailureBlock) _defaultFailureHandler:(FWTRequestManagerFailureBlock)failure success:(FWTRequestManagerSuccessBlock)success
{
    return ^(NSInteger responseCode, NSError * _Nonnull error) {
        if (responseCode == 200) {
            if (success) {
                success(nil);
            }
        } else if(failure) {
            failure(responseCode, error);
        }
    };
}

- (void) _updateAuthenticationForPath:(NSString *)path
                           httpMethod:(NSString *)httpMethod
{
    NSDictionary *headers = [self.authenticator authHeadersForPath:path
                                                        httpMethod:httpMethod
                                                        andHeaders:self.httpSessionManager.HTTPRequestHeaders];
    for (NSString *header in headers.keyEnumerator) {
        [self.httpSessionManager setValue:headers[header]
                       forHTTPHeaderField:header];
    }
}

- (NSError *) _errorForStatusCode:(NSInteger)statusCode withUnderlyingError:(NSError *)underlyingError
{
    switch (statusCode) {
        case 401:
            return [NSError fwt_userAliasErrorWithUnderlyingError:underlyingError];
        case 403:
            return [NSError fwt_forbiddenErrorWithUnderlyingError:underlyingError];
        case 404:
            return [NSError fwt_invalidOperationErrorWithUnderlyingError:underlyingError];
        default:
            return underlyingError;
            break;
    }
}

@end
