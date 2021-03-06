//
//  FWTNotifiableManager.h
//  FWTNotifiable
//  Copyright (c) 2013 Future Workshops. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString * const FWTNotifiableNotificationDevice;
extern NSString * const FWTNotifiableNotificationError;
extern NSString * const FWTNotifiableNotificationDeviceToken;

@protocol FWTNotifiableLogger;

@class FWTNotifiableDevice;
@class FWTNotifiableManager;

typedef void (^FWTNotifiableOperationCompletionHandler)(FWTNotifiableDevice * _Nullable device, NSError * _Nullable error);
typedef void (^FWTNotifiableListOperationCompletionHandler)(NSArray<FWTNotifiableDevice*> * _Nullable devices, NSError * _Nullable error);

typedef void (^FWTNotifiableDidRegisterBlock)(FWTNotifiableManager *manager, NSData * token);
typedef void (^FWTNotifiableDidReceiveNotificationBlock)(FWTNotifiableManager *manager, FWTNotifiableDevice * device, NSDictionary *notification);

@protocol FWTNotifiableManagerListener <NSObject>

@optional
- (void)applicationDidRegisterForRemoteNotificationsWithToken:(NSData *)token NS_SWIFT_NAME(applicationDidRegisterForRemoteNotification(token:));
- (void)applicationDidReciveNotification:(NSDictionary *)notification NS_SWIFT_NAME(applicationDidReceive(notification:));
- (void)notifiableManager:(FWTNotifiableManager *)manager didRegisterDevice:(FWTNotifiableDevice *)device NS_SWIFT_NAME(manager(_:didRegisterDevice:));
- (void)notifiableManager:(FWTNotifiableManager *)manager didFailToRegisterDeviceWithError:(NSError *)error NS_SWIFT_NAME(manager(_:didFailToRegisterDevice:));

@end

/**
 The FWTNotifiableManager is the interface between the iOS application and a Notifiable-Rails gem server
 
 @see <a href="https://github.com/FutureWorkshops/notifiable-rails">Notifiable-Rails gem</a>
*/
NS_SWIFT_NAME(NotifiableManager)
@interface FWTNotifiableManager : NSObject

/** Number of times that the manager will try to resend the informations in case of error */
@property (nonatomic, assign) NSInteger retryAttempts;
/** Delay between retries  */
@property (nonatomic, assign) NSTimeInterval retryDelay;
/** Level of the informations that will be logged by the manager */
@property (nonatomic, strong) id<FWTNotifiableLogger> logger;
/** Current device. If the device is not registered, it will be nil. */
@property (nonatomic, copy, readonly, nullable) FWTNotifiableDevice *currentDevice;

#pragma mark - Support Methods


/**
 This is a support method that allows you to move an old Notifiable data to a shared container

 @param groupId Group Id where the data will be copied to
 */
+ (void) syncronizeDataWithGroupId:(NSString *)groupId;

#pragma mark - Permission notification
/**
 Inform the Notifiable Manager that the application did register for remote notifications
 
 @param application Application that was registered
 @param deviceToken Device APNS token
 */
