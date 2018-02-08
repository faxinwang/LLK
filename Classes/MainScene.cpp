#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include <vector>
#include "UserData.h"
#include "StartScene.h"
#include <string>
USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

Scene* MainScene::createScene(int level)
{
    return MainScene::create(level);
}

MainScene* MainScene::create(int level) {
	auto scene = new MainScene();
	if (scene && scene->init(level)) {
		scene->autorelease();
		return scene;
	}
	else {
		delete scene;
		scene = 0;
		return 0;
	}
}

// on "init" you need to initialize your instance
bool MainScene::init(int level)
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	srand(time(0));
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//背景
	auto bgLayer = LayerColor::create(Color4B(160, 170, 180, 255), visibleSize.width, visibleSize.height);
	this->addChild(bgLayer, -2);

	//加载dog图片
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/imgs/Dogs.plist");

	
	_row = 12;
	_col = 14;
	_level = level;
	_lvObj = 0;
	//初始级别大于1说明选择的是从最高历史记录开始游戏,所以金币要设置为记录中的金币
	if (_level > 1) {
		UserData::getInstance()->setCoin(UserData::getInstance()->getMaxLevelCoin());
	}
	else {
		UserData::getInstance()->setCoin(0);
	}

	//初始化界面
	initTable(_level);
	
	//显示Level
	auto lab_lv = Label::create("", "Hobo Std", 20);
	lab_lv->setString(String::createWithFormat("Level:%d", level)->getCString());
	lab_lv->setPosition(lab_lv->getContentSize().width / 2+10, visibleSize.height - 10);
	this->addChild(lab_lv, 1, "lab_lv");

	//金币图片
	auto coinSpr = Sprite::create("res/imgs/coin2.png");
	coinSpr->setScale(0.4);
	coinSpr->setAnchorPoint(ccp(0.5, 0.5));
	coinSpr->setPosition(visibleSize.width - 30, visibleSize.height - 30);
	this->addChild(coinSpr, 0, "coinSpr");
	//金币数量
	_coins = Label::create("", "Arial Black", 35);
	//如果从头开始, 金币数量从设为0，如果从最高记录开始,金币数量设为历史记录
	_coins->setString(String::createWithFormat("%d", 
		level==1 ? 0 : UserData::getInstance()->getCoin())->getCString());
	_coins->setAnchorPoint(ccp(1, 1));
	_coins->setPosition(visibleSize.width - coinSpr->getContentSize().width + 50, visibleSize.height - 3);
	_coins->setTextColor(Color4B(250,218,31,255));
	this->addChild(_coins);
	
	//服务按钮
	auto services = Label::create("Service------", "Hobo Std", 25);
	services->setAnchorPoint(ccp(1, 1));
	services->setPosition(visibleSize.width,
		coinSpr->getPositionY() - 40);
	this->addChild(services);

	//服务-添加时间
	auto str = String::createWithFormat("AddTime \n(%d$=10S) ", _serv_addTime)->getCString();
	auto serv_addTime = Label::create(str, "Hobo Std",22);
	menuItem_addTime = MenuItemLabel::create(serv_addTime,
		CC_CALLBACK_0(MainScene::serviceAddTime, this));
	menuItem_addTime->setAnchorPoint(ccp(1, 1));
	menuItem_addTime->setPosition(0, -20);
	
	//服务-交换位置
	str = String::createWithFormat("SwapDog  \n(%d$) ",_serv_swapDog)->getCString();
	auto serv_swapDog = Label::create(str, "Hobo Std", 22);
	menuItem_swapDog = MenuItemLabel::create(serv_swapDog, 
		CC_CALLBACK_0(MainScene::serviceSwapDog, this));
	menuItem_swapDog->setAnchorPoint(ccp(1, 1));
	menuItem_swapDog->setPosition(-10,-100);

	//服务-获取提示
	str = String::createWithFormat("ShowHint \n(%d$) ",_serv_showHint)->getCString();
	auto serv_showHint = Label::create(str, "Hobo Std", 22);
	menuItem_showHint = MenuItemLabel::create(serv_showHint,
		CC_CALLBACK_0(MainScene::serviceShowHint, this));
	menuItem_showHint->setPosition(ccp(1, 1));
	menuItem_showHint->setPosition(-60, -210);

	//返回开始场景
	auto back = Label::create(" back ", "Hobo Std", 22);
	auto menuItem_back = MenuItemLabel::create(back, 
		CC_CALLBACK_0(MainScene::backToStartScene, this));
	menuItem_back->setAnchorPoint(ccp(1, 1));
	menuItem_back->setPosition( -10, 90 - services->getPositionY());

	//菜单
	auto menus = Menu::create(menuItem_addTime,menuItem_swapDog,menuItem_showHint,
		menuItem_back, NULL);
	menus->setAnchorPoint(ccp(1, 1));
	menus->setPosition(visibleSize.width, services->getPositionY() - 20);
	this->addChild(menus);

	//初始化时间线
	timeLine = TimeLine::create(_lvObj->getTimeLimit());
	this->addChild(timeLine);

	//点击事件监听器
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegin, this);
	_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	

	//drawNode 画两个图片之间的路径
	drawNode = DrawNode::create();
	this->addChild(drawNode);

