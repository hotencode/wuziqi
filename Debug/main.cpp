#include<graphics.h>  //easyX插件装好的图形库
#include<stdio.h>   //c语言标准文件
#include<conio.h>  //_kbhit()
#include<mmsystem.h>  //音频库                 
#include<stdlib.h>  
#include<Windows.h>
#pragma comment(lib,"winmm.lib")               //静态图库
#define N 15   //棋盘落子定位数组的大小
int chess[N][N] = { 0 };
int playgrade[N][N] = { 0 };
int count = 0,flag1=1;
POINT p;
MOUSEMSG m;
//***********************************************画棋盘**********************************************************//
void draw()
{
	setlinecolor(BLACK);                        //用黑色线画棋盘线，每两条线之间的距离是25
	for (int i = 400; i <= 750; i = i + 25)     //画棋盘横线
		line(i, 50, i, 400);                   
	for (int j = 50; j <= 400; j = j + 25)      //画棋盘竖线
		line(400, j, 750, j);
	setlinestyle(PS_SOLID, 4);                  //线型为实心线，线宽为4
	line(400, 50, 400, 400);                    //画棋盘边缘的四条线，棋盘的长为350
	line(400, 400, 750, 400);
	line(400, 50, 750, 50);
	line(750, 50, 750, 400);
	fillcircle(475, 125, 2);                    //画棋盘中的五个黑色标记点
	fillcircle(675, 125, 2);
	fillcircle(475, 325, 2);
	fillcircle(675, 325, 2);                     
	fillcircle(575, 225, 2);
}
//*****************************************************游戏再开始时初始化数据**************************************************//
void initialize()        
{
	count = 0;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			playgrade[i][j] = 0;
			chess[i][j] = 0;
		}
	}
	flag1 = 1;
}
//************************************************计算机的下棋位置判断**********************************************************//
int getLine(POINT p, int i, int j) { // i:方向 j:相对p的顺序值（以p为0） p:当前点
	int x = p.x, y = p.y;           //根据evaluate中的for循环一次考虑i不同值时的八种情况，达到遍历棋盘棋子状况
	switch (i) {//右
	case 1:
		x = x + j;
		break;
	case 2:
		x = x + j;
		y = y + j;
		break;
	case 3:
		y = y + j;
		break;
	case 4:
		x = x - j;
		y = y + j;
		break;
	case 5:
		x = x - j;
		break;
	case 6:
		x = x - j;
		y = y - j;
		break;
	case 7:
		y = y - j;
		break;
	case 8:
		x = x + j;
		y = y - j;
	}
	if (x < 0 || y < 0 || x > 14 || y > 14) { // 越界处理
		return -1;
	}
	return chess[y][x];
}
//***********************************************人机对弈游戏算法*****************************************************//
int evaluate(POINT p, int me, int plyer) { // me:我的代号  plyer:当前计算的player的代号
	int value = 0;
	int numoftwo = 0;
	for (int i = 1; i <= 8; i++) { // 8个方向
		// 活四 01111* *代表当前空位置  0代表其他空位置    下同 
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, -3) == plyer && getLine(p, i, -4) == plyer
			&& getLine(p, i, -5) == 0) {
			value += 300000;
			if (me != plyer){ value -= 500; }//如果此时的位置不是白棋占优势的位置，则权值减小
			continue;
		}
		// 死四A 21111*
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, -3) == plyer && getLine(p, i, -4) == plyer
			&& (getLine(p, i, -5) == 3 - plyer || getLine(p, i, -5) == -1)) {
			value += 250000;
			if (me != plyer){ value -= 500; }
			continue;
		}
		// 死四B 111*1
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, -3) == plyer && getLine(p, i, 1) == plyer) {
			value += 240000;
			if (me != plyer){ value -= 500; }
			continue;
		}
		// 死四C 11*11
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, 1) == plyer && getLine(p, i, 2) == plyer) {
			value += 230000;
			if (me != plyer){ value -= 500; }
			continue;
		}
		// 活三 近3位置 111*0
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, -3) == plyer) {
			if (getLine(p, i, 1) == 0) {
				value += 750;
				if (getLine(p, i, -4) == 0) {
					value += 3150;
					if (me != plyer){ value -= 300; }
				}
			}
			if ((getLine(p, i, 1) == 3 - plyer || getLine(p, i, 1) == -1) && getLine(p, i, -4) == 0) {
				value += 500;
			}
			continue;
		}
		// 活三 远3位置 1110*
		if (getLine(p, i, -1) == 0 && getLine(p, i, -2) == plyer
			&& getLine(p, i, -3) == plyer && getLine(p, i, -4) == plyer) {
			value += 350;
			continue;
		}
		// 死三 11*1
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, 1) == plyer) {
			value += 600;
			if (getLine(p, i, -3) == 0 && getLine(p, i, 2) == 0) {
				value += 3150;
				continue;
			}
			if ((getLine(p, i, -3) == 3 - plyer || getLine(p, i, -3) == -1) && (getLine(p, i, 2) == 3 - plyer || getLine(p, i, 2) == -1)) {
				continue;
			}
			else {
				value += 700;
				continue;
			}
		}
		//活二的个数   
		if (getLine(p, i, -1) == plyer && getLine(p, i, -2) == plyer
			&& getLine(p, i, -3) != 3 - plyer&&getLine(p, i, 1) != 3 - plyer) {
			numoftwo++;
		}
		//其余散棋
		int numOfplyer = 0; // 因为方向会算两次？
		for (int k = -4; k <= 0; k++) { // ++++* +++*+ ++*++ +*+++ *++++
			int temp = 0;
			for (int l = 0; l <= 4; l++) {
				if (getLine(p, i, k + l) == plyer) {
					temp++;
				}
				else
				if (getLine(p, i, k + l) == 3 - plyer
					|| getLine(p, i, k + l) == -1) {
					temp = 0;
					break;
				}
			}
			numOfplyer += temp;
		}
		value += numOfplyer * 15;
		if (numOfplyer != 0) {
		}
	}
	if (numoftwo >= 2){
		value += 3000;
		if (me != plyer){
			value -= 100;
		}
	}
	return value;
}
//**********************************************************电脑防守进攻判断**********************************************************//
int Evaluate(POINT p, int code)
{
	return evaluate(p, code, 1)
		+ evaluate(p, code, 2);
}
//*****************************************************游戏开始前的界面设置***************************************************************//
void GameBegin()
{
	IMAGE backgroundImage;//存储照片
	loadimage(&backgroundImage, "background.jpg");
	//获取照片的 高和宽
	int width = backgroundImage.getwidth();
	int height = backgroundImage.getheight();
	initgraph(width, height);
	putimage(0, 0, &backgroundImage);
	mciSendString("open 1.mp3 alias music", 0, 0, 0);     //播放音乐的文件
	mciSendString("play music repeat",0,0,0);
	//设置字体格式
	settextstyle(30, 0, "华文楷体");
	setbkmode(TRANSPARENT);
	while (!_kbhit())             //如果没有按下键
	{
		settextcolor(RED);
		outtextxy(270, 250, "鼠标左键操作");                
		outtextxy(270, 290, "按任意键开始");         //闪烁字体
		Sleep(500);
		settextcolor(BLUE);
		outtextxy(270, 250, "鼠标左键操作");
		outtextxy(270, 290, "按任意键开始");
		Sleep(500);
		settextcolor(RGB(0, 255, 0));
		outtextxy(270, 250, "鼠标左键操作");
		outtextxy(270, 290, "按任意键开始");
		Sleep(500);
	}
}
//*******************************************************画提示框确定黑白棋*********************************************************//
void messagebox()
{
	//char s[100];
	setlinecolor(BLACK);
	setfillcolor(RGB(255, 201, 14)); //调整背景颜色，用三原色混色设置
	fillrectangle(170, 50, 370, 180);
	settextstyle(50, 20, "华文楷体");//设置字体格式
	settextcolor(BLACK);
	setfillcolor(BLACK);
	fillcircle(340, 90, 15); //画填充的黑色实心圆表示黑棋
	setbkmode(TRANSPARENT);
	outtextxy(200, 60, "黑棋");
	settextcolor(WHITE);
	setfillcolor(WHITE);//画填充的白色实心圆表示白棋
	setlinecolor(WHITE);
	fillcircle(340,140,15);
	setbkmode(TRANSPARENT);
	outtextxy(200, 110, "白棋");
	setlinecolor(BLACK);
	setfillcolor(RGB(255, 201, 14));
	fillrectangle(170, 220, 370, 350);
	setbkmode(TRANSPARENT);
	settextstyle(40, 0, "华文楷体");
	settextcolor(BLACK);
	
}
//*******************************************************获得鼠标信息*********************************************************//
void getmousemsg()
{
	m = GetMouseMsg();
}
//***************************************胜负检查，即判断当前走子位置有没有造成五连珠的情况*************************************//
int check(int k, int l, int flag)
{
	int  i, w = 1, x = 1, y = 1, z = 1;//当前位置已有一颗棋子，估计数的初值为1
	for (i = 1; i < 5; i++)if (chess[k + i][l] == flag &&k + i < N)w++; else break;
	for (i = 1; i < 5; i++)if (chess[k - i][l] == flag&&k - i >=0)w++; else break;//达到五子连珠，则返回当前玩家的代号
	if  (w >= 5) return flag;               //竖行是否有连珠状况
	for (i = 1; i < 5; i++)if (chess[k][l + i] == flag &&l + i < N)x++; else break;
	for (i = 1; i < 5; i++)if (chess[k][l - i] == flag&&l - i >= 0)x++; else break;
	if  (x >= 5) return flag;               //横行是否有连珠状况
	for (i = 1; i < 5; i++)if (chess[k + i][l + i] == flag &&l + i < N&&k + i < N)y++; else break;
	for (i = 1; i < 5; i++)if (chess[k - i][l - i] == flag&&l - i >= 0 &&k- i >= 0)y++; else break;
	if  (y>= 5) return flag;                 //斜行是否有连珠状况
	for (i = 1; i < 5; i++)if (chess[k - i][l + i] == flag &&l + i < N&&k - i >= 0)z++; else break;
	for (i = 1; i < 5; i++)if (chess[k + i][l - i] == flag&&l - i >= 0 && k + i < N)z++; else break;
	if  (z >= 5) return flag;                //斜行是否有连珠状况
	else//如果都没有达到五子连珠的情况，返回0
		return 0;
}
///*******************************************************双人对弈游戏并显示输赢数据******************************************//
void pvp()
{
	char s[100];
	while (flag1)      //游戏一直循环
		{
			int x, y, k, l;
			MOUSEMSG m;       //获取鼠标信息
			int now = count % 2;//确定回合，如果是0，则下黑子，如果是1，则下白子
			m = GetMouseMsg();
			switch (now)
			{
			case 0:
				if (m.uMsg == WM_LBUTTONDOWN)    //如果按下鼠标左键 
				{
					if (m.x >= 400 && m.x <= 750 && m.y >= 50 && m.y <= 400)   //判断是否在棋盘内
					{
						x = m.x % 25;        //不在棋盘格子上，则取余，取最近的整数点
						if (x <= 12.5)
							m.x = m.x - x;
						else
							m.x = m.x - x + 25;
						y = m.y % 25;
						if (y <= 12.5)
							m.y = m.y - y;
						else
							m.y = m.y - y + 25;
						k = (m.x / 25) - 16;   //计算出对应数组下标
						l = (m.y / 25) - 2;
						if (chess[l][k] != 0)//如果当前位置已经有子（数组元素大小不为0），就继续循环
							continue;
						setlinecolor(BLACK);
						setfillcolor(BLACK);  //画棋子
						fillcircle(m.x, m.y, 8);
						chess[l][k] = 1;
						count++;
						clearrectangle(170, 220, 370, 350);
						setlinecolor(BLACK);
						setfillcolor(RGB(255, 201, 14));
						fillrectangle(170, 220, 370, 350);
						setbkmode(TRANSPARENT);
						settextstyle(40, 0, "华文楷体");
						settextcolor(BLACK);
						_itoa_s(count, s, 10);
						outtextxy(175, 280, "回合数：");
						outtextxy(325, 280, s);

						if (check(l, k, 1) == 1)
						{
							settextstyle(50, 0, "华文楷体");
							settextcolor(BLACK);
							outtextxy(175, 230, "黑方胜");
							flag1 = 0;
							break;
						}
					}
				}
				break;
			case 1:
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					if (m.x >= 400 && m.x <= 750 && m.y >= 50 && m.y <= 400)//取整，使棋子落在交线处
					{
						x = m.x % 25;
						if (x <= 12.5)
							m.x = m.x - x;
						else
							m.x = m.x - x + 25;
						y = m.y % 25;
						if (y <= 12.5)
							m.y = m.y - y;
						else
							m.y = m.y - y + 25;
						k = (m.x / 25) - 16;
						l = (m.y / 25) - 2;
						if (chess[l][k] != 0)
							continue;
						setlinecolor(WHITE);
						setfillcolor(WHITE);
						fillcircle(m.x, m.y, 8);
						chess[l][k] = 2;//回合数加1
						count++;
						clearrectangle(170, 220, 370, 350);
						setlinecolor(BLACK);
						setfillcolor(RGB(255, 201, 14));
						fillrectangle(170, 220, 370, 350);
						setbkmode(TRANSPARENT);
						settextstyle(40, 0, "华文楷体");
						settextcolor(WHITE);
						_itoa_s(count, s, 10);
						outtextxy(175, 280, "回合数：");
						outtextxy(325, 280, s);
						if (check(l, k, 2) == 2)
						{
							settextstyle(50, 0, "华文楷体");
							settextcolor(WHITE);
							outtextxy(175, 230, "白方胜");
							flag1 = 0; 回合结束，修改flag1的值，跳出循环
							break;
						}
					}
				}
				break;
			}
		}
	}
	//****************************************************计算下棋的回合数以及显示输赢数据************************************************//