+ (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(nonnull NSData *)deviceToken NS_SWIFT_NAME(application(_:didRegisterForRemoteNotificationsWithDeviceToken:));

#pragma mark - Listener operations
/**
 Register an object to be informed when a asynchronous operation related to the managers is performed
 @param listener    Object to listen for the notifications
 */
+ (void)registerManagerListener:(id<FWTNotifiableManagerListener>)listener NS_SWIFT_NAME(register(_:));

/**
 Unregister an object previously registered as listener
 @param listener    Object previously registered
 */
+ (void)unregisterManagerListener:(id<FWTNotifiableManagerListener>)listener NS_SWIFT_NAME(unregister(_:));

/**
 Remove all the registered listeners
 */
+ (void) cleanUp;

#pragma mark - Read receipts can be delivered back to server via this method
/**
 Notify the server that a notification was read.
 
 @param notificationInfo    The information of the notification given by the system
 
 @return A flag to indicate if the notifications is from Notifiable server or not
 */
+ (BOOL)applicationDidReceiveRemoteNotification:(NSDictionary *)notificationInfo DEPRECATED_MSG_ATTRIBUTE("This is no longer used. Now, see the markNotificationAsReceived method, and isValidNotification to achieve the same result");


/**
 Check if a notification was sent from a Notifiable server. This is useful when there
 is multiple frameworks sending notifications.
 
 @param notificationInfo    The information of the notification given by the system
 
 @return A flag to indicate if the notifications is from Notifiable server or not
 */
+ (BOOL)isValidNotification:(NSDictionary *)notificationInfo;

#pragma mark - Notification operations

/**
 Notify the server that a notification was read and listen for the server response
 
 @param notificationInfo    The information of the notification given by the system
 @param handler             Block called once that the operation is finished.
 
 @return A flag to indicate if the notifications is from Notifiable server or not
 */
- (BOOL)markNotificationAsOpened:(NSDictionary *)notificationInfo
           withCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(markAsOpen(notification:completion:)) DEPRECATED_MSG_ATTRIBUTE("Use class method instead");

/**
 Notify the server that a notification was read and listen for the server response
 
 @param notificationInfo    The information of the notification given by the system
 @param groupId             Group being used to share the server configuration (useful for extensions)
 @param handler             Block called once that the operation is finished.
 
 @return A flag to indicate if the notifications is from Notifiable server or not
 */
+ (BOOL)markNotificationAsOpened:(NSDictionary *)notificationInfo
                         groupId:(NSString * _Nullable)groupId
           withCompletionHandler:(nullable void(^)(NSError * _Nullable))handler NS_SWIFT_NAME(markAsOpen(notification:groupId:completion:));

/**
 Notify the server that a notification was read and listen for the server response
 
 @param notificationInfo    The information of the notification given by the system
 @param groupId             Group being used to share the server configuration (useful for extensions)
 @param logger              Logger object, used to store messages from the system
 @param handler             Block called once that the operation is finished.
 
 @return A flag to indicate if the notifications is from Notifiable server or not
 */
+ (BOOL)markNotificationAsOpened:(NSDictionary *)notificationInfo
                         groupId:(NSString * _Nullable)groupId
                          logger:(id<FWTNotifiableLogger> _Nullable)logger
           withCompletionHandler:(nullable void(^)(NSError * _Nullable))handler NS_SWIFT_NAME(markAsOpen(notification:groupId:logger:completion:));


/**
 Notify the server that a notification was read and listen for the server response
 
 @param notificationInfo    The information of the notification given by the system
 @param handler             Block called once that the operation is finished.
 
 @return A flag to indicate if the notifications is from Notifiable server or not
 */
+ (BOOL)markNotificationAsOpened:(NSDictionary *)notificationInfo
           withCompletionHandler:(nullable void(^)(NSError * _Nullable))handler NS_SWIFT_NAME(markAsOpen(notification:completion:));

/**
 This informs the server that a notification was delivered to the device

 @param notificationInfo The payload that is provided in the notification
 @param handler Method that is called once the method is completed
 @return An indication if it is a valid notification or not
 */
+ (BOOL)markNotificationAsReceived:(NSDictionary *)notificationInfo
             withCompletionHandler:(nullable void(^)(NSError * _Nullable error))handler NS_SWIFT_NAME(markAsReceived(notification:completion:));


/**
 This informs the server that a notification was delivered to the device
 
 @param notificationInfo The payload that is provided in the notification
 @param groupId          Group being used to share the server configuration (useful for extensions)
 @param handler          Method that is called once the method is completed
 @return An indication if it is a valid notification or not
 */
+ (BOOL)markNotificationAsReceived:(NSDictionary *)notificationInfo
                           groupId:(NSString * _Nullable)groupId
             withCompletionHandler:(nullable void(^)(NSError * _Nullable error))handler NS_SWIFT_NAME(markAsReceived(notification:groupId:completion:));

/**
 This informs the server that a notification was delivered to the device
 
 @param notificationInfo The payload that is provided in the notification
 @param groupId          Group being used to share the server configuration (useful for extensions)
 @param logger           Logger object, used to store messages from the system
 @param handler          Method that is called once the method is completed
 @return An indication if it is a valid notification or not
 */
+ (BOOL)markNotificationAsReceived:(NSDictionary *)notificationInfo
                           groupId:(NSString * _Nullable)groupId
                            logger:(id<FWTNotifiableLogger> _Nullable)logger
             withCompletionHandler:(nullable void(^)(NSError * _Nullable error))handler NS_SWIFT_NAME(markAsReceived(notification:groupId:logger:completion:));

#pragma mark - Initialization

- (instancetype)init NS_UNAVAILABLE;

/**
 Init a notifiable manager with the configurations of the Notifiable-Rails server
 
 @see <a href="https://github.com/FutureWorkshops/notifiable-rails">Notifiable-Rails gem</a>
 
 @param url                 Notifiable-Rails server url
 @param accessId            Access Id of the app
 @param secretKey           Secret key of the app
 @param registerBlock       Block that is called once that the device is registered for receiving notifications
 @param notificationBlock   Block that is called once that the device receives a notification;
 
 @return Manager configured to access a specific Notifiable-Rails server
*/
- (instancetype)initWithURL:(NSURL *)url
                   accessId:(NSString *)accessId
                  secretKey:(NSString *)secretKey
           didRegisterBlock:(_Nullable FWTNotifiableDidRegisterBlock)registerBlock
       andNotificationBlock:(_Nullable FWTNotifiableDidReceiveNotificationBlock)notificationBlock NS_SWIFT_NAME(init(url:accessId:secretKey:didRegister:didRecieve:))   DEPRECATED_MSG_ATTRIBUTE("Use the configureWithURL:accessId:secretKey: and simpler initializer instead");

/**
Init a notifiable manager with the configurations of the Notifiable-Rails server
 
@see <a href="https://github.com/FutureWorkshops/notifiable-rails">Notifiable-Rails gem</a>

@param registerBlock       Block that is called once that the device is registered for receiving notifications
@param notificationBlock   Block that is called once that the device receives a notification;

@return Manager configured to access a specific Notifiable-Rails server
*/
- (instancetype)initWithDidRegisterBlock:(_Nullable FWTNotifiableDidRegisterBlock)registerBlock
                    andNotificationBlock:(_Nullable FWTNotifiableDidReceiveNotificationBlock)notificationBlock NS_SWIFT_NAME(init(didRegister:didRecieve:));

/**
Init a notifiable manager with the configurations of the Notifiable-Rails server
 
@see <a href="https://github.com/FutureWorkshops/notifiable-rails">Notifiable-Rails gem</a>

@param urlSession                         NSURLSession used to perform network requests
@param registerBlock       Block that is called once that the device is registered for receiving notifications
@param notificationBlock   Block that is called once that the device receives a notification;

@return Manager configured to access a specific Notifiable-Rails server
*/
- (instancetype)initWithURLSession:(NSURLSession *)urlSession
                  didRegisterBlock:(_Nullable FWTNotifiableDidRegisterBlock)registerBlock
                    andNotificationBlock:(_Nullable FWTNotifiableDidReceiveNotificationBlock)notificationBlock NS_SWIFT_NAME(init(session:didRegister:didRecieve:));

/**
 Init a notifiable manager with the configurations of the Notifiable-Rails server
 
 @see <a href="https://github.com/FutureWorkshops/notifiable-rails">Notifiable-Rails gem</a>
 
 @param group               An string representing the group id in which the SDK saved data will be accessible. If nil, no data is available outside the app.
 @param registerBlock       Block that is called once that the device is registered for receiving notifications
 @param notificationBlock   Block that is called once that the device receives a notification;
 
 @return Manager configured to access a specific Notifiable-Rails server
 */
- (instancetype)initWithGroupId:(NSString * _Nullable)group
               didRegisterBlock:(_Nullable FWTNotifiableDidRegisterBlock)registerBlock
           andNotificationBlock:(_Nullable FWTNotifiableDidReceiveNotificationBlock)notificationBlock NS_SWIFT_NAME(init(groupId:didRegister:didRecieve:));

/**
 Init a notifiable manager with the configurations of the Notifiable-Rails server
 
 @see <a href="https://github.com/FutureWorkshops/notifiable-rails">Notifiable-Rails gem</a>
 
 @param group                            An string representing the group id in which the SDK saved data will be accessible. If nil, no data is available outside the app.
 @param urlSession                         NSURLSession used to perform network requests
 @param registerBlock           Block that is called once that the device is registered for receiving notifications
 @param notificationBlock   Block that is called once that the device receives a notification;
 
 @return Manager configured to access a specific Notifiable-Rails server
 */
- (instancetype)initWithGroupId:(NSString * _Nullable)group
                     urlSession:(NSURLSession * _Nonnull)urlSession
               didRegisterBlock:(_Nullable FWTNotifiableDidRegisterBlock)registerBlock
           andNotificationBlock:(_Nullable FWTNotifiableDidReceiveNotificationBlock)notificationBlock NS_SWIFT_NAME(init(groupId:session:didRegister:didRecieve:)) NS_DESIGNATED_INITIALIZER;

/**
 This method configures the SDK to a specific Notifiable configuration

 @param url URL of the Notifiable server
 @param accessId Access Id of the specific application
 @param secretKey Secret Key of the specific application
 */
+ (void) configureWithURL:(NSURL *)url
                 accessId:(NSString *)accessId
                secretKey:(NSString *)secretKey NS_SWIFT_NAME(configure(url:accessId:secretKey:));

/**
 This method configures the SDK to a specific Notifiable configuration
 
 @param url         URL of the Notifiable server
 @param accessId    Access Id of the specific application
 @param secretKey   Secret Key of the specific application
 @param groupId     Context to which this configuration will be available
 */
+ (void) configureWithURL:(NSURL *)url
                 accessId:(NSString *)accessId
                secretKey:(NSString *)secretKey
                  groupId:(NSString * _Nullable)groupId NS_SWIFT_NAME(configure(url:accessId:secretKey:groupId:));

#pragma mark - Register Anonymous device

/**
 Register a device, without a user associated to it, but with a name to represent the device.
 If the token already exists in the server, the device locale will be updated.
 Otherwise, a new device will be created with the token and locale provided.
 
 @param name                A label for the device.
 @param locale              The locale of the device.
 @param customProperties   Aditional information about the device.
 @param handler             Block called once that the operation is finished.
 */
-(void)registerAnonymousDeviceWithName:(NSString * _Nullable)name
                                locale:(NSLocale * _Nullable)locale
                      customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
                  andCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(register(name:locale:properties:completion:));

/**
 Register a device, without a user associated to it, but with a name to represent the device.
 If the token already exists in the server, the device locale will be updated. 
 Otherwise, a new device will be created with the token and locale provided.
 
 @param name                A label for the device.
 @param locale              The locale of the device.
 @param customProperties   Aditional information about the device.
 @param platformProperties  Aditional information that can be send as extra settings on the server
 @param handler             Block called once that the operation is finished.
 */
-(void)registerAnonymousDeviceWithName:(NSString * _Nullable)name
                                locale:(NSLocale * _Nullable)locale
                      customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
                    platformProperties:(NSDictionary<NSString *, id> * _Nullable)platformProperties
                  andCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(register(name:locale:properties:platform:completion:));

#pragma mark - Register device to a specific user

/**
 Register a device, with a user associated to it, but with a name to represent the device. 
 If the token already exists in the server, the device locale will be updated. 
 Otherwise, a new device will be created with the token and locale provided. 
 If the user alias doesn't exist, a new user will be created.
 
 @param deviceName  A label for the device.
 @param locale      The locale of the device.
 @param userAlias   The alias of the user in the server.
 @param customProperties   Aditional information about the device
 @param handler     Block called once that the operation is finished.
 */
- (void)registerDeviceWithName:(NSString * _Nullable)deviceName
                     userAlias:(NSString *)userAlias
                        locale:(NSLocale * _Nullable)locale
              customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
          andCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(register(name:userAlias:locale:properties:completion:));

/**
 Register a device, with a user associated to it, but with a name to represent the device.
 If the token already exists in the server, the device locale will be updated.
 Otherwise, a new device will be created with the token and locale provided.
 If the user alias doesn't exist, a new user will be created.
 
 @param deviceName  A label for the device.
 @param locale      The locale of the device.
 @param userAlias   The alias of the user in the server.
 @param customProperties   Aditional information about the device
 @param platformProperties  Aditional information that can be send as extra settings on the server
 @param handler     Block called once that the operation is finished.
 */
- (void)registerDeviceWithName:(NSString * _Nullable)deviceName
                     userAlias:(NSString *)userAlias
                        locale:(NSLocale * _Nullable)locale
              customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
            platformProperties:(NSDictionary<NSString *, id> * _Nullable)platformProperties
          andCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(register(name:userAlias:locale:properties:platform:completion:));

#pragma mark - Update device information
/**
 Update the token of the device
 
 @param token   New device token.
 @param handler Block called once that the operation is finished.
*/
- (void)updateDeviceToken:(NSData *)token
        completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(deviceToken:completion:));