//	makeValid(); //由于提供了交换两个狗狗图片的服务，可以去掉这一步。

	scheduleUpdate();

	SimpleAudioEngine::getInstance()->playBackgroundMusic("res/sounds/BGM.mid",true);

    return true;
}

void MainScene::initTable(int level){
	_win = false;
	if (_lvObj) delete _lvObj;
	_lvObj = new Levels(level);

	//时间线
	if (timeLine) {
		timeLine->removeFromParent();
		timeLine = TimeLine::create(_lvObj->getTimeLimit());
		this->addChild(timeLine);
	}
	//Level
	auto lab_lv = (Label*)this->getChildByName("lab_lv");
	if (lab_lv) lab_lv->setString(String::createWithFormat("Level:%d", level)->getCString());

	//1.由于相同的图片必须出现偶数次才能消除所有的图片，所以每选中一张图片后就添加两次。
	//2.N是总共需要添加的狗狗图片的数量
	//3.ndog是狗狗图片的种数，种数越多，难度越大
	//4.需要保证给定范围内的所有狗狗图片都被选中，所以先将给定范围内的图片各添加一次（两张）到vector中
	vector<int> v;
	int x, y = 25, idx = 0, N = _row*_col, ndog=_lvObj->getNDog();
	v.reserve(N + 5);
	int i = 1;
	for (; i <= ndog; ++i) {
		v.push_back(i);
		v.push_back(i);
	}
	for (int n=N/2; i <= n; ++i) {
		int id = random() % ndog + 1;
		v.push_back(id);
		v.push_back(id);
	}
	//混洗， 打乱顺序
	for (int i = 0, n = v.size(); i < n; ++i) swap(v[i], v[random() % n]);

	//计算图片所在位置，周边留有一圈空位用于路径搜索和路径绘制
	for (int i = 0; i <= _row + 1; ++i) {
		x = 0;
		for (int j = 0; j <= _col + 1; ++j) {
			x += 42;
			_pos[i][j].x = x;
			_pos[i][j].y = y;
			//留空位
			if (i == 0 || j == 0 || i == _row + 1 || j == _col + 1) continue;

			auto dog = Dog::create(v[idx++]);
			this->addChild(dog);
			dog->setPosition(x, y);
			dog->setAnchorPoint(ccp(0.5, 0.5));
			_dogs[i][j] = dog;
		}
		y += 42;
	}
}

