///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// main driver
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-06-28
// UPDATED: 2013-11-27
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN             // exclude rarely-used stuff from Windows headers

#include <windows.h>
// #include <commctrl.h>                   // common controls
#include "Window.h"
//#include "DialogWindow.h"
#include "ControllerMain.h"
#include "ControllerGL.h"
#include "ViewGL.h"
#include "ModelGL.h"
#include "ControllerDW.h"
#include "ViewDW.h"
#include "resource.h"
#include "Log.h"


// function declarations
int mainMessageLoop(HACCEL hAccelTable=0);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdArgs, int cmdShow)
{
   // Win::logMode(Win::LOG_MODE_BOTH);
	Win::logMode(Win::LOG_MODE_DIALOG);

	int widthUnit(200);     // Set width DW and GL of windows
	int startHeigth(920);  
	int DWwidth(4*widthUnit);
	int GLwidth(5*widthUnit);

    wchar_t name[256];      // get app name from resource file
    ::LoadString(hInst, IDS_APP_NAME, name, 256);

    ModelGL modelGL;     // The OpenGL model window class
    Win::ViewGL viewGL;  // The OpenGL view window class
    Win::ViewDW viewDW;
    Win::ControllerMain mainCtrl(&modelGL, &viewDW);  
    Win::Window mainWin(hInst, name, 0, &mainCtrl, FALSE, 3);

    // add menu to window class
 //   mainWin.setMenuName(MAKEINTRESOURCE(IDR_MAIN_MENU));
    mainWin.setWidth(DWwidth + GLwidth);
    mainWin.setHeight(startHeigth);
//	mainWin.setWindowStyle(WS_POPUP);
	mainWin.setWindowStyleEx(WS_EX_WINDOWEDGE);
	mainWin.setBackground(BLACK_BRUSH); 
//   mainWin.setWindowStyleEx(WS_BORDER);

    // create a window and show
    if(mainWin.create()) Win::log("WinMain(HINSTANCE hInst,... ) Main window is created.");
    else Win::log("[ERROR] Failed to create main window.");

// will generate WM_SYSCOMMAND: message's
	HMENU  hMenu = GetSystemMenu(mainWin.getHandle(), FALSE);
	AppendMenu (hMenu, MF_SEPARATOR, 0, NULL) ;
	AppendMenu (hMenu, MF_STRING, ID_FILE_OPEN, TEXT("&File Open")) ; 
	AppendMenu (hMenu, MF_STRING, 5, TEXT (" About...")) ; 
	AppendMenu (hMenu, MF_STRING, 6, TEXT (" Help...")) ; 
	AppendMenu (hMenu, MF_STRING, 7, TEXT (" Remove")) ;

	//WM_SYSCOMMAND:

    // create model and view components for controller
    // create OpenGL rendering window as a child
    Win::ControllerGL glCtrl(&modelGL, &viewGL);
    Win::Window glWin(hInst, L"WindowGL", mainWin.getHandle(), &glCtrl, FALSE, 2);
    glWin.setClassStyle(CS_OWNDC);
    glWin.setWindowStyle(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
//	glWin.setPosition( DWwidth, 0);
//	glWin.setWidth(GLwidth);
	glWin.setPosition( DWwidth + GLwidth, 0);

    glWin.setWidth(0);
    glWin.setHeight(startHeigth);
	glWin.setBackground(BLACK_BRUSH);
    if(glWin.create()) Win::log("OpenGL child window is created.");
    else Win::log("[ERROR] Failed to create OpenGL window.");


 // ::SetWindowPos(glWin.getHandle(), 0, startWidth, 0, startWidth, startHeigth, SWP_NOZORDER);
    // create a child dialog box contains controls
    Win::ControllerDW dwCtrl(&modelGL, &viewDW);
    Win::Window dwWin(hInst, L"WindowDW", mainWin.getHandle(), &dwCtrl, FALSE, 1);

//    Win::DialogWindow glDialog(hInst, IDD_CONTROLS, mainWin.getHandle(), &dwCtrl);
    dwWin.setClassStyle(CS_OWNDC);
    dwWin.setWindowStyle(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
	dwWin.setBackground( BLACK_BRUSH );
	dwWin.setPosition( 0, 0);
//    dwWin.setWidth( DWwidth );

    dwWin.setWidth(DWwidth  + GLwidth);
    dwWin.setHeight(startHeigth);
    if(dwWin.create()) Win::log("DW child window is created.");
    else Win::log("[ERROR] Failed to create DW child window.");

    // create status bar window using CreateWindowEx()
    // mainWin must pass WM_SIZE message to the status bar
    // So, mainWin accesses the status bar with GetDlgItem(handle, IDC_STATUSBAR).
//    HWND statusHandle = ::CreateWindowEx(0, STATUSCLASSNAME, 0, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
//                         0, 0, 0, 0, mainWin.getHandle(), (HMENU)IDC_STATUSBAR, ::GetModuleHandle(0), 0);

//    if(statusHandle) Win::log("Status bar window is created.");
//    else  Win::log("[ERROR] Failed to create status bar window.");
//    ::SendMessage(statusHandle, SB_SETTEXT, 0, (LPARAM)L"Ready");

    // send window handles to mainCtrl, they are used for resizing window
    mainCtrl.setGLHandle(glWin.getHandle());
    mainCtrl.setDWHandle(dwWin.getHandle());
	RegisterTouchWindow(dwWin.getHandle(), 0);  // handle WM_TOUCH

    // place the DW window in right place left side of main window
//    ::SetWindowPos(dwWin.getHandle(), 0, 0, 0, DWwidth, startHeigth, SWP_NOZORDER);

    // compute height of all sub-windows
	
	RECT rect;
 //   ::GetWindowRect(statusHandle, &rect);           // get size of status bar
 //   height = rect.bottom - rect.top;
	rect.top = 0;
	rect.bottom = startHeigth;
	rect.left = 0;
	rect.right = DWwidth + GLwidth;
    // resize main window, so all sub windows are fit into the client area of main window
    DWORD style = ::GetWindowLong(mainWin.getHandle(), GWL_STYLE);      // get current window style
    DWORD styleEx = ::GetWindowLong(mainWin.getHandle(), GWL_EXSTYLE);  // get current extended window style
 
	::AdjustWindowRectEx(&rect, style, TRUE, styleEx);
    ::SetWindowPos(mainWin.getHandle(), 0, 0, 0, DWwidth + GLwidth, startHeigth, SWP_NOZORDER);
//::SetWindowPos( mainWin.getHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    // show all windows

// BOOL WINAPI SetWindowText( _In_     HWND    hWnd, _In_opt_ LPCTSTR lpString );
	SetWindowText( mainWin.getHandle(), TEXT("3D Book Reader"));

    glWin.show();
    dwWin.show();
    mainWin.show();
    //::SendMessage(mainWin.getHandle(), WM_NCPAINT, 1, 0);   // repaint window frame

    // main message loop //////////////////////////////////////////////////////
 //   int exitCode;
    HACCEL hAccelTable = 0;

	return mainMessageLoop(hAccelTable);

}

int mainMessageLoop(HACCEL hAccelTable)
{
	   Win::log(" mainMessageLoop");
 //   HWND activeHandle;
    MSG msg;

    while(::GetMessage(&msg, 0, 0, 0) > 0)  // loop until WM_QUIT(0) received
    {
        // determine the activated window is dialog box
        // skip if messages are for the dialog windows    
 /*       activeHandle = GetActiveWindow();
        if(::GetWindowLongPtr(activeHandle, GWL_EXSTYLE) & WS_EX_CONTROLPARENT)
			// WS_EX_CONTROLPARENT is automatically added by CreateDialogBox()
        {
            if(::IsDialogMessage(activeHandle, &msg)) continue;   // message handled, back to while-loop
        }
*/
        // now, handle window messages
        if(!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;                 // return nExitCode of PostQuitMessage()
}
