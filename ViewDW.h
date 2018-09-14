///////////////////////////////////////////////////////////////////////////////
// ViewDW.h
// ============
// Direct Write headers, Direct D2D  headers.
// Class ViewDW: methods for DW text and D2D graphicd
//
///////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_FORM_GL_H
#define VIEW_FORM_GL_H
	#ifndef UNICODE
	#define UNICODE
	#endif
//#pragma comment(lib, "dwrite.lib")
#include <windows.h>
#include <commdlg.h>
#include "Controls.h"
#include <d2d1.h>
//#include <d2d1helper.h>  //  for RenderTargetProperties() but seems to be incl elsewhere
#include <dwrite.h>
#include <wincodec.h> // needed for IWICImagingFactory* m_pWICFactory;
// file io
#include <iostream>
#include <sstream>
#include <streambuf>
//#include <fstream>

#include <string>
#include <vector>
#include <locale>

#include "resource.h"     // for controls
#include "Log.h"
#include <DirectXMath.h>
namespace Win{

	struct txtRange{ 
		int total;
		std::vector<int> txStart;
		std::vector<int> txEnd;
	};

	struct fontNameRanges{ 
		int total;
		std::vector<int> txStart;
		std::vector<int> txEnd;
		std::vector<int> fontNo;
	};

	struct fontSizeRanges{ 
		int total;
		std::vector<int> txStart;
		std::vector<int> txEnd;
		std::vector<float> points;
	};

	struct tableParams{ 
		int total;
		int totalWords;
		int page;
		std::vector<int> cellWidth;
		std::vector<float> top;
		std::vector<int> rows;
		std::vector<int> cols;
		std::vector<std::vector<int>> colWidth;
		std::vector<std::wstring> cellText;
	};

	struct imageParams{
		int total;
		int pageTotal;
		int page;
		int originX;
		int originY;
		int width;
		int height;
		std::wstring name;
	};
	struct fWords{
		int total;
	};
    class ViewDW{
    public:
        ViewDW();
        ~ViewDW();

        void initControls(HWND handle);         // init all controls
			// int ControllerDW::create() view->initControls(handle);
		HRESULT initDW();
			// int ControllerDW::create() view->initDW();
		void drawDW( HWND handle );
			// int ControllerDW::paint()
        HRESULT makePage( int pageLength, int number);
			// void ViewDW::loadResourceText()
			// int ControllerDW::lButtonDown(WPARAM state, int x, int y)
			// int ControllerMain::keyDown(int key, LPARAM lParam)
		int size(int w, int h);
			//int ControllerDW::size(int width, int height, WPARAM wParam)
		bool openDW_file(HWND handle, int ReloadFlag );
			// int ControllerDW::command(int id, int command, LPARAM msg){
			//		case ID_FILE_OPEN:
			//			if( view->openDW_file(handle) == TRUE )
			//	int ControllerMain::command(int id, int cmd, LPARAM msg)
			//		case ID_FILE_OPEN:
			//			if( view->openDW_file(handle) == TRUE )
		std::wstring ReadUTF16(const std::wstring &filename);
			// bool ViewDW::openDW_file(HWND handle){ 
		void WriteUTF16();
			// int ControllerMain::close()view->WriteUTF16();
		int menuSize(){ return iMenuSize; }
			// int ControllerDW::mouseMove(WPARAM state, int x, int y, int menu){
		int sendHandle(HWND handle){ mainHandle = handle; return pageCount; }
			// called from ControllerMainn to give view the page count 
			//int ControllerMain::getChar( WPARAM message, int menuItem, int x, int y){
			//int ControllerMain::size(int w, int h, WPARAM wParam)
 		void displayMenu( int action , int menuItem);
			//int ControllerDW::mouseMove(WPARAM state, int x, int y, int menu){
			//int ControllerDW::lButtonDown(WPARAM state, int x, int y){ 
			//int ControllerDW::mouseMove(WPARAM state, int x, int y, int menu){
			//int ControllerMain::keyDown(int key, LPARAM lParam){
			//int ControllerMain::getChar( WPARAM message, int menuItem, int x, int y){
		void getCharGL( WPARAM findChar );
			// int ControllerMain::getChar( WPARAM message, int menuItem, int x, int y){
		void getChar(HWND handle, WPARAM findChar,  int lowWord, int highWord);
		int findString( int direction);
			// int ControllerMain::getChar( WPARAM message, int menuItem, int x, int y){
		void textMetrix( int mouseX, int  mouseY, D2D1_SIZE_F DWSize);
		void getFindRect( int posLeft, int posRight, int side );
		HRESULT CreateDeviceResources(HWND handle);
			// void ViewDW::drawDW(HWND handle)
		void DiscardDeviceResources();
			// int ViewDW::size( int width, int height)
			// void ViewDW::drawDW(HWND handle)
			// ViewDW::~ViewDW()
		void loadResourceText();
			// HRESULT ViewDW::initDW()
			// bool ViewDW::openDW_file(HWND handle)
			// void ViewDW::errorMessage(int page, int word, wstring msg)
		void readFontTable();
			// void ViewDW::loadResourceText()
		int checkForEscape( int index, int page );
			// void ViewDW::loadResourceText()
		void setTextState( int pageIndex,  int wordIndex );
			// void ViewDW::loadResourceText()
		void errorMessage( int page, int word, std::wstring msg);
			// void ViewDW::loadResourceText()
		void drawMessageBox( D2D1_SIZE_F size );
		void setTextLayout(int pageIndex, int side);
			// void ViewDW::drawDW(HWND handle)
		void newDrawTables( tableParams table, int tableType, D2D1_SIZE_F size );
			// void ViewDW::drawDW(HWND handle)
		void colorWheel( D2D1_SIZE_F size );  // not called
			// void ViewDW::drawDW(HWND handle)
		D2D1_COLOR_F BGcolor( int page );
		void colorSliders( D2D1_SIZE_F rtSize, int color, int mouseY );
			// void ViewDW::drawDW(HWND handle)
		void setMouseXY( int x, int y ){ slidersY = y; getTextX = x; getTextY = y; }
		void colorBox( D2D1_SIZE_F size );
			// void ViewDW::drawDW(HWND handle)
		HRESULT CreateGradientResources();
			// void ViewDW::colorWheel(D2D1_SIZE_F size )
		void ViewDW::animateControl( WPARAM button, HWND handle,   int x , int y);
		void getD2DforPage(HWND handle, int pageIndex, D2D1_RECT_F rect );
			// void ViewDW::drawDW(HWND handle)
		void labelGraphics(const WCHAR *string,  D2D1_POINT_2F loc, D2D1_COLOR_F color,
			float size, int length );

