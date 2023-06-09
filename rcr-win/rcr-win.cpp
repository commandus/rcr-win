// rcr-win.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "rcr-win.h"
#include "commctrl.h"
#include "windowsx.h"

#include "string-helper.h"

#include <string>
#include <sstream>
#include <vector>

#define MAX_LOADSTRING 100

struct RCR_MAIN_WINDOW_STATE {
    HINSTANCE hinst;
    HWND hwParent;
    HWND hwTopPanel;
    HWND hwTreePanel;
    HWND hwCardPanel;

    HWND hwComponent;
    HWND hwSearch;
    HWND hwTVBox;

    int topHeight;
    int treeWidth;
    int componentWidth;
    int componentX;
    int componentY;
    int SearchHeight;
    int hwComponentHeight;

    std::vector <std::wstring> componentName;

} MW;

/*
BOOL InitTreeViewImageLists(HWND hwndTV)
{
    HIMAGELIST himl;  // handle to image list 
    HBITMAP hbmp;     // handle to bitmap 

    // Create the image list. 
    if ((himl = ImageList_Create(CX_BITMAP,
        CY_BITMAP,
        FALSE,
        NUM_BITMAPS, 0)) == NULL)
        return FALSE;

    // Add the open file, closed file, and document bitmaps. 
    hbmp = LoadBitmap(MW.hinst, MAKEINTRESOURCE(IDB_OPEN_FILE));
    g_nOpen = ImageList_Add(himl, hbmp, (HBITMAP) NULL);
    DeleteObject(hbmp);

    hbmp = LoadBitmap(MW.hinst, MAKEINTRESOURCE(IDB_CLOSED_FILE));
    g_nClosed = ImageList_Add(himl, hbmp, (HBITMAP) NULL);
    DeleteObject(hbmp);

    hbmp = LoadBitmap(MW.hinst, MAKEINTRESOURCE(IDB_DOCUMENT));
    g_nDocument = ImageList_Add(himl, hbmp, (HBITMAP) NULL);
    DeleteObject(hbmp);

    // Fail if not all of the images were added. 
    if (ImageList_GetImageCount(himl) < 3)
        return FALSE;

    // Associate the image list with the tree-view control. 
    TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL);

    return TRUE;
}
*/

HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, int nLevel)
{
    TVITEM tvi;
    TVINSERTSTRUCT tvins;
    static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
    static HTREEITEM hPrevRootItem = NULL;
    static HTREEITEM hPrevLev2Item = NULL;
    HTREEITEM hti;

    tvi.mask = TVIF_TEXT | TVIF_IMAGE
        | TVIF_SELECTEDIMAGE | TVIF_PARAM;

    // Set the text of the item. 
    tvi.pszText = lpszItem;
    tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

    // Assume the item is not a parent item, so give it a 
    // document image. 
    // 
    // 
    // TODO ---------------
    // tvi.iImage = g_nDocument;
    //tvi.iSelectedImage = g_nDocument;

    // Save the heading level in the item's application-defined 
    // data area. 
    tvi.lParam = (LPARAM)nLevel;
    tvins.item = tvi;
    tvins.hInsertAfter = hPrev;

    // Set the parent item based on the specified level. 
    if (nLevel == 1)
        tvins.hParent = TVI_ROOT;
    else if (nLevel == 2)
        tvins.hParent = hPrevRootItem;
    else
        tvins.hParent = hPrevLev2Item;

    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM,
        0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

    if (hPrev == NULL)
        return NULL;

    // Save the handle to the item. 
    if (nLevel == 1)
        hPrevRootItem = hPrev;
    else if (nLevel == 2)
        hPrevLev2Item = hPrev;

    // The new item is a child item. Give the parent item a 
    // closed folder bitmap to indicate it now has child items. 
    if (nLevel > 1)
    {
        hti = TreeView_GetParent(hwndTV, hPrev);
        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvi.hItem = hti;
//        tvi.iImage = g_nClosed;
//        tvi.iSelectedImage = g_nClosed;
        TreeView_SetItem(hwndTV, &tvi);
    }

    return hPrev;
}

