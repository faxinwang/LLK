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
	
	//消除生成的初始界面中可能存在的BUG
	//由于提供了交换两个狗狗图片的服务，可以去掉这一步。
	void makeValid();

	void update(float dt);

	//完成点击图片时需要完成的游戏逻辑
	bool onTouchBegin(Touch* touch, Event* event);

	//深度优先搜索，用于在用户点击的两个图片中搜索路径
	void DFS(int x, int y, char dir, int turn, std::vector<cocos2d::Vec2>& path);
	//用于搜索一条提示路径
	void autoDFS(int x, int y, char dir, int turn, int id, std::vector<cocos2d::Vec2>&path);

	//搜索出一条路径后，用该函数绘制出搜索到的路径
	void drawPath(std::vector<cocos2d::Vec2>& path);

	void gameOver(); //游戏失败时调用
	void gameWin(); //游戏胜利是调用
	

	void initTable(int lv); //初始化狗狗界面
	
	//给用户提高三种付费(金币)服务。
	void serviceAddTime(); //增加通关所需时间
	void serviceSwapDog(); //交换两个相邻狗狗的位置
	void serviceShowHint();//显示一个提示

	void backToStartScene(); //回到第一个场景

	void doLink(); //完成连接两张图片过程中的所有动画效果

	//完成获得金币时的动画效果和声音效果。
	void playGainCoinAnimation(cocos2d::Vec2 &pos, int coin);

private:

	//游戏失败时清除所有剩余的图片
	void clearTable(); 

	//每消除两张图片后检查所有图片是否全部消除
	bool checkWin(); 
	
    //更新系统中的金币数量，并是界面上显示的金币数量同步改变，同时播放声音和动画效果。
	void updateCoin(int delta); 

	//播放参数给定的声音效果。
	void playEffect(const char* file) { SimpleAudioEngine::getInstance()->playEffect(file);}

private:
	Dog *_dogs[20][20]; //保存狗狗精灵(继承自精灵)
	Dog *sel1=0, *sel2=0;  //保存选择的图片，最多可以同时选中两张
	cocos2d::Vec2 _pos[20][20]; //图片可以放置的位置, 单位为像素，坐标原点为左下角
	std::vector<cocos2d::Vec2> _path; //保存DFS(),autoDFS()搜索到的路径
	bool _can; //保存DFS(),autoDFS()的搜索结果，ture表示找到了一条可以连接的路径
	bool _swapingDog; //为true时表示正在进行交换两个图片位置的服务
	bool _win; //true表示游戏获胜。
	int _row, _col; //游戏中图片的行数和列数
	int sel1_x, sel1_y; //保存第一张被选中的图片在_pos[][]二维数组中的位置
	int sel2_x, sel2_y; //保存第二张被选中的图片在_pos[][]二维数组中的位置

	DrawNode* drawNode; //使用drawNode绘制连接两张图片的连线
	TimeLine* timeLine; //自定义类，用于绘制时间进度条
	Label* _coins; //在界面上显示当前拥有的金币数量

	//services
	MenuItem *menuItem_addTime;  //购买时间服务按钮
	MenuItem *menuItem_swapDog;  //交换图片位置服务按钮
	MenuItem *menuItem_showHint; //购买提示服务按钮
	//购买相应服务需要的金币数
	int _serv_addTime=100;  
	int _serv_swapDog=150;
	int _serv_showHint=200;

	int _level; //当前的难度级别， 共10个等级
	Levels *_lvObj; //自定义类，保存各级别中的难度信息，难度可以去这里类定义中去修改
};

#endif // __HELLOWORLD_SCENE_H__
