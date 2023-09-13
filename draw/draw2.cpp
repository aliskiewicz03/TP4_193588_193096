// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw2.h"
#include <vector>
#include <cstdio>
#include <list>
#include <string>

#define MAX_LOADSTRING 100
#define TMR_1 1
#define FLOOR0 0
#define FLOOR1 100
#define FLOOR2 215
#define FLOOR3 350
#define FLOOR4 480
#define MAX_PEOPLE 8
#define COOLDOWN 50

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

INT value;
int height;
int destinationHeight = 0;
bool isWaiting = false; //Czy winda stoi na piêtrze i czeka na pasa¿erów
double waitingCooldown = COOLDOWN; 
bool isMovingUp = true;
int numberOfPassengers = 0;

bool fistTimeDraw = true;

void FloorAction();

// buttons
HWND hwndButton;

int col = 0;
std::vector<Point> data;;
RECT drawArea2 = { 300, 0, 600, 800 };
RECT drawArea3 = { 800, 120, 1000, 135 };

struct WaitingPassenger
{
	int destinationFloor;
	bool isGoingUp;
};

struct Passenger
{
	int destinationFloor;
};

struct Floorr
{
	std::list <WaitingPassenger> waiting; // destination
	int away;
};

Floorr floors[5];
std::list<Passenger> passengers;


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);


void MyOnPaint(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 255));
	Pen pen2(Color(255, 25 * col, 0, 255));
	Pen pen3(Color(255, 0, 0, 0));

	fistTimeDraw = false;
	// piêtra:
	graphics.DrawLine(&pen3, 70, 600, 350, 600); // 0
	graphics.DrawLine(&pen3, 520, 500, 800, 500); // 1
	graphics.DrawLine(&pen3, 70, 385, 350, 385); // 2
	graphics.DrawLine(&pen3, 520, 250, 800, 250); // 3
	graphics.DrawLine(&pen3, 70, 120, 350, 120); // 4

	// szyb:
	graphics.DrawRectangle(&pen3, 350, 70, 170, 550);


	// winda:
	graphics.DrawRectangle(&pen, 350, height, 170, 50); 


	// Wypisyanie iloœci czekaj¹cych
	std::string text = std::to_string(numberOfPassengers * 70);
	TextOutA(hdc, 900, 120, text.c_str(), text.size());

	std::string floor0Number = std::to_string(floors[0].waiting.size());
	TextOutA(hdc, 340, 580, floor0Number.c_str(), floor0Number.size());

	std::string floor1Number = std::to_string(floors[1].waiting.size());
	TextOutA(hdc, 522, 482, floor1Number.c_str(), floor1Number.size());

	std::string floor2Number = std::to_string(floors[2].waiting.size());
	TextOutA(hdc, 340, 368, floor2Number.c_str(), floor2Number.size());

	std::string floor3Number = std::to_string(floors[3].waiting.size());
	TextOutA(hdc, 522, 234, floor3Number.c_str(), floor3Number.size());

	std::string floor4Number = std::to_string(floors[4].waiting.size());
	TextOutA(hdc, 340, 100, floor4Number.c_str(), floor4Number.size());

	//Wypisywanie iloœci osób, które opuœci³y windê na danym piêtrze
	std::string floor0Leave = std::to_string(floors[0].away);
	TextOutA(hdc, 300, 580, floor0Leave.c_str(), floor0Leave.size());

	std::string floor1Leave = std::to_string(floors[1].away);
	TextOutA(hdc, 562, 482, floor1Leave.c_str(), floor1Leave.size());

	std::string floor2Leave = std::to_string(floors[2].away);
	TextOutA(hdc, 300, 368, floor2Leave.c_str(), floor2Leave.size());

	std::string floor3Leave = std::to_string(floors[3].away);
	TextOutA(hdc, 562, 234, floor3Leave.c_str(), floor3Leave.size());

	std::string floor4Leave = std::to_string(floors[4].away);
	TextOutA(hdc, 300, 100, floor4Leave.c_str(), floor4Leave.size());

	//Wypisywanie iloœci osób w windzie
	std::string passengersNumber = std::to_string(passengers.size());
	TextOutA(hdc, 430, height + 20, passengersNumber.c_str(), passengersNumber.size());

}

void repaintWindow(HWND hWnd, HDC& hdc, PAINTSTRUCT& ps, RECT* drawArea)
{
	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	MyOnPaint(hdc);
	EndPaint(hWnd, &ps);
}

void inputData()
{
	data.push_back(Point(0, 0));
	for (int i = 1; i < 100; i++) {
		data.push_back(Point(2 * i + 1, 200 * rand() / RAND_MAX));
	}
}