/**
 Update the device locale.
 
 @param locale  New device locale.
 @param handler Block called once that the operation is finished.
*/
- (void)updateDeviceLocale:(NSLocale *)locale
         completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(locale:completion:));
/**
 Update the device token and locale.
 
 @param token   New device token.
 @param locale  New device locale.
 @param handler Block called once that the operation is finished.
*/
- (void)updateDeviceToken:(NSData *)token
              andLocation:(NSLocale *)locale
        completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(deviceToken:locale:completion:));

/**
 Update the device name
 
 @param name    The name of the device in the server (not related to the user).
 @param handler Block called once that the operation is finished.
*/
- (void)updateDeviceName:(NSString *)name
       completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(name:completion:));

/**
 Update the device aditional informations
 
 @param customProperties   Aditional information about the device
 @param handler             Block called once that the operation is finished.
 */
- (void)updateCustomProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
             completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(properties:completion:));

/**
 Update the device platform properties
 
 @param platformProperties  Platform properties
 @param handler             Block called once that the operation is finished.
 */
- (void)updatePlatformProperties:(NSDictionary<NSString *, id> * _Nullable)platformProperties
               completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(platform:completion:));

/**
 Update the informations of the device without change the user.
 
 @param token               New device token.
 @param deviceName          The name of the device
 @param locale              New device locale.
 @param customProperties   Aditional information about the device
 @param handler             Block called once that the operation is finished.
*/
- (void)updateDeviceToken:(NSData * _Nullable)token
               deviceName:(NSString * _Nullable)deviceName
                   locale:(NSLocale * _Nullable)locale
         customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
        completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(deviceToken:name:locale:properties:completion:));