/*
当最后剩下4张图片且局面如下时，就存在bug
 1 0
 0 1
 这种情况下就没办法消除掉这剩下的四张图片
 在还没开发出交换图片位置的服务时，必须消除这种情况。
*/
void MainScene::makeValid() {
	for (int i = 1, n = _row ; i < n; ++i) {
		for (int j = 1, m = _col; j < m; ++j) {
			if (_dogs[i][j]->_id == _dogs[i + 1][j + 1]->_id
				&& _dogs[i + 1][j]->_id == _dogs[i][j + 1]->_id
				&& _dogs[i][j]->_id != _dogs[i][j+1]->_id) {
				_dogs[i][j]->runAction(MoveTo::create(0.5, _pos[i][j + 1]));
				_dogs[i][j+1]->runAction(MoveTo::create(0.5, _pos[i][j]));
				swap(_dogs[i][j], _dogs[i][j + 1]);
			}
		}
	}
}


//不断更新绘制时间进度条，当进度条走满而图片未消除完毕时，游戏结束。
void MainScene::update(float dt) {
	if (timeLine->getState() == TimeLine::State::State_Running && !_win) {
		timeLine->Increase(dt);
		timeLine->draw();
		if (timeLine->getState() == TimeLine::State::State_Over) {
			gameOver();
			timeLine->reset();
		}
	}
}


/*
1.当游戏处于交换图片位置状态时
	1.1如果选中的两张图片不相邻，则取消选择第一张选中的图片，将当前选择的图片设为第一张选中的图片
	1.2如果选中的两张图片相邻，交换选中的相邻的两张图片
2.当游戏处于正常状态时
	2.1如果选择的第二张图片与第一张图片不同（通过ID判断），则取消选择的第一张图片， 
	   将当前选择的图片设为选中的第一张图片
	2.2如果选中的两张图片相同，则用DFS()搜索路线
		2.2.1如果两张图片之间存在一条可行的路线，则绘制出找到的路线，然后消除掉这两种图片。
		2.2.2如果两张图片之间不存在可行路线，则将这两张图片设为未选中状态。

*/
bool MainScene::onTouchBegin(Touch* touch, Event* event) {
//	if (_gameOver) return false;
	for (int i = 1; i <= _row; ++i) {
		for (int j = 1; j <= _col; ++j) {
			if (_dogs[i][j] && _dogs[i][j]->selected) {
				if (_swapingDog) {
					if (sel1 == 0) {
						sel1 = _dogs[i][j];
						sel1_x = i;
						sel1_y = j;
					}
					else if (sel1 != _dogs[i][j]) {
						if (abs(sel1_x - i) + abs(sel1_y - j) == 1) {
							sel2 = _dogs[i][j];
							sel2_x = i;
							sel2_y = j;
							serviceSwapDog();
						}
						else {
							sel1->unselect();
							_dogs[i][j]->unselect();
							sel1 = _dogs[i][j];
						}
					}
				}
				else {
					if (sel1 == 0) {
						sel1 = _dogs[i][j];
						sel1_x = i;
						sel1_y = j;
					}
					else if (sel1->_id != _dogs[i][j]->_id) {
						sel1->unselect();
						sel1 = _dogs[i][j];
						sel1_x = i;
						sel1_y = j;
					}
					else if (sel1->_id == _dogs[i][j]->_id) {
						if (sel1 != _dogs[i][j]) {
							sel2 = _dogs[i][j];
							sel2_x = i;
							sel2_y = j;
							doLink();
						}
					}
				}

			}//end if
		}//end for
	}// end for

	return true;
}


