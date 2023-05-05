/*
*    开发日志
*    2023/4/27
*  1.加载背景和植物栏
*    2023/4/28
*  2.添加部分卡牌
*  3.实现拖动卡牌
*  4.加载植物动作
*    2023/4/29
*  5.实现动物种植
*    2023/5/2
*  6.实现植物摆动
*    2023/5/5
*  7.布置开始菜单
*  8.实现随机阳光生成
   9.打印获得阳光值
  10.
*/
#include<stdio.h>
#include<graphics.h>
#include<map>
#include<iostream>
#include <utility>
#include<stdlib.h>
#include<string>
#include<time.h>
#include "png.h"
#define WIN_WIDTH 900
#define WIN_HEIGHT 600
#define Card_WIDTH 56
#define Card_HEIGHT 79
#define Soil_WIDTH 73                                                                                                
#define Soil_HEIGHT 105
#define map_top  80
#define map_bottom  575
#define map_left  225
#define map_right  890
#define sunwidth 78
#define sunheight 78
using namespace	std;

enum {td,wd,xrk,plant_count};
IMAGE imgCards[plant_count];
IMAGE Background;
IMAGE Imgbar;
IMAGE* imgPlant[plant_count][20];
IMAGE imgsun[22];
IMAGE imgzm[22];

struct Sunball 
{
	int x, y;
	bool appear;//是否出现在画面
	bool click ;//是否被拾取
	int frameindex;//序列帧
	int EY;//最终停在Y坐标
};
struct Zom
{
	int x, y;
	int frameindex;
	bool used;
	int speed;
	int blood;
};
struct Plant
{
	int blood;
	int price;
	int ATK;
};

Plant plants[plant_count];
Zom zms[10];
Sunball sun[10];
int sunpoint;
int Plant_frame[plant_count];//标记植物有多少帧图片
int now_x, now_y; //植物移动过程中的坐标
int index; //点击的植物种类，从1开始,0表示还未选择
int Soil[5][9];//五行就列，数据表示植物的种类,0表示没选中
map<pair< int, int >,int >mp;//pair指坐标，右边int指第几帧
int max_frame[10];//植物种类最多的帧数

