/*
*    ������־
*    2023/4/27
*  1.���ر�����ֲ����
*    2023/4/28
*  2.��Ӳ��ֿ���
*  3.ʵ���϶�����
*  4.����ֲ�ﶯ��
*    2023/4/29
*  5.ʵ�ֶ�����ֲ
*    2023/5/2
*  6.ʵ��ֲ��ڶ�
*  

*/
#include<stdio.h>
#include<graphics.h>
#include<map>
#include<iostream>
#include <utility>
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

using namespace	std;

enum {wd,xrk,td,plant_count};
IMAGE imgCards[plant_count];
IMAGE Background;
IMAGE Imgbar;
IMAGE* imgPlant[plant_count][20];

int Plant_frame[plant_count];//���ֲ���ж���֡ͼƬ
int now_x, now_y; //ֲ���ƶ������е�����
int index; //�����ֲ�����࣬��1��ʼ,0��ʾ��δѡ��
int Soil[5][9];//���о��У����ݱ�ʾֲ�������,0��ʾûѡ��
map<pair< int, int >,int >mp;//pairָ���꣬�ұ�intָ�ڼ�֡
int max_frame[10];//ֲ����������֡��
bool FileExist(char* path)//�ж��Ƿ�����֡ͼƬ
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
void Get_Cards()
{
	char path[32];
	for (int i = 0; i < plant_count; i++)
	{
		sprintf_s(path, sizeof(path),"res/Cards/card%d.png", i + 1);//���鱣��·��
		loadimage(&imgCards[i],path);
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/plant%d/%d.png", i, j + 1);//���鱣��·��
			if (FileExist(path))
			{
				max_frame[i] = j + 1;
				imgPlant[i][j] = new IMAGE;//�����ڴ�
				loadimage(imgPlant[i][j], path);
				Plant_frame[i] = j + 1;
			}       
			else
			{
				break;
			}
		}
	}
}
void GameInit()
{
	memset(imgPlant, 0, sizeof(imgPlant));
	//���ر���ͼƬ
	//�ַ����޸�Ϊ���ֽ�
	loadimage(&Background, "res/background.jpg");
    //ֲ����
	loadimage(&Imgbar, "res/bar.png");
	//ֲ�￨Ƭ
	Get_Cards();
	//��������
	initgraph(WIN_WIDTH,WIN_HEIGHT,1);
}
void Cards_put()
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
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN && !msg_status)//���ͼƬ
		{
			msg_status = 1;
			if (msg.x > 325 && msg.x < 325 + plant_count * Card_WIDTH && msg.y>3 && msg.y < 3 + Card_HEIGHT)
			{
				index = (msg.x - 325) / Card_WIDTH + 1;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && msg_status == 1)
		{
			now_x = msg.x;
			now_y = msg.y;
		}
		else if (msg.message == WM_LBUTTONDOWN && msg_status)//�������
		{
			msg_status = 0;

			if (msg.x <map_right && msg.x >map_left && msg.y > map_top && msg.y < map_bottom)
			{
				int row = (msg.y - map_top) / Soil_HEIGHT;
				int column = (msg.x - map_left) / Soil_WIDTH;
				Soil[row][column] = index ;
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
void UpdataWindow()
{
	BeginBatchDraw();
	putimage(0, 0, &Background);//����
	putimage(250, 0, &Imgbar);//ֲ����
	Cards_put();//ֲ�￨Ƭ
	if(index>0) //ѡ����ֲ�￨Ƭ
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
				int k = mp[make_pair(i, j)] % max_frame[Soil[i][j]-1]; //ʹ���������֡��
				putimagePNG(x, y, imgPlant[Soil[i][j] - 1][k]);
			}
		}
	}
	EndBatchDraw();
}
void UpdataGame()
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
}
int main()
{
	GameInit();
	int timer = 0;
	bool flag = true;
	while (1)
	{
		userClick();;
		timer += getDelay();
		if (timer > 50)
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