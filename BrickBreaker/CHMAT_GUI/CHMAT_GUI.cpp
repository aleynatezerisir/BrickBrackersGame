#include <windows.h>  
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include "chmat.h"
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <synchapi.h>
#include <string>
using namespace std;
#define ID_Start 1
#define ID_PAUSE 2
#define ID_Exit 3
#define ID_ReStartGame 4
#define ID_BLUE 5
#define ID_YELLOW 7
#define ID_ORANGE 8
#define ID_PURPLE 9
#define ID_VWS 6
#define ID_EASY 10
#define ID_MEDIUM 11
#define ID_HARD 12
#define ID_VERYEASY 13
#define ID_RED 14
#define ID_INSTROCTION 15

#define IDI_ICON1 101
#define IDC_OPEN	3000
#define IDC_BUTTON1 3001
#define IDC_BUTTON2 3002
#define IDC_BUTTON3 3003
#define NUMBER_OF_SEMAPHORES 3
#define NUMBER_OF_THREAD 9

CHMAT m(685, 685, CHMAT_INT);
HANDLE hTimerQueue = NULL;
HMENU hMenubar;
HMENU Level;
HMENU hBricksColor;
HINSTANCE hInst;   // current instance
HWND     hWnd, hEdit, HButton1, HButton2, HButton3, Hmainbmp;    //parent window
LPCTSTR lpszAppName = "GEN32";
int keypressed;
LPCTSTR lpszTitle = "Brick Breaker";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void drawBrick(CHMAT& target, int x1, int y1, int width, int height, int color, bool is_filled, int brickNum);
void setinitialValues();
HANDLE ghSemaphore;

//settings of ball
int oldBallCordX = 300;
int oldBallCordY = 450;
int ballCordX = 300;
int ballCordY = 450;
int yonX = -1;
int yonY = -1;
int Score = 0;

int MainBrickColor = 0x00ff0000;
int HızDegeri = 3;
int flag = 0;


//settings of controller
int controlX = 300;
int oldControlX = 999;
int controlLength = 120;
int isOldControllerDeleted = 1;

bool isPlaying = false;

struct Cord
{
	int x;
	int y;
};

struct Bricks
{
	Cord solUst;
	Cord solAlt;
	Cord sagUst;
	Cord sagAlt;
	bool isBroken = true;
	bool color = true;
}myBricks[200];
     

