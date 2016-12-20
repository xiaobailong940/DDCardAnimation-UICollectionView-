# DDCardAnimation-UICollectionView-
控件--UICollectionView
这个动画是用UICollectionView实现的，简单讲下UICollectionView的工作原理。这里用到的UICollectionView也就3部分：ViewController（简称VC）、UICollectionViewCell、UICollectionViewLayout。

VC：
在VC里，UICollectionView的用法跟UITableView的用法类似。这里的初始化方法与UITableview有所不同，多了个collectionViewLayout属性，每个collectionView都会绑定一个UICollectionViewLayout对象，collectionView根据这个layout对象来布局cell。

UICollectionViewCell：
这里用的Cell实现起来和UITableViewCell没什么大区别，我们只要实现它的initwithFrame的初始化方法即可，然后实现你想要的布局。

UICollectionViewLayout：
https://developer.apple.com/reference/uikit/uicollectionviewlayout

UICollectionViewLayout是让UICollectionView千变万化的精髓所在，所以上面的动画的重点也就是在layout里实现的。
系统提供了一个UICollectionViewFlowLayout，是一个流式布局，可以通过设置scrollDirection来指定滚动方向，如果这个系统提供的布局不能满足我们的需求，那我们就要自己实现一个UICOllectionViewLayout的子类来达到我们想要的效果了，接下来说下自定义的layout需要重写哪几个方法。

//当layout第一次展示时调用，显式或隐式地调用invalidatedlayout也会调用prepareLayout，During each layout update, the collection view calls this method first to give your layout object a chance to prepare for the upcoming layout operation
- (void)prepareLayout；

//返回collectionView的contentSize，来决定collectionView的滚动范围
- (CGSize)collectionViewContentSize；

//Returns the content offset to use after an animated layout update or change。如果你的动画有两个layout的切换，那么这个方法至关重要，用它来返回一个目标contentOffset，能保证动画的正常表现。
-(CGPoint)targetContentOffsetForProposedContentOffset:(CGPoint)proposedContentOffset；

//Returns the layout attributes for all of the cells and views in the specified rectangle.
- (NSArray *)layoutAttributesForElementsInRect:(CGRect)rect；

//重中之重的方法，用来计算指定indexPath所对应的布局信息。这里的布局信息是一个UICollectionViewLayoutAttributes对象，我们可以通过frame、center、transform3D、transform来控制cell的表现状态。最后将这个attributes对象返回给上层使用。
-(UICollectionViewLayoutAttributes*)layoutAttributesForItemAtIndexPath:(NSIndexPath *)indexPath；
动画实现细节
上面是关于UICollectionView的一些初步介绍，还有很多使用细节待君探索。接下来说下我们这个动画的实现细节吧。

上面的动画中，包括了位移、缩放、透明度三个方面的变化，我们先来谈谈重点的位移部分吧。

思路：动画的效果是，随着手指的滑动，cell从上往下慢慢位移，位移的过程中速度越来越快；相反，如果往上滑动，cell移动的速度就慢慢变小。这里的自变量是collectionView.contentOffset.y（竖直方向），因变量是cell.center.y（或者说cell.frame.origin.y），所以需要为他们之间找一个函数，这个函数需要满足上面的动画效果。

根据变化的规律，先确定一个初步的函数：equation，它符合从小到大变化递增的特性，可以通过一些在线工具查看函数的图像来比较直观的看到函数的变化规律

可以看到这个函数是有两段的，那我们需要的只是左边这部分，因为当我们手指下滑的时候，collectionView.contentOffset.y实际上是变小的，而cell.origin.y却是在变大的，所以左边半部分的变化正是我们想要的。

然而equation并不能满足要求，需要将它①向右拉伸m个点、②向上拉伸n个点，也就是要构造一个函数：


这里解释一下m、n的值的含义。

将

做上述两步变化，m=600，n=500，生成如下函数
Alt text

