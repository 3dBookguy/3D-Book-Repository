///////////////////////////////////////////////////////////////////////////////
// ControllerDW.h
// ==================
// Derived Controller class for OpenGL dialog window
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_DW_H
#define WIN_CONTROLLER_DW_H

#include "Controller.h"
#include "ViewDW.h"
#include "ModelGL.h"

namespace Win
{
    class ControllerDW : public Controller
    {
    public:
        ControllerDW(ModelGL* model, ViewDW* view);
        ~ControllerDW() {};
		int cursor( bool menu, int x );
		// July17 int mouseMove(WPARAM state, int x, int y, int menu);
		int mouseMove(WPARAM state, int x, int y);
		int getChar( WPARAM message, int menuItem, int x, int y);// July17 
		int keyDown(int key, LPARAM lParam);
        int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
		int size(int w, int h, WPARAM wParam);
        int create();                               // for WM_CREATE
        int hScroll(WPARAM wParam, LPARAM lParam);  // for WM_HSCROLL
		int vScroll(WPARAM wParam, LPARAM lParam, int color);
		int lButtonDown(WPARAM wParam, int x, int y ); 
		int lButtonUp(WPARAM wParam, int x, int y );
        int timer(WPARAM id, LPARAM lParam);   // for WM_TIMER: ID, ptr to 		
		int paint();  // added psl 3 - 18  - 14
		int animateControl(WPARAM button,  int x, int y);
    private:
		int dwWidth;
		int iMenus;
		int menuItem;
		float menuWidth;
		bool bMouseCaptured;
		bool bMainMenu;  // New July17
		bool bFileMenu;  // New July17
		bool bPageMenu;

        ModelGL* model;	// pointer to model component
        ViewDW* view;	// pointer to view component
    };
}

#endif
