//
//  FWTNotifiableAuthenticator.m
//  Notifiable-iOS
//
//  Created by Igor Fereira on 15/01/2016.
//  Copyright © 2016 Future Workshops. All rights reserved.
//

#import "FWTNotifiableAuthenticator.h"
#import "NSDate+FWTNotifiable.h"
#import <CommonCrypto/CommonCrypto.h>

NSString * const FWTAuthFormat = @"APIAuth %@:%@";

NSString * const FWTAuthHeader = @"Authorization";
NSString * const FWTTimestampHeader = @"Date";

NSString * const FWTContentTypeHeader = @"Content-Type";
NSString * const FWTContentAcceptHeader = @"Content-Accept";

NSString * const FWTDefaultContentType = @"application/x-www-form-urlencoded";

@interface FWTNotifiableAuthenticator ()

@property (nonatomic, strong) NSDateFormatter *httpDateFormatter;
@property (nonatomic, strong) NSString *accessId;
@property (nonatomic, strong) NSString *secretKey;

@end

@implementation FWTNotifiableAuthenticator

- (instancetype)initWithAccessId:(NSString *)accessId
                    andSecretKey:(NSString *)secretKey
{
    self = [super init];
    if (self) {
        self.accessId = accessId;
        self.secretKey = secretKey;
    }
    return self;
}

- (NSDateFormatter *)httpDateFormatter
{
    if (!self->_httpDateFormatter) {
        self->_httpDateFormatter = [[NSDateFormatter alloc] init];
        [self->_httpDateFormatter setDateFormat:@"EEE',' dd' 'MMM' 'yyyy HH':'mm':'ss 'GMT'"];
    }
    return self->_httpDateFormatter;
}

- (NSDictionary *) authHeadersForPath:(NSString *)path
                           andHeaders:(NSDictionary <NSString *, NSString *>*)headers
{
    NSString *contentType = headers[FWTContentTypeHeader];
    if (contentType == nil) {
        contentType = FWTDefaultContentType;
    }
    
    NSDate *timestamp = [NSDate fwt_gmtDate];
    NSString* canonicalString = [self _canonicalStringForPath:path
                                                  contentType:contentType
                                                      andDate:timestamp];
    
    NSString* encryptedString = [self _hmacHashForString:canonicalString
                                                 withKey:self.secretKey];
    
    NSString* authField = [NSString stringWithFormat:FWTAuthFormat, self.accessId, encryptedString];
    
    return @{FWTAuthHeader:authField,
             FWTTimestampHeader:[self.httpDateFormatter stringFromDate:timestamp],
             FWTContentTypeHeader: contentType};
}

#pragma mark - Private methods

- (NSString *) _canonicalStringForPath:(NSString *)path
                           contentType:(NSString *)contentType
                                  andDate:(NSDate *)date
{
    NSString* uri = [NSString stringWithFormat:@"/%@", path];
    NSString* timestamp = [self.httpDateFormatter stringFromDate:date];
    
    return [NSString stringWithFormat:@"%@,,%@,%@",contentType, uri, timestamp];
}

- (NSString *) _hmacHashForString:(NSString *)string
                          withKey:(NSString *)key
{
    unsigned char cHMAC[CC_SHA1_DIGEST_LENGTH];
    const char *keyChar = [key UTF8String];
    const char *stringChar = [string UTF8String];
    CCHmac(kCCHmacAlgSHA1, keyChar, strlen(keyChar), stringChar, strlen(stringChar), cHMAC);
    NSData *encriptedData = [[NSData alloc] initWithBytes:cHMAC length:sizeof(cHMAC)];
    NSString *base64 = [encriptedData base64EncodedStringWithOptions:0];
    return base64;
}

@end
