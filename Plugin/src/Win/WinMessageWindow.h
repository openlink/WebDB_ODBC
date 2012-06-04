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
#pragma once
#ifndef WinMessageWindow_h__
#define WinMessageWindow_h__

#define WM_ASYNCTHREADINVOKE    WM_USER + 1

class WinMessageWindow //: boost::noncopyable
{
public:
    WinMessageWindow();
    ~WinMessageWindow();

    HWND getHWND();

//    bool _scheduleAsyncCall(NPP _npp, void (*func)(void *), void *userData) const;
    bool _scheduleAsyncCall(NPP _npp, void (*func)(void *), void *userData);

    static LRESULT CALLBACK _WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
    HWND m_hWnd;
};

#endif // WinMessageWindow_h__

