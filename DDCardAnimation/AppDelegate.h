//
//  AppDelegate.h
//  DDCardAnimation
//
//  Created by tondyzhang on 16/10/10.
//  Copyright © 2016年 tondy. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (readonly, strong) NSPersistentContainer *persistentContainer;

- (void)saveContext;


@end

