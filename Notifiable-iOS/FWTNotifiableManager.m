//
//  FWTNotifiableManager.m
//  FWTNotifiable
//
//  Created by Kamil Kocemba on 18/09/2013.
//  Copyright (c) 2013 Future Workshops. All rights reserved.
//

#import "FWTNotifiableManager.h"

#import <CommonCrypto/CommonCrypto.h>
#import <SystemConfiguration/SystemConfiguration.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <AFNetworking/AFNetworking.h>
#import <AFNetworking/AFJSONRequestOperation.h>


NSString * const FWTNotifiableAuthTokenKey = @"auth_token";
NSString * const FWTNotifiableUserInfoKey = @"user";
NSString * const FWTNotifiableDeviceTokenKey = @"token";
NSString * const FWTNotifiableProviderKey = @"provider";

@interface FWTNotifiableManager ()

@property (nonatomic, strong) NSString *deviceToken;
@property (nonatomic, strong) AFHTTPClient *httpClient;

@end

@implementation FWTNotifiableManager

+ (instancetype)sharedManager
{
    static FWTNotifiableManager *sharedManagerInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedManagerInstance = [[FWTNotifiableManager alloc] init];
    });
    return sharedManagerInstance;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.retryAttempts = 5;
        self.retryDelay = 60;
    }
    return self;
}

- (AFHTTPClient *)httpClient
{
    if (!self->_httpClient) {
        self->_httpClient = [AFHTTPClient clientWithBaseURL:self.baseURL];
        self->_httpClient.parameterEncoding = AFJSONParameterEncoding;
    }
    return self->_httpClient;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    self.deviceToken = [[deviceToken.description stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"<>"]] stringByReplacingOccurrencesOfString:@" " withString:@""];
}

- (void)registerTokenWithUserInfo:(NSDictionary *)userInfo
{
    [self registerTokenWithUserInfo:userInfo completionHandler:nil];
}

- (void)registerTokenWithUserInfo:(NSDictionary *)userInfo completionHandler:(FWTNotifiableOperationCompletionHandler)hanlder
{
    NSMutableDictionary *p = [NSMutableDictionary dictionaryWithDictionary:userInfo];
    p[FWTNotifiableDeviceTokenKey] = self.deviceToken;
    p[FWTNotifiableProviderKey] = @"apns";
    [self _registerDeviceWithParams:p attempts:self.retryAttempts completionHandler:hanlder];
}


- (void)unregisterToken
{
    [self unregisterTokenWithCompletionHandler:nil];
}

- (void)unregisterTokenWithCompletionHandler:(FWTNotifiableOperationCompletionHandler)hanlder
{
    [self _unregisterTokenWithAttempts:self.retryAttempts completionHandler:hanlder];
}

#pragma mark - Private

- (void)_registerDeviceWithParams:(NSDictionary *)params
                         attempts:(NSUInteger)attempts
                completionHandler:(FWTNotifiableOperationCompletionHandler)handler
{
    if (attempts == 0){
        if(handler)
            handler(NO);
        return;
    }
    
    [self.httpClient postPath:@"device_tokens" parameters:params success:^(AFHTTPRequestOperation *operation, NSData * responseData) {
        NSError *error;
        NSDictionary *JSON = [NSJSONSerialization JSONObjectWithData:responseData options:0 error:&error];
        if ([[JSON valueForKey:@"status"] integerValue] == 0) {
            NSLog(@"Did register for push notifications with token: %@", self.deviceToken);
            if(handler)
                handler(YES);
        } else {
            [self _registerDeviceWithParams:params attempts:(attempts - 1) completionHandler:handler];
        }
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"Failed to register device token: %@", error);
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(self.retryDelay * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
            [self _registerDeviceWithParams:params attempts:(attempts - 1)  completionHandler:handler];
        });
    }];
}

- (void)_unregisterTokenWithAttempts:(NSUInteger)attempts
                   completionHandler:(FWTNotifiableOperationCompletionHandler)handler
{
    if (attempts == 0){
        if(handler)
            handler(NO);
        return;
    }
    
    NSString *path = [NSString stringWithFormat:@"device_tokens/%@", self.deviceToken];
    
    [self.httpClient deletePath:path parameters:nil success:^(AFHTTPRequestOperation *operation, NSData * responseData) {
        NSError *error;
        NSDictionary *JSON = [NSJSONSerialization JSONObjectWithData:responseData options:0 error:&error];
        if ([[JSON valueForKey:@"status"] integerValue] == 0) {
            NSLog(@"Did unregister for push notifications");
            if(handler)
                handler(YES);
        } else {
            [self _unregisterTokenWithAttempts:attempts - 1 completionHandler:handler];
        }
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"Failed to unregister for push notifications");
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(self.retryDelay * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
            [self _unregisterTokenWithAttempts:attempts - 1 completionHandler:handler];
        });
    }];

}

@end