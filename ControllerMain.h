///////////////////////////////////////////////////////////////////////////////
// ControllerMain.h
// ================
// Derived Controller class for main window
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_MAIN_H
#define WIN_CONTROLLER_MAIN_H

#include "Controller.h"
#include "ControllerDW.h" //July19 added
#include "ViewDW.h"
#include "ModelGL.h"
namespace Win
{
    class ControllerMain :public Controller
    {
    public:
        ControllerMain(ModelGL* model, ViewDW* view); // July19

        ~ControllerMain() {};
        void setGLHandle(HWND handle){ glHandle = handle;}  //These are called from
        void setDWHandle(HWND handle){ dwHandle = handle;}  //WINAPI WinMain to set the handles
		// Contoller override
        int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
//		int cursor( LPARAM Hit_Test_From_Windows);
		int cursor(	bool menu, int Hit_Test_From_Windows);


        int close();
        int create();                               // for WM_CRERATE
        int destroy();
        int size(int w, int h, WPARAM wParam);      // for WM_SIZE: width, height
		int keyDown(int key, LPARAM lParam);
		int getChar( WPARAM message,  int lowWord, int highWord);

    private:
		std::wstring findText;
        HWND glHandle;                              // handle of gl window
        HWND dwHandle;                            // handle of DW window
        ModelGL* model;                             // pointer to model component
        ViewDW* view;                           // pointer to view component
		float aspectRatio;							// relative width of DW and GL windows
    };
}

#endif
