#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class TimeLine : public DrawNode{
public:
	enum State { State_Over, State_Running, State_Pause };
private:
	Vec2 Start, End,Len;
	float MaxTime, CurTime;
	State state;

	TimeLine(int maxTime, Vec2 &start = Vec2(0, 3), Vec2 &end = Vec2(800, 3))
		:MaxTime(maxTime), Start(start), End(end) {
		Len = End - Start;
		state = State_Running;
		CurTime = 0;
	}

public:
	float curTime() { return CurTime; }

	static TimeLine* create(int MaxTime) {
		auto timeLine = new TimeLine(MaxTime);
		if (timeLine && timeLine->init()) {
			timeLine->autorelease();
			return timeLine;
		}
		else {
			delete timeLine;
			timeLine = 0;
			return 0;
		}
	}

	//增加进度
	void Increase(float delta) {
		if (state == State_Running) {
			if (CurTime < MaxTime) {
				CurTime += delta;
			}
			else {
				state = State_Over;
			//	unscheduleAllCallbacks();
			}
		}
	}

	virtual bool init() {
		if (!DrawNode::init()) return false;
//		this->schedule(SEL_SCHEDULE(&TimeLine::Increase), incre);
		return true;
	}

	//画一条从起点到终点的半径为给定参数的直线
	void draw(float radius=2) {
		if (MaxTime == 0) return;
		if (state == State_Running) {
			float p = CurTime / MaxTime;
			Vec2 end(Start.x + p*Len.x, Start.y + p * Len.y);
			drawSegment(Start, end, radius, Color4F(p, 1-p, 0.25,1));
		}
	}

	int getMaxTime() { return MaxTime; }
	void setMaxTime(int maxTime) { this->MaxTime = maxTime; }

	void reset() {
		CurTime = 0;
		state = State_Running;
	}
	State getState() { return state; }
};