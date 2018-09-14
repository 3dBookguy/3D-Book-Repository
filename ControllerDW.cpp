///////////////////////////////////////////////////////////////////////////////
// ControllerDW.cpp
// ====================
// Derived Controller class for DirectWrite window
//
///////////////////////////////////////////////////////////////////////////////

//#include <process.h>       // for _beginthreadex()  why was this here?
#include "ControllerDW.h"
#include "resource.h"
#include "Log.h"
 #define DEBUG_GB
using namespace Win;
int mouseInGraphicWindow = NONE;
extern int graphicWindow;
extern int cursorState;

ControllerDW::ControllerDW (ModelGL* model, ViewDW* view) :
	model(model),
	view(view), 
	bMouseCaptured(FALSE), 
	bMainMenu(TRUE), // New July17
	bFileMenu(FALSE),
	bPageMenu(FALSE) 
	// New July17
{ }

int ControllerDW::animateControl(WPARAM button, int x, int y ){
#ifdef DEBUG_GB
	log(L"ControllerDW::animateControl() button = %i", button);
#endif
	view->animateControl( button, handle , x, y );

	return 0;
}


int ControllerDW::cursor(bool mouseInMenu, int x){ // handle WM_SETCURSOR
#ifdef DEBUG_GB
//	log(L"ControllerDW::cursor()mouse x = %i, dwWidth = %i", x ,dwWidth);
#endif
//#define NONE		0   graphicWindow values
//#define LEFT		1
//#define RIGHT		2

//#define ARROW       1  cursorState values
//#define IBEAM       2

	log(L"ControllerDW::cursor() graphicWindow = %i, cursorState = %i", graphicWindow ,cursorState);

	if( mouseInMenu == TRUE && cursorState != ARROW){
		log(L"mouseInMenu && cursorState != ARROW");
		cursorState = ARROW;
		mouseInGraphicWindow = FALSE;
		SetCursor(LoadCursor(NULL, IDC_ARROW)); return 0;}
	else if( graphicWindow == NONE && cursorState != IBEAM ){
		log(L"graphicWindow == NONE && cursorState != IBEAM");
		cursorState = IBEAM;
		mouseInGraphicWindow = FALSE;
		SetCursor(LoadCursor(NULL, IDC_IBEAM)); return 0;}
	else if( graphicWindow == RIGHT && x > dwWidth/2 && cursorState != ARROW  ){
		cursorState = ARROW;
		mouseInGraphicWindow = RIGHT;
		SetCursor(LoadCursor(NULL, IDC_ARROW)); return 0;}
	else if( graphicWindow == RIGHT && x < dwWidth/2 && cursorState != IBEAM  ){
		cursorState = IBEAM;
		mouseInGraphicWindow = FALSE;
		SetCursor(LoadCursor(NULL, IDC_IBEAM)); return 0;}
	else if( graphicWindow == LEFT && x < dwWidth/2 && cursorState != ARROW  ){
		cursorState = ARROW;
		mouseInGraphicWindow = RIGHT;
		SetCursor(LoadCursor(NULL, IDC_ARROW)); return 0;}
	else if( graphicWindow == LEFT && x > dwWidth/2 && cursorState != IBEAM  ){
		cursorState = IBEAM;
		mouseInGraphicWindow = FALSE;
		SetCursor(LoadCursor(NULL, IDC_IBEAM)); return 0;}
	//else if( cursorState != ARROW)
	//{ SetCursor(LoadCursor(NULL, IDC_ARROW));
	//	cursorState = ARROW;
	//    return 0;}

}


int ControllerDW::create() // handle WM_CREATE
{
#ifdef DEBUG_GB
	log(L"ControllerDW::create()");
#endif
    // initialize all controls
    view->initControls(handle);  // Controller.handle private member
    view->initDW();

    // place the opengl form dialog in right place, bottom of the opengl rendering window
    //RECT rect = {0, 0, 4, 8};
    //::MapDialogRect(glDialog.getHandle(), &rect);
    //int width = MulDiv(300, 4, rect.right);
    //::SetWindowPos(handle, HWND_TOP, 0, 300, 300, 200, 0);
    return 0;
}

