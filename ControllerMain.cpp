///////////////////////////////////////////////////////////////////////////////
// ControllerMain.cpp
// ==================
// Derived Controller class for main window 
// main has two child child windows the Direct Write window and the OpenGL window.
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>                   // common controls
#include <sstream>
#include "ControllerMain.h"
#include "resource.h"
#include "Log.h"
#define DEBUG_GB
using namespace Win;

// handle events(messages) on all child windows that belong to the parent window.
// For example, close all child windows when the parent got WM_CLOSE message.
// lParam can be used to specify a event or message.
bool CALLBACK enumerateChildren(HWND childHandle, LPARAM lParam);


ControllerMain::ControllerMain(ModelGL* model, ViewDW* view):	
model(model),
view(view),
glHandle(0), 
dwHandle(0), 
aspectRatio(0.45f){} 

extern int cursorState;

// Set the cursor for resize and parts main window.
int ControllerMain::cursor(bool menu, int Hit_Test_From_Windows){ // handle WM_SETCURSOR
#ifdef DEBUG_GB
	log(L"ControllerMain::cursor()");
#endif
if( Hit_Test_From_Windows == HTCLOSE || Hit_Test_From_Windows == HTTOP || // mouse is in main window title bar
			Hit_Test_From_Windows == HTMINBUTTON  || Hit_Test_From_Windows == HTMAXBUTTON  ||
			Hit_Test_From_Windows == HTSYSMENU  || Hit_Test_From_Windows == HTCAPTION  && cursorState != ARROW){
			cursorState = ARROW;
			SetCursor(LoadCursor(NULL, IDC_ARROW)); 
			return 0;}
		if( Hit_Test_From_Windows == HTRIGHT || Hit_Test_From_Windows == HTLEFT){
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			cursorState = 3;
			return TRUE; }
		else if( Hit_Test_From_Windows == HTBOTTOM ){ 
			SetCursor(LoadCursor(NULL, IDC_SIZENS)); 
			cursorState = 3;
			return TRUE; }
		else if( Hit_Test_From_Windows == HTBOTTOMRIGHT ){
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE)); 
			cursorState = 3;
			return TRUE; }
		else if( Hit_Test_From_Windows == HTBOTTOMLEFT ){
			SetCursor(LoadCursor(NULL, IDC_SIZENESW)); 
			cursorState = 3;
			return TRUE;}
	SetCursor(LoadCursor(NULL, IDC_ARROW)); 
	cursorState = ARROW;
	return 0;

}
int ControllerMain::keyDown(int key, LPARAM lParam){
	log(L"ControllerMain::keyDown( key = %i LOWORD = %i HIWORD = %i)", key, LOWORD(lParam),  HIWORD(lParam));

	if( HIWORD(lParam) == 28) // User hit Enter 
	SendMessage(dwHandle, WM_SYSKEYDOWN, 0, 0);

	else if(key == VK_DOWN){
		view->makePage( key, VK_DOWN );
		InvalidateRect( dwHandle, NULL, FALSE );
		}
	else if(key == VK_UP){
		view->makePage( key, VK_UP );
		InvalidateRect( dwHandle, NULL, FALSE ); 
		}
	else if(key == VK_HOME){
		view->makePage( key, VK_HOME );
		InvalidateRect( dwHandle, NULL, FALSE ); 
		}
	else if(key == VK_END){
		view->makePage( key, VK_END );
		InvalidateRect( dwHandle, NULL, FALSE ); 
		}

	return 0;
/*
VK_DOWN
	if(key == FIND_ID ){
		view->displayMenu( FIND_MENU, FIND_ID  );
		InvalidateRect( dwHandle, NULL, FALSE );
		return 0;}
	if( key == PAGE_ID ){
		view->displayMenu( PAGE_MENU, PAGE_ID );
		InvalidateRect( dwHandle, NULL, FALSE );
		return 0;}
	// Display page menu '?'
	else if( key == PAGE_MENU && charCount == 0 ){ // Enter was hit with no data
		view->displayMenu( REMOVE_MENU, NULL);   // so just remove the menu
		InvalidateRect( dwHandle, NULL, FALSE );
		return 0;}
	else if( key == PAGE_MENU || key == 1 ){	// User pressed enter or space - 'p'
		pageDigit[0] = 0; pageDigit[1] = 0; pageDigit[2] = 0; charCount = 0; }
	if( pageNumber < pageCount ){
		view->makePage( key, pageNumber + 1 );
		InvalidateRect( dwHandle, NULL, FALSE ); }
*/
	
}
int ControllerMain::getChar( WPARAM message,  int lowWord, int highWord ){
#ifdef DEBUG_GB
	log(L"ControllerMain::getChar(int message = %i  lowWord  = %i  highWord = %i)", message,  lowWord, highWord);
#endif	

	view->getChar(dwHandle, message,  lowWord, highWord);
	return 0;
}