/*
当选中了两张相同的图片后，就调用该函数进行接下来的工作：
1.首先用DFS()函数搜索可行路径
2.如果存在可行路径，则进行如下一些列工作：
	2.1播放狗叫的音效
	2.2绘制找到的路线
	2.3为了能够看到绘制的路线，必须停顿一定的时间，这里停顿了0.5秒
	2.4将两张图片移除
	2.5播放获得金币的动画和音效两次，中级间隔0.3秒
	2.6检查是否移除了所有的图片，如果是，则游戏胜利。

*/
void MainScene::doLink() {
	if (sel1 && sel2) {
		_can = false;
		vector<Vec2> path;
		path.push_back(sel1->getPosition());
		DFS(sel1_x, sel1_y, ' ', 0, path);
		if (_can) {
			_dogs[sel1_x][sel1_y] = 0;
			_dogs[sel2_x][sel2_y] = 0;
			this->runAction(Sequence::create(
				CallFunc::create(CC_CALLBACK_0(MainScene::playEffect, this, "res/sounds/DogBark_LinkSuccess.wav")),
				CallFuncN::create(CC_CALLBACK_0(MainScene::drawPath, this, _path)),
				DelayTime::create(0.5),
				CallFuncN::create(CC_CALLBACK_0(Dog::removeFromParent, sel1)),
				CallFuncN::create(CC_CALLBACK_0(Dog::removeFromParent, sel2)),
				CallFuncN::create(CC_CALLBACK_0(DrawNode::clear, drawNode)),
				CallFunc::create(CC_CALLBACK_0(MainScene::playGainCoinAnimation, this, _pos[sel1_x][sel1_y], _lvObj->getAward())),
				DelayTime::create(0.3),
				CallFunc::create(CC_CALLBACK_0(MainScene::playGainCoinAnimation, this, _pos[sel2_x][sel2_y], _lvObj->getAward())),
				NULL
			));

			if (checkWin()) gameWin();
		}
		else {
			sel1->unselect();
			sel2->unselect();

		}
		sel1 = 0;
		sel2 = 0;
	}
}

/*
播放获得金币的动画和音效
1.首先用精灵显示一张金币的图片，并设置到合适的大小
2.将金币的位置设置在参数给定的位置处（是被消除的图片的位置）
3.动画过程如下：
	3.1金币先向右上“浮动”一小段距离
	3.2金币移动到屏幕右上角大金币所在位置
	3.3将金币图片(精灵)从游戏中移除（否则会占用越来越多的内存）
	3.4更新金币（包括系统中的金币数和界面上显示的金币数）
*/
void MainScene::playGainCoinAnimation(Vec2& pos, int coin) {
	auto spr1 = Sprite::create("res/imgs/coin.png");
	spr1->setScale(0.2);
	spr1->setPosition(pos);
	spr1->setAnchorPoint(ccp(1, 1));
	this->addChild(spr1);
	spr1->runAction(Sequence::create(
		MoveBy::create(0.5, Vec3(5, 20, 0)),
		MoveTo::create(0.5, this->getChildByName("coinSpr")->getPosition()),
		RemoveSelf::create(),
		CallFunc::create(CC_CALLBACK_0(MainScene::updateCoin, this, coin)),
		NULL
	));
}

/*
整个游戏的核心就在这里了，但是这个函数真心不难，只不过搜索的时候需要考虑几点因素。
先介绍参数：
 x,y表示当前在_dogs[][]二维数组中的位置。
 dir表示上一步移动的方向'U','D','L','R' 分别表示Up,Down,Left,Right
 turn表示转向次数，不能超过3次
 path保存了从起点到当前位置的坐标位置。

 这是一个递归函数，思想是在每一个位置都向四周扩展。但是需要注意如下几点：
 1.不能后退，这个通过dir参数控制，例如当上一步是'U'时，表示是向上走的，
   那么接下来的一步就不能往下走，其他情况类似
 2.不能越界。可走的范围为0 <= x <= _row+1, 0 <= y <= _col+1
 3.不能穿过图片，只能从空白地方走(但路径的起点和终点的位置上却是有图片的)。
 4.如果这一步走的方向与上一步走的方向不同，则转弯次数加一
 5.由于是递归函数，肯定有退栈的情况(说明路径不可行)，在退栈的时候，也要从path中去掉相应的位置
*/
void MainScene::DFS(int x, int y,char dir, int turn, vector<Vec2>& path) {
	if (turn > 3 || _can) return;

	if (_dogs[x][y] == sel2){
		_can = true;
		_path = path;
		return;
	}
	//go up
	if (x <= _row && dir != 'D' && (_dogs[x + 1][y] == 0 || _dogs[x + 1][y] == sel2)) {
		path.push_back(_pos[x+1][y]);
		DFS(x + 1, y, 'U', (dir == 'U') ? turn : turn + 1, path);
		path.pop_back();
	}
		
	//go Down
	if (x >= 1 && dir != 'U' && (_dogs[x - 1][y] == 0 || _dogs[x - 1][y] == sel2)) {
		path.push_back(_pos[x-1][y]);
		DFS(x - 1, y, 'D', (dir == 'D') ? turn : turn + 1, path);
		path.pop_back();
	}
		
	//go Left
	if (y >= 1 && dir != 'R' && (_dogs[x][y - 1] == 0 || _dogs[x][y - 1] == sel2)) {
		path.push_back(_pos[x][y-1]);
		DFS(x, y - 1, 'L', (dir == 'L') ? turn : turn + 1, path);
		path.pop_back();
	}
		
	//go Right
	if (y <= _col&& dir != 'L' && (_dogs[x][y + 1] == 0 || _dogs[x][y + 1] == sel2)) {
		path.push_back(_pos[x][y+1]);
		DFS(x, y + 1, 'R', (dir == 'R') ? turn : turn + 1, path);
		path.pop_back();
	}
}

