#pragma once
#include "cocos2d.h"
#include "MainScene.h"
using namespace cocos2d;

class StartScene : public Scene {
public:

	CREATE_FUNC(StartScene);

	static Scene* createScene() {
		return StartScene::create();
	}
	
	virtual bool init() {
		if (!Scene::init()) return false;

		auto origin = _director->getVisibleOrigin();
		auto vsize = _director->getVisibleSize();

		//背景
		auto bgSpr = Sprite::create("res/imgs/MainScene_bg.png");
		this->addChild(bgSpr,-1);
		bgSpr->setAnchorPoint(ccp(0,0));
		bgSpr->setPosition(ccp(origin.x, origin.y));

		//从头开始游戏
		auto fromScratch = Label::create("Start Game From Scratch","Hobo Std",30);
		auto item_fromScratch = MenuItemLabel::create(fromScratch, CC_CALLBACK_0(StartScene::goToMainScene, this, 1));
		item_fromScratch->setPosition(0, 60);

		//从最高记录开始游戏
		auto fromBestRecord = Label::create("Start Game From Best Record", "Hobo Std", 30);
		auto item_fromBest = MenuItemLabel::create(fromBestRecord, CC_CALLBACK_0(StartScene::goToMainScene, this,
			UserDefault::getInstance()->getIntegerForKey("_maxLevel",1)));
		item_fromBest->setPosition(0,0);
		//结束游戏
		auto endGame = Label::create("End  Game","Hobo Std", 30);
		auto item_endGame = MenuItemLabel::create(endGame, CC_CALLBACK_0(StartScene::endGame, this));
		item_endGame->setPosition(0, -60);

		//菜单
		auto menus = Menu::create(item_fromScratch, item_fromBest, item_endGame, NULL);
		menus->setPosition(vsize/2);
		this->addChild(menus);

		return true;
	}

	void goToMainScene(int level) {
		_director->replaceScene(TransitionFade::create(1, MainScene::createScene(level)));
	}

	void endGame() {
		UserDefault::getInstance()->flush();
		_director->end();
	}
};