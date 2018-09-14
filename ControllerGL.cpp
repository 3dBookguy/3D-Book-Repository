///////////////////////////////////////////////////////////////////////////////
// ControllerGL.cpp
// ================
// Derived Controller class for OpenGL window
// It is the controller of OpenGL rendering window. It initializes DC and RC,
// when WM_CREATE called, then, start new thread for OpenGL rendering loop.
//
// When this class is constructed, it gets the pointers to model and view
// components.
//
///////////////////////////////////////////////////////////////////////////////

#include <process.h>        // for _beginthreadex()
#include "ControllerGL.h"
#include "Log.h"
using namespace Win;
extern int cursorState;
ControllerGL::ControllerGL(ModelGL* model, ViewGL* view)
: model(model), view(view), threadHandle(0), threadId(0), loopFlag(false)
{ }
//int ControllerGL::cursor(bool menu){ // handle WM_SETCURSOR
int ControllerGL::cursor(bool menu, int Hit_Test_From_Windows){ // handle WM_SETCURSOR
//#ifdef DEBUG_GB
	log(L"ControllerGL::cursor()");
//#endif

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return 0;

}
int ControllerGL::close(){ // handle WM_CLOSE

    loopFlag = false;
    ::WaitForSingleObject(threadHandle, INFINITE);   // wait for rendering thread is terminated
 
    view->closeContext(handle);    // close OpenGL Rendering Context (RC)

    ::DestroyWindow(handle);
    return 0;
}

int ControllerGL::create(){ // handle WM_CREATE
/*    // create a OpenGL rendering context
    if(!view->createContext(handle, 32, 24, 8))
    {
        Win::log(L"[ERROR] Failed to create OpenGL rendering context from ControllerGL::create().");
        return -1;
    }
    else
    {
        Win::log(L" OpenGL rendering context created a from ControllerGL::create().");
    }

    return 0;
*/
    // create a thread for OpenGL rendering
    // The params of _beginthreadex() are security, stackSize, functionPtr, argPtr, initFlag, threadId.
    threadHandle = (HANDLE)_beginthreadex(0, 0, (unsigned (__stdcall *)(void *))
					threadFunction, this, 0, &threadId);
    if(threadHandle)
    {
        loopFlag = true;
        Win::log(L"Created a rendering thread for OpenGL.");
    }
    else Win::log(L"[ERROR] Failed to create rendering thread from ControllerGL::create().");

    return 0;
}


int ControllerGL::paint(){ return 0; } // handle WM_PAINT

int ControllerGL::command(int id, int cmd, LPARAM msg) { return 0; } // handle 


///////////////////////////////////////////////////////////////////////////////
// route to worker thread
// The function prototype is:
// unsigned int (__stdcall *)(void *)
///////////////////////////////////////////////////////////////////////////////
void ControllerGL::threadFunction(void* param)
{
    ((ControllerGL*)param)->runThread();
}


///////////////////////////////////////////////////////////////////////////////
// rendering thread
// initialize OpenGL states and start rendering loop
///////////////////////////////////////////////////////////////////////////////
void ControllerGL::runThread(){

	Win::log(L"In runThread");

    if(!view->createContext(handle, 32, 24, 8))   // create a OpenGL rendering context
    {
        Win::log(L"[ERROR] Failed to create OpenGL rendering context from ControllerGL::create().");
        
    }
    else  Win::log(L" OpenGL rendering context created a from ControllerGL::create().");

	model->hookUpShaders();    // hook up shaders
    Win::log(L"Shaders ready.");


    RECT rect;
    ::GetClientRect(handle, &rect);
    model->resizeWindow(rect.right, rect.bottom);  
	//model->hello_Gamepad();
    Win::log(L"Initialized OpenGL viewport and projection matrix.");

    // rendering loop
    Win::log(L"Entering OpenGL rendering thread...");

    while( loopFlag )
    {   
 //       Sleep(10);      // 10 millisecond yield to other processes or threads
        model->drawGL();
        view->swapBuffers();
    }

    // terminate rendering thread
    ::wglMakeCurrent(0, 0);             // unset RC
    ::CloseHandle(threadHandle);
    Win::log(L"Exit OpenGL rendering thread.");
}


int ControllerGL::lButtonDown(WPARAM state, int x, int y){
    // update mouse position
//    model->setMousePosition(x, y);
   Win::log(L"ControllerGL::lButtonDown");  
   model->load_obj();
    if(state == MK_LBUTTON)
    {
       model->load_obj();
    }

    return 0;
}

int ControllerGL::lButtonUp(WPARAM state, int x, int y){
    // update mouse position
 //   model->setMousePosition(x, y);

 //   model->setMouseLeft(false);

    return 0;
}

int ControllerGL::rButtonDown(WPARAM state, int x, int y){
    // update mouse position
//    model->setMousePosition(x, y);
   Win::log(L"ControllerGL::rButtonDown"); 
    if(state == MK_RBUTTON)
    {
//        model->setMouseRight(true);
    }

    return 0;
}

int ControllerGL::rButtonUp(WPARAM state, int x, int y){
    // update mouse position
//    model->setMousePosition(x, y);

//    model->setMouseRight(false);

    return 0;
}

int ControllerGL::mouseMove(WPARAM state, int x, int y, int menu){

    Win::log(L"int ControllerGL::mouseMove %dx%d", x, y);
	if( cursorState != ARROW){
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		cursorState = ARROW; }

    if(state == MK_LBUTTON)
    {
 //       model->rotateCamera(x, y);
    }
    if(state == MK_RBUTTON)
    {
 //       model->zoomCamera(y);
    }

    return 0;
}

int ControllerGL::size(int w, int h, WPARAM wParam){ // handle WM_SIZE

    model->resizeWindow(w, h);
    Win::log(L"OpenGL window is resized: %dx%d.", w, h);
    return 0;
}
