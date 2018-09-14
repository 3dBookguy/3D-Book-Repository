///////////////////////////////////////////////////////////////////////////////
// procedure.cpp
// =============
// Window procedure and dialog procedure callback functions.
// Windows will call this function whenever an event is triggered. It routes
// the message to the controller associated with window handle.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-06-24
// UPDATED: 2012-08-27
///////////////////////////////////////////////////////////////////////////////

#include "procedure.h"
#include "Controller.h"
#include "ControllerDW.h"
#include "Log.h"


bool bOGLmode = TRUE;
int cursorState = ARROW;
extern int graphicWindow;
extern int mouseInGraphicWindow;

///////////////////////////////////////////////////////////////////////////////
// Window Procedure
///////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK Win::windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT returnValue = 0;        // return value

    // find controller associated with window handle ; set by Window::create()
	// in ::CreateWindowEx

    static Win::Controller *ctrl;
    ctrl = (Controller*)::GetWindowLongPtr(hwnd, GWL_USERDATA);

    if(msg == WM_NCCREATE)  // Non-Client Create
    {
				 Win::log("in winProc rcvd WM_NCCREATE");
        // WM_NCCREATE message is called before non-client parts(border,
        // titlebar, menu,etc) are created. This message comes with a pointer
        // to CREATESTRUCT in lParam. The lpCreateParams member of CREATESTRUCT
        // actually contains the value of lpParam of CreateWindowEX().
        // First, retrieve the pointer to the controller specified when
        // Win::Window is setup.
        ctrl = (Controller*)(((CREATESTRUCT*)lParam)->lpCreateParams);
        ctrl->setHandle(hwnd);

        // Second, store the pointer to the Controller into GWL_USERDATA,
        // so, other messege can be routed to the associated Controller.
        ::SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)ctrl);

        return ::DefWindowProc(hwnd, msg, wParam, lParam);
    }

    // check NULL pointer, because GWL_USERDATA is initially 0, and
    // we store a valid pointer value when WM_NCCREATE is called.
    if(!ctrl)
        return ::DefWindowProc(hwnd, msg, wParam, lParam);

    // route messages to the associated controller
    switch(msg)
    {
    case WM_CREATE:
				 Win::log("in winProc rcvd WM_CREATE");
		ctrl->create();
 //       returnValue = ctrl->createThread();
		
        break;

    case WM_SIZE:
	//  Windows will send this when we resize, max or min the window.
	//	ViewDW will send this when changing the graphics mode or the OGL routine.
	//  The graphics mode will be in wParam GL_MODE = 40007, DW_MODE = 40012
	//  The OGL routine number will be in LOWORD(lParam)
		if((int)wParam == GL_MODE) bOGLmode = TRUE;
		if((int)wParam == DW_MODE) bOGLmode = FALSE;
        returnValue = ctrl->size(LOWORD(lParam), HIWORD(lParam), (int)wParam);   
		Win::log("in winProc rcvd WM_SIZE %i  %i  %i", LOWORD(lParam), HIWORD(lParam), (int)wParam);
		if(bOGLmode) Win::log("We are in GL_MODE  We are in GL_MODE  We are in GL_MODE)");
		// width, height, type
        break;

    case WM_ENABLE:
        {
        bool flag;
        if(wParam) flag = true;
        else flag = false;
        returnValue = ctrl->enable(flag);  // TRUE or FALSE
        break;
        }

    case WM_PAINT:
	//	Win::log("win WM_PAINT");			

        ctrl->paint();
        returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
        break;

//	case WM_SETCURSOR:  //Sent by Windows for left and right button up and down, mouse move CO 2-1-17
	    //	LOWORD(lParam) and HIWORD(lParam) are not mouse coordinates, but indicate areas
		//  such as window borders, title bar, client area, etc.
		// Win::log("winProc WM_SETCURSOR: LOWORD(lParam) = %i HIWORD(lParam) = %i");
		//	,LOWORD(lParam),HIWORD(lParam));

/*
		if( LOWORD(lParam) == HTCLOSE || LOWORD(lParam) == HTTOP || // mouse is in main window title bar
			LOWORD(lParam) == HTMINBUTTON  || LOWORD(lParam) == HTMAXBUTTON  ||
			LOWORD(lParam) == HTSYSMENU  || LOWORD(lParam) == HTCAPTION  && cursorState != ARROW){
			cursorState = ARROW;
			SetCursor(LoadCursor(NULL, IDC_ARROW)); 
			return 0;}
		if( LOWORD(lParam) == HTRIGHT || LOWORD(lParam) == HTLEFT){
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			cursorState = 3;
			return TRUE; }
		else if( LOWORD(lParam) == HTBOTTOM ){ 
			SetCursor(LoadCursor(NULL, IDC_SIZENS)); 
			cursorState = 3;
			return TRUE; }
		else if( LOWORD(lParam) == HTBOTTOMRIGHT ){
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE)); 
			cursorState = 3;
			return TRUE; }
		else if( LOWORD(lParam) == HTBOTTOMLEFT ){
			SetCursor(LoadCursor(NULL, IDC_SIZENESW)); 
			cursorState = 3;
			return TRUE;}  */

	//returnValue =  ctrl->cursor( FALSE, int(LOWORD(lParam )));   CO 2-1-17
	//	break; CO 2-1-17

    case WM_COMMAND:  // Not sure when we get this
		Win::log("winProc WM_COMMAND");
        returnValue = ctrl->command(LOWORD(wParam), HIWORD(wParam), lParam);   // id, code, msg
        break;

    case WM_MOUSEMOVE:  // ctrl->mouseMove in ControllerDW and  ControllerGL
						// Sets Cursor to IDC_IBeam in DW window or IDC_ARROW in menu and cursorState 
						// Sets Cursor to IDC_IBeam in GL window and cursorState
						// returns menuItem

		   returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
	 break;
/*  These lines CO 2-1-17  moved to ControllerDW.cpp
		if( HIWORD(lParam) <= MENU_HEIGHT ){ // SetCursor IDC_ARRO
			bMouseInMenu = TRUE;
			mouseInGraphicWindow = FALSE;
			if(cursorState != ARROW){
				ctrl->cursor( bMouseInMenu,  LOWORD(lParam));
				Win::log("winProc case WM_MOUSEMOVE: cursorState != ARROW");}
		}

			else{
			Win::log("winProccase WM_MOUSEMOVE: cursorState == ARROW");
			bMouseInMenu = FALSE;  // SetCursor IDC_IBEAM
			ctrl->cursor( bMouseInMenu,  LOWORD(lParam));
		}				
       if(mouseInGraphicWindow){
		   returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), 0);}

		if( !bpPage ){  // if( bPage ) skip  
			if( bpFind &&  HIWORD(lParam) <= MENU_HEIGHT )  // In Find Sub-Menu.
				returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), FIND_MENU);
			else if( bpColor )  // In Color Sub-Menu.
				returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), RGB_MENU);
			else if( bpFile && HIWORD(lParam) <= MENU_HEIGHT )  // In File Sub-Menu.
				returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), FILE_MENU);
			else if( !bpMenu  && HIWORD(lParam) <= MENU_HEIGHT ){  //  Display Main Menu.
				bpMenuDisplayed = TRUE;
				returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), MAIN_MENU);
			}                                       //  Remove Main Menu.       
			else if( !bpMenu  && HIWORD(lParam) > MENU_HEIGHT && bpMenuDisplayed == TRUE){ 
				bpMenuDisplayed = FALSE;
				returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), MAIN_MENU);
			}
		}
*/
		//returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam); 
  //      break;

    case WM_CLOSE:
        returnValue = ctrl->close();
        break;

    case WM_DESTROY:
        returnValue = ctrl->destroy();
        break;

    case WM_SYSCOMMAND:
		if(LOWORD (wParam) == ID_FILE_OPEN){ Win::log("ID_FILE_OPEN ID_FILE_OPEN");
			returnValue = ctrl->command(LOWORD(wParam), HIWORD(wParam), lParam);}
        else returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    case 0x0240:   //   case WM_TOUCH:
		Win::log("case WM_TOUCH:  0x0240:");
		returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
        break;

    case WM_CHAR:	//  keyboard input goes ControllerMain but we want to handle this msg in ControllerDW
					//  so we SendMessage( dwHandle, WM_CHAR, GL_MODE,  menuItem );
					//  from ControllerMain::getChar(wParam, PAGE_ID, LOWORD(lParam), HIWORD(lParam));		
					//	and then it get ControllerDW::getChar(wParam ....
		Win::log("winProc( .. case WM_CHAR:");
		ctrl->getChar(wParam,  LOWORD(lParam), HIWORD(lParam));

//ViewDW::getCharGL( WPARAM findChar ){
	case WM_KEYDOWN:{  // these go to ControllerMain 
		Win::log("winProc( .. case WM_KEYDOWN:");
		switch( wParam ){
			case VK_RETURN:	
				returnValue = ctrl->keyDown( NEXT_ID, lParam);		
				break;
				//if( bpMenu ){  // User has hit Enter: process requested menu item and								//set menu inactive: bpMenu = FALSE;
				//	bpMenu = FALSE;
				//	if( bpPage ){   //  Process page request
				//		returnValue = ctrl->keyDown( PAGE_MENU, lParam );
				//		bpPage = FALSE;
				//	}
				//	else if( bpFind ){  //  Process find request - Remove find menu
				//		returnValue = ctrl->keyDown( PAGE_MENU, lParam );
				//		bpFind = FALSE;}
				//	else if( bpColor ){  //  Process color request - Remove find menu
				//		returnValue = ctrl->keyDown( RGB_MENU, lParam );
				//		bpColor = FALSE;}
				//}  // if( bpMenu )
				//else  returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
				//break;
			case VK_DOWN:
				returnValue = ctrl->keyDown( VK_DOWN, lParam);
				Win::log("winProc( .. case VK_DOWN:");
				break;
			case VK_UP:
				returnValue = ctrl->keyDown( VK_UP, lParam);
				break;				
			case VK_PRIOR:
				returnValue = ctrl->keyDown( VK_PRIOR, lParam);
				break;
			case VK_NEXT:
				returnValue = ctrl->keyDown( VK_NEXT, lParam);
				Win::log("winProc( .. case WM_VK_NEXT:");
				break;
			case VK_END:
				returnValue = ctrl->keyDown( VK_END, lParam);
				break;
			case VK_HOME:
				returnValue = ctrl->keyDown( VK_HOME, lParam);
				break;
			case 67:	// c for Color
				if( GetKeyState(VK_SPACE) & 0x8000 ) {  // Display color pallete
				returnValue = ctrl->keyDown( COLOR_ID, lParam);
				}
				break;
			case 70:	// f for file dialog
				if( GetKeyState(VK_SPACE) & 0x8000 ){  // Display the open file dialog
				returnValue = ctrl->command( ID_FILE_OPEN, HIWORD(wParam), lParam);
				}
				break;
			case 72:	// h for Help
				if( GetKeyState(VK_SPACE) & 0x8000 ) {  // Display the help menu
				returnValue = ctrl->keyDown( HELP_ID, lParam);			
				}
				break;
			case 73:	// i for Index
				if( GetKeyState(VK_SPACE) & 0x8000 ) {  // Display the index page
				returnValue = ctrl->keyDown( INDEX_ID, lParam);
				}
				break;
			case 80:	// p for Page
				//if(( GetKeyState(VK_SPACE) & 0x8000 ) && !bpMenu){  // Activate the page menu
				//returnValue = ctrl->keyDown( PAGE_ID, lParam);			
				//bpMenu = TRUE;
				//bpPage = TRUE;
				//}
				break;
			case 84:	// t for Table of Contents
				if( GetKeyState(VK_SPACE) & 0x8000 ) {  // Display the table of contents
				returnValue = ctrl->keyDown( TOC_ID, lParam);
				}
				break;
			case 87:	// w for find
				//if( GetKeyState(VK_SPACE) & 0x8000 && !bpFind) {  // Display the find menu
				//returnValue = ctrl->keyDown( FIND_ID, lParam);
				//bpMenu = TRUE;
				//bpFind = TRUE;
				//bpFirst = TRUE; // flag for WM_CHAR so it doesn't send shift + w
				//}
				break;
			} // switch( wParam ) end
	} // case WM_KEYDOWN:{ end
			break;
    case WM_SYSKEYDOWN:  // ControllerMain sends this to ControllerDW when user hit Enter
		Win::log("winProc( .. case WM_SYSKEYDOWN:");
        returnValue = ctrl->keyDown(28, lParam);  // keyCode, 
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        returnValue = ctrl->keyUp((int)wParam, lParam);  // keyCode, keyDetail
        break;

    case WM_LBUTTONDOWN:
		returnValue = ctrl->lButtonDown( wParam, LOWORD(lParam), HIWORD(lParam));
        break;
/* July17
		if(  HIWORD(lParam) > MENU_HEIGHT && bpFind ){ // Check for mouse move
//			ctrl->lButtonDown( wParam, FIND_MENU, GET_TEXT);
			ctrl->lButtonDown( HIWORD(lParam), FIND_MENU,  LOWORD(lParam));

		} // 
		else if(  HIWORD(lParam) > MENU_HEIGHT && graphicWindow ){ 
			//Win::log("WM_LBUTTONDOWN: mouseInGraphicWindow = RIGHT or LEFT");
			//Win::log("WM_LBUTTONDOWN: wparam = %i", wParam);
			ctrl->cursor( bMouseInMenu,  LOWORD(lParam));
			ctrl->animateControl( wParam, LOWORD(lParam), HIWORD(lParam));
		} // 							

		if(  HIWORD(lParam) < MENU_HEIGHT && !bpPage ){ // if mouse is not in menu box and ignore
														// if bpPage ignore
			if( !bpMenu ){  // You're in main menu // returnValue = menuItem 
				returnValue = 
					ctrl->mouseMove( wParam, LOWORD(lParam), HIWORD(lParam), MAIN_MENU );
				if( returnValue == FILE_ID ){
						//ctrl->command( ID_FILE_OPEN, HIWORD(wParam), lParam );
						bpMenu = TRUE;
						bpFile = TRUE; }
				else if( returnValue == PAGE_ID ){ // this is the Page menu item
						bpMenu = TRUE;
						bpPage = TRUE; }
				else if( returnValue == FIND_ID ){ // this is the Find menu item
						bpMenu = TRUE;
						bpFind = TRUE; }
				else if( returnValue == COLOR_ID ){ // this is the Color menu item
						bpMenu = TRUE;
						bpColor = TRUE; }
				ctrl->lButtonDown( wParam, MAIN_MENU, returnValue );
			}  // Handle Sub Menus 
			else if( bpFile ){// handle File stuf here
				returnValue = 
					ctrl->mouseMove( wParam, LOWORD(lParam), HIWORD(lParam), FILE_MENU);
				if( returnValue == FILE_CANCEL_ID ){
					returnValue = ctrl->lButtonDown( wParam, FILE_MENU, FILE_CANCEL_ID);
					bpFile = FALSE;
					bpMenu = FALSE;}
				else if( returnValue == OPEN_ID ){
					bpFile = FALSE;
					bpMenu = FALSE;
					ctrl->command( ID_FILE_OPEN, HIWORD(wParam), lParam );}
				else if( returnValue == REOPEN_ID ){
					bpFile = FALSE;
					bpMenu = FALSE;
					returnValue = ctrl->lButtonDown( wParam, FILE_MENU, FILE_CANCEL_ID);
					ctrl->command( REOPEN_ID, HIWORD(wParam), lParam );
				}
				else ctrl->lButtonDown( wParam, FILE_MENU, returnValue);  // REOPEN_ID, OPEN_ID
			}
			else if( bpFind ){// handle Find stuf here
				returnValue = 
					ctrl->mouseMove( wParam, LOWORD(lParam), HIWORD(lParam), FIND_MENU);
				if( returnValue == CANCEL_ID ){
					returnValue = ctrl->lButtonDown( wParam, FIND_MENU, CANCEL_ID );
					bpFind = FALSE;
					bpMenu = FALSE;}
				if( returnValue == UP_ID ){
					returnValue = ctrl->lButtonDown( wParam, FIND_MENU, FIND_PREVIOUS );}
				if( returnValue == DOWN_ID ){
					returnValue = ctrl->lButtonDown( wParam, FIND_MENU, FIND_NEXT );}

			}
			else if( bpColor ){// handle Color stuf here
				returnValue = 
					ctrl->mouseMove( wParam, LOWORD(lParam), HIWORD(lParam), RGB_MENU);
				if( returnValue == CANCEL_ID ){
					returnValue = ctrl->lButtonDown( wParam, RGB_MENU, CANCEL_ID );
					bpColor = FALSE;
					bpMenu = FALSE;}

				else ctrl->lButtonDown( wParam, RGB_MENU, returnValue);  // RED_ID, GREEN_ID, 

//				ctrl->lButtonDown( wParam, RGB_MENU, returnValue );

			} // else if( bpColor )
		} // if(  HIWORD(lParam) < MENU_HEIGHT && !bpPage ){
		returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
        break;

    case WM_LBUTTONUP:
			if(  HIWORD(lParam) > MENU_HEIGHT && graphicWindow ){ 
			//Win::log("WM_LBUTTONUP: mouseInGraphicWindow = RIGHT or LEFT ");
			//Win::log("WM_LBUTTONUP:  wparam = %i", wParam);
			ctrl->cursor( bMouseInMenu,  LOWORD(lParam));
			ctrl->animateControl( wParam, LOWORD(lParam), HIWORD(lParam));}

        returnValue = ctrl->lButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
        break;
    case WM_RBUTTONDOWN:
			if(  HIWORD(lParam) > MENU_HEIGHT && graphicWindow ){ 
			//Win::log("WM_LBUTTONDOWN: mouseInGraphicWindow = RIGHT or LEFT");
			//Win::log("WM_LBUTTONDOWN: wparam = %i", wParam);
			ctrl->cursor( bMouseInMenu,  LOWORD(lParam));
			ctrl->animateControl( wParam, LOWORD(lParam), HIWORD(lParam));
		} // 							

        returnValue = ctrl->rButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
        break;
 */
    case WM_RBUTTONUP:
					if(  HIWORD(lParam) > MENU_HEIGHT && graphicWindow ){ 
			//Win::log("WM_LBUTTONDOWN: mouseInGraphicWindow = RIGHT or LEFT");
			//Win::log("WM_LBUTTONDOWN: wparam = %i", wParam);
//			ctrl->cursor( bMouseInMenu,  LOWORD(lParam));
			ctrl->animateControl( 3, LOWORD(lParam), HIWORD(lParam));
		} // 							

        returnValue = ctrl->rButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
        break;

    case WM_MBUTTONDOWN:
        returnValue = ctrl->mButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
        break;

    case WM_MBUTTONUP:
        returnValue = ctrl->mButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
        break;

    case WM_MOUSEWHEEL:
        returnValue = ctrl->mouseWheel((short)LOWORD(wParam), (short)HIWORD(wParam)/WHEEL_DELTA, (short)LOWORD(lParam), (short)HIWORD(lParam));   // state, delta, x, y
        break;

    case WM_HSCROLL:
        returnValue = ctrl->hScroll(wParam, lParam);
        break;

    case WM_VSCROLL:
        returnValue = ctrl->vScroll(wParam, lParam, 0);
        break;

    case WM_TIMER:
        returnValue = ctrl->timer(LOWORD(wParam), HIWORD(wParam));
        break;

    case WM_NOTIFY:
        returnValue = ctrl->notify((int)wParam, lParam); // controllerID, lParam
        break;

    case WM_CONTEXTMENU:
        returnValue = ctrl->contextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));    // handle, x, y (from screen coords)

    //case WM_ERASEBKGND:
    //    returnValue = ctrl->eraseBkgnd((HDC)wParam);  // handle to device context
    //    break;

    default:
        returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return returnValue;
}


