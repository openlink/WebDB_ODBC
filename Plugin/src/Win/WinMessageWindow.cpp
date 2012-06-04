/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Jan 5, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/
#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include "../sdk/npfunctions.h"
#include "../mthreads.h"

#include "WinMessageWindow.h"


HINSTANCE gInstance(NULL);

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (!gInstance)
        gInstance = hInstance;

    return TRUE;
}





WinMessageWindow::WinMessageWindow() {
    WNDCLASSEX wc;
    DWORD err(0);
    static ATOM clsAtom(NULL);

    wchar_t *wszWinName = L"NPODBCEventWindow";
    wchar_t *wszClassName = L"ODBCEventWindow";

    if (!clsAtom) {
        //Step 1: Registering the Window Class
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = 0;
        wc.lpfnWndProc   = &WinMessageWindow::_WinProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = gInstance;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = wszClassName;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hIconSm = NULL;
        wc.hbrBackground = NULL;
    
        if (!(clsAtom = ::RegisterClassEx(&wc))) {
            err = ::GetLastError();
//??            if (err != ERROR_CLASS_ALREADY_EXISTS) {
//??                throw std::runtime_error("Could not register window class");
//??            }
        }
    }
    // Step 2: Creating the Window
    HWND messageWin = CreateWindowEx(
        WS_OVERLAPPED,
        wszClassName,
        wszWinName,
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, gInstance, NULL);
    if (!messageWin) {
        err = ::GetLastError();
//????        throw std::runtime_error("Could not create Message Window");
    }
    m_hWnd = messageWin;
}

LRESULT CALLBACK WinMessageWindow::_WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if (uMsg == WM_ASYNCTHREADINVOKE) {
        AsyncFunctionCall *evt = static_cast<AsyncFunctionCall*>((void*)lParam);
        evt->func(evt->userData);
        delete evt;
        return S_OK;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND WinMessageWindow::getHWND()
{
    return m_hWnd;
}

WinMessageWindow::~WinMessageWindow()
{
    ::DestroyWindow(m_hWnd);
}

//bool WinMessageWindow::_scheduleAsyncCall(NPP _npp, void (*func)(void *), void *userData) const
bool WinMessageWindow::_scheduleAsyncCall(NPP _npp, void (*func)(void *), void *userData)
{
    return ::PostMessage(this->getHWND(), WM_ASYNCTHREADINVOKE, NULL, 
                  (LPARAM)new AsyncFunctionCall(_npp, func, userData)) ? true : false;
}