bool FileExist(char* path)//判断是否有这帧图片
{
	FILE* fp = fopen(path, "r");
	//return fp == NULL ? false : true;
	if (fp==NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}
void Initplant()//定义植物体数据
{
	plants[td] = { 10,50,0 };
	plants[wd] = { 4,100,1 };
	plants[xrk] = { 4,50,0 };
}
void Get_Cards()
{
	char path[32];
	for (int i = 0; i < plant_count; i++)
	{
		sprintf_s(path, sizeof(path),"res/Cards/card%d.png", i + 1);//数组保存路径
		loadimage(&imgCards[i],path);
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/plant%d/%d.png", i, j + 1);//数组保存路径
			if (FileExist(path))
			{
				max_frame[i] = j + 1;
				imgPlant[i][j] = new IMAGE;//分配内存
				loadimage(imgPlant[i][j], path);
				Plant_frame[i] = j + 1;
			}       
			else
			{
				break;
			}
		}
	}
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(path, sizeof(path), "res/sunball/%d.png", i + 1);
		loadimage(&imgsun[i], path);
	}
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(path, sizeof(path), "res/zm/%d.png", i + 1);
		loadimage(&imgzm[i], path);
	}
}
void GameInit()
{
	memset(imgPlant, 0, sizeof(imgPlant));
	//加载背景图片
	//字符集修改为多字节
	loadimage(&Background, "res/background.jpg");

    //植物栏
	loadimage(&Imgbar, "res/bar.png");
	//植物卡片
	Get_Cards();
	Initplant();
	//创建窗口
	srand(time(NULL));
	memset(sun, 0, sizeof(sun));
	memset(zms, 0, sizeof(zms));
	initgraph(WIN_WIDTH,WIN_HEIGHT,1);
}
void Cards_put()//植物栏的卡片
{
	for (int i = 0; i < plant_count; i++)
	{
		int x = 325 + Card_WIDTH * i;
		int y = 3;
		putimage(x, y, &imgCards[i]);
	}
}
void userClick()
{
	ExMessage msg;
	static bool msg_status = 0;
	int ballmax = sizeof(sun) / sizeof(sun[0]);
	if (peekmessage(&msg))
	{
		now_x = msg.x;
		now_y = msg.y;
		for (int i = 0; i < ballmax; i++)//点击阳光
			{
				if (msg.x > sun[i].x && msg.x < sun[i].x + sunwidth &&
					msg.y > sun[i].y && msg.y < sun[i].y + sunheight&& msg.message == WM_LBUTTONDOWN)
				{
					sun[i].click = 1;
					sun[i].appear = 0;//回到阳光池，使得阳光反复出现
					sunpoint += 50;
					break;
				}
			}
	    if (msg.message == WM_LBUTTONDOWN && !msg_status)//点击图片
		{
			int fake_index = (msg.x - 325) / Card_WIDTH + 1;//先用fake_index判断钱够不够
			if (msg.x > 325 && msg.x < 325 + plant_count * Card_WIDTH && msg.y>3 && msg.y < 3 + Card_HEIGHT&&sunpoint>=plants[fake_index-1].price)
			{
				msg_status = 1;
				index = (msg.x - 325) / Card_WIDTH + 1;
			}
		}
		else if (msg.message == WM_LBUTTONDOWN && msg_status)//点击土地
		{
			msg_status = 0;
			int row = (msg.y - map_top) / Soil_HEIGHT;
			int column = (msg.x - map_left) / Soil_WIDTH;
			if (msg.x <map_right && msg.x >map_left && msg.y > map_top && msg.y < map_bottom&&!Soil[row][column])
			{
				Soil[row][column] = index ;
				sunpoint -= plants[index - 1].price;
			}
			index = 0;
		}
	}
}
int getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime = GetTickCount();
	if (lastTime == 0)
	{
		lastTime = currentTime;
		return 0;
	}
	else
	{
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}
void createSun()
{
	static int count = 0;//计数，当前第几帧
	int frame = 50;//第一次阳光落下需要多少帧
	count++;
	if (count >= frame)
	{
		count = 0;//计数清零
		frame = 100 + rand() % 300;//使眼光落下随机性
		int ballmax = sizeof(sun) / sizeof(sun[0]);
		int i;
		for (i = 0; i < ballmax && sun[i].appear; i++);
		if (i >= ballmax)//所有太阳都在画面中
		{
			return;
		}
		sun[i].x = map_left + rand() % (map_right - map_left - sunwidth);
		sun[i].y = 0;
		sun[i].appear = 1;
		sun[i].EY = (rand() % 4) * Soil_HEIGHT + 100;
		sun[i].frameindex = 0;
		sun[i].click = 0;
	}

}
void updateSun()
{
	int ballmax = sizeof(sun) / sizeof(sun[0]);
	for (int i = 0; i < ballmax; i++)
	{
		if (sun[i].appear)
		{
			sun[i].frameindex = (sun[i].frameindex + 1) % 22;
			if (sun[i].y < sun[i].EY)
			{
				sun[i].y += 2;
			}
		}
	}
}
void UpdataWindow()//更新动画窗口
{
	BeginBatchDraw();
	putimage(0, 0, &Background);//背景
	putimage(250, 0, &Imgbar);//植物栏
	Cards_put();//植物卡片
	if(index>0&&sunpoint) //选中了植物卡片
	{
		int x = now_x - Card_WIDTH / 2;
	    int y = now_y - Card_HEIGHT / 2;

		if(x>0&&y>0) //PNG_bug
		putimagePNG( x ,y , imgPlant[index - 1][0]);
	}
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (Soil[i][j])
			{  
				int x = 227 + j * Soil_WIDTH;
				int y = 79 + i * Soil_HEIGHT;
              //  putimagePNG( x, y, imgPlant[Soil[i][j]-1][0]);
				int k = mp[make_pair(i, j)] % max_frame[Soil[i][j]-1]; //使不超过最大帧数
				putimagePNG(x, y, imgPlant[Soil[i][j] - 1][k]);
			}
		}
	}
	int ballmax = sizeof(sun) / sizeof(sun[0]);
	for (int i = 0; i < ballmax; i++)
	{
		if (sun[i].appear&&!sun[i].click)//产生后但是未被点击
		{
			IMAGE img = imgsun[sun[i].frameindex];
			putimagePNG(sun[i].x, sun[i].y,&img);
		}
	}
	setbkmode(TRANSPARENT);//数字背景透明，否则黑色背景
	settextcolor(WHITE);//数字线条为白色
	outtextxy(274, 65, to_string(sunpoint).c_str());//数字要先转换成字符串
	EndBatchDraw();
}
void UpdataGame()//更新底层游戏数据
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (Soil[i][j] > 0)
			{
				mp[make_pair(i,j)] ++;
			}
		}
	}
	createSun();
	updateSun();
}
void startUI()
{
	IMAGE Adventure_0, Adventure_1, MainMenu;
	loadimage(&Adventure_0, "res/start/Adventure_0.png");
	loadimage(&Adventure_1, "res/start/Adventure_1.png");
	loadimage(&MainMenu, "res/start/MainMenu.png");
	bool flag = 0;//未选中冒险模式
	while (1)
	{
		BeginBatchDraw();
		putimagePNG(0, 0, &MainMenu);
		putimagePNG(480, 65, flag?&Adventure_0:&Adventure_1);

		ExMessage msg;
		if (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN&&msg.x >480&&msg.x<780&&msg.y>65&&msg.y<191)
			{
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP && flag)
			{
				return;
			}
		}
		EndBatchDraw();
	}
}

int main()
{
	GameInit();
	startUI();
	int timer = 0;
	bool flag = true;
	while (1)
	{
		userClick();
		timer += getDelay();
		if (timer > 45)
		{
			flag = true;
			timer = 0;
		}
		if (flag)
		{
			flag = false;
			UpdataWindow();
			UpdataGame();
		}
	}
	system("pause");
	return 0;
}