/**
 Update the informations of the device without change the user.
 
 @param token               New device token.
 @param deviceName          The name of the device
 @param locale              New device locale.
 @param customProperties   Aditional information about the device
 @param platformProperties  Aditional information that can be send as extra settings on the server
 @param handler             Block called once that the operation is finished.
 */
- (void)updateDeviceToken:(NSData * _Nullable)token
               deviceName:(NSString * _Nullable)deviceName
                   locale:(NSLocale * _Nullable)locale
         customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
       platformProperties:(NSDictionary<NSString *, id> * _Nullable)platformProperties
        completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(deviceToken:name:locale:properties:platform:completion:));

/**
 Update the informations of the device and change the user.
 
 @param token               New device token.
 @param deviceName          The name of the device
 @param locale              New device locale.
 @param customProperties   Aditional information about the device.
 @param userAlias   The alias of the user in the server.
 @param handler             Block called once that the operation is finished.
 */
- (void)updateDeviceToken:(NSData * _Nullable)token
               deviceName:(NSString * _Nullable)deviceName
                userAlias:(NSString * _Nullable)userAlias
                   locale:(NSLocale * _Nullable)locale
         customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
        completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(deviceToken:name:userAlias:locale:properties:completion:));

/**
 Update the informations of the device and change the user.
 
 @param token               New device token.
 @param deviceName          The name of the device
 @param locale              New device locale.
 @param customProperties    Aditional information about the device.
 @param platformProperties  Aditional information that can be send as extra settings on the server
 @param userAlias   The alias of the user in the server.
 @param handler             Block called once that the operation is finished.
 */