int OnCreate(HWND window)
{
	inputData();
	return 0;
}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	value = 0;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;


	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	hwndButton = CreateWindow(TEXT("button"),      // The class name required is button
		TEXT("DrawAll"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		900, 0,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON2,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	// create button and store the handle                                                       

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		900, 155, 100, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		900, 200, 100, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	//guziki parter:

	hwndButton = CreateWindow(TEXT("button"), TEXT("1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		70, 500, 50, 40, hWnd,
		(HMENU)ID_BUTTON01,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		120, 500, 50, 40, hWnd,
		(HMENU)ID_BUTTON02,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("3"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		170, 500, 50, 40, hWnd,
		(HMENU)ID_BUTTON03,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("4"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		220, 500, 50, 40, hWnd,
		(HMENU)ID_BUTTON04,
		hInstance, NULL);

	//guziki piêtro 1:

	hwndButton = CreateWindow(TEXT("button"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		600, 400, 50, 40, hWnd,
		(HMENU)ID_BUTTON10,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		650, 400, 50, 40, hWnd,
		(HMENU)ID_BUTTON12,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("3"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		700, 400, 50, 40, hWnd,
		(HMENU)ID_BUTTON13,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("4"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		750, 400, 50, 40, hWnd,
		(HMENU)ID_BUTTON14,
		hInstance, NULL);

	//guziki piêtro 2:

	hwndButton = CreateWindow(TEXT("button"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		70, 285, 50, 40, hWnd,
		(HMENU)ID_BUTTON20,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		120, 285, 50, 40, hWnd,
		(HMENU)ID_BUTTON21,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("3"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		170, 285, 50, 40, hWnd,
		(HMENU)ID_BUTTON23,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("4"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		220, 285, 50, 40, hWnd,
		(HMENU)ID_BUTTON24,
		hInstance, NULL);

	//guziki piêtro 3:

	hwndButton = CreateWindow(TEXT("button"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		600, 150, 50, 40, hWnd,
		(HMENU)ID_BUTTON30,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		650, 150, 50, 40, hWnd,
		(HMENU)ID_BUTTON31,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		700, 150, 50, 40, hWnd,
		(HMENU)ID_BUTTON32,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("4"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		750, 150, 50, 40, hWnd,
		(HMENU)ID_BUTTON34,
		hInstance, NULL);

	//guziki piêtro 4:

	hwndButton = CreateWindow(TEXT("button"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		70, 20, 50, 40, hWnd,
		(HMENU)ID_BUTTON40,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		120, 20, 50, 40, hWnd,
		(HMENU)ID_BUTTON41,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		170, 20, 50, 40, hWnd,
		(HMENU)ID_BUTTON42,
		hInstance, NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("3"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		220, 20, 50, 40, hWnd,
		(HMENU)ID_BUTTON43,
		hInstance, NULL);


	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		900, 155, 100, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		900, 200, 100, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	
	// Okno z mas¹ w windzie:
	HWND hwndWeightText = CreateWindow(
		TEXT("STATIC"),      
		TEXT("Masa w windzie:"), 
		WS_VISIBLE | WS_CHILD,
		900, 100, 200, 20, 
		hWnd, 
		NULL, 
		hInstance, 
		NULL
	);

	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	int size = passengers.size();


	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON1:
			//col++;
			//if (col > 10)
			//	col = 0;
			//repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_BUTTON2:
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case ID_RBUTTON1:
			SetTimer(hWnd, TMR_1, 25, 0);
			break;
		case ID_RBUTTON2:
			KillTimer(hWnd, TMR_1);
			break;

		case ID_BUTTON01:
			floors[0].waiting.push_back({ FLOOR1, true }); // Kolejkowanie pasa¿erów
			if (isWaiting && value == FLOOR0) 
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON02:
			floors[0].waiting.push_back({ FLOOR2, true });
			if (isWaiting && value == FLOOR0)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON03:
			floors[0].waiting.push_back({ FLOOR3, true });
			if (isWaiting && value == FLOOR0)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON04:
			floors[0].waiting.push_back({ FLOOR4, true });
			if (isWaiting && value == FLOOR0)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON10:
			floors[1].waiting.push_back({ FLOOR0, false });
			if (isWaiting && value == FLOOR1)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON12:
			floors[1].waiting.push_back({ FLOOR2, true });
			if (isWaiting && value == FLOOR1)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON13:
			floors[1].waiting.push_back({ FLOOR3, true });
			if (isWaiting && value == FLOOR1)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON14:
			floors[1].waiting.push_back({ FLOOR4, true });
			if (isWaiting && value == FLOOR1)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON20:
			floors[2].waiting.push_back({ FLOOR0, false });
			if (isWaiting && value == FLOOR2)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON21:
			floors[2].waiting.push_back({ FLOOR1, false });
			if (isWaiting && value == FLOOR2)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON23:
			floors[2].waiting.push_back({ FLOOR3, true });
			if (isWaiting && value == FLOOR2)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON24:
			floors[2].waiting.push_back({ FLOOR4, true });
			if (isWaiting && value == FLOOR2)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON30:
			floors[3].waiting.push_back({ FLOOR0, false });
			if (isWaiting && value == FLOOR3)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON31:
			floors[3].waiting.push_back({ FLOOR1, false });
			if (isWaiting && value == FLOOR3)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON32:
			floors[3].waiting.push_back({ FLOOR2, false });
			if (isWaiting && value == FLOOR3)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON34:
			floors[3].waiting.push_back({ FLOOR4, true });
			if (isWaiting && value == FLOOR3)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON40:
			floors[4].waiting.push_back({ FLOOR0, false });
			if (isWaiting && value == FLOOR4)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON41:
			floors[4].waiting.push_back({ FLOOR1, false });
			if (isWaiting && value == FLOOR4)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON42:
			floors[4].waiting.push_back({ FLOOR2, false });
			if (isWaiting && value == FLOOR4)
				waitingCooldown = COOLDOWN;
			break;

		case ID_BUTTON43:
			floors[4].waiting.push_back({ FLOOR3, false });
			if (isWaiting && value == FLOOR4)
				waitingCooldown = COOLDOWN;
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			//force window to repaint
			repaintWindow(hWnd, hdc, ps, &drawArea2);
			repaintWindow(hWnd, hdc, ps, &drawArea3);

			height = 550 - value; // wysokoœæ windy
			if (isWaiting == true)
			{
				waitingCooldown--; // cooldown coraz mniejszy
				if (waitingCooldown < 0) 
				{
					waitingCooldown = COOLDOWN;
					isWaiting = false;
					if (passengers.empty() && value != FLOOR0) //Je¿eli nikogo nie ma w windzie po zatrzymaniu winda zje¿d¿a na parter
					{
						destinationHeight = FLOOR0;
						isMovingUp = false;
					}
					else if (passengers.empty() && value == FLOOR0) 
						isMovingUp = true;
				}
				break;
			}
			FloorAction();

			if (!passengers.empty()) 
			{
				destinationHeight = passengers.front().destinationFloor;
				if (destinationHeight < value)
					isMovingUp = false;
				
				else if (destinationHeight > value)
					isMovingUp = true;
			}
			if (value < destinationHeight && isMovingUp == true)
				value++;
			else if (value > destinationHeight && isMovingUp == false)
				value--;
			else
			{
				if (value == FLOOR0)
				{
					if (!floors[4].waiting.empty())
					{
						destinationHeight = FLOOR4;
						isMovingUp = true;
						break;
					}
					else if (!floors[3].waiting.empty())
					{
						destinationHeight = FLOOR3;
						isMovingUp = true;
						break;
					}
					else if (!floors[2].waiting.empty())
					{
						destinationHeight = FLOOR2;
						isMovingUp = true;
						break;
					}
					else if (!floors[1].waiting.empty())
					{
						destinationHeight = FLOOR1;
						isMovingUp = true;
						break;
					}

				}
				isWaiting = true;
			}
			FloorAction();
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void FloorAction() {
	if (value == FLOOR0) //Piêtro 0
	{

		//Rotujemy wszystkich na liœcie sprawdzaj¹c czy jakiœ nie chcia³ wyjœæ na tym piêtrze
		int size = passengers.size();
		for (int i = 0; i < size; i++)
		{
			if (passengers.front().destinationFloor == value)
			{
				isWaiting = true;
				passengers.pop_front();
				numberOfPassengers--;
				floors[0].away++;
			}
			else
			{
				passengers.push_back({ passengers.front().destinationFloor });
				passengers.pop_front();
			}
		}

		//Wpuszczamy pasa¿erów czekaj¹cych na danym piêtrze
		int queueSize = floors[0].waiting.size();
		for (int i = 0; i < queueSize; i++)
		{
			if (floors[0].waiting.front().isGoingUp == isMovingUp && passengers.size() < MAX_PEOPLE)
			{
				isWaiting = true;
				int dest = floors[0].waiting.front().destinationFloor;
				floors[0].waiting.pop_front();
				passengers.push_back({ dest });
				numberOfPassengers++;
			}
			else
			{
				floors[0].waiting.push_back({ floors[0].waiting.front().destinationFloor, !isMovingUp });
				floors[0].waiting.pop_front();

			}
		}

	}
	else if (value == FLOOR1) //Piêtro 1
	{
		int size = passengers.size();
		for (int i = 0; i < size; i++)
		{
			if (passengers.front().destinationFloor == value)
			{
				isWaiting = true;
				passengers.pop_front();
				numberOfPassengers--;
				floors[1].away++;
			}
			else
			{
				passengers.push_back({ passengers.front().destinationFloor });
				passengers.pop_front();
			}
		}

		int queueSize = floors[1].waiting.size();
		for (int i = 0; i < queueSize; i++)
		{
			if (floors[1].waiting.front().isGoingUp == isMovingUp && passengers.size() < MAX_PEOPLE)
			{
				isWaiting = true;
				int dest = floors[1].waiting.front().destinationFloor;
				floors[1].waiting.pop_front();
				passengers.push_back({ dest });
				numberOfPassengers++;
			}
			else
			{
				floors[1].waiting.push_back({ floors[1].waiting.front().destinationFloor, !isMovingUp });
				floors[1].waiting.pop_front();

			}
		}
	}
	else if (value == FLOOR2) //Piêtro 2
	{

		int size = passengers.size();
		for (int i = 0; i < size; i++)
		{
			if (passengers.front().destinationFloor == value)
			{
				isWaiting = true;
				passengers.pop_front();
				numberOfPassengers--;
				floors[2].away++;
			}
			else
			{
				passengers.push_back({ passengers.front().destinationFloor });
				passengers.pop_front();
			}
		}

		int queueSize = floors[2].waiting.size();
		for (int i = 0; i < queueSize; i++)
		{
			if (floors[2].waiting.front().isGoingUp == isMovingUp && passengers.size() < MAX_PEOPLE)
			{
				isWaiting = true;
				int dest = floors[2].waiting.front().destinationFloor;
				floors[2].waiting.pop_front();
				passengers.push_back({ dest });
				numberOfPassengers++;
			}
			else
			{
				floors[2].waiting.push_back({ floors[2].waiting.front().destinationFloor, !isMovingUp });
				floors[2].waiting.pop_front();

			}
		}
	}
	else if (value == FLOOR3) //Piêtro 3
	{

		int size = passengers.size();
		for (int i = 0; i < size; i++)
		{
			if (passengers.front().destinationFloor == value)
			{
				isWaiting = true;
				passengers.pop_front();
				numberOfPassengers--;
				floors[3].away++;
			}
			else
			{
				passengers.push_back({ passengers.front().destinationFloor });
				passengers.pop_front();
			}
		}

		int queueSize = floors[3].waiting.size();
		for (int i = 0; i < queueSize; i++)
		{
			if (floors[3].waiting.front().isGoingUp == isMovingUp && passengers.size() < MAX_PEOPLE)
			{
				isWaiting = true;
				int dest = floors[3].waiting.front().destinationFloor;
				floors[3].waiting.pop_front();
				passengers.push_back({ dest });
				numberOfPassengers++;
			}
			else
			{
				floors[3].waiting.push_back({ floors[3].waiting.front().destinationFloor, !isMovingUp });
				floors[3].waiting.pop_front();

			}
		}
	}
	else if (value == FLOOR4) //Piêtro 4
	{
		int size = passengers.size();
		for (int i = 0; i < size; i++)
		{
			if (passengers.front().destinationFloor == value)
			{
				isWaiting = true;
				passengers.pop_front();
				numberOfPassengers--;
				floors[4].away++;
			}
			else
			{
				passengers.push_back({ passengers.front().destinationFloor });
				passengers.pop_front();
			}
		}

		int queueSize = floors[4].waiting.size();
		for (int i = 0; i < queueSize; i++)
		{
			if (floors[4].waiting.front().isGoingUp == isMovingUp && passengers.size() < MAX_PEOPLE)
			{
				isWaiting = true;
				int dest = floors[4].waiting.front().destinationFloor;
				floors[4].waiting.pop_front();
				passengers.push_back({ dest });
				numberOfPassengers++;
			}
			else
			{
				floors[4].waiting.push_back({ floors[4].waiting.front().destinationFloor, !isMovingUp });
				floors[4].waiting.pop_front();

			}
		}
		isMovingUp = false;
	}
}


