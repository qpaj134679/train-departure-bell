// bell-windows.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "bell-windows.h"
#include "eki.h"
#include <memory>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <windowsx.h>
#include <vector>
#include <string>

#include "utils.h"
#include "STM_IO.h"


#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//HWND hWndWindow;
HWND hLineCmb, hStationCmb, hOKBtn;          // hwnd - btn and combox
EKI* global_eki = nullptr;                      // eki class
STM_IO* global_io = nullptr;                    // stmIO class

std::vector<std::wstring> vNowLines;           // save all lines
std::vector<std::wstring> vNowStation;         // save the station showed on the window

// Forward declarations of functions included in this code module:
ATOM                RegisterWindowClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL                InitClassInst();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BELLWINDOWS, szWindowClass, MAX_LOADSTRING);
    RegisterWindowClass(hInstance);

    if (!InitClassInst())
        ExitProcess(1);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BELLWINDOWS));


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
//  FUNCTION: RegisterWindowClass()
//
//  PURPOSE: Registers the window class.
//
ATOM RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BELLWINDOWS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BELLWINDOWS);
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

   HWND hWndWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 300, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWndWindow)
   {
      return FALSE;
   }

   ShowWindow(hWndWindow, nCmdShow);
   UpdateWindow(hWndWindow);

   global_io->SetWindowHWND(hWndWindow);

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
    case WM_CREATE:
        {
            // create combobox...etc
            HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            hLineCmb = CreateWindow(WC_COMBOBOX, L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 20, 10, 120, 100, hWnd, (HMENU)1, hInst, NULL);
            hStationCmb = CreateWindow(WC_COMBOBOX, L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 160, 10, 120, 100, hWnd, (HMENU)1, hInst, NULL);
            hOKBtn = CreateWindowW(L"BUTTON", L"Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 80, 50, 100, 30, hWnd, (HMENU)3, hInst, NULL);

            SendMessage(hLineCmb, WM_SETFONT, (WPARAM)hSysFont, TRUE);
            SendMessage(hStationCmb, WM_SETFONT, (WPARAM)hSysFont, TRUE);
            SendMessage(hOKBtn, WM_SETFONT, (WPARAM)hSysFont, TRUE);

            vNowLines = global_eki->get_lines();
            for (std::wstring l : vNowLines) {
                SendMessage(hLineCmb, CB_ADDSTRING, 0, (LPARAM)l.c_str());
            }
        }
    case WM_COMMAND:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                // combobox change lines
                if ((HWND)lParam == hLineCmb) {
                    int iIndex = ComboBox_GetCurSel(hLineCmb);
                    if (iIndex == CB_ERR)
                        return 1;
                    vNowStation.clear();
                    ComboBox_ResetContent(hStationCmb);
                    vNowStation = global_eki->get_line_eki(vNowLines[iIndex]);
                    for (std::wstring l : vNowStation) {
                        SendMessage(hStationCmb, CB_ADDSTRING, 0, (LPARAM)l.c_str());
                    }
                }
            }
            if (HIWORD(wParam) == BN_CLICKED) {
                if ((HWND)lParam == hOKBtn) {
                    int iIndex_line = ComboBox_GetCurSel(hLineCmb);
                    int iIndex_station = ComboBox_GetCurSel(hStationCmb);
                    if (iIndex_line == CB_ERR || iIndex_station == CB_ERR)
                        return 1;

                    std::wstring sPath = global_eki->GetSound(vNowLines[iIndex_line], vNowStation[iIndex_station]);
                    if (global_io->btnClick(sPath))
                        Button_SetText(hOKBtn, TEXT("stop"));
                    else
                        Button_SetText(hOKBtn, TEXT("start"));

                }
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            std::wstring sText = L"connecting";
            if (global_io->GetStatus()) {
                sText = L"connected";
            }

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            const wchar_t* text = sText.c_str();
            SIZE textSize;
            GetTextExtentPoint32(hdc, text, lstrlen(text), &textSize);
            int xText = rcClient.right - textSize.cx - 20;
            int yText = rcClient.bottom - textSize.cy - 20;

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, xText, yText, text, lstrlen(text));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL InitClassInst()
{
    try
    {
        global_eki = new EKI(PATH);
        global_io = new STM_IO();
    }
    catch (const std::exception& e)
    {
        MessageBoxW(NULL, std::wstring(e.what(), e.what() + strlen(e.what())).c_str(), L"Error", MB_ICONERROR | MB_OK);
        utils::OutputLog(e.what());
        return 0;
    }
    return 1;
}