int ControllerDW::size(int width, int height, WPARAM wParam)
{
#ifdef DEBUG_GB
	log(L"ControllerDW::window is resized: %dx%d.", width, height);
#endif
	dwWidth = width;
	view->size( width,  height);
	InvalidateRect( handle, NULL, FALSE );
    return 0;
}
int ControllerDW::timer(WPARAM id, LPARAM lParam){
#ifdef DEBUG_GB
//	 log(L"ControllerDW:::timer(WPARAM id, LPARAM lParam)");
#endif
	 view->drawDW(handle);
	 return 0;
}
int ControllerDW::paint() // This function added psl 3 - 18 -14
{
#ifdef DEBUG_GB
//	 log(L"ControllerDW::paint()");
#endif
    // initialize all controls
	//   view->drawControl(handle); // Controller.handle private member

//	if( aspectRatio = 1.0f )
	view->drawDW(handle);		//  Draw the left hand page

//	view->drawDW(handle, FALSE);	//  Draw The right hand page

    // place the opengl form dialog in right place, bottome of the opengl rendering window
    //RECT rect = {0, 0, 4, 8};
    //::MapDialogRect(glDialog.getHandle(), &rect);
    //int width = MulDiv(300, 4, rect.right);
    //::SetWindowPos(handle, HWND_TOP, 0, 300, 300, 200, 0);

    return 0;
}
//
// This procedure.cpp sends this
//		   returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), 0);

