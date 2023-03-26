// rcr-win.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "rcr-win.h"
#include "commctrl.h"
#include "windowsx.h"

#define MAX_LOADSTRING 100

struct {
    HINSTANCE g_hInst;
    HWND g_hWndTab;
    HWND g_hwndEdit;
    HWND g_hwndStatic;
} RCR_MAIN_WINDOW;

HRESULT doCreateMainWindow(HWND hwndParent)
{
    RECT rcClient;
    INITCOMMONCONTROLSEX icex;
    HWND hwndTab;
    TCITEM tie;
    int i;
    TCHAR achTemp[256];  // Temporary buffer for strings.

    // Initialize common controls.
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    // Get the dimensions of the parent window's client area, and 
    // create a tab control child window of that size. Note that g_hInst
    // is the global instance handle.
    GetClientRect(hwndParent, &rcClient);
    RCR_MAIN_WINDOW.g_hWndTab = CreateWindow(WC_TABCONTROL, L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, rcClient.right, rcClient.bottom,
        hwndParent, NULL, RCR_MAIN_WINDOW.g_hInst, NULL);
    if (RCR_MAIN_WINDOW.g_hWndTab == NULL)
        return S_FALSE;

    // Add tabs for each day of the week. 
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;
    tie.pszText = achTemp;

    for (i = 0; i < 7; i++) {
        // Load the day string from the string resources. Note that
        // g_hInst is the global instance handle.
        lstrcpyW(&achTemp[0], L"11");
        if (TabCtrl_InsertItem(RCR_MAIN_WINDOW.g_hWndTab, i, &tie) == -1)
        {
            DestroyWindow(RCR_MAIN_WINDOW.g_hWndTab);
            return NULL;
        }
    }

    // edit
    RCR_MAIN_WINDOW.g_hwndEdit = CreateWindowEx(
        0, L"EDIT",   // predefined class 
        NULL,         // no window title 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT ,
        0, 0, 0, 0,   // set size in WM_SIZE message 
        RCR_MAIN_WINDOW.g_hWndTab,         // parent window 
        (HMENU) NULL,   // edit control ID 
        (HINSTANCE)GetWindowLongPtr(RCR_MAIN_WINDOW.g_hWndTab, GWLP_HINSTANCE),
        NULL);        // pointer not needed 

    TCHAR lpszLatin[] = L"Lorem ipsum dolor sit amet, consectetur ";
    // Add text to the window. 
    SendMessage(RCR_MAIN_WINDOW.g_hwndEdit, WM_SETTEXT, 0, (LPARAM)lpszLatin);


    // static
    RCR_MAIN_WINDOW.g_hwndStatic = CreateWindow(WC_STATIC, L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
        100, 100, 100, 100,
        RCR_MAIN_WINDOW.g_hWndTab, NULL, RCR_MAIN_WINDOW.g_hInst, NULL);
    return S_OK;
}


// Handles the WM_SIZE message for the main window by resizing the 
//   tab control. 
// hwndTab - handle of the tab control.
// lParam - the lParam parameter of the WM_SIZE message.
//
HRESULT OnSize(HWND hwnd, LPARAM lParam)
{
    RECT rc;

    if (hwnd == NULL)
        return E_INVALIDARG;

    // Resize the tab control to fit the client are of main window.
    if (!SetWindowPos(RCR_MAIN_WINDOW.g_hWndTab, HWND_TOP, 0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), SWP_SHOWWINDOW))
        return E_FAIL;

    MoveWindow(RCR_MAIN_WINDOW.g_hwndEdit,
        0, 30,                  // starting x- and y-coordinates 
        LOWORD(lParam),        // width of client area 
        40,        // height of client area 
        TRUE);

    return S_OK;
}

// Handles notifications from the tab control, as follows: 
//   TCN_SELCHANGING - always returns FALSE to allow the user to select a 
//     different tab.  
//   TCN_SELCHANGE - loads a string resource and displays it in a static 
//     control on the selected tab.
// hwndTab - handle of the tab control.
// hwndDisplay - handle of the static control. 
// lParam - the lParam parameter of the WM_NOTIFY message.
//
BOOL OnNotify(HWND hwndTab, HWND hwndDisplay, LPARAM lParam)
{
    TCHAR achTemp[256]; // temporary buffer for strings

    switch (((LPNMHDR)lParam)->code)
    {
    case TCN_SELCHANGING:
    {
        // Return FALSE to allow the selection to change.
        return FALSE;
    }

    case TCN_SELCHANGE:
    {
        int iPage = TabCtrl_GetCurSel(hwndTab);

        // Note that g_hInst is the global instance handle.
        lstrcpyW(&achTemp[0], L"--");
        LRESULT result = SendMessage(hwndDisplay, WM_SETTEXT, 0,
            (LPARAM)achTemp);
        break;
    }
    }
    return TRUE;
}


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    RCR_MAIN_WINDOW.g_hInst = hInstance;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RCRWIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RCRWIN));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RCRWIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RCRWIN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   doCreateMainWindow(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
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
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        OnSize(hWnd, lParam);
        break;
    case WM_NOTIFY:
        OnNotify(RCR_MAIN_WINDOW.g_hWndTab, RCR_MAIN_WINDOW.g_hwndStatic, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SETFOCUS:
        SetFocus(RCR_MAIN_WINDOW.g_hwndEdit);
        return 0;
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