//    Needed for GLUT -  OGL startup
LRESULT CALLBACK Win::tempProcedure(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{    LRESULT returnValue = 0;        // return value
 Win::log("in LRESULT CALLBACK Win::tempProcedure");
/*
    // find controller associated with window handle
    static Win::Controller *ctrl;
    ctrl = (Controller*)::GetWindowLongPtr(hwnd, GWL_USERDATA);
	PAINTSTRUCT ps;
    if(uiMsg == WM_NCCREATE)  // Non-Client Create
    {
				 Win::log("WM_NCCREATE");
        // WM_NCCREATE message is called before non-client parts(border,
        // titlebar, menu,etc) are created. This message comes with a pointer
        // to CREATESTRUCT in lParam. The lpCreateParams member of CREATESTRUCT
        // actually contains the value of lpPraram of CreateWindowEX().
        // First, retrieve the pointrer to the controller specified when
        // Win::Window is setup.
        ctrl = (Controller*)(((CREATESTRUCT*)lParam)->lpCreateParams);
       // ctrl->setHandle(hwnd);

        // Second, store the pointer to the Controller into GWL_USERDATA,
        // so, other messege can be routed to the associated Controller.
        ::SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)ctrl);

        return ::DefWindowProc(hwnd, uiMsg, wParam, lParam);
    }
	switch(uiMsg)
	{
    case WM_CREATE:
				 Win::log("temp WM_CREATE");
        returnValue = ctrl->paint();
        break;
		case WM_PAINT:									
			BeginPaint(hwnd, &ps);							
			EndPaint(hwnd, &ps);					
			break;

		default:
			return DefWindowProc(hwnd, uiMsg, wParam, lParam); // Default window procedure
	}
	return returnValue;

	


	PAINTSTRUCT ps;
	switch(uiMsg)
	{
		case WM_PAINT:									
			BeginPaint(hwnd, &ps);							
			EndPaint(hwnd, &ps);					
			break;

		default:
			return DefWindowProc(hwnd, uiMsg, wParam, lParam); // Default window procedure
	}
*/
	return 0;
}


/*
///////////////////////////////////////////////////////////////////////////////
// Dialog Procedure
// It must return true if the message is handled.
///////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK Win::dialogProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
			 Win::log("in dialogProcedure");
    // find controller associated with window handle
   static Win::Controller* ctrl;
    ctrl = (Controller*)::GetWindowLongPtr(hwnd, GWL_USERDATA);

    // WM_INITDIALOG message is called before displaying the dialog box.
    // lParam contains the value of dwInitParam of CreateDialogBoxParam(),
    // which is the pointer to the Controller.
    if(msg == WM_INITDIALOG)
    {
		 Win::log("WM_INITDIALOG.");
        // First, retrieve the pointer to the controller associated with
        // the current dialog box.  dwCtrl or
        ctrl = (Controller*)lParam;
        ctrl->setHandle(hwnd);

        // Second, store the pointer to the Controller into GWL_USERDATA,
        // so, other messege can be routed to the associated Controller.
        ::SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)ctrl);

        // When WM_INITDIALOG is called, all controls in the dialog are created.
        // It is good time to initalize the appearance of controls here.
        // NOTE that we don't handle WM_CREATE message for dialogs because
        // the message is sent before controls have been create, so you cannot
        // access them in WM_CREATE message handler.
        ctrl->create();
        return true;
    }
    if(msg == WM_DRAWITEM)
    {
		 Win::log("WM_DRAWITEM");
		 if(drawflag == FALSE)
		{	ctrl->paint();  
		 ::DefWindowProc(hwnd, msg, wParam, lParam);}
		 drawflag = FALSE;
//		 ctrl = (Controller*)lParam;
//        ctrl->setHandle(hwnd);
//        ctrl->create();
     return true;
    }

    // check NULL pointer, because GWL_USERDATA is initially 0, and
    // we store a valid pointer value when WM_NCCREATE is called.
    if(!ctrl)
        return false;

    switch(msg)
    {
    case WM_COMMAND:
		Win::log("dialog WM_COMMAND");
        ctrl->command(LOWORD(wParam), HIWORD(wParam), lParam);   // id, code, msg
        return true;

    case WM_TIMER:
        ctrl->timer(LOWORD(wParam), HIWORD(wParam));
        return true;

    case WM_PAINT:
		Win::log("dialog WM_PAINT");
        ctrl->paint();
        ::DefWindowProc(hwnd, msg, wParam, lParam);
        return true;

    case WM_DESTROY:
        ctrl->destroy();
        return true;

    case WM_CLOSE:
        ctrl->close();
        return true;

    case WM_HSCROLL:
        ctrl->hScroll(wParam, lParam);
        return true;

    case WM_VSCROLL:
        ctrl->vScroll(wParam, lParam, 0);
        return true;

    case WM_NOTIFY:
        ctrl->notify((int)wParam, lParam);                      // controllerID, lParam
        return true;

    case WM_MOUSEMOVE:

        ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), 10 );

        //ctrl->mouseMove(wParam, (int)GET_X_LPARAM(lParam), (int)GET_Y_LPARAM(lParam));  // state, x, y
        return true;

    case WM_LBUTTONUP:
        ctrl->lButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));    // state, x, y
        return true;

    case WM_CONTEXTMENU:
        ctrl->contextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));    // handle, x, y (from screen coords)
        return true;

    default:
        return false;
    }
}
*/