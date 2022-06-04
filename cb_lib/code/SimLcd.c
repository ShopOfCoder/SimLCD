#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "KeyLcdIn.h"
#include "SimKey.h"
#include "SimLcd.h"


typedef struct
{
	// 预留：供用户自定义窗体名称
	const char		*strWinTitle;
	// SimLCD线程句柄
	HANDLE			hThrd;
	// 窗口句柄
	HWND			hWnd;
	// 按键回调
	FKeyDeal		fnKeyDeal;
	// 鼠标回调
	FMouseDeal		fnMouseDeal;
	// 液晶规格
	CSlcdSpecs		tLcdSpecs;
}CSimLcd;
static CSimLcd s_tWinLcd = {_T("SimLCD"), NULL, NULL, NULL, NULL, {0}};


static void SimLcd_DrawBmp(HDC hDC, int vLeft, int vTop, int vWidth, int vHeight, LPCTSTR strFile)
{
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, strFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 画一个矩形，用当前的画笔画矩形轮廓，用当前画刷进行填充
	Rectangle(hDC, vLeft, vTop, vLeft+vWidth, vTop+vHeight);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);
	DeleteObject(hBitmap);
}

static int SimLcd_Paint(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int iRes;

	hdc = BeginPaint(hWnd, &ps);

	iRes = _access(s_tWinLcd.tLcdSpecs.strBmpPath, 0);
	if(iRes == 0)
	{
		SimLcd_DrawBmp(hdc, 0, 0, s_tWinLcd.tLcdSpecs.vWidth, s_tWinLcd.tLcdSpecs.vHeight, s_tWinLcd.tLcdSpecs.strBmpPath);
	}

	EndPaint(hWnd, &ps);

	return 0;
}

LRESULT CALLBACK SimLcd_MsgProc(HWND hWnd, UINT vMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT vRet = 0;
	
    switch(vMsg)
    {
        case WM_CREATE:
            s_tWinLcd.hWnd = hWnd;
			break;

        case WM_PAINT:
            SimLcd_Paint(hWnd);
            break;

		case WM_KEYDOWN:
		    if(s_tWinLcd.fnKeyDeal != NULL){
                s_tWinLcd.fnKeyDeal(KEY_EVENT_DOWN, wParam);
		    }
			break;

		case WM_KEYUP:
		    if(s_tWinLcd.fnKeyDeal != NULL){
                s_tWinLcd.fnKeyDeal(KEY_EVENT_UP, wParam);
		    }
			break;

        case WM_LBUTTONDOWN:
            if(s_tWinLcd.fnMouseDeal != NULL){
                s_tWinLcd.fnMouseDeal(MOUSE_EVENT_DOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		    }
            break;

		case WM_LBUTTONUP:
		    if(s_tWinLcd.fnMouseDeal != NULL){
                s_tWinLcd.fnMouseDeal(MOUSE_EVENT_UP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		    }
            break;

		case WM_MOUSEMOVE:
		    if(s_tWinLcd.fnMouseDeal != NULL){
                s_tWinLcd.fnMouseDeal(MOUSE_EVENT_MOVE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		    }
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;
		
        default:                      
            vRet = DefWindowProc (hWnd, vMsg, wParam, lParam);
			break;
    }

    return vRet;
}

static int SL_RegisterClass(HINSTANCE hThisInstance, WNDCLASSEX *ptWincl)
{
	int vRet = 0;
	WNDCLASSEX wincl;

	/* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = _T("SimLcdClass");
    wincl.lpfnWndProc = SimLcd_MsgProc;       /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl)){
		vRet = -1;
	}

	*ptWincl = wincl;

	return vRet;
}

DWORD WINAPI SimLcd_ThrdFunc(LPVOID pvParam)
{
    int vRet = 0, vRes;
	HWND hWnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
	// 入参
	CSimLcd *ptSimLcd = (CSimLcd*)pvParam;
	HINSTANCE hThisInstance = GetModuleHandle(0);

    vRes = SL_RegisterClass(hThisInstance, &wincl);
	if(vRes != 0){
		vRet = -1;
		goto LB_END;
	}

    /* The class is registered, let's create the program*/
    hWnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           wincl.lpszClassName,      /* Classname */
           ptSimLcd->strWinTitle,    /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           ptSimLcd->tLcdSpecs.vWidth,                 /* The programs width */
           ptSimLcd->tLcdSpecs.vHeight,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hWnd, SW_SHOWDEFAULT);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    vRet = messages.wParam;

LB_END:
	return vRet;
}

int SimLcd_CheckExit(void)
{
	DWORD dwExitCode = 0;

	GetExitCodeThread(s_tWinLcd.hThrd, &dwExitCode);
	if(dwExitCode != STILL_ACTIVE){
		ExitProcess(0);
	}

	return 0;
}

int SimLcd_Flush(void)
{
	if(s_tWinLcd.hWnd != NULL){
        InvalidateRect(s_tWinLcd.hWnd, NULL, 1);
        SendMessage(s_tWinLcd.hWnd, WM_PAINT, 0, 0);
	}

    return 0;
}

void SimLcd_SetKeyFunc(FKeyDeal fnKey)
{
	s_tWinLcd.fnKeyDeal = fnKey;
}

void SimLcd_SetMouseFunc(FMouseDeal fnMouse)
{
	s_tWinLcd.fnMouseDeal = fnMouse;
}

int SimLcd_Init(CSlcdSpecs *ptLcdPara)
{
    DWORD	vThrdID = 0;
	int		vRet=0;

	memcpy(&s_tWinLcd.tLcdSpecs, ptLcdPara, sizeof(CSlcdSpecs));
	s_tWinLcd.hWnd = NULL;
	// 创建窗口线程
	s_tWinLcd.hThrd = CreateThread(NULL, 0, SimLcd_ThrdFunc, &s_tWinLcd, 0, &vThrdID);
	if(!s_tWinLcd.hThrd){
		vRet = -1;
		goto LB_END;
	}
	// 等待窗口创建完成
	while(s_tWinLcd.hWnd == NULL)
	{
		Sleep(100);
	}

LB_END:
	return vRet;
}

