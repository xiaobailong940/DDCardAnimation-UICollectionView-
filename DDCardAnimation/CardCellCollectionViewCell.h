//
//  CardCellCollectionViewCell.h
//  DDCardAnimation
//
//  Created by tondyzhang on 16/10/11.
//  Copyright © 2016年 tondy. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CardCellCollectionViewCell : UICollectionViewCell    

@property(nonatomic, strong)NSString* title;
@property(nonatomic, strong)UIColor* bgColor;
@property(nonatomic, strong)UIImage* image;

-(void)setBlur:(CGFloat)ratio; //设置毛玻璃效果

@end