函数与x轴交点是(600,0)，与y轴交点是(0,500)。
(0,500)比较好理解，就是当collectionView.contentOffset.y等于0的时候，cell对应的y坐标为500。
(600,0)也不难理解，就是当我们手指往上滑600个点，使collectionView.contentOffset.y=600时，这个时候cell.origin.y会等于0。

上述部分是实现位移动画的函数基础。下面解释函数的具体应用。

定义第0个cell的位移函数

假设第0个cell（简称cell0）对应函数的m、n分别为n0=250，m0=1000，即当collectionView.contentOffset.y=0时，cell0.origin.y=250；当我们往上滑1000个点，collectionView.contentOffset.y=1000时，cell0.origin.y=0。同理，
ni则表示当contentOffset.y=0时，第i个cell的y坐标。mi则表示当contentOffset.y=mi时，第i个cell的y坐标为0.

所以对于第0个cell，我们可以给出一个函数来计算它的y坐标：；这里x是指collectionView.contentOffset.y，y0是指cell0.origin.y；

生成第i个cell的位移函数

定义了cell0的位置函数，就可以以一定规律生成cell1、cell2……的位置函数了，也就是生成每个cell的m、n值。

的计算

我们可以定义，当手指往下滑动140个点时，第1个cell会运动到第2个cell的位置，以此类推，每个cell会运动到下一个cell的位置。所以我们定义$m_i=m_{i-1}+140$ 也就是 $m_i=m_0 + itimes140$; （ps:这里140决定了cell之间的间距，当然可以根据需求改变这个值来调整视觉效果）

的计算

需要通过$y_0=((1000-x)/1000)^4times250$来计算。我们知道，当x=0时，$y_0=n_0=250$。在上一步计算时，我们定义了手指往下滑140时第0个cell会运动到第1个cell的位置，也就是说cell1的位置$n_1$可以由$y_0=((1000-x)/1000)^4times250$得到，这里x的值应该是手指从0下滑140个点，也就是collectionView.contentOffset.y=-140，所以x=(-140)。（ps：往下滑动时contentOffset.y是递减的，所以这里的x是负的140。）所以.同理，可以推出的公式：;

的公式

ok，mi和ni都可由m0、n0得到，那么的公式就可以转化成和的表达式，即。虽然这个函数看起来挺长的，但是其中m0和n0都是我们定的初始值，140也是我们定义的常量。变量x就是contentOffset.y。所以到此我们已经能根据手指的滑动，计算出每个cell的y坐标，从而实现了这个滚动动画。

公式体现在下面的函数里：

#pragma mark -- 公式
//根据下标、当前偏移量来获取对应的y坐标
-(CGFloat)getOriginYWithOffsetY:(CGFloat)offsetY row:(NSInteger)row
{
    // 公式： y0 = ((m0 - x)/m0)^4*n0
    // 公式:  yi=((m0 + i*140-x)/(m0 + i*140))^4*((m0+140*i)/m0)^4*n0
    CGFloat x = offsetY;    //这里offsetY就是自变量x
    CGFloat ni = [self defaultYWithRow:row];
    CGFloat mi = self.m0+row*self.deltaOffsetY;
    CGFloat tmp = mi - x;
    tmp = fmaxf(0, tmp);    //不小于0
    CGFloat y = powf((tmp)/mi, 4)*ni;
//    NSLog(@"%d--y:%f ",(int)row,y);
    return y;
}

//获取当contentOffset.y=0时每个cell的y值
-(CGFloat)defaultYWithRow:(NSInteger)row
{
    CGFloat x0 = 0;     //初始状态
    CGFloat xi = x0 - self.deltaOffsetY*row;
    CGFloat ni = powf((self.m0 - xi)/self.m0, 4)*self.n0;
//    NSLog(@"defaultY-%d: %f",(int)row,ni);
    return ni;
}
关于大小变化、透明度变化，都是根据位置来计算的，这里就不详细讲了，有兴趣可以看下代码，也可以换个函数来达到你想要的效果。