/*
该函数提供提示一条可行路径的服务
其逻辑与DFS()函数类似
*/
void MainScene::autoDFS(int x, int y, char dir, int turn, int id, vector<Vec2>& path) {
	if (turn > 3 || _can) return;
	//log("id=%d", _dogs[x][y]->_id);
	if (_dogs[x][y] && _dogs[x][y]->_id == id && _dogs[x][y]!=sel1) {
		_can = true;
		sel2_x = x;
		sel2_y = y;
		sel2 = _dogs[x][y];
		_path = path;
		
	//	log("_  path size:%d", path.size());
	//	_dogs[x][y] = 0;
		return;
	}
	//go up
	if (x <= _row && dir != 'D' && (_dogs[x + 1][y] == 0 || _dogs[x + 1][y]->_id == id)) {
		path.push_back(_pos[x + 1][y]);
		autoDFS(x + 1, y, 'U', (dir == 'U') ? turn : turn + 1,id, path);
		path.pop_back();
	}

	//go Down
	if (x >= 1 && dir != 'U' && (_dogs[x - 1][y] == 0 || _dogs[x - 1][y]->_id == id)) {
		path.push_back(_pos[x - 1][y]);
		autoDFS(x - 1, y, 'D', (dir == 'D') ? turn : turn + 1,id, path);
		path.pop_back();
	}

	//go Left
	if (y >= 1 && dir != 'R' && (_dogs[x][y - 1] == 0 || _dogs[x][y - 1]->_id == id)) {
		path.push_back(_pos[x][y - 1]);
		autoDFS(x, y - 1, 'L', (dir == 'L') ? turn : turn + 1, id, path);
		path.pop_back();
	}

	//go Right
	if (y <= _col&& dir != 'L' && (_dogs[x][y + 1] == 0 || _dogs[x][y + 1]->_id == id)) {
		path.push_back(_pos[x][y + 1]);
		autoDFS(x, y + 1, 'R', (dir == 'R') ? turn : turn + 1,id, path);
		path.pop_back();
	}
}

//使用drawNode绘制搜索到的路径
void MainScene::drawPath(vector<Vec2>& path) {
	for (int i = 0, n = path.size()-1; i < n; ++i) {
		drawNode->drawSegment(path[i], path[i + 1], 1, Color4F::YELLOW);
	//	log("path[%d]: x=%.0f, y=%.0f", i, path[i].x, path[i].y);
	}
}

//当游戏结束时，清除掉所有剩余的图片，带有动画效果。
void MainScene::clearTable() {
	for (int i = 1; i <= _row; ++i) {
		for (int j = 1; j <= _col; ++j) {
			if (_dogs[i][j]) {
				auto dog = _dogs[i][j];
				dog->runAction(Sequence::create(
					MoveTo::create(1, ccp((i + 1) * 150, ((j + 2) * 15) * 10)),
					CallFuncN::create(CC_CALLBACK_0(Dog::removeFromParent, dog)),
					NULL
				));
				_dogs[i][j] = 0;
			}
		}
	}
}

