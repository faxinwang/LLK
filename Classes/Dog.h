#ifndef _Dog_H_
#define _Dog_H_

#include "cocos2d.h"
using namespace cocos2d;
//#include "SimpleAudioEngine.h"
//using namespace CocosDenshion;
#include <string>
using namespace std;


class Dog : public Sprite {
public:
	int _id; //����ù�����ID�ţ���ͼƬ�ļ������Ӧ
	bool selected; //����ͼƬѡ��״̬
public:
	static Dog* create(int id) {
		auto dog = new Dog();
		if (dog && dog->init(id)) {
			dog->autorelease();
			return dog;
		}
		else {
			delete dog;
			dog = 0;
			return 0;
		}
	}

	//����ID��ʹ����Ӧ��ͼƬ���г�ʼ��
	virtual bool init(int id) {
		_id = id;
		auto name = String::createWithFormat("Dog_%d.png", id);
		if (!Sprite::initWithSpriteFrameName(string(name->getCString()))) return false;
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(Dog::onTouchBegan, this);
		_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
		return true;
	}

	//���һ�α�Ϊѡ��״̬���ٵ�һ��ȡ��ѡ��
	bool onTouchBegan(Touch* touch, Event* event) {
		if (this->getBoundingBox().containsPoint(touch->getLocation())) {
			if (selected) {
				unselect();
			}
			else {
				select();
			}
		//	return true;
		}
	//	SimpleAudioEngine::getInstance()->playEffect("res/sounds/Click.wav");
		return false;
	}

	//��Ϊѡ��״̬
	void select() {
		this->setScale(1.2);
		this->selected = true;
		this->runAction(TintTo::create(0.5,255, 0, 255));
	}
	//��Ϊδѡ��״̬
	void unselect() {
		this->setScale(1);
		this->selected = false;
		this->runAction(TintTo::create(0.5, 255, 255, 255));
	}

};


#endif // !_Dog_H_