int ControllerMain::command(int id, int cmd, LPARAM msg)
{ 
#ifdef DEBUG_GB
	log(L"ControllerMain::command");
#endif	
	static bool flag = false;
    switch(id)
    {
    case ID_FILE_OPEN:
        log(L"ControllerMain::ID_FILE_OPEN ID_FILE_OPEN  ID_FILE_OPEN");
		if( view->openDW_file(handle, 0) == TRUE ) {
			log(L"ControllerMain::ID_FILE_OPEN invalidate");
			InvalidateRect(dwHandle, NULL, FALSE );}
        break;
    case REOPEN_ID:
        log(L"ControllerMain::REOPEN_ID REOPEN_ID REOPEN_ID");
		if( view->openDW_file(handle, 1) == TRUE ) {
			log(L"ControllerMain::REOPEN_ID invalidate");
			InvalidateRect(dwHandle, NULL, FALSE );}
        break;
    case ID_FILE_EXIT:
        ::PostMessage(handle, WM_CLOSE, 0, 0);
        break;
    case ID_ANIMATE:
        Win::log("ControllerMain::command animate");
		     flag = !flag;
      //      model->animate(flag);
      //      view->animate(flag);
        break;
    case ID_FILL:
 //         model->setDrawMode(0);
       
       break;

    case ID_WIRE:

 //           model->setDrawMode(1);
   
        break;

    case ID_POINTS:
 
    //        model->setDrawMode(2);
     
        break;
    }

    return 0;
}

int ControllerMain::close()
{
// write page number to file on close

	view->WriteUTF16();
    ::EnumChildWindows(handle, (WNDENUMPROC)enumerateChildren, (LPARAM)WM_CLOSE);
    ::DestroyWindow(handle);    // close itself
    return 0;
}

int ControllerMain::destroy()
{
    ::PostQuitMessage(0);       // exit the message loop
    return 0;
}

int ControllerMain::create()
{
	log(L"ControllerMain::create()"); 
    return 0;
}

int ControllerMain::size(int routine, int h, WPARAM wParam){

	log(L"ControllerMain::size( routine = %i  h  = %i wParam = %i)",routine, h, wParam);

	if(  wParam == GL_MODE ){
		aspectRatio = 0.5f;
		model->hiFromDW( routine );
	}
	else if(  wParam == DW_MODE ){
		aspectRatio = 1.0f;
		model->hiFromDW( -1 );}

    RECT rect;

    // get client dimension of mainWin
    ::GetClientRect(handle, &rect);
    int mainClientWidth =   rect.right - rect.left;
    int mainClientHeight =    rect.bottom - rect.top;
//	aspectRatio = 1.0f;
	int DWwidth = aspectRatio*mainClientWidth;
	int GLWidth = mainClientWidth - DWwidth;
//	log(L"ControllerMain::size(mainClientWidth = %i )", mainClientWidth);

//	int DWwidth = mainClientWidth;
//	int GLWidth = 0.0;

/* 3 - 21 - 14  if you replace status bar uncomment this code
    HWND statusHandle = ::GetDlgItem(handle, IDC_STATUSBAR);
    ::GetWindowRect(statusHandle, &rect);
    int statusHeight = rect.bottom - rect.top;
*/
 
    // resize the height of glWin and reposition glDialog
 //   int glHeight = mainClientHeight; // 3-21-14 - statusHeight;
//    ::SetWindowPos(0, 0, DWwidth, 0, GLWidth, mainClientHeight, SWP_NOZORDER);
	::SetWindowPos(glHandle, 0, DWwidth, 0, GLWidth, mainClientHeight, SWP_NOZORDER);
 //   ::SetWindowPos(dwHandle, 0, 0, 0, mainClientWidth, mainClientHeight, SWP_NOZORDER);
   ::SetWindowPos(dwHandle, 0, 0, 0, DWwidth, mainClientHeight, SWP_NOZORDER);

//    ::SetWindowPos(glHandle, 0, 0, 0, 0, mainClientHeight, SWP_NOZORDER);
//    ::SetWindowPos(dwHandle, 0, 0, 0, mainClientWidth, mainClientHeight, SWP_NOZORDER);
 //   ::InvalidateRect(dwHandle, 0, TRUE);      // force to repaint


/* 3-21-14  uncomment if you replace staus bar in main window
    ::SendMessage(statusHandle, WM_SIZE, 0, 0); // automatically resize width, so send 0s
    ::InvalidateRect(statusHandle, 0, FALSE);   // force to repaint

    // display OpenGL window dimension on the status bar
    std::wstringstream wss;
    wss << "OpenGL Window Size: " << mainClientWidth << " x " << glHeight;
    ::SendMessage(statusHandle, SB_SETTEXT, 0, (LPARAM)wss.str().c_str());
*/
	
   return view->sendHandle( handle ); // This sends the mainController handle to viewDW 
										// which uses it a send a msg back 
										//SendMessage( mainHandle, WM_SIZE, GL_MODE,  glRoutineNumber );
}


bool CALLBACK enumerateChildren(HWND handle, LPARAM lParam)
{
    if(lParam == WM_CLOSE) ::SendMessage(handle, WM_CLOSE, 0, 0);      // close child windows
 
    return true;
}