void AI()
{
	char s[100];
	while (flag1)      //游戏一直循环
	{
		int x, y, k, l;
		//获取鼠标信息
		int id1 = 0, jd1 = 0, i = 0, j = 0;
		int now = count % 2;//确定回合，如果是0，则下黑子，如果是1，则下白子
		switch (now)
		{
		case 0:
			MOUSEMSG m;
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN)    //如果按下鼠标左键 
			{
				if (m.x >= 400 && m.x <= 750 && m.y >= 50 && m.y <= 400)   //判断是否在棋盘内
				{
					x = m.x % 25;        //不在棋盘格子上，则取余，取最近的整数点
					if (x <= 12.5)
						m.x = m.x - x;
					else
						m.x = m.x - x + 25;
					y = m.y % 25;
					if (y <= 12.5)
						m.y = m.y - y;
					else
						m.y = m.y - y + 25;
					k = (m.x / 25) - 16;   //计算出对应数组下标
					l = (m.y / 25) - 2;
					if (chess[l][k] != 0)//如果当前位置已经有子（数组元素大小不为0），就继续循环
						continue;
					setlinecolor(BLACK);
					setfillcolor(BLACK);    //画棋子
					fillcircle(m.x, m.y, 8);
					chess[l][k] = 1;
					count++;
					clearrectangle(170, 220, 370, 350);
					setlinecolor(BLACK);
					setfillcolor(RGB(255, 201, 14));
					fillrectangle(170, 220, 370, 350);
					setbkmode(TRANSPARENT);
					settextstyle(40, 0, "华文楷体");
					settextcolor(BLACK);
					_itoa_s(count, s, 10);
					outtextxy(175, 280, "回合数：");
					outtextxy(325, 280, s);
					if (check(l, k, 1) == 1)   //如果黑子五连珠.0O
					{
						settextstyle(50, 0, "华文楷体");
						settextcolor(BLACK);
						outtextxy(175, 230, "黑方胜");
						flag1 = 0;
						break;
					}
				}
			}
			break;
		case 1:
			int x ;
			int y ;
			for (y = 0; y < N; y++)
			{
				for (x = 0; x < N; x++)
				{
						p.x = x;
						p.y = y;
						playgrade[y][x] = Evaluate(p, 1);//找出最大权值对应的数组下标
				}
			}
			int max = playgrade[0][0];
			int yd = 0, xd = 0;
			for (y = 0; y < N; y++)
			{
				for (x = 0; x < N; x++)
				{
					if (playgrade[y][x]>max&&chess[y][x]==0)
					{
						max = playgrade[y][x];
						yd = y;
						xd = x;
					}
				}
			}
			for (i = 0; i < 2;i++)
			{
				setlinecolor(WHITE);
				setfillcolor(WHITE);  //画棋子
				fillcircle(25*(xd+16), 25*(yd+2), 8);
				Sleep(500);
				setlinecolor(RGB(200,255,255));
				setfillcolor(RGB(200, 255, 255));    //画棋子
				fillcircle(25 * (xd + 16), 25 * (yd + 2), 8);
				Sleep(500);
			}
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillcircle(25 * (xd + 16), 25 * (yd + 2), 8);
			chess[yd][xd] = 2;
			count++;//通过清除矩形框区域 的内容来达到刷新消息提示的目的
			clearrectangle(170, 220, 370, 350);
			setlinecolor(BLACK);
			setfillcolor(RGB(255, 201, 14));
			fillrectangle(170, 220, 370, 350);
			setbkmode(TRANSPARENT);
			settextstyle(40, 0, "华文楷体");
			settextcolor(BLACK);
			_itoa_s(count, s, 10);
			outtextxy(175, 280, "回合数：");
			outtextxy(325, 280, s);
			if (check(yd, xd, 2) == 2)///检验白棋是否五连珠
			{
				settextstyle(50, 0, "华文楷体");
				settextcolor(WHITE);
				outtextxy(175, 230, "白方胜");
				_itoa_s(count, s, 10);
				flag1 = 0;
				break;
			}
			break;

		}
	}
}
//******************************************************游戏开始界面的显示************************************************//
void GameBackground()
{
	IMAGE Gamebegin;                               //定义图片
	loadimage(&Gamebegin, "gamebegin.jpg");       //加载图片
	int width = Gamebegin.getwidth();
	int height = Gamebegin.getheight();
	initgraph(width, height);                      //设置宽和高
	putimage(0, 0, &Gamebegin);                    //放图片
	setfillcolor(RGB(255, 201, 14));
	fillrectangle(20, 50, 135, 100); //画矩形框，作为消息传递的窗口
	fillrectangle(20, 110, 135, 160);
	fillrectangle(20, 170, 135, 220);
	setbkmode(TRANSPARENT); //设置背景颜色为透明，消去不美观的黑色背景
	outtextxy(40, 65, "人人对弈"); //三种游戏功能的实现
	outtextxy(40, 125, "人机对弈");
	outtextxy(40, 185, "退出游戏");
	messagebox(); //调用消息框函数	
	
}
//*******************************************************游戏时实现功能选项******************************************************//
void playgame()
{
	GameBackground();
	settextcolor(BLACK);
	settextstyle(25, 0, "华文楷体");
	while (true)
	{
		getmousemsg();
		if (m.x > 20 && m.x < 135 && m.y>50 && m.y < 100 && m.uMsg == WM_LBUTTONDOWN)
		{
			clearrectangle(20, 50, 135, 100);
			for (int i = 0; i < 2; i++)
			{
				setbkmode(TRANSPARENT);
				setfillcolor(RGB(255, 255, 128));
				fillrectangle(20, 50, 135, 100);
				outtextxy(30, 65, "人人对弈");
				Sleep(200);
				setbkmode(TRANSPARENT);
				setfillcolor(RGB(255, 255, 0));
				fillrectangle(20, 50, 135, 100);
				outtextxy(30, 65, "人人对弈");
				Sleep(200);
			}
			setbkmode(TRANSPARENT);
			fillrectangle(20, 50, 135, 100);
			outtextxy(40, 65, "人人对弈");
			fillrectangle(20, 50, 135, 100);
			cleardevice();
			initialize();       //游戏再开始时初始化数据
			GameBackground();       //游戏开始背景界面
			draw();               //画棋盘
			pvp();//双人游戏
		}
		if (m.x > 20 && m.x < 135 && m.y>110 && m.y < 160 && m.uMsg == WM_LBUTTONDOWN)
		{
			clearrectangle(20, 110, 135, 160);
			for (int i = 0; i < 2; i++)
			{
				setbkmode(TRANSPARENT);
				setfillcolor(RGB(255, 255, 128));
				fillrectangle(20, 110, 135,160 );
				outtextxy(30, 125, "人机对弈");
				Sleep(200);
				setbkmode(TRANSPARENT);
				setfillcolor(RGB(255, 255, 0));
				fillrectangle(20, 110, 135, 160);
				outtextxy(30, 125, "人机对弈");
				Sleep(200);
			}
			setbkmode(TRANSPARENT);
			fillrectangle(20, 50, 135, 100);
			outtextxy(40, 65, "人机对弈");

			cleardevice();
			initialize();
			GameBackground();
			draw();
			AI();//人机对弈
		}
		if (m.x > 20 && m.x < 135 && m.y>170 && m.y < 220 && m.uMsg == WM_LBUTTONDOWN)
		{
			clearrectangle(20, 170, 135, 220);
			for (int i = 0; i < 2; i++)
			{
				setbkmode(TRANSPARENT);
				setfillcolor(RGB(255, 255, 128));
				fillrectangle(20, 170, 135, 220);
				outtextxy(30, 185, "退出游戏");
				Sleep(200);
				setbkmode(TRANSPARENT);
				setfillcolor(RGB(255, 255, 0));
				fillrectangle(20, 170, 135, 220);
				outtextxy(30, 185, "退出游戏");
				Sleep(200);
			}
			setbkmode(TRANSPARENT);
			fillrectangle(20, 170, 135, 220);
			outtextxy(40, 125, "退出游戏");
			exit(0);
		}
	}


}
//*************************************************************主函数*************************************************************//
int main()
{
	    GameBegin();
		
		playgame();
		closegraph();
	return 0;
}
