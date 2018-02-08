# 连连看

这个是游戏架构课程实践的时候用了一天半的时间做的，后面又花了点时间完善了一下。最后就是现在这个样子了。游戏具有简洁的界面和舒缓的背景音乐及音效。

有两种开始游戏的方式，一种是从头开始，即每次都是进入新的游戏。 第二种是从最高历史记录开始游戏，系统会加载保存的最高历史记录，包括关卡等级和金币数量。关卡等级越高，图片的种类越多，难度就越大。

## 游戏功能简介

1. 添加时间 `100金币`

    游戏是有时间限制的， 游戏界面底部有一个时间进度条，当进度条走到头时，游戏结束。当时间快要结束的时候，可以用获得的金币购买时间，100个金币可购买10秒钟的时间。

2. 交换位置 `150金币`

    当游戏关卡越高时，图片的种类就越多。有可能存在这样一种情况，就是由于连线的限制，确实不存在可以通过连线消除的图片。这种情况下交换图片位置的功能就显得尤为重要，因为交换两张图片的位置后，就可能让原本不可以连接的两张图片连接起来进而消除掉。但时该功能有一个限制，就是只能交换相邻两张图片的位置，否则如果可以交换任意两个图片的位置，那游戏就没有什么难度可言了。

3. 显示提示 `200金币`

    还有一个更加省心省力省时间的功能，那就是给用户显示一对可以消除的图片，当用户很长时间都没有找到可以连接消除的图片时，点一下这个按钮，见证眼瞎的时刻就到了。我点击过很多次这个按钮，都希望是游戏存在BUG，就是确实不存在可以继续消除的图片的情况，结果每一次都证明是自己眼瞎了。也可能是图片中的那些狗长得太像了吧。

![start1.png](https://github.com/faxinwang/LLK/raw/master/imgs/start1.gif "从头开始")

![start2.png](https://github.com/faxinwang/LLK/raw/master/imgs/start2.gif "从最高历史记录开始")