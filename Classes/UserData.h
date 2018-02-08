#pragma once

#include "cocos2d.h"
using namespace cocos2d;

/*
使用单例模式，管理用户金币数量，历史最高等级以及达到历史最高等级时的金币数
*/

class UserData {
private:
	int _coin; //当前金币数
	int _maxLevel; //历史最高等级
	int _maxLevelCoin; //历史最高等级对应的金币数
	static UserData* instance; 
	UserData(){
		_coin = 0;
		_maxLevel = UserDefault::getInstance()->getIntegerForKey("_maxLevel",1);
		_maxLevelCoin = UserDefault::getInstance()->getIntegerForKey("_maxLevelCoin", 0);
	}
	UserData(const UserData& b) {}
public:
	void addCoin(int amount) { 
		_coin += amount; 
	}

	int getCoin() { return _coin; }
	void setCoin(int coin) { _coin = coin; }
	int getMaxLevel() { return _maxLevel; }
	void setMaxLevel(int level) { _maxLevel = level; }
	int getMaxLevelCoin() { return _maxLevelCoin; }
	void setMaxLevelCoin(int coin) { _maxLevelCoin = coin; }

	static UserData* getInstance() {
		if (instance) return instance;
		return instance = new UserData();
	}
	void saveData() {
		UserDefault::getInstance()->setIntegerForKey("_maxLevelCoin", instance->_maxLevelCoin);
		UserDefault::getInstance()->setIntegerForKey("_maxLevel", instance->_maxLevel);
		UserDefault::getInstance()->flush();
	}
	static void destroy() { 
		if (instance) {
			instance->saveData();
			delete instance;
			instance = 0;
		}
	}
	~UserData() { destroy(); }
};

UserData* UserData::instance = 0;