//检查是否游戏获胜，当所有图片都消除时游戏获胜
bool MainScene::checkWin() {
	for (int i = 1; i <= _row; ++i) {
		for (int j = 1; j <= _col; ++j) {
			if (_dogs[i][j]) return false;
		}
	}
	return true;
}

/*
该函数完成游戏获胜后需要处理的一些列事情，包括：
1.增加难度级别
2.播放游戏获胜的音效(好像一直都没有播放，不知道是什么原因)
3.奖励一定数量的金币（在随机位置播放获得金币的动画，播放获得金币的音效，难度级别越高获得的奖励金币数越多）
4.重新初始化游戏界面
5.存档游戏数据
*/
void MainScene::gameWin() {
	++_level;
	_win = true;
	auto aVeryComplicatedAction = Sequence::create(
		CallFunc::create(CC_CALLBACK_0(MainScene::playEffect, this, "res/sounds/winMusic.wav")),
		DelayTime::create(3),
		Repeat::create(
			Sequence::create(
				CallFunc::create(CC_CALLBACK_0(MainScene::playGainCoinAnimation, this,
					_pos[random() % _row + 1][random() % _col + 1], _lvObj->getAwardPass())),
				CallFunc::create(CC_CALLBACK_0(MainScene::playEffect, this, "res/sounds/gainCoin.wav")),
				DelayTime::create(0.33),
				NULL),
			_level),
		DelayTime::create(1),
		CallFunc::create(CC_CALLBACK_0(MainScene::initTable, this, _level)),
		NULL
	);
	this->runAction(aVeryComplicatedAction);
	if (_level > UserData::getInstance()->getMaxLevel()) {
		UserData::getInstance()->setMaxLevel(_level);
		UserData::getInstance()->setMaxLevelCoin(UserData::getInstance()->getCoin());
		UserData::getInstance()->saveData();
	}
}

/*
游戏结束
存档数据
清除剩余的图片
返回到开始场景
*/
void MainScene::gameOver() {
	if (_level > UserDefault::getInstance()->getIntegerForKey("_level",1)) {
		UserDefault::getInstance()->setIntegerForKey("_level", _level);
		UserData::getInstance()->setMaxLevelCoin(UserData::getInstance()->getCoin());
		UserData::getInstance()->saveData();
	}

	this->runAction(Sequence::create(
		CallFunc::create(CC_CALLBACK_0(MainScene::clearTable, this)),
		DelayTime::create(1.5),
		CallFunc::create(CC_CALLBACK_0(MainScene::backToStartScene, this)),
		NULL
	));
}

/*
1.更新金币系统中记录的金币数量
2.同步更新界面上显示的金币数
3.播放获得金币的音效
4.将右上角的大金币旋转一定的角度
*/
void MainScene::updateCoin(int delta) {
	if (delta == 0) return;
	UserData::getInstance()->addCoin(delta);
	if (delta > 0) {
		this->getChildByName("coinSpr")->runAction(Sequence::create(
			CallFunc::create(CC_CALLBACK_0(MainScene::playEffect,this, "res/sounds/gainCoin.wav")),
			RotateBy::create(0.5, Vec3(0, 360, 0)),
			NULL
		));
	}
	_coins->setString(String::createWithFormat("%d",
		UserData::getInstance()->getCoin())->getCString());
}

/*
添加时间服务，100金币购买10秒钟的时间。
需要完成的事物：
1.如果金币足够，更新金币系统，并将timeLine增大10秒钟
2.如果金币不足，则将金币菜单按钮“闪红”三次以提示金币不足
*/
void MainScene::serviceAddTime() {
	if (UserData::getInstance()->getCoin() < _serv_addTime) {
		auto weave = Sequence::create(TintTo::create(0.1,250,0,0),
			TintTo::create(0.1, 255, 255, 255), NULL);
		menuItem_addTime->runAction(Sequence::create(
			Repeat::create(weave,3),
			NULL
		));
	}
	else {
		updateCoin(-_serv_addTime);
		timeLine->setMaxTime(timeLine->getMaxTime() + 10); //加10秒
		timeLine->clear();
	}
}

