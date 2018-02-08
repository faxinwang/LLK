#pragma once

class Levels{
private:
	int _level; //难度级别
	int _award; //奖励分数
	int _timeLimit; //second
	int _nDog; //狗狗种数
	int _awardPass; //通关奖励
public:
	Levels(int lv) :_level(lv<=10? lv:10) {
		_award = 5 + _level;
		_nDog = (_level - 1) * 3 + 10;
		_timeLimit = (2 + _level ) * 60;
		_awardPass = 20 + _level; //奖励_level+1次
	}

	int getLevel() { return _level; }
	int getAward() { return _award; }
	int getTimeLimit() { return _timeLimit; }
	int getNDog() { return _nDog; }
	int getAwardPass() { return _awardPass; }
};