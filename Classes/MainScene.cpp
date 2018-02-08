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

	//����
	auto bgLayer = LayerColor::create(Color4B(160, 170, 180, 255), visibleSize.width, visibleSize.height);
	this->addChild(bgLayer, -2);

	//����dogͼƬ
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/imgs/Dogs.plist");

	
	_row = 12;
	_col = 14;
	_level = level;
	_lvObj = 0;
	//��ʼ�������1˵��ѡ����Ǵ������ʷ��¼��ʼ��Ϸ,���Խ��Ҫ����Ϊ��¼�еĽ��
	if (_level > 1) {
		UserData::getInstance()->setCoin(UserData::getInstance()->getMaxLevelCoin());
	}
	else {
		UserData::getInstance()->setCoin(0);
	}

	//��ʼ������
	initTable(_level);
	
	//��ʾLevel
	auto lab_lv = Label::create("", "Hobo Std", 20);
	lab_lv->setString(String::createWithFormat("Level:%d", level)->getCString());
	lab_lv->setPosition(lab_lv->getContentSize().width / 2+10, visibleSize.height - 10);
	this->addChild(lab_lv, 1, "lab_lv");

	//���ͼƬ
	auto coinSpr = Sprite::create("res/imgs/coin2.png");
	coinSpr->setScale(0.4);
	coinSpr->setAnchorPoint(ccp(0.5, 0.5));
	coinSpr->setPosition(visibleSize.width - 30, visibleSize.height - 30);
	this->addChild(coinSpr, 0, "coinSpr");
	//�������
	_coins = Label::create("", "Arial Black", 35);
	//�����ͷ��ʼ, �����������Ϊ0���������߼�¼��ʼ,���������Ϊ��ʷ��¼
	_coins->setString(String::createWithFormat("%d", 
		level==1 ? 0 : UserData::getInstance()->getCoin())->getCString());
	_coins->setAnchorPoint(ccp(1, 1));
	_coins->setPosition(visibleSize.width - coinSpr->getContentSize().width + 50, visibleSize.height - 3);
	_coins->setTextColor(Color4B(250,218,31,255));
	this->addChild(_coins);
	
	//����ť
	auto services = Label::create("Service------", "Hobo Std", 25);
	services->setAnchorPoint(ccp(1, 1));
	services->setPosition(visibleSize.width,
		coinSpr->getPositionY() - 40);
	this->addChild(services);

	//����-���ʱ��
	auto str = String::createWithFormat("AddTime \n(%d$=10S) ", _serv_addTime)->getCString();
	auto serv_addTime = Label::create(str, "Hobo Std",22);
	menuItem_addTime = MenuItemLabel::create(serv_addTime,
		CC_CALLBACK_0(MainScene::serviceAddTime, this));
	menuItem_addTime->setAnchorPoint(ccp(1, 1));
	menuItem_addTime->setPosition(0, -20);
	
	//����-����λ��
	str = String::createWithFormat("SwapDog  \n(%d$) ",_serv_swapDog)->getCString();
	auto serv_swapDog = Label::create(str, "Hobo Std", 22);
	menuItem_swapDog = MenuItemLabel::create(serv_swapDog, 
		CC_CALLBACK_0(MainScene::serviceSwapDog, this));
	menuItem_swapDog->setAnchorPoint(ccp(1, 1));
	menuItem_swapDog->setPosition(-10,-100);

	//����-��ȡ��ʾ
	str = String::createWithFormat("ShowHint \n(%d$) ",_serv_showHint)->getCString();
	auto serv_showHint = Label::create(str, "Hobo Std", 22);
	menuItem_showHint = MenuItemLabel::create(serv_showHint,
		CC_CALLBACK_0(MainScene::serviceShowHint, this));
	menuItem_showHint->setPosition(ccp(1, 1));
	menuItem_showHint->setPosition(-60, -210);

	//���ؿ�ʼ����
	auto back = Label::create(" back ", "Hobo Std", 22);
	auto menuItem_back = MenuItemLabel::create(back, 
		CC_CALLBACK_0(MainScene::backToStartScene, this));
	menuItem_back->setAnchorPoint(ccp(1, 1));
	menuItem_back->setPosition( -10, 90 - services->getPositionY());

	//�˵�
	auto menus = Menu::create(menuItem_addTime,menuItem_swapDog,menuItem_showHint,
		menuItem_back, NULL);
	menus->setAnchorPoint(ccp(1, 1));
	menus->setPosition(visibleSize.width, services->getPositionY() - 20);
	this->addChild(menus);

	//��ʼ��ʱ����
	timeLine = TimeLine::create(_lvObj->getTimeLimit());
	this->addChild(timeLine);

	//����¼�������
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegin, this);
	_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	

	//drawNode ������ͼƬ֮���·��
	drawNode = DrawNode::create();
	this->addChild(drawNode);