// bool bMainMenu;
int ControllerDW::mouseMove(WPARAM state, int x, int y){
#ifdef DEBUG_GB
	log(L"ControllerDW::mouseMove(WPARAM state = %i, int x = %i , int y = %i)", state, x, y);
#endif

	if( bMainMenu == FALSE &&  y >  MENU_HEIGHT){
		log(L"bMainMenu == FALSE &&  y >  MENU_HEIGHT)");
		return 0;  // sub menu active mouse out: do nothing
	}
	// findout which menu item the mouse is in
	iMenus = view->menuSize() + 1;
	menuWidth = ((float)dwWidth/(float)iMenus);      
	menuItem = x/(menuWidth) - 1 ;


	if (bMainMenu == FALSE &&  y <= MENU_HEIGHT){	// sub menu active mouse in
		log(L"bMainMenu == FALSE &&  y <= MENU_HEIGHT)");
		if( bFileMenu == TRUE ){  // File sub menu active
			view->displayMenu( FILE_MENU, menuItem );
			InvalidateRect( handle, NULL, FALSE );
			return 0;
		}
		if( bPageMenu == TRUE ) return 0;  // Page menu is active, wait for user input
	}	
	

	if( y > MENU_HEIGHT && bPageMenu == FALSE){  // mouse is out of menu so remove it
		log(L"y > MENU_HEIGHT && bPageMenu == FALSE");
		view->displayMenu( REMOVE_MENU, NULL );
		InvalidateRect( handle, NULL, FALSE );
		return 0;}

	// mouse is in menuItem of the main menu 
	log(L"Fell through");
	view->displayMenu( MAIN_MENU, menuItem ); // highlight menuitem
	InvalidateRect( handle, NULL, FALSE );
	return 0;
}
/*  July17
int ControllerDW::mouseMove(WPARAM state, int x, int y, int menu){
#ifdef DEBUG_GB
//	log(L"ControllerDW::mouseMove(WPARAM state = %i, int x = %i , int y = %i)", state, x,y);
#endif

		if( y <= MENU_HEIGHT ){ // SetCursor IDC_ARROW
		Win::log("ControllerDW::mouseMove: in menu cursorState = %i", cursorState);
			mouseInGraphicWindow = FALSE;
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			//if(cursorState != ARROW){
			//	Win::log("ControllerDW::mouseMove: cursorState != ARROW");
			//	SetCursor(LoadCursor(NULL, IDC_ARROW));
			//	cursorState = ARROW;
			//}
		}

		else{
			Win::log("ControllerDW::mouseMove( WM_MOUSEMOVE: out of menu");
			if(cursorState != IBEAM){
				Win::log("ControllerDW::mouseMove: cursorState != IBEAM");
				SetCursor(LoadCursor(NULL, IDC_IBEAM));
				cursorState = IBEAM;
			} // 
		}				
     //  if(mouseInGraphicWindow){
		   //returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam), 0);}

		if( !bpPage ){  // if( bPage ) skip  
			if( bpFind &&  y <= MENU_HEIGHT )  // In Find Sub-Menu.
				menu =  FIND_MENU;
			else if( bpColor )  // In Color Sub-Menu.
				menu = RGB_MENU;
			else if( bpFile && y <= MENU_HEIGHT )  // In File Sub-Menu.
				menu = FILE_MENU;
			else if( !bpMenu  && y <= MENU_HEIGHT ){  //  Display Main Menu.
				bpMenuDisplayed = TRUE;
				menu = MAIN_MENU;
			}                                       //  Remove Main Menu.       
			else if( !bpMenu  && y > MENU_HEIGHT && bpMenuDisplayed == TRUE){ 
				bpMenuDisplayed = FALSE;
				menu = MAIN_MENU;
			}
		}
	//  if mouse is out and not a sub-menu remove the menu 
	if( y >= MENU_HEIGHT && menu == MAIN_MENU ){ 
		view->displayMenu( REMOVE_MENU, NULL );
		InvalidateRect( handle, NULL, FALSE );
		return REMOVE_MENU; }	// remove the menu

	iMenus = view->menuSize() + 1;
	menuWidth = ((float)dwWidth/(float)iMenus);      
	menuItem = x/(menuWidth) - 1 ;

//	log(L"mouseMove(menuWidth %f,  x %i, menuItem %i)", menuWidth, x, menuItem);	
	if( menu == FIND_MENU  ){
		view->displayMenu( FIND_MENU, menuItem );
		InvalidateRect( handle, NULL, FALSE );
		return menuItem;}
	else if( menu == FILE_MENU  ){
		view->displayMenu( FILE_MENU, menuItem );
		InvalidateRect( handle, NULL, FALSE );
		return menuItem;}
	else if( menu == RGB_MENU  ){
		if( bMouseCaptured ){
			view->setMouseXY( x, y );
			view->displayMenu( RGB_SLIDER, menuItem );}

		else view->displayMenu( RGB_MENU, menuItem );
		InvalidateRect( handle, NULL, FALSE );
		return menuItem;}
	else{	
		if( menuItem == HELP_ID ) view->displayMenu( HELP_MENU, menuItem );
		else view->displayMenu( MAIN_MENU, menuItem );} //
	InvalidateRect( handle, NULL, FALSE );
	return menuItem;
}
*/
//  (WPARAM state, int x, int y
int ControllerDW::keyDown(int key, LPARAM lParam){
#ifdef DEBUG_GB
log(L"int ControllerDW::keyDown(int key= %i, LPARAM lParam )", key);
#endif

//void ViewDW::getChar(HWND handle, WPARAM findChar,  int lowWord, int highWord)
	if( key == 28 ){
		log(L"User hit Enter");
		if( bPageMenu == TRUE ) { // page menu, user is entering page
			log(L"bPageMenu == TRUE");
			bPageMenu = FALSE;
			bMainMenu = TRUE;
			view->getChar( handle, key,  NULL,  NULL);
			view->displayMenu( MAIN_MENU, menuItem );  // sets bpFile = TRUE
			InvalidateRect( handle, NULL, FALSE );

		}
	}


	return 0;
}
int ControllerDW::lButtonDown(WPARAM state, int x, int y){
#ifdef DEBUG_GB
log(L"ControllerDW::lButtonDown(WPARAM state =%i, x = %i, y = %i)", state,  x,  y);
#endif

	if( y > MENU_HEIGHT ) return 0;  // mouse is out of menu do nothing 

	iMenus = view->menuSize() + 1;
	menuWidth = ((float)dwWidth/(float)iMenus);      
	menuItem = x/(menuWidth) - 1; 

	if( bMainMenu == FALSE ) {  // A sub-menu is active handle sub menuItem
		log(L"if( bMainMenu == FALSE ) yes we are in a sub menu");
		if( bFileMenu == TRUE ){  //This is the File sub menu
			log(L"if( bFileMenu == TRUE ) yes we are in the File sub-menu");
			//Check which File sub menu item this is
			if( menuItem == FILE_CANCEL_ID ){
				log(L"if( menuItem == FILE_CANCEL_ID ) yes, clicked Cancel in file sub menu");
				view->displayMenu( MAIN_MENU, menuItem );
				InvalidateRect( handle, NULL, FALSE );
				bFileMenu = FALSE;
				bMainMenu = TRUE;
				return 0;
			}
			if( menuItem == OPEN_ID ){
				log(L"if( menuItem == OPEN_ID ) yes, clicked Load in file sub menu");
				command( ID_FILE_OPEN, 0, 0 );
				view->displayMenu( MAIN_MENU, menuItem );
				InvalidateRect( handle, NULL, FALSE );
				bFileMenu = FALSE;
				bMainMenu = TRUE;
				return 0;
			}
			if( menuItem == REOPEN_ID ){
				log(L"if( menuItem == REOPEN_ID ) yes, clicked Reload in file sub menu");
				command( REOPEN_ID, 0, 0 );
				view->displayMenu( MAIN_MENU, menuItem );
				InvalidateRect( handle, NULL, FALSE );
				bFileMenu = FALSE;
				bMainMenu = TRUE;
				return 0;}
		}	// end if( bFileMenu == TRUE ){
		//if( bPageMenu == TRUE ){ //This is the Page sub menu
		//	log(L"if( bPageMenu == TRUE ) yes we are in the Page sub-menu");
		//		view->displayMenu( MAIN_MENU, menuItem );
		//		InvalidateRect( handle, NULL, FALSE );
		//		bPageMenu = FALSE;
		//		bMainMenu = FALSE;
		//		return 0;
		//}  // end if( bPageMenu == TRUE ){
	}   // if( bMainMenu == FALSE ){

	// 
	if( bMainMenu == TRUE ) { // main menu 
		if( menuItem == FILE_ID  ){
			log(L"if( menuItem == FILE_ID  )yes, clicked File in main menu" );
			bFileMenu = TRUE;
			bMainMenu = FALSE;
			view->displayMenu( FILE_MENU, FILE_ID );  // sets bpFile = TRUE
			InvalidateRect( handle, NULL, FALSE );
			return 0;
		}
		if( menuItem == PAGE_ID  ){
			log(L"if( menuItem == PAGE_ID  )yes, clicked Page in main menu" );
			bMainMenu = FALSE;
			bPageMenu = TRUE;
			view->displayMenu( PAGE_MENU, menuItem );  // sets bpFile = TRUE
			InvalidateRect( handle, NULL, FALSE );
			return 0;
		}
		
	}
	return 0;
}
// For the most part this duplicates what keyDown in ControllerMain.cpp does. 
/* July17
int ControllerDW::lButtonDown(WPARAM state, int menu, int menuItem){ 
#ifdef DEBUG_GB
log(L"ControllerDW::lButtonDown(WPARAM state =%i, int menu%i, int menuItem%i))", state,  menu, menuItem);
#endif

 menuItem
	0	File
	1	Page
	2	Previous
	3	Next
	4	Home
	5	Table of Contents
	6	Index
	7	End
	8	Back
	9	Forward
	10	Help
	11  Color
	12  Find

//  menuItem == PAGE_ID rcvd only happens if !bMenu,  once per page request
//	display the page '?' menu

//  only call makePage if item needs to change the page
	if( menu == MAIN_MENU  ){  
		if( menuItem > PAGE_ID && menuItem < HELP_ID ) view->makePage( menuItem, 1 );
		else if( menuItem == FILE_ID  )	view->displayMenu( FILE_MENU, FILE_ID );
		else if( menuItem == PAGE_ID  )	view->displayMenu( PAGE_MENU, PAGE_ID );
		else if( menuItem == FIND_ID  )	view->displayMenu( FIND_MENU, FIND_ID );
		else if( menuItem == COLOR_ID )	view->displayMenu( RGB_MENU, COLOR_ID ); }

	else if( menu == FIND_MENU ){
		if( menuItem == FIND_PREVIOUS || menuItem == FIND_NEXT ){
			menuItem = view->findString( menuItem );
			view->makePage( FIND_MENU, menuItem ); }

		else if( menuItem >= 0 ){
			if( !bMouseCaptured ){ SetCapture (handle);	bMouseCaptured = TRUE; }
			view->setMouseXY( menuItem, state );		
		}

	
		else if( menuItem == CANCEL_ID ) view->displayMenu( MAIN_MENU, CANCEL_ID );
	} //  menu == FIND_MENU

	else if( menu == FILE_MENU && menuItem == FILE_CANCEL_ID )
		view->displayMenu( MAIN_MENU, FILE_CANCEL_ID );
	else if( menu == RGB_MENU ){
		if( menuItem == CANCEL_ID ) view->displayMenu( MAIN_MENU, CANCEL_ID );
		else if( menuItem == RED_ID ){
			if( !bMouseCaptured ){ SetCapture (handle);	bMouseCaptured = TRUE; }
			view->setMouseXY( 0, 0 );
			view->displayMenu( RGB_SLIDER, RED_ID );}
	} // menu == RGB_MENU

InvalidateRect( handle, NULL, FALSE ); 	return 0;
}
*/