HRESULT createMainWindow(HWND hwndParent)
{
    MW.hwParent = hwndParent;

    INITCOMMONCONTROLSEX icex;
    // Initialize common controls.
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    
    // Get the dimensions of the parent window's client area, and 
    // create a tab control child window of that size. Note that g_hInst
    // is the global instance handle.
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);
    MW.topHeight = 32;
    MW.treeWidth = 240;
    MW.componentWidth = 200;
    MW.componentX = 2;
    MW.componentY = 2;
    MW.SearchHeight = 22;
    MW.hwComponentHeight = 300;

    MW.hwTopPanel = CreateWindow(WC_STATIC, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, rcClient.right, MW.topHeight,
        MW.hwParent, nullptr, MW.hinst, nullptr);
    if (!MW.hwTopPanel)
        return S_FALSE;
    MW.hwTreePanel = CreateWindow(WC_STATIC, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, MW.topHeight, MW.treeWidth, rcClient.bottom - MW.topHeight,
        MW.hwParent, nullptr, MW.hinst, nullptr);
    if (!MW.hwTreePanel)
        return S_FALSE;
    MW.hwCardPanel = CreateWindow(WC_STATIC, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        MW.treeWidth, MW.topHeight, rcClient.right - MW.treeWidth, rcClient.bottom - MW.topHeight,
        MW.hwParent, nullptr, MW.hinst, nullptr);
    if (!MW.hwTreePanel)
        return S_FALSE;

    MW.hwComponent = CreateWindow(WC_COMBOBOX, TEXT("Component"),
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        MW.componentX, MW.componentY, MW.componentWidth, MW.hwComponentHeight, MW.hwTopPanel, nullptr, MW.hinst,
        nullptr);

    MW.hwSearch = CreateWindow(WC_EDIT, TEXT("Search"),
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        MW.componentX + MW.componentWidth + 2, MW.componentY, 
        rcClient.right - MW.componentWidth - MW.componentX - 4, MW.SearchHeight, MW.hwTopPanel, nullptr, MW.hinst,
        nullptr);

    MW.hwTVBox = CreateWindowEx(0, WC_TREEVIEW, TEXT("Boxes"),
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
        0, 0, MW.treeWidth, rcClient.bottom - MW.topHeight,
        MW.hwTreePanel, (HMENU) nullptr, MW.hinst, nullptr);
    // if (!InitTreeViewImageLists(MW.hwTVBox) || !InitTreeViewItems(MW.hwTVBox))
    //    return S_FALSE;
    // SendMessage(MW.hwComponent, CB_SHOWDROPDOWN, (WPARAM) TRUE, (LPARAM)0);
    
    return S_OK;
}


// Handles the WM_SIZE message for the main window by resizing the 
//   tab control. 
// hwndTab - handle of the tab control.
// lParam - the lParam parameter of the WM_SIZE message.
//
HRESULT OnSize(HWND hwnd, LPARAM lParam)
{
    if (!hwnd)
        return E_INVALIDARG;

    // Resize the tab control to fit the client are of main window.

    int w = GET_X_LPARAM(lParam);
    int h = GET_Y_LPARAM(lParam);

    if (!SetWindowPos(MW.hwTopPanel, HWND_TOP, 0, 0, w, MW.topHeight, SWP_SHOWWINDOW))
        return E_FAIL;
    RECT rc;
    if (!SetWindowPos(MW.hwTreePanel, HWND_TOP, 0, MW.topHeight, MW.treeWidth, h - MW.topHeight, SWP_SHOWWINDOW))
        return E_FAIL;
    if (!SetWindowPos(MW.hwCardPanel, HWND_TOP, MW.treeWidth, MW.topHeight, w - MW.treeWidth, h - MW.topHeight, SWP_SHOWWINDOW))
        return E_FAIL;
    if (!SetWindowPos(MW.hwComponent, HWND_TOP, MW.componentX, MW.componentY, 200, MW.hwComponentHeight, SWP_SHOWWINDOW))
        return E_FAIL;
    if (!SetWindowPos(MW.hwSearch, HWND_TOP, MW.componentX + MW.componentWidth + 2, MW.componentY, 
        w - MW.componentWidth - MW.componentX - 4, MW.SearchHeight, SWP_SHOWWINDOW))
        return E_FAIL;
    
    if (!SetWindowPos(MW.hwTVBox, HWND_TOP, 0, 0, MW.treeWidth, h - MW.topHeight, SWP_SHOWWINDOW))
        return E_FAIL;
    
    return S_OK;
}

HRESULT fillData()
{
    for (int i = 0; i < 22; i++) {
        std::stringstream ss;
        ss << "component " << i;
        MW.componentName.push_back(utf82wstring(ss.str()));

        // Add string to combobox.
        SendMessage(MW.hwComponent, (UINT) CB_ADDSTRING, (WPARAM)0, (LPARAM)MW.componentName[i].c_str());
    }

    // Send the CB_SETCURSEL message to display an initial item 
    //  in the selection field  
    SendMessage(MW.hwComponent, CB_SETCURSEL, (WPARAM) 2, (LPARAM) 4);

    wchar_t s[] = L"root";
    HTREEITEM hti = AddItemToTree(MW.hwTVBox, (LPTSTR) &s, 0);
    hti = AddItemToTree(MW.hwTVBox, (LPTSTR)&s, 1);
    hti = AddItemToTree(MW.hwTVBox, (LPTSTR)&s, 1);

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


    MW.hinst = hInstance;

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

   createMainWindow(hWnd);
   fillData();


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
            int wId = HIWORD(wParam);
            if (wId == CBN_SELCHANGE) {
                int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                TCHAR  ListItem[256];
                (TCHAR) SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
                MessageBox(MW.hwComponent, (LPCWSTR)ListItem, TEXT("Item Selected"), MB_OK);
                break;
            }
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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SETFOCUS:
        SetFocus(MW.hwSearch);
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