		HRESULT LoadResourceBitmap(
        ID2D1RenderTarget *pRenderTarget,
        IWICImagingFactory *pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap **ppBitmap
        );
 			//void ViewDW::drawDW(HWND handle)

//			HRESULT drawPaper();
//		void drawControl(HWND handle);  not called

		//HRESULT LoadBitmapFromFile(   not called
		//	ID2D1RenderTarget *pRenderTarget,
		//	IWICImagingFactory *pIWICFactory,
		//	PCWSTR uri,
		//	UINT destinationWidth,
		//	UINT destinationHeight,
		//	ID2D1Bitmap **ppBitmap
  //      );

//		void drawD2D(); //  not called
//		void drawTable(int pageNumber, int rows,  std::vector<tableParams> helpTable);
        //void animate(bool flag);
        //void updateTrackbars(HWND handle, int position);

		std::vector<std::wstring> pages;

    private:

	HWND  DWhandle;
	HWND  mainHandle;  // Used in SendMessage to change window mode

// file stuff
 	OPENFILENAMEW ofn;
	std::wstring activeFile;

// Direct2D  and DirectWrite
	bool bBitMap;
	bool bBitMapRight;
	std::vector<imageParams> pageImages;
	std::vector<int> isImageOnPage;
	int imageCount;
	int imageTally;

	DWRITE_FONT_WEIGHT weight_;
    DWRITE_FONT_STYLE style_;
	D2D1_GRADIENT_STOP colWheelStop[2];
	D2D1_RECT_F layoutRect;

    IWICImagingFactory* m_pWICFactory;  // bitmap thing
    ID2D1Bitmap* m_pBitmap;
	ID2D1Bitmap* m_pBitmapRight;
	IDWriteFactory* pDWriteFactory_;
	IDWriteTextFormat* pVectorFormat_;
	IDWriteTextFormat* pTextFormat_;
	IDWriteTextLayout* pTextLayout_;
	IDWriteTextLayout* pRightLayout_;
	IDWriteTextFormat* pMenuFormat_;
	IDWriteTextLayout* pMenuLayout_;
	IDWriteTextLayout* pFindLayout_;
	IDWriteTextLayout* pGraphicMenuLayout_;
	IDWriteTextFormat* pDW_TextFormat;
	IDWriteTextLayout* pPageNumberLayout_;
	IDWriteTextLayout* pTableLayout_;
    ID2D1Factory* pD2DFactory_;
    ID2D1HwndRenderTarget* pRT_;
    ID2D1SolidColorBrush* pRedBrush_;
    ID2D1SolidColorBrush* pBlackBrush_;
    ID2D1SolidColorBrush* pDarkGrayBrush_;
    ID2D1SolidColorBrush* CS_AxisBrush_;
    ID2D1SolidColorBrush* radiusBrush_;
	ID2D1PathGeometry* pPathGeometry;
//	ID2D1PathGeometry* pBoxGeometry;
	ID2D1PathGeometry* pRight_vs_Left;
//	ID2D1PathGeometry* pFeatherGeometry;
    ID2D1LinearGradientBrush* pLinearGradientBrush;

