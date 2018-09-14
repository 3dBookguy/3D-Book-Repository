///////////////////////////////////////////////////////////////////////////////
// ViewGL.cpp
// ==========
// View component of OpenGL window
//
///////////////////////////////////////////////////////////////////////////////


#include "ViewGL.h"

using namespace Win;

ViewGL::ViewGL() : hdc(0), hglrc(0){}

ViewGL::~ViewGL(){ }

void ViewGL::closeContext(HWND handle){

    if(!hdc || !hglrc) return;

    // delete DC and RC
    ::wglMakeCurrent(0, 0);
    ::wglDeleteContext(hglrc);
    ::ReleaseDC(handle, hdc);

    hdc = 0;
    hglrc = 0;
}

///////////////////////////////////////////////////////////////////////////////
// create Modern OpenGL rendering context
///////////////////////////////////////////////////////////////////////////////

bool ViewGL::createContext(HWND handle, int colorBits, int depthBits, int stencilBits)
{
Win::log(L"createContext");

//	When it comes time to create a window, each module then passes its own 
//	HINSTANCE when creating the window, and the window manager uses the 
//	combination of the instance handle and the class name to look up the class
	HINSTANCE tempInstance = (HINSTANCE) GetWindowLong(handle, GWL_HINSTANCE);

	WNDCLASSEX wc;
//You're not required to save the return value of CreateWindow(Ex),
//	and you may not really want to. So merely removing hwnd2 will help you. 
//	As for hThisInstance, again, you can make it global, or you can pull it from the window.
//	(Replace it with: (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE) and it should work - read the manual on what that function does.)

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)tempProcedure;
	wc.cbClsExtra = 0; wc.cbWndExtra = 0;
	wc.hInstance = tempInstance;
	wc.hIcon = LoadIcon(tempInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hIconSm = LoadIcon(tempInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"tempClass";

	RegisterClassEx(&wc);

	HWND hWndFake = ::CreateWindowEx(0,L"tempClass", L"FAKE", 
		WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, tempInstance, NULL);
	hdc = ::GetDC(hWndFake);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
 
	int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (iPixelFormat == 0)return false;

	if(!SetPixelFormat(hdc, iPixelFormat, &pfd))return false;
    // create a new OpenGL rendering context
    HGLRC hRCFake = ::wglCreateContext(hdc);
    ::wglMakeCurrent(hdc, hRCFake);
	if(glewInit() != GLEW_OK)
	{
		Win::log("glewInit() failed"); 
		return false;
	}
	else{
		const GLubyte* version;
		version = glewGetString( GLEW_VERSION );
		Win::log("Yeah! ... , we have GLEW version"); 
		Win::log( (const char*)version ); 
	}

	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(hglrc);
	::DestroyWindow(hWndFake);

	if (GLEW_ARB_vertex_program)
	{
	log("GLEW_ARB_vertex_program");
  // Core profile OpenGL 1.3 is supported.
	}

	bool bError = false;
    hdc = ::GetDC(handle);

	if(WGLEW_ARB_create_context && WGLEW_ARB_pixel_format)
	{
		const int iPixelFormatAttribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			0 // End of attributes list
		};
		int iContextAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0 // End of attributes list
		};
		int iPixelFormat, iNumFormats;
		wglChoosePixelFormatARB(hdc, iPixelFormatAttribList, NULL,
								1, &iPixelFormat, (UINT*)&iNumFormats);

		// PFD seems to be only redundant parameter now
		if(!SetPixelFormat(hdc, iPixelFormat, &pfd))return false;

		hglrc = wglCreateContextAttribsARB(hdc, 0, iContextAttribs);
		// If everything went OK
		if( hglrc){ wglMakeCurrent(hdc, hglrc);
		const GLubyte* version;
		version = glGetString( GL_VERSION );
		Win::log("Yeah! ... , we have GL version");
		Win::log( (const char*)version ); 
		version = glGetString( GL_VENDOR );
		Win::log("Yeah! ... , we have GL vendor");
		Win::log( (const char*)version );
		version = glGetString( GL_RENDERER );
		Win::log("Yeah! ... , we have GPU");
		Win::log( (const char*)version ); 
		version = glGetString( GL_SHADING_LANGUAGE_VERSION );
		Win::log("Yeah! ... , we have GLSL");
		Win::log( (const char*)version );		
		} 
		else bError = true;
	}
	else bError = true;

    // set pixel format
    if(bError)
    {
        MessageBox(0, L"Problem in Glew land", L"Error", MB_ICONEXCLAMATION | MB_OK);
    //    ::ReleaseDC(handle, hdc);                     // remove device context
        return false;
    }

   // ::ReleaseDC(handle, hdc);
    return true;
}

/*

///////////////////////////////////////////////////////////////////////////////
// choose pixel format
// By default, pdf.dwFlags is set PFD_DRAW_TO_WINDOW, PFD_DOUBLEBUFFER and PFD_SUPPORT_OPENGL.
///////////////////////////////////////////////////////////////////////////////
bool ViewGL::setPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits)
{
    PIXELFORMATDESCRIPTOR pfd;

    // find out the best matched pixel format
    int pixelFormat = findPixelFormat(hdc, colorBits, depthBits, stencilBits);
    if(pixelFormat == 0)
        return false;

    // set members of PIXELFORMATDESCRIPTOR with given mode ID
    ::DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd);

    // set the fixel format
    if(!::SetPixelFormat(hdc, pixelFormat, &pfd))
        return false;

    return true;
}

*/
/*
///////////////////////////////////////////////////////////////////////////////
// find the best pixel format
///////////////////////////////////////////////////////////////////////////////
int ViewGL::findPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits)
{
    int currMode;                               // pixel format mode ID
    int bestMode = 0;                           // return value, best pixel format
    int currScore = 0;                          // points of current mode
    int bestScore = 0;                          // points of best candidate
    PIXELFORMATDESCRIPTOR pfd;

    // search the available formats for the best mode
    bestMode = 0;
    bestScore = 0;
    for(currMode = 1; ::DescribePixelFormat(hdc, currMode, sizeof(pfd), &pfd) > 0; ++currMode)
    {
        // ignore if cannot support opengl
        if(!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
            continue;

        // ignore if cannot render into a window
        if(!(pfd.dwFlags & PFD_DRAW_TO_WINDOW))
            continue;

        // ignore if cannot support rgba mode
        if((pfd.iPixelType != PFD_TYPE_RGBA) || (pfd.dwFlags & PFD_NEED_PALETTE))
            continue;

        // ignore if not double buffer
        if(!(pfd.dwFlags & PFD_DOUBLEBUFFER))
            continue;

        // try to find best candidate
        currScore = 0;

        // colour bits
        if(pfd.cColorBits >= colorBits) ++currScore;
        if(pfd.cColorBits == colorBits) ++currScore;

        // depth bits
        if(pfd.cDepthBits >= depthBits) ++currScore;
        if(pfd.cDepthBits == depthBits) ++currScore;

        // stencil bits
        if(pfd.cStencilBits >= stencilBits) ++currScore;
        if(pfd.cStencilBits == stencilBits) ++currScore;

        // alpha bits
        if(pfd.cAlphaBits > 0) ++currScore;

        // check if it is best mode so far
        if(currScore > bestScore)
        {
            bestScore = currScore;
            bestMode = currMode;
        }
    }

    return bestMode;
}

*/

///////////////////////////////////////////////////////////////////////////////
// swap OpenGL frame buffers
///////////////////////////////////////////////////////////////////////////////
void ViewGL::swapBuffers()
{
    ::SwapBuffers(hdc);
}



