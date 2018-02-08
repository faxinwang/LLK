#pragma once

class Levels{
private:
	int _level; //�Ѷȼ���
	int _award; //��������
	int _timeLimit; //second
	int _nDog; //��������
	int _awardPass; //ͨ�ؽ���
public:
	Levels(int lv) :_level(lv<=10? lv:10) {
		_award = 5 + _level;
		_nDog = (_level - 1) * 3 + 10;
		_timeLimit = (2 + _level ) * 60;
		_awardPass = 20 + _level; //����_level+1��
	}

	int getLevel() { return _level; }
	int getAward() { return _award; }
	int getTimeLimit() { return _timeLimit; }
	int getNDog() { return _nDog; }
	int getAwardPass() { return _awardPass; }
};