//int ControllerDW::getChar( WPARAM message, int menuItem, int y, int x){
//	log(L"ControllerDW::getChar(int menuItem = %i  x  = %i  y = %i)", menuItem, x, y);

//delete this func


	//if( menuItem == PAGE_ID ){   //  Page menuitem
	//	pageCount = view->getPageCount( handle );  // in case user loaded a new file!
	//	if( y == 11 ) y = 0;		// convert char codes for 0 - 9
	//	else if( 1 < y  &&  y <  11 ) y--;
	//	else if( y != 14 ) return 0;  // return if not a digit or backspace
	//	if( y == 14 && charCount > 0 ) charCount--;  // have backspace; remove last digit
	//	if( charCount < 3 ){
	//		if( y == 14  ) pageDigit[charCount] = 0;
	//		else { pageDigit[charCount] = y; charCount++;} 
	//	}
	//	if( charCount == 0 ) pageNumber = -1; // tell display menu to display "?"
	//	else if( charCount == 1 ) pageNumber = pageDigit[0] ;
	//	else if( charCount == 2 ) pageNumber = 10*pageDigit[0] + pageDigit[1];
	//	else if( charCount == 3 ) pageNumber = 100*pageDigit[0] + 10*pageDigit[1] + pageDigit[2];
	//	if( pageNumber >= pageCount) pageNumber = pageCount - 1; // Catch attemps to go past end of book.
	//	view->displayMenu( PAGE_NUMBER_MENU, pageNumber);  
	//}  // if( menuItem == PAGE_ID ) 