- (void)updateDeviceToken:(NSData * _Nullable)token
               deviceName:(NSString * _Nullable)deviceName
                userAlias:(NSString * _Nullable)userAlias
                   locale:(NSLocale * _Nullable)locale
         customProperties:(NSDictionary<NSString *, id> * _Nullable)customProperties
       platformProperties:(NSDictionary<NSString *, id> * _Nullable)platformProperties
        completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(update(deviceToken:name:userAlias:locale:properties:platform:completion:));

#pragma mark - Device/user relationship
/**
 Associate an anonymous device to a user. If the user alias doesn't exist, 
 a new user will be created.
 
 @warning   The device id will remain the same on the server, but associated with another user.
 
 @param userAlias   The alias of the user in the server.
 @param handler     Block called once that the operation is finished.
*/
- (void)associateDeviceToUser:(NSString *)userAlias
            completionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(associated(to:completion:));

/**
 Remove a token from a specific user and anonymise it. 
 It has a similar effect as registering the device as anonymous.
 
 @warning   The device id and configuration will remain the same in the server.
 
 @param handler Block called once that the operation is finished.
*/
- (void)anonymiseTokenWithCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(anonymise(completion:));

#pragma mark - Unregister
/**
 Delete the device from the server.

 @param handler Block called once that the operation is finished.
*/
- (void)unregisterTokenWithCompletionHandler:(_Nullable FWTNotifiableOperationCompletionHandler)handler NS_SWIFT_NAME(unregister(completion:));

@end

NS_ASSUME_NONNULL_END
