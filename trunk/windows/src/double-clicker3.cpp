/*
  <double-clicker3.cpp> - <Simulates mouse single-click and double-click by using keyboard hotkeys>
  
Copyright 2008-2009 Autelic Association (http://www.autelic.org)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed 
under the License is distributed on an "AS IS" BASIS,WITHOUT WARRANTIES OR CONDITIONS 
OF ANY KIND, either express or implied. See the License for the specific language 
governing permissions and limitations under the License.
*/

// double-clicker3.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "shellapi.h"
#include "double-clicker3.h"

#include <assert.h>

#define MAX_LOADSTRING 100

#define MYTRAY_ICON_MSG (WM_USER+50)

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


const int singleClickVKey = VK_F6;
const int doubleClickVKey = VK_F7;

static INPUT g_mouseLeftClick[2];
static INPUT g_mouseDoubleLeftClick[4];

static TCHAR g_prgInfoMsg[1024] = _T("Autelic\n");

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static BOOL DCInit()
{
	g_mouseLeftClick[0].type = INPUT_MOUSE; 
	g_mouseLeftClick[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	g_mouseLeftClick[1].type = INPUT_MOUSE; 
	g_mouseLeftClick[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	g_mouseDoubleLeftClick[0].type = INPUT_MOUSE; 
	g_mouseDoubleLeftClick[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	g_mouseDoubleLeftClick[1].type = INPUT_MOUSE; 
	g_mouseDoubleLeftClick[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	g_mouseDoubleLeftClick[2].type = INPUT_MOUSE; 
	g_mouseDoubleLeftClick[2].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	g_mouseDoubleLeftClick[3].type = INPUT_MOUSE; 
	g_mouseDoubleLeftClick[3].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	return TRUE;
}

static BOOL RegisterHotKeys()
{
	TCHAR buf[1024];

	// register single-click hotkey
	//
    if (RegisterHotKey(
        NULL,
        0,	//hot key identifier
        0, //MOD_SHIFT, //MOD_ALT,// | MOD_NOREPEAT,
        singleClickVKey))
    {
        _stprintf_s(buf, 1024, _T("F%d -> single-click\n"), singleClickVKey - VK_F1 + 1 );
		OutputDebugString( buf );
		_tcscat_s( g_prgInfoMsg, _countof(g_prgInfoMsg), buf );
    }
	else
    {
        _stprintf_s(buf, 1024, _T("Couldn't assign F% key for single-click"), singleClickVKey - VK_F1 + 1 );
		OutputDebugString( buf );
		_tcscat_s( g_prgInfoMsg, _countof(g_prgInfoMsg), buf );
    }

	// register double-click hotkey
	//
 	if (RegisterHotKey(
        NULL,
        1,
        0, //MOD_SHIFT, //MOD_ALT,// | MOD_NOREPEAT,
        doubleClickVKey))
    {
        _stprintf_s(buf,1024, _T("F%d -> double-click\n"), doubleClickVKey - VK_F1 + 1 );
		OutputDebugString( buf );
		_tcscat_s( g_prgInfoMsg, _countof(g_prgInfoMsg), buf );
    }
	else
	{
        _stprintf_s(buf, 1024, _T("Couldn't assign F% key for double-click"), doubleClickVKey - VK_F1 + 1 );
		OutputDebugString( buf );
		_tcscat_s( g_prgInfoMsg, _countof(g_prgInfoMsg), buf );
    }

	return TRUE;
}

static BOOL NotificationIconMessage(HWND hWnd, DWORD dwMessage)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_AUTELIC;
	nid.uFlags = NIF_MESSAGE|NIF_ICON;
	nid.uCallbackMessage = MYTRAY_ICON_MSG;
	nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE (IDI_AUTELIC));
	_tcscpy_s( nid.szTip, _countof( nid.szTip ), (LPCTSTR)g_prgInfoMsg );

	return Shell_NotifyIcon(dwMessage, &nid);
}

static BOOL ShowContextMenu( HWND hWnd )
{
	HMENU ctxMenu = LoadMenu( hInst, MAKEINTRESOURCE (IDR_AUTELIC) );
	if( NULL == ctxMenu )
	{
		OutputDebugString( _T("Can´t retrieve app main menu") );
		return FALSE;
	}

	HMENU trackPopup = GetSubMenu(ctxMenu, 0 );

	// Get the position of the cursor
	POINT pCursor;
	GetCursorPos(&pCursor);

	// needed
    SetForegroundWindow(hWnd);

	// Popup the menu with cursor position as the coordinates to pop it up
	TrackPopupMenu(trackPopup, TPM_LEFTBUTTON | TPM_RIGHTALIGN, pCursor.x, pCursor.y, 0, hWnd, NULL);

	DestroyMenu( ctxMenu );

	return TRUE;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DOUBLECLICKER3, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
//	if (!InitInstance (hInstance, nCmdShow))
	if (!InitInstance (hInstance, SW_HIDE))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DOUBLECLICKER3));


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if( msg.message == WM_HOTKEY )
        {
			#ifdef _DEBUG
            OutputDebugString(_T("WM_HOTKEY received\n"));
			#endif

			int vKey = (msg.lParam) >> 16;
			UINT res = 0;

			assert( singleClickVKey == vKey || doubleClickVKey == vKey );
			if( singleClickVKey == vKey )
			{
				res = SendInput(      
					sizeof( g_mouseLeftClick ) / sizeof( g_mouseLeftClick[0] ),
					&g_mouseLeftClick[0],
					sizeof( INPUT )
					);
				assert( 2 == res );
			}
			else if( doubleClickVKey == vKey )
			{
				res = SendInput(      
					sizeof( g_mouseDoubleLeftClick ) / sizeof( g_mouseDoubleLeftClick[0] ),
					&g_mouseDoubleLeftClick[0],
					sizeof( INPUT )
					);
				assert( 4 == res );
			}
			else
			{
				OutputDebugString( _T("Unknown hotkey received\n") );
			}
        }
		else
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int) msg.wParam;
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DOUBLECLICKER3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DOUBLECLICKER3);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
	  return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Initialize program
	DCInit();

	// Register hot keys
	//
	RegisterHotKeys();

	// show the tray icon
	//
	NotificationIconMessage(hWnd, NIM_ADD);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		// remove the notification icon
		//
		NotificationIconMessage(hWnd, NIM_DELETE);

		PostQuitMessage(0);
		break;

	case MYTRAY_ICON_MSG:
		{
			switch (lParam)
			{
				case WM_LBUTTONDBLCLK:
					switch (wParam)
					{
						case IDI_AUTELIC:
							//ShowWindow(hWnd, SW_NORMAL);
							//SetForegroundWindow(hWnd);
							//SetFocus(hWnd);
							//return TRUE;
							break;
						default:
							break;
					}
					break;
				case WM_RBUTTONDOWN:
				case WM_CONTEXTMENU:
					ShowContextMenu(hWnd);
					break;

				default:
					break;
			}
		}
		break;


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