	std::vector<int> pageMode;
	std::vector<int> glRoutineNumberPageNumber;
	int currentMode;
	int currentPage;

	std::vector<fWords> d2Dblock;
	int fbCount;
	std::vector<fontNameRanges> names;
	int nameCount;
	std::vector<int> isSizeOnPage;
	std::vector<fontSizeRanges> sizes;
	int sizeCount;

	std::vector<int> isItalOnPage;
	std::vector<int> pageItalIsOn;
	std::vector<txtRange> italics;
	int italCount;

	std::vector<int> isBoldOnPage;
	std::vector<int> pageBoldIsOn;
	std::vector<txtRange> bold;
	int boldCount;

	std::vector<txtRange> ulines;
	int ulineCount;
	std::vector<txtRange> strikes;
	int strikeCount;
	std::vector<int> indexTablesOnPage;
	std::vector<int> pageTablesAreOn;
	std::vector<tableParams> pageTable;
	std::vector<tableParams> tables;
	std::vector<int> temp2;

	int tableCount;
	int cellCount;
	int colCount;
	int tableWords;
	int cols;
	int rows;
	int tableWordCount;

	std::vector<int> formattedWords;
	std::vector<std::wstring> fontNames;
	std::vector<std::wstring> menuString;
	std::vector<std::wstring> bitmapFile;

// file i/o stuff
	bool bLoadFile;
	std::wstring reload_Filename;

// menu stuff
	bool bMenu;		// Display Menu
	bool bFile;		// Display File menu
	bool bHelp;		// Display Help menu
	bool bColor;	// Display Color menu
	bool bFind;		// Display Find menu
	bool bPage;		// Page menu is active

	BOOL gbRightPage;
	BOOL gbInside;
	D2D1_RECT_F gfindRect;

	int findIndex;
	int getTextX;
	int getTextY;


	bool bMessage;	// Display Message box


// D2D graphics variables

	bool bLeftButtonDown;
	bool bLeftButtonUp;
	bool bRightButtonDown;
	bool bRightButtonUp;
	bool gbArrived;
	bool gbSaveRadiusVector;
	bool gbWindowResized;
	POINT mousePos;
	D2D1_POINT_2F CSorigin;
	D2D1_POINT_2F Xaxis;
	D2D1_POINT_2F Yaxis;
	D2D1_POINT_2F scale;
	D2D1_POINT_2F end; 
	D2D1_POINT_2F ePoint;
	D2D1_POINT_2F velocity;
	D2D1_POINT_2F arrowLeft;
	D2D1_POINT_2F arrowRight;

	D2D1_ELLIPSE cannonBall;
	DWORD  deltaT;
				float r;
				float cosR;				
				float sinR;				

	int iMenuSize;
	int slidersY;
	//static const float sliderXradius = 12.0f;
	//static const float sliderYradius = 6.0f;
	tableParams menuTable;
	tableParams findTable;
	tableParams fileMenu;
	tableParams helpMenu;
	tableParams colorMenu;
	D2D1_RECT_F menuBox;
	D2D1_POINT_2F pageNumberOrigin;
	D2D1_POINT_2F menuOrigin;
	D2D1_POINT_2F origin;

//	bool bTableReset;
	int pageCount;	// Number of pages in the book
	int pageNumber;	// Page to display
	float fontSize_;
	float margin;
    float dpiScaleX_;
    float dpiScaleY_;
	static HGLOBAL hResource;

	std::wstring find;
	std::wstring graphicMenu;
	std::wstring oops;
	std::wstring menu;
	std::wstring font;
	std::wstring pnum;  //page number
	std::wstring book;
	std::wstring test;
	std::wstring page;
	std::wstring fbChange;

    };//  end class ViewDW
}

#endif //#ifndef VIEW_FORM_GL_H