/*
交换图片位置的服务
当难度级别升高后，图片的种数将增多，这时候，如果能够交换某两张图片的位置，将会是很大的帮助，但该服务有
一个限制，就是只能交换相邻两张图片的位置。

完成该服务的过程为：
1.当用户点击了该服务按钮后，如果用户的金币数量足够，则进入交换图片状态
2.然后由用户点击要交换位置的图片
3.如果两张图片的位置相邻，则交换两张图片的位置，并更新用户金币数

*/
void MainScene::serviceSwapDog() {
	if (UserData::getInstance()->getCoin() < _serv_swapDog) {
		auto weave = Sequence::create(TintTo::create(0.1, 250, 0, 0),
			TintTo::create(0.1, 255, 255, 255), NULL);
		menuItem_swapDog->runAction(Sequence::create(
			Repeat::create(weave, 3),
			NULL
		));
	}
	else {
		if (!_swapingDog) {
			_swapingDog = true;
			if (sel1) {
				sel1->unselect();
				sel1 = 0;
			}
			if (sel2) {
				sel2->unselect();
				sel2 = 0;
			}
		}
		else if(sel1 && sel2){
			_swapingDog = false;
			auto tmp = _dogs[sel1_x][sel1_y];
			_dogs[sel1_x][sel1_y] = _dogs[sel2_x][sel2_y];
			_dogs[sel2_x][sel2_y] = tmp;
			sel1->runAction(MoveTo::create(0.5, _pos[sel2_x][sel2_y]));
			sel2->runAction(MoveTo::create(0.5, _pos[sel1_x][sel1_y]));
			sel1->unselect();
			sel2->unselect();
			sel1 = sel2 = 0;
			updateCoin(-_serv_swapDog);
		}
	}
}

/*
获取提示服务。
用户点击获取提示服务菜单按钮后，将会调用该函数搜索出一条可行的路径并予以短暂的显示。
该函数的逻辑为：
1.对_dogs[][]二维数组进行遍历
2.如果某一个位置上存在图片，则从该位置开始调用autoDFS()搜索可行路径，
  2.1如果能够搜索到一条可行路径，则显示出该路径(闪烁三次)并更新金币系统，然后结束搜索
  2.2如果从该位置不能找到一条可行路径，则继续从后续图片进行搜索
*/
void MainScene::serviceShowHint() {
	if (UserData::getInstance()->getCoin() < _serv_showHint) {
		auto weave = Sequence::create(TintTo::create(0.1, 250, 0, 0),
			TintTo::create(0.1, 255, 255, 255), NULL);
		menuItem_showHint->runAction(Sequence::create(
			Repeat::create(weave, 3),
			NULL
		));
	}
	else {
		if (sel1) { sel1->unselect(); sel1 = 0; }
		if (sel2) { sel2->unselect(); sel2 = 0; }
		for (int i = 1; i <= _row; ++i) {
			for (int j = 1; j <= _col; ++j) {
				if (_dogs[i][j]) {
					_can = false;
					vector<Vec2> path;
					path.push_back(_pos[i][j]);
					sel1 = _dogs[i][j];
					autoDFS(i, j, ' ', 0, _dogs[i][j]->_id, path);
					if (_can) {
						sel1 = sel2 = 0;
						this->runAction(Sequence::create(
							CallFunc::create(CC_CALLBACK_0(MainScene::drawPath,this,_path)),
							DelayTime::create(0.3),
							CallFuncN::create(CC_CALLBACK_0(DrawNode::clear, drawNode)),
							DelayTime::create(0.3),
							CallFunc::create(CC_CALLBACK_0(MainScene::drawPath, this, _path)),
							DelayTime::create(0.3),
							CallFuncN::create(CC_CALLBACK_0(DrawNode::clear, drawNode)),
							NULL
						));
						updateCoin(-_serv_showHint);
						return;
					}
				}
			}
		}
	}
}


void MainScene::backToStartScene() {
	UserDefault::getInstance()->flush();
	_director->replaceScene(TransitionFadeBL::create(1, StartScene::createScene()));
}