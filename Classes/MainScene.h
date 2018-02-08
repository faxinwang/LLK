#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Dog.h"
#include "TimeLine.h"
#include <vector>
#include "Levels.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int level);

    virtual bool init(int level);
   
    // implement the "static create()" method manually
	static MainScene* create(int level);
	
	//�������ɵĳ�ʼ�����п��ܴ��ڵ�BUG
	//�����ṩ�˽�����������ͼƬ�ķ��񣬿���ȥ����һ����
	void makeValid();

	void update(float dt);

	//��ɵ��ͼƬʱ��Ҫ��ɵ���Ϸ�߼�
	bool onTouchBegin(Touch* touch, Event* event);

	//��������������������û����������ͼƬ������·��
	void DFS(int x, int y, char dir, int turn, std::vector<cocos2d::Vec2>& path);
	//��������һ����ʾ·��
	void autoDFS(int x, int y, char dir, int turn, int id, std::vector<cocos2d::Vec2>&path);

	//������һ��·�����øú������Ƴ���������·��
	void drawPath(std::vector<cocos2d::Vec2>& path);

	void gameOver(); //��Ϸʧ��ʱ����
	void gameWin(); //��Ϸʤ���ǵ���
	

	void initTable(int lv); //��ʼ����������
	
	//���û�������ָ���(���)����
	void serviceAddTime(); //����ͨ������ʱ��
	void serviceSwapDog(); //�����������ڹ�����λ��
	void serviceShowHint();//��ʾһ����ʾ

	void backToStartScene(); //�ص���һ������

	void doLink(); //�����������ͼƬ�����е����ж���Ч��

	//��ɻ�ý��ʱ�Ķ���Ч��������Ч����
	void playGainCoinAnimation(cocos2d::Vec2 &pos, int coin);

private:

	//��Ϸʧ��ʱ�������ʣ���ͼƬ
	void clearTable(); 

	//ÿ��������ͼƬ��������ͼƬ�Ƿ�ȫ������
	bool checkWin(); 
	
    //����ϵͳ�еĽ�����������ǽ�������ʾ�Ľ������ͬ���ı䣬ͬʱ���������Ͷ���Ч����
	void updateCoin(int delta); 

	//���Ų�������������Ч����
	void playEffect(const char* file) { SimpleAudioEngine::getInstance()->playEffect(file);}

private:
	Dog *_dogs[20][20]; //���湷������(�̳��Ծ���)
	Dog *sel1=0, *sel2=0;  //����ѡ���ͼƬ��������ͬʱѡ������
	cocos2d::Vec2 _pos[20][20]; //ͼƬ���Է��õ�λ��, ��λΪ���أ�����ԭ��Ϊ���½�
	std::vector<cocos2d::Vec2> _path; //����DFS(),autoDFS()��������·��
	bool _can; //����DFS(),autoDFS()�����������ture��ʾ�ҵ���һ���������ӵ�·��
	bool _swapingDog; //Ϊtrueʱ��ʾ���ڽ��н�������ͼƬλ�õķ���
	bool _win; //true��ʾ��Ϸ��ʤ��
	int _row, _col; //��Ϸ��ͼƬ������������
	int sel1_x, sel1_y; //�����һ�ű�ѡ�е�ͼƬ��_pos[][]��ά�����е�λ��
	int sel2_x, sel2_y; //����ڶ��ű�ѡ�е�ͼƬ��_pos[][]��ά�����е�λ��

	DrawNode* drawNode; //ʹ��drawNode������������ͼƬ������
	TimeLine* timeLine; //�Զ����࣬���ڻ���ʱ�������
	Label* _coins; //�ڽ�������ʾ��ǰӵ�еĽ������

	//services
	MenuItem *menuItem_addTime;  //����ʱ�����ť
	MenuItem *menuItem_swapDog;  //����ͼƬλ�÷���ť
	MenuItem *menuItem_showHint; //������ʾ����ť
	//������Ӧ������Ҫ�Ľ����
	int _serv_addTime=100;  
	int _serv_swapDog=150;
	int _serv_showHint=200;

	int _level; //��ǰ���Ѷȼ��� ��10���ȼ�
	Levels *_lvObj; //�Զ����࣬����������е��Ѷ���Ϣ���Ѷȿ���ȥ�����ඨ����ȥ�޸�
};

#endif // __HELLOWORLD_SCENE_H__