DWORD WINAPI myThread(LPVOID lpParam)
{
	CHMAT& m = *((CHMAT*)lpParam);
	DWORD dwWaitResult = WaitForSingleObject(ghSemaphore, INFINITE);
	while (true)
	{
		srand(time(NULL));
		if (isPlaying == false)
			continue;
		Rectangle(m, 0, 60, 685, 20, 0x000000, true);
		Rectangle(m, controlX, 60, controlLength, 20, 0xADFF2F, true);
		Sleep(1);
		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
	}
	ReleaseSemaphore(ghSemaphore, 1, NULL);

	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);

	return true;
}
DWORD WINAPI WriteScore(LPVOID lpParam)
{
	CHMAT& m = *((CHMAT*)lpParam);
	int temp;
	DWORD dwWaitResult = WaitForSingleObject(ghSemaphore, INFINITE);
	while (true)
	{
		srand(time(NULL));
		int Score2 = Score;
		temp = Score2 % 10;
		Score2 = Score2 / 10;
		m.printnumber(665, 660, temp, 0xFFFFFF, 10);
		temp = Score2 % 10;
		Score2 = Score2 / 10;
		m.printnumber(652, 660, temp, 0xFFFFFF, 10);
		temp = Score2 % 10;
		Score2 = Score2 / 10;
		m.printnumber(639, 660, temp, 0xFFFFFF, 10);
		temp = Score2 % 10;
		Score2 = Score2 / 10;
		m.printnumber(626, 660, temp, 0xFFFFFF, 10);
		temp = Score2 % 10;
		Score2 = Score2 / 10;
		m.printnumber(613, 660, temp, 0xFFFFFF, 10);
		temp = Score2 % 10;
		Score2 = Score2 / 10;
		m.printnumber(600, 660, temp, 0xFFFFFF, 10);
			
		Sleep(1);
		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
	}
	ReleaseSemaphore(ghSemaphore, 1, NULL);

	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);

	return true;
}
void TahtaVur() {
	if (HızDegeri == 0 && Score > 0) {
		Score -= 1;
	}
	else if (HızDegeri == 3 && Score > 1) {
		Score -= 2;
	}
	else if (HızDegeri == 6 && Score > 2) {
		Score -= 3;
	}
	else if (HızDegeri == 9 && Score > 3) {
		Score -= 4;
	}
}
void TuglaVur() {
	if (HızDegeri == 0) {
		Score += 25;
	}
	else if (HızDegeri == 3) {
		Score += 20;
	}
	else if (HızDegeri == 6) {
		Score += 15;
	}
	else if (HızDegeri == 9) {
		Score += 10;
	}
}
DWORD WINAPI myThread2(LPVOID lpParam)
{
	CHMAT& m = *((CHMAT*)lpParam);
	DWORD dwWaitResult = WaitForSingleObject(ghSemaphore, INFINITE);
	while (true)
	{
		if (isPlaying == false)
			continue;
		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
		if(oldBallCordX != 9999)
			Ellipse(m, oldBallCordX, oldBallCordY, 10, 10, 0x000000, true);
		Ellipse(m, ballCordX, ballCordY, 10, 10, 0xFFFFFF, true);

		oldBallCordX = ballCordX;
		oldBallCordY = ballCordY;

		ballCordX += yonX * 1;
		ballCordY += yonY * 1;

		int ballCenterX = ballCordX + 5;
		int ballCenterY = ballCordY + 5;

		if (ballCordY < 2) {
			//GAMEOVER
			char buff[100];
			string name = std::to_string(Score);
			sprintf_s(buff, "Your Score: %s", name.c_str());
			::MessageBox(hWnd, buff, "Game Over", MB_OK);

			Rectangle(m, controlX, 60, controlLength, 20, 0x000000, true);
			Score = 0;
			controlX = 300;
			setinitialValues();
			Rectangle(m, 300, 60, controlLength, 20, 0xADFF2F, true);
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
		}
		//PENCERE KENARLARI KONTROLU
		//SOL
		if (ballCordX == 0)
			yonX *= -1;
		//SAG
		else if (ballCordX == 675)
			yonX *= -1;
		//UST
		 else if (ballCordY == 675)
			yonY *= -1;

		//TAHTA KONTROLU
		//TAHTA USTU
		int uzunlukSagUstTh = ((controlX + 120 - ballCenterX) * (controlX + 120 - ballCenterX)) + ((80 - ballCenterY) * (80 - ballCenterY));
		int uzunlukSolUstTh = ((controlX - ballCenterX) * (controlX - ballCenterX)) + ((80 - ballCenterY) * (80 - ballCenterY));
		int uzunlukSolAltTh = ((controlX - ballCenterX) * (controlX - ballCenterX)) + ((60 - ballCenterY) * (60 - ballCenterY));
		int uzunlukSagAltTh = ((controlX + 120 - ballCenterX) * (controlX + 120 - ballCenterX)) + ((60 - ballCenterY) * (60 - ballCenterY));

		if (ballCordX+5 < controlX + 120 && ballCordX+5 > controlX && ballCordY == 80) {
			TahtaVur();
			yonY *= -1;
		}
		//TAHTA SOLU
		else if (ballCordY+5 < 80 && ballCordY+5 > 60 && ballCordX+10 == controlX) {
			TahtaVur();
			yonX *= -1;
		}
		// TAHTA SAGI
		else if (ballCordY+5 < 80 && ballCordY+5 > 60 && ballCordX == controlX+120) {
			TahtaVur();
			yonX *= -1;
		}
		else if (uzunlukSagUstTh <= 25) {
			TahtaVur();
			int uzunluk1 = (((controlX + 120) - (ballCenterX - 5)) * ((controlX + 120) - (ballCenterX - 5))) + ((80 - ballCenterY) * (80 - ballCenterY));
			int uzunluk2 = (((controlX + 120) - ballCenterX) * (controlX + 120 - ballCenterX)) + ((80 - (ballCenterY - 5)) * (80 - (ballCenterY - 5)));
			if (uzunluk1 > uzunluk2) {
				yonY *= -1;
			}
			else if (uzunluk2 > uzunluk1) {
				yonX *= -1;
			}
			else
			{
				yonY *= -1;
				yonX *= -1;
			}
		}
		else if (uzunlukSolUstTh <= 25) {
			TahtaVur();
			int uzunluk1 = ((controlX - (ballCenterX + 5)) * (controlX - (ballCenterX + 5))) + ((80 - ballCenterY) * (80 - ballCenterY));
			int uzunluk2 = ((controlX - ballCenterX) * (controlX - ballCenterX)) + ((80 - (ballCenterY - 5)) * (80 - (ballCenterY - 5)));
			if (uzunluk1 > uzunluk2) {
				yonY *= -1;
			}
			else if (uzunluk2 > uzunluk1) {
				yonX *= -1;
			}
			else
			{
				yonY *= -1;
				yonX *= -1;
			}
		}
		else if (uzunlukSolAltTh <= 25) {
			TahtaVur();
			int uzunluk1 = ((controlX - ballCenterX) * (controlX - ballCenterX)) + ((60 - (ballCenterY + 5)) * (60 - (ballCenterY + 5)));
			int uzunluk2 = ((controlX - (ballCenterX + 5)) * (controlX - (ballCenterX + 5))) + ((60 - ballCenterY) * (60 - ballCenterY));
			if (uzunluk1 > uzunluk2) {
				yonX *= -1;

			}
			else if (uzunluk2 > uzunluk1) {
				yonX *= -1;
			}
			else
			{
				yonX *= -1;
			}
		}
		else if (uzunlukSagAltTh <= 25) {
			TahtaVur();
			int uzunluk1 = ((controlX+120 -( ballCenterX - 5)) * (controlX + 120 - (ballCenterX - 5))) + ((60 - ballCenterY) * (60 - ballCenterY));
			int uzunluk2 = ((controlX + 120 - ballCenterX) * (controlX + 120 - ballCenterX)) + ((60 - (ballCenterY + 5)) * (60 - (ballCenterY + 5)));
			if (uzunluk1 > uzunluk2) {
				yonX *= -1;

			}
			else if (uzunluk2 > uzunluk1) {
				yonX *= -1;
			}
			else
			{
				yonX *= -1;
			}
		}
		/// HER BIR BRICK KONTROLU
		for (int i = 0; i < 180; i++) {
			int uzunlukSagUst = ((myBricks[i].sagUst.x - ballCenterX) * (myBricks[i].sagUst.x - ballCenterX)) + ((myBricks[i].sagUst.y - ballCenterY) * (myBricks[i].sagUst.y - ballCenterY));
			int uzunlukSolUst = ((myBricks[i].solUst.x - ballCenterX) * (myBricks[i].solUst.x - ballCenterX)) + ((myBricks[i].solUst.y - ballCenterY) * (myBricks[i].solUst.y - ballCenterY));
			int uzunlukSolAlt = ((myBricks[i].solAlt.x - ballCenterX) * (myBricks[i].solAlt.x - ballCenterX)) + ((myBricks[i].solAlt.y - ballCenterY) * (myBricks[i].solAlt.y - ballCenterY));
			int uzunlukSagAlt = ((myBricks[i].sagAlt.x - ballCenterX) * (myBricks[i].sagAlt.x - ballCenterX)) + ((myBricks[i].sagAlt.y - ballCenterY) * (myBricks[i].sagAlt.y - ballCenterY));
			//UST
			if (ballCordX+5 <myBricks[i].sagAlt.x && ballCordX+5 > myBricks[i].solAlt.x && ballCordY == myBricks[i].solUst.y && myBricks[i].isBroken == false) {
				TuglaVur();
				yonY *= -1;
				myBricks[i].isBroken = true;
				break;
			}
			else if (uzunlukSagUst <=25 && myBricks[i].isBroken == false) {
				TuglaVur();
				int uzunluk1 = ((myBricks[i].sagUst.x - (ballCenterX - 5)) * (myBricks[i].sagUst.x - (ballCenterX - 5))) + ((myBricks[i].sagUst.y - ballCenterY) * (myBricks[i].sagUst.y - ballCenterY));
				int uzunluk2 = ((myBricks[i].sagUst.x - ballCenterX) * (myBricks[i].sagUst.x - ballCenterX)) + ((myBricks[i].sagUst.y - (ballCenterY - 5)) * (myBricks[i].sagUst.y - (ballCenterY - 5)));
				if (uzunluk1 > uzunluk2) {
					yonY *= -1;
				}
				else if (uzunluk2 > uzunluk1) {
					yonX *= -1;
				}
				else
				{
					yonY *= -1;
					yonX *= -1;
				}
				myBricks[i].isBroken = true;
				break;
			}
			else if (uzunlukSolUst <= 25 && myBricks[i].isBroken == false) {
				TuglaVur();
				int uzunluk1 = ((myBricks[i].solUst.x - (ballCenterX + 5)) * (myBricks[i].solUst.x - (ballCenterX + 5))) + ((myBricks[i].solUst.y - ballCenterY) * (myBricks[i].solUst.y - ballCenterY));
				int uzunluk2 = ((myBricks[i].solUst.x - ballCenterX) * (myBricks[i].solUst.x - ballCenterX)) + ((myBricks[i].solUst.y - (ballCenterY - 5)) * (myBricks[i].solUst.y -( ballCenterY - 5)));
				if (uzunluk1 > uzunluk2) {
					yonY *= -1;
				}
				else if (uzunluk2 > uzunluk1) {
					yonX *= -1;
				}
				else
				{
					yonY *= -1;
					yonX *= -1;
				}
				myBricks[i].isBroken = true;
				break;
			}
			//ALT
			else if (ballCordX+5 <myBricks[i].sagAlt.x && ballCordX+5 > myBricks[i].solAlt.x && ballCordY+10 == myBricks[i].solAlt.y && myBricks[i].isBroken == false) {
				TuglaVur();
				yonY *= -1;
				myBricks[i].isBroken = true;
				break;
			}
			else if (uzunlukSagAlt <= 25 && myBricks[i].isBroken == false) {
				TuglaVur();
				int uzunluk1 = ((myBricks[i].sagAlt.x - (ballCenterX - 5)) * (myBricks[i].sagAlt.x - (ballCenterX - 5))) + ((myBricks[i].sagAlt.y - ballCenterY) * (myBricks[i].sagAlt.y - ballCenterY));
				int uzunluk2 = ((myBricks[i].sagAlt.x - ballCenterX) * (myBricks[i].sagAlt.x - ballCenterX)) + ((myBricks[i].sagAlt.y - (ballCenterY + 5)) * (myBricks[i].sagAlt.y - (ballCenterY + 5)));
				if (uzunluk1 > uzunluk2) {
					if (yonY == -1) {
						yonX *= -1;
					}
					else
						yonY *= -1;
				}
				else if (uzunluk2 > uzunluk1) {
					yonX *= -1;
				}
				else
				{
					if (yonY == -1) {
						yonX *= -1;
					}
					else {
						yonY *= -1;
						yonX *= -1;
					}

				}
				myBricks[i].isBroken = true;
				break;
			}
			else if (uzunlukSolAlt <= 25 && myBricks[i].isBroken == false) {
				TuglaVur();
				int uzunluk1 = ((myBricks[i].solAlt.x - ballCenterX) * (myBricks[i].solAlt.x - ballCenterX)) + ((myBricks[i].solAlt.y - (ballCenterY + 5)) * (myBricks[i].solAlt.y - (ballCenterY + 5)));
				int uzunluk2 = ((myBricks[i].solAlt.x - (ballCenterX + 5)) * (myBricks[i].solAlt.x - (ballCenterX + 5))) + ((myBricks[i].solAlt.y - ballCenterY) * (myBricks[i].solAlt.y - ballCenterY));
				if (uzunluk1 > uzunluk2) {
					if (yonY == -1) {
						yonX *= -1;
					}
					else
						yonY *= -1;
				}
				else if (uzunluk2 > uzunluk1) {
					yonX *= -1;
				}
				else
				{
					if (yonY == -1) {
						yonX *= -1;
					}
					else {
						yonY *= -1;
						yonX *= -1;
					}
					
				}
				myBricks[i].isBroken = true;
				break;
			}
			//SOL
			else if (ballCordY+5 < myBricks[i].solUst.y && ballCordY+5 > myBricks[i].solAlt.y && ballCordX+10 == myBricks[i].solAlt.x && myBricks[i].isBroken == false) {
				TuglaVur();
				yonX *= -1;
				myBricks[i].isBroken = true;
				break;
			}
			//SAG
			else if (ballCordY+5 < myBricks[i].solUst.y && ballCordY+5 > myBricks[i].solAlt.y && ballCordX == myBricks[i].sagAlt.x && myBricks[i].isBroken == false) {
				TuglaVur();
				yonX *= -1;
				myBricks[i].isBroken = true;
				break;
			}
		}
		for (int i = 0; i < HızDegeri; i++)
		{
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
			Sleep(0.001);
		}
		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
	}
	ReleaseSemaphore(ghSemaphore, 1, NULL);

	//SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);

	return true;
}
void setinitialValues() {
	int k = 15;
	int t = 500;
	yonX = -1;
	yonY = -1;
	srand(time(NULL));
	int a = rand()%10;
	if (a >= 5) {
		yonX *= -1;
	}
	ballCordX = rand() % 601 + 25;
	ballCordY = 450;
	for(int i=0;i<200;i++){
		myBricks[i].solUst.x = 0;
		myBricks[i].solUst.y = 0;
		myBricks[i].solAlt.x = 0;
		myBricks[i].solAlt.y = 0;
		myBricks[i].sagAlt.x = 0;
		myBricks[i].sagAlt.y = 0;
		myBricks[i].sagUst.x = 0;
		myBricks[i].sagUst.y = 0;
	}
	for (int i = 0; i < 180; i++) {
		myBricks[i].isBroken = false;
		myBricks[i].color = false;
	}
	for (int i = 0; i < 18; i++) {
		drawBrick(m, k + 10, t, 30, 10, MainBrickColor, true, (i*10)+0);          //0 10 20 30
		drawBrick(m, k + 10, t + 15, 30, 10, MainBrickColor, true, (i * 10) + 1);     //1 11
		drawBrick(m, k + 10, t + 30, 30, 10, MainBrickColor, true, (i * 10) + 2);     //2 12
		drawBrick(m, k + 10, t + 45, 30, 10, MainBrickColor, true, (i * 10) + 3);     //3
		drawBrick(m, k + 10, t + 60, 30, 10, MainBrickColor, true, (i * 10) + 4);     //4
		drawBrick(m, k + 10, t + 75, 30, 10, MainBrickColor, true, (i * 10) + 5);     //5
		drawBrick(m, k + 10, t + 90, 30, 10, MainBrickColor, true, (i * 10) + 6);
		drawBrick(m, k + 10, t + 105, 30, 10, MainBrickColor, true, (i * 10) + 7);
		drawBrick(m, k + 10, t + 120, 30, 10, MainBrickColor, true, (i * 10) + 8);
		drawBrick(m, k + 10, t + 135, 30, 10, MainBrickColor, true, (i * 10) + 9); //10 tane satır
		k += 35;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	MSG      msg;
	WNDCLASSEX wc;
	HMENU MainMenu, FileMenu;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = lpszAppName;
	wc.lpszClassName = lpszAppName;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm = (HICON)LoadImage(hInstance, lpszAppName,
		IMAGE_ICON, 16, 16,
		LR_DEFAULTCOLOR);

	if (!RegisterClassEx(&wc))
		return(FALSE);

	hInst = hInstance;
	hWnd = CreateWindowEx(0, lpszAppName,
		lpszTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		720, 740,
		NULL,
		NULL,
		hInstance,
		NULL
	);


	if (!hWnd)
		return(FALSE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);
}


//Thread called by the timer
int counter = 0;
int foo(int loc)
{
	return 0;
}

VOID CALLBACK SlidingBox(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	
	
}
void drawBrick(CHMAT& target, int x1, int y1, int width, int height, int color, bool is_filled,int brickNum) {
	Rectangle(target, x1, y1, width, height, color, is_filled);
	myBricks[brickNum].solUst.x = x1;
	myBricks[brickNum].solUst.y = y1+height;
	myBricks[brickNum].solAlt.x = x1;
	myBricks[brickNum].solAlt.y = y1;
	myBricks[brickNum].sagAlt.x = x1+width;
	myBricks[brickNum].sagAlt.y = y1;
	myBricks[brickNum].sagUst.x = x1+width;
	myBricks[brickNum].sagUst.y = y1+height;
}
VOID CALLBACK play2(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	HANDLE hTimer = NULL;
	hTimerQueue = CreateTimerQueue();
	for (int i = 0; i < 180; i++) {
		if (myBricks[i].isBroken == true && myBricks[i].color == false) {
			drawBrick(m, myBricks[i].solAlt.x, myBricks[i].solAlt.y, myBricks[i].sagAlt.x - myBricks[i].solAlt.x, myBricks[i].solUst.y - myBricks[i].solAlt.y, 0x000000, true, i);
			myBricks[i].color = true;
		}
		else if (myBricks[i].isBroken == false)
			drawBrick(m, myBricks[i].solAlt.x, myBricks[i].solAlt.y, myBricks[i].sagAlt.x - myBricks[i].solAlt.x, myBricks[i].solUst.y - myBricks[i].solAlt.y, MainBrickColor, true, i);
	}
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
}
void play() {
	int k = 15;
	int t= 500;
	HANDLE hTimer = NULL;
	for (int i = 0; i < 18; i++) {
		Rectangle(m, k + 10, t, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t+15, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t+30, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t+45, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t+60, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t+75, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t + 90, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t + 105, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t + 120, 30, 10, 0x00ff0000, true);
		Rectangle(m, k + 10, t + 135, 30, 10, 0x00ff0000, true); //10 tane satır
		k += 35;
	}

	DWORD myThreadID;
	HANDLE myHandle = CreateThread(0, 0, myThread, &m, 0, &myThreadID);

	hTimerQueue = CreateTimerQueue();
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
}
void menu() {
	play();
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	srand(time(NULL));
	switch (uMsg)
	{
	case WM_CREATE:
	{
		hMenubar = CreateMenu();
		HMENU hFile = CreateMenu();
		HMENU hEdit = CreateMenu();
		HMENU hHelp = CreateMenu();
		HMENU habout = CreateMenu();
		hBricksColor = CreateMenu();
		Level = CreateMenu();
		//HICON icon = CreateIcon();

		HANDLE hTimer = NULL;
		hTimerQueue = CreateTimerQueue();
		if (NULL == hTimerQueue)
		{
			break;
		}
		Hmainbmp = CreateWindowEx(NULL, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_THICKFRAME, 0, 0, 700,700, hWnd, NULL, hInst, NULL);
		setinitialValues();
		AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, "Game");
		AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hBricksColor, "Bricks Color");
		AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)Level, "Level");


		AppendMenu(hFile, MF_STRING, ID_Start, "Start Game");
		AppendMenu(hFile, MF_STRING, ID_PAUSE, "Pause/Continue Game (P)");
		AppendMenu(hFile, MF_STRING, ID_ReStartGame, "Restart Game");
		AppendMenu(hBricksColor, MF_STRING, ID_RED, "Red");
		AppendMenu(hBricksColor, MF_STRING, ID_BLUE, "Blue");
		AppendMenu(hBricksColor, MF_STRING, ID_YELLOW, "Yellow");
		AppendMenu(hBricksColor, MF_STRING, ID_ORANGE, "Orange");
		AppendMenu(hBricksColor, MF_STRING, ID_PURPLE, "Purple");
		AppendMenu(Level, MF_STRING, ID_VERYEASY, "Very Easy");
		AppendMenu(Level, MF_STRING, ID_EASY, "Easy");
		AppendMenu(Level, MF_STRING, ID_MEDIUM, "Medium");
		AppendMenu(Level, MF_STRING, ID_HARD, "Hard");

		CheckMenuItem(hBricksColor, ID_RED, MF_CHECKED);
		CheckMenuItem(Level, ID_MEDIUM, MF_CHECKED);



		AppendMenu(hFile, MF_STRING, ID_Exit, "Exit Game");
	

		AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)habout, "About Games");
		AppendMenu(habout, MF_STRING, ID_INSTROCTION, "Instruction");

		SetMenu(hWnd, hMenubar);
		break; 
		if (LOWORD(wParam) == ID_Exit) {
			exit(0);
		}
		

	
	}
	break;

	case WM_KEYDOWN:
	{
		keypressed = (int)wParam;
		if (isPlaying == true) {
			if (keypressed == 37 && controlX > 8) {
				oldControlX = controlX;
				controlX -= 8; }
			else if (keypressed == 39 && controlX < 560) {
				oldControlX = controlX;
				controlX += 8; }
		}
		if (keypressed == 112 || keypressed == 80) {
			if (isPlaying == true)
				isPlaying = false;
			else
				isPlaying = true;
		}
	}
	break;
	case WM_COMMAND:
		flag = 0;
		for (int i = 0; i < 180; i++) {
			if (myBricks[i].isBroken == true)
			{
				flag = 1;
				break;
			}
		}
		if (LOWORD(wParam) == ID_VERYEASY)
		{
			if (flag == 0) {
				CheckMenuItem(Level, ID_MEDIUM, MF_UNCHECKED);
				CheckMenuItem(Level, ID_EASY, MF_UNCHECKED);
				CheckMenuItem(Level, ID_HARD, MF_UNCHECKED);
				CheckMenuItem(Level, ID_VERYEASY, MF_CHECKED);
				HızDegeri = 9;
			}
			
		}
		if (LOWORD(wParam) == ID_EASY)
		{
			if (flag == 0) {
				CheckMenuItem(Level, ID_VERYEASY, MF_UNCHECKED);
				CheckMenuItem(Level, ID_MEDIUM, MF_UNCHECKED);
				CheckMenuItem(Level, ID_HARD, MF_UNCHECKED);
				CheckMenuItem(Level, ID_EASY, MF_CHECKED);
				HızDegeri = 6;
			}
			
		}
		if (LOWORD(wParam) == ID_MEDIUM)
		{
			if (flag == 0) {
				CheckMenuItem(Level, ID_VERYEASY, MF_UNCHECKED);
				CheckMenuItem(Level, ID_EASY, MF_UNCHECKED);
				CheckMenuItem(Level, ID_HARD, MF_UNCHECKED);
				CheckMenuItem(Level, ID_MEDIUM, MF_CHECKED);
				HızDegeri = 3;
			}
			
		}
		if (LOWORD(wParam) == ID_HARD)
		{
			if (flag == 0) {
				CheckMenuItem(Level, ID_VERYEASY, MF_UNCHECKED);
				CheckMenuItem(Level, ID_EASY, MF_UNCHECKED);
				CheckMenuItem(Level, ID_MEDIUM, MF_UNCHECKED);
				CheckMenuItem(Level, ID_HARD, MF_CHECKED);
				HızDegeri = 0;
			}
		}
		if (LOWORD(wParam) == ID_RED)
		{
			CheckMenuItem(hBricksColor, ID_YELLOW, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_ORANGE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_PURPLE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_BLUE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_RED, MF_CHECKED);

			MainBrickColor = 0x00ff0000;
		}
		if (LOWORD(wParam) == ID_BLUE)
		{
			CheckMenuItem(hBricksColor, ID_RED, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_YELLOW, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_ORANGE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_PURPLE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_BLUE, MF_CHECKED);
			MainBrickColor = 0x0000ff;
		}
		if (LOWORD(wParam) == ID_YELLOW)
		{
			CheckMenuItem(hBricksColor, ID_RED, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_BLUE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_ORANGE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_PURPLE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_YELLOW, MF_CHECKED);
			MainBrickColor = 0xffff00;
		}
		if (LOWORD(wParam) == ID_ORANGE)
		{
			CheckMenuItem(hBricksColor, ID_RED, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_YELLOW, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_BLUE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_PURPLE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_ORANGE, MF_CHECKED);
			MainBrickColor = 0xffa500;
		}
		if (LOWORD(wParam) == ID_PURPLE)
		{
			CheckMenuItem(hBricksColor, ID_RED, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_YELLOW, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_ORANGE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_BLUE, MF_UNCHECKED);
			CheckMenuItem(hBricksColor, ID_PURPLE, MF_CHECKED);
			MainBrickColor = 0x800080;
		}

		if (LOWORD(wParam) == ID_ReStartGame)
		{
			Rectangle(m, controlX, 60, controlLength, 20, 0x000000, true);
			Score = 0;
			controlX = 300;
			setinitialValues();
			Rectangle(m, 300, 60, controlLength, 20, 0xADFF2F, true);
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);
		}

		if (LOWORD(wParam) == ID_PAUSE)
		{
			if (isPlaying == true)
				isPlaying = false;
			else
				isPlaying = true;
		}
		if (LOWORD(wParam) == ID_Start)
		{
			if (isPlaying == false) {
				HANDLE hTimer = NULL;
				hTimerQueue = CreateTimerQueue();
				if (NULL == hTimerQueue)
				{
					break;
				}
				isPlaying = true;
				setinitialValues();
				SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)m.HBitmap);

				CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)play2, NULL, 1000, 30, 0);
				DWORD myThreadID;
				HANDLE myHandle = CreateThread(0, 0, myThread, &m, 0, &myThreadID);
				DWORD myThreadID2;
				HANDLE myHandle2 = CreateThread(0, 0, myThread2, &m, 0, &myThreadID2);
				DWORD myThreadID3;
				HANDLE myHandle3 = CreateThread(0, 0, WriteScore, &m, 0, &myThreadID3);
			}
		}
		if (LOWORD(wParam) == ID_Exit) {
			exit(0);
		}
		if (LOWORD(wParam) == ID_INSTROCTION) {
			::MessageBox(hWnd, "The board is moved by arrow keys.\nPressing P can pause and continue the game.", "About Games", MB_OK);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return(0L);
}