//	makeValid(); //�����ṩ�˽�����������ͼƬ�ķ��񣬿���ȥ����һ����

	scheduleUpdate();

	SimpleAudioEngine::getInstance()->playBackgroundMusic("res/sounds/BGM.mid",true);

    return true;
}

void MainScene::initTable(int level){
	_win = false;
	if (_lvObj) delete _lvObj;
	_lvObj = new Levels(level);

	//ʱ����
	if (timeLine) {
		timeLine->removeFromParent();
		timeLine = TimeLine::create(_lvObj->getTimeLimit());
		this->addChild(timeLine);
	}
	//Level
	auto lab_lv = (Label*)this->getChildByName("lab_lv");
	if (lab_lv) lab_lv->setString(String::createWithFormat("Level:%d", level)->getCString());

	//1.������ͬ��ͼƬ�������ż���β����������е�ͼƬ������ÿѡ��һ��ͼƬ���������Ρ�
	//2.N���ܹ���Ҫ��ӵĹ���ͼƬ������
	//3.ndog�ǹ���ͼƬ������������Խ�࣬�Ѷ�Խ��
	//4.��Ҫ��֤������Χ�ڵ����й���ͼƬ����ѡ�У������Ƚ�������Χ�ڵ�ͼƬ�����һ�Σ����ţ���vector��
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
	//��ϴ�� ����˳��
	for (int i = 0, n = v.size(); i < n; ++i) swap(v[i], v[random() % n]);

	//����ͼƬ����λ�ã��ܱ�����һȦ��λ����·��������·������
	for (int i = 0; i <= _row + 1; ++i) {
		x = 0;
		for (int j = 0; j <= _col + 1; ++j) {
			x += 42;
			_pos[i][j].x = x;
			_pos[i][j].y = y;
			//����λ
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
�����ʣ��4��ͼƬ�Ҿ�������ʱ���ʹ���bug
 1 0
 0 1
 ��������¾�û�취��������ʣ�µ�����ͼƬ
 �ڻ�û����������ͼƬλ�õķ���ʱ�������������������
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


//���ϸ��»���ʱ�����������������������ͼƬδ�������ʱ����Ϸ������
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
1.����Ϸ���ڽ���ͼƬλ��״̬ʱ
	1.1���ѡ�е�����ͼƬ�����ڣ���ȡ��ѡ���һ��ѡ�е�ͼƬ������ǰѡ���ͼƬ��Ϊ��һ��ѡ�е�ͼƬ
	1.2���ѡ�е�����ͼƬ���ڣ�����ѡ�е����ڵ�����ͼƬ
2.����Ϸ��������״̬ʱ
	2.1���ѡ��ĵڶ���ͼƬ���һ��ͼƬ��ͬ��ͨ��ID�жϣ�����ȡ��ѡ��ĵ�һ��ͼƬ�� 
	   ����ǰѡ���ͼƬ��Ϊѡ�еĵ�һ��ͼƬ
	2.2���ѡ�е�����ͼƬ��ͬ������DFS()����·��
		2.2.1�������ͼƬ֮�����һ�����е�·�ߣ�����Ƴ��ҵ���·�ߣ�Ȼ��������������ͼƬ��
		2.2.2�������ͼƬ֮�䲻���ڿ���·�ߣ���������ͼƬ��Ϊδѡ��״̬��

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
��ѡ����������ͬ��ͼƬ�󣬾͵��øú������н������Ĺ�����
1.������DFS()������������·��
2.������ڿ���·�������������һЩ�й�����
	2.1���Ź��е���Ч
	2.2�����ҵ���·��
	2.3Ϊ���ܹ��������Ƶ�·�ߣ�����ͣ��һ����ʱ�䣬����ͣ����0.5��
	2.4������ͼƬ�Ƴ�
	2.5���Ż�ý�ҵĶ�������Ч���Σ��м����0.3��
	2.6����Ƿ��Ƴ������е�ͼƬ������ǣ�����Ϸʤ����

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
���Ż�ý�ҵĶ�������Ч
1.�����þ�����ʾһ�Ž�ҵ�ͼƬ�������õ����ʵĴ�С
2.����ҵ�λ�������ڲ���������λ�ô����Ǳ�������ͼƬ��λ�ã�
3.�����������£�
	3.1����������ϡ�������һС�ξ���
	3.2����ƶ�����Ļ���ϽǴ�������λ��
	3.3�����ͼƬ(����)����Ϸ���Ƴ��������ռ��Խ��Խ����ڴ棩
	3.4���½�ң�����ϵͳ�еĽ�����ͽ�������ʾ�Ľ������
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
������Ϸ�ĺ��ľ��������ˣ���������������Ĳ��ѣ�ֻ����������ʱ����Ҫ���Ǽ������ء�
�Ƚ��ܲ�����
 x,y��ʾ��ǰ��_dogs[][]��ά�����е�λ�á�
 dir��ʾ��һ���ƶ��ķ���'U','D','L','R' �ֱ��ʾUp,Down,Left,Right
 turn��ʾת����������ܳ���3��
 path�����˴���㵽��ǰλ�õ�����λ�á�

 ����һ���ݹ麯����˼������ÿһ��λ�ö���������չ��������Ҫע�����¼��㣺
 1.���ܺ��ˣ����ͨ��dir�������ƣ����統��һ����'U'ʱ����ʾ�������ߵģ�
   ��ô��������һ���Ͳ��������ߣ������������
 2.����Խ�硣���ߵķ�ΧΪ0 <= x <= _row+1, 0 <= y <= _col+1
 3.���ܴ���ͼƬ��ֻ�ܴӿհ׵ط���(��·���������յ��λ����ȴ����ͼƬ��)��
 4.�����һ���ߵķ�������һ���ߵķ���ͬ����ת�������һ
 5.�����ǵݹ麯�����϶�����ջ�����(˵��·��������)������ջ��ʱ��ҲҪ��path��ȥ����Ӧ��λ��
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
�ú����ṩ��ʾһ������·���ķ���
���߼���DFS()��������
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

//ʹ��drawNode������������·��
void MainScene::drawPath(vector<Vec2>& path) {
	for (int i = 0, n = path.size()-1; i < n; ++i) {
		drawNode->drawSegment(path[i], path[i + 1], 1, Color4F::YELLOW);
	//	log("path[%d]: x=%.0f, y=%.0f", i, path[i].x, path[i].y);
	}
}

//����Ϸ����ʱ�����������ʣ���ͼƬ�����ж���Ч����
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

//����Ƿ���Ϸ��ʤ��������ͼƬ������ʱ��Ϸ��ʤ
bool MainScene::checkWin() {
	for (int i = 1; i <= _row; ++i) {
		for (int j = 1; j <= _col; ++j) {
			if (_dogs[i][j]) return false;
		}
	}
	return true;
}

/*
�ú��������Ϸ��ʤ����Ҫ�����һЩ�����飬������
1.�����Ѷȼ���
2.������Ϸ��ʤ����Ч(����һֱ��û�в��ţ���֪����ʲôԭ��)
3.����һ�������Ľ�ң������λ�ò��Ż�ý�ҵĶ��������Ż�ý�ҵ���Ч���Ѷȼ���Խ�߻�õĽ��������Խ�ࣩ
4.���³�ʼ����Ϸ����
5.�浵��Ϸ����
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
��Ϸ����
�浵����
���ʣ���ͼƬ
���ص���ʼ����
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
1.���½��ϵͳ�м�¼�Ľ������
2.ͬ�����½�������ʾ�Ľ����
3.���Ż�ý�ҵ���Ч
4.�����ϽǵĴ�����תһ���ĽǶ�
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
���ʱ�����100��ҹ���10���ӵ�ʱ�䡣
��Ҫ��ɵ����
1.�������㹻�����½��ϵͳ������timeLine����10����
2.�����Ҳ��㣬�򽫽�Ҳ˵���ť�����족��������ʾ��Ҳ���
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
		timeLine->setMaxTime(timeLine->getMaxTime() + 10); //��10��
		timeLine->clear();
	}
}

/*
����ͼƬλ�õķ���
���Ѷȼ������ߺ�ͼƬ�����������࣬��ʱ������ܹ�����ĳ����ͼƬ��λ�ã������Ǻܴ�İ��������÷�����
һ�����ƣ�����ֻ�ܽ�����������ͼƬ��λ�á�

��ɸ÷���Ĺ���Ϊ��
1.���û�����˸÷���ť������û��Ľ�������㹻������뽻��ͼƬ״̬
2.Ȼ�����û����Ҫ����λ�õ�ͼƬ
3.�������ͼƬ��λ�����ڣ��򽻻�����ͼƬ��λ�ã��������û������

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
��ȡ��ʾ����
�û������ȡ��ʾ����˵���ť�󣬽�����øú���������һ�����е�·�������Զ��ݵ���ʾ��
�ú������߼�Ϊ��
1.��_dogs[][]��ά������б���
2.���ĳһ��λ���ϴ���ͼƬ����Ӹ�λ�ÿ�ʼ����autoDFS()��������·����
  2.1����ܹ�������һ������·��������ʾ����·��(��˸����)�����½��ϵͳ��Ȼ���������
  2.2����Ӹ�λ�ò����ҵ�һ������·����������Ӻ���ͼƬ��������
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