//	return 0;
//}
int ControllerDW::lButtonUp(WPARAM state, int menu, int menuItem){ 
#ifdef DEBUG_GB
log(L"ControllerDW::lButtonUp(WPARAM state, int x, int y))", state,  menu, menuItem);
#endif
if( bMouseCaptured = TRUE) { bMouseCaptured = FALSE; ReleaseCapture (); }
// InvalidateRect( handle, NULL, FALSE ); 
	return 0;
}

int ControllerDW::command(int id, int command, LPARAM msg){ // handle WM_COMMAND
#ifdef DEBUG_GB
log(L"ControllerDW::command(int id, int command, LPARAM msg)");
#endif
    static bool flag = false;

    switch(id)
    {
    case IDC_ANIMATE:
        if(command == BN_CLICKED)
        {
            flag = !flag;
//            model->animate(flag);
//            view->animate(flag);
        }
        break;
    case ID_FILE_OPEN:
        if(command == BN_CLICKED)
        {
//        log(L"File open here");
//		view->openDW_file(handle);
		if( view->openDW_file(handle, 0) == TRUE ){
			log(L"ControllerDW::ID_FILE_OPEN invalidate");
			InvalidateRect( handle, NULL, FALSE );}

        }
        break;
    case REOPEN_ID:
        if(command == BN_CLICKED)
        {
//        log(L"ControllerDW::REOPEN_ID REOPEN_ID REOPEN_ID");
		if( view->openDW_file(handle, 1) == TRUE ) {
//			log(L"ControllerMain::REOPEN_ID invalidate");
			InvalidateRect(handle, NULL, FALSE );}
		}
        break;
    case IDC_FILL:
        if(command == BN_CLICKED)
        {
 //           model->setDrawMode(0);
        }
        break;

    case IDC_WIREFRAME:
        if(command == BN_CLICKED)
        {
  ///          model->setDrawMode(1);
        }
        break;

    case IDC_POINT:
        if(command == BN_CLICKED)
        {
 //           model->setDrawMode(2);
        }
        break;
    }

    return 0;
}
int ControllerDW::vScroll(WPARAM wParam, LPARAM lParam, int color)
{
#ifdef DEBUG_GB
log(L"ControllerDW::vScroll(WPARAM wParam, LPARAM lParam, int color);");
#endif
// D2D_SIZE_F size = {0};
//  view->colorSliders( size, color );
return 0;
}
///////////////////////////////////////////////////////////////////////////////
// handle horizontal scroll notification
///////////////////////////////////////////////////////////////////////////////
int ControllerDW::hScroll(WPARAM wParam, LPARAM lParam)
{
    // check if the message comming from trackbar
    HWND trackbarHandle = (HWND)lParam;

    int position = HIWORD(wParam);              // current tick mark position
    if(trackbarHandle)
    {
        // get control ID
        int trackbarId = ::GetDlgCtrlID(trackbarHandle);

        switch(LOWORD(wParam))
        {
        case TB_THUMBTRACK:     // user dragged the slider
//            view->updateTrackbars(trackbarHandle, position);
			/*
            if(trackbarId == IDC_RED)
                model->setBackgroundRed(position / 255.0f);
            else if(trackbarId == IDC_GREEN)
                model->setBackgroundGreen(position / 255.0f);
            else if(trackbarId == IDC_BLUE)
                model->setBackgroundBlue(position / 255.0f);
				*/
            break;

        case TB_THUMBPOSITION:  // by WM_LBUTTONUP
            break;

        case TB_LINEUP:         // by VK_RIGHT, VK_DOWN
            break;

        case TB_LINEDOWN:       // by VK_LEFT, VK_UP
            break;

        case TB_TOP:            // by VK_HOME
            break;

        case TB_BOTTOM:         // by VK_END
            break;

        case TB_PAGEUP:         // by VK_PRIOR (User click the channel to prior.)
            break;

        case TB_PAGEDOWN:       // by VK_NEXT (User click the channel to next.)
            break;

        case TB_ENDTRACK:       // by WM_KEYUP (User release a key.)
            position = ::SendMessage(trackbarHandle, TBM_GETPOS, 0, 0);
			/*
            view->updateTrackbars(trackbarHandle, position);
            if(trackbarId == IDC_RED)
                model->setBackgroundRed(position / 255.0f);
            else if(trackbarId == IDC_GREEN)
                model->setBackgroundGreen(position / 255.0f);
            else if(trackbarId == IDC_BLUE)
                model->setBackgroundBlue(position / 255.0f);
				*/
            break;
        }
    }

    return 0;
}



