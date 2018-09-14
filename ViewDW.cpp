///////////////////////////////////////////////////////////////////////////////
// ViewDW.cpp
// ==============
// View component of OpenGL dialog window
//
///////////////////////////////////////////////////////////////////////////////

#include "ViewDW.h"


#define DEBUG_GB // Activate the log file
// SafeRelease inline function .
		
		template <class T> inline void SafeRelease(T **ppT)
{
    if (*ppT) {  (*ppT)->Release();  *ppT = NULL; }
}
using namespace std;// used for string functions
using namespace Win;
static const float g_siMargin = 50.0f;  //sets page margin 
int graphicWindow = NONE; //#define NONE		0   graphicWindow values
					      //#define LEFT		1
					      //#define RIGHT		2
///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
ViewDW::ViewDW():
	currentMode(NULL),
	currentPage(-1),
	pageNumber(0),
	pageCount(0),
	iMenuSize(MENU_SIZE),
	findIndex(-1),
	getTextX(-1),
	//Memu flags
	bHelp(FALSE),
	bMenu(FALSE),
	bFile(FALSE),
	bLoadFile(FALSE),	
	bColor(FALSE),
	bFind(FALSE),
	bPage(FALSE),

	bMessage(FALSE),
	bBitMap(FALSE),
	bBitMapRight(FALSE),
	bLeftButtonDown(FALSE),
	bLeftButtonUp(FALSE),
	bRightButtonDown(FALSE),
	bRightButtonUp(FALSE),
	gbSaveRadiusVector(FALSE),
	gbWindowResized(TRUE),
	gbArrived(FALSE),
//	bFirstTime(TRUE),
// D2D 
    pD2DFactory_(NULL),
    pRT_(NULL),
	pPathGeometry(NULL),
	// pBoxGeometry(NULL),
	pRight_vs_Left(NULL),
	// pFeatherGeometry(NULL),
//brushes
    pBlackBrush_(NULL),
	pRedBrush_(NULL),
	pDarkGrayBrush_(NULL),
    pLinearGradientBrush(NULL),
    CS_AxisBrush_(NULL),
    radiusBrush_(NULL),

// DirectWrite
    pDWriteFactory_(NULL),
    pTextFormat_(NULL),
    pVectorFormat_(NULL),
	pDW_TextFormat(NULL),
	pTableLayout_(NULL),
	pFindLayout_(NULL),
	pTextLayout_(NULL),
	pRightLayout_(NULL),
	pMenuLayout_(NULL),
	pPageNumberLayout_(NULL),
	pGraphicMenuLayout_(NULL),
	m_pWICFactory(NULL),//  bitmap interfaces
	m_pBitmap(NULL),
	m_pBitmapRight(NULL)
	{}

///////////////////////////////////////////////////////////////////////////////
// default dtor
///////////////////////////////////////////////////////////////////////////////
ViewDW::~ViewDW()
{  
	SafeRelease(&pTextFormat_);
	SafeRelease(&pVectorFormat_);
	SafeRelease(&pDW_TextFormat);
	SafeRelease(&pMenuFormat_);
    SafeRelease(&pPathGeometry);
  //  SafeRelease(&pBoxGeometry);
  //  SafeRelease(&pFeatherGeometry);
	SafeRelease(&pRight_vs_Left);
    SafeRelease(&pD2DFactory_);
    SafeRelease(&pDWriteFactory_);
	SafeRelease(&m_pWICFactory);
	DiscardDeviceResources();
}
	//IDWriteFactory* pDWriteFactory_;
	//IDWriteTextFormat* pTextFormat_;
	//IDWriteTextLayout* pTextLayout_;
	//IDWriteTextFormat* pMenuFormat_;
	//IDWriteTextLayout* pMenuLayout_;
	//IDWriteTextFormat* pDW_TextFormat;
	//IDWriteTextLayout* pPageNumberLayout_;
	//IDWriteTextLayout* pTableLayout_;
 //   ID2D1Factory* pD2DFactory_;
 //   ID2D1HwndRenderTarget* pRT_;
 //   ID2D1SolidColorBrush* pRedBrush_;
 //   ID2D1SolidColorBrush* pBlackBrush_;
 //   ID2D1SolidColorBrush* pDarkGrayBrush_;
	//ID2D1PathGeometry* pPathGeometry;
	//ID2D1PathGeometry* pBoxGeometry;
 //   ID2D1LinearGradientBrush* pLinearGradientBrush;
void ViewDW::DiscardDeviceResources(){
#ifdef DEBUG_GB
	log(L"ViewDW::DiscardDeviceResources()");
#endif
	SafeRelease(&pDarkGrayBrush_);
    SafeRelease(&pBlackBrush_);
    SafeRelease(&pRedBrush_);
    SafeRelease(&CS_AxisBrush_);
    SafeRelease(&radiusBrush_);
	SafeRelease(&pLinearGradientBrush);
	SafeRelease(&pPageNumberLayout_);
	SafeRelease(&pTextLayout_);
	SafeRelease(&pGraphicMenuLayout_);
	SafeRelease(&pRightLayout_);
	SafeRelease(&pFindLayout_);
	SafeRelease(&pTableLayout_);
	SafeRelease(&pMenuLayout_);
	SafeRelease(&m_pBitmap);
	SafeRelease(&m_pBitmapRight);
//    SafeRelease(&pRT_);
		if (pRT_)
		pRT_->Release();
}
HRESULT ViewDW::initDW()
{		
#ifdef DEBUG_GB
	log(L"ViewDW::initDW()");
#endif




	//ID2D1GeometrySink *pSink = NULL;
	//oops = L" oops error";
	oops = L"oops error";  // init error message string

	loadResourceText();   
 
	HRESULT hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory_ );
	pD2DFactory_->GetDesktopDpi(&dpiScaleX_, &dpiScaleY_);
	dpiScaleX_ = 96/dpiScaleX_;
	dpiScaleY_ = 96/dpiScaleY_;

/*

//	log(L"ViewDW::initDW dpiScaleX_ %f  dpiScaleY_ %f" ,dpiScaleX_, dpiScaleY_);
    if (SUCCEEDED(hr)){        // Create a path geometry.
       hr = pD2DFactory_->CreatePathGeometry(&pPathGeometry); 
        // Use the geometry sink to write to the path geometry.
	   //pPathGeometry  Used in void ViewDW::colorWheel( D2D1_SIZE_F size )
	   
       hr = pPathGeometry->Open(&pSink);
	}
    if (SUCCEEDED(hr)){
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);//   D2D1_FILL_MODE_WINDING = 1,
        pSink->BeginFigure(
            D2D1::Point2F(0, 0),
            D2D1_FIGURE_BEGIN_FILLED );  //  D2D1_FIGURE_BEGIN_HOLLOW = 1,
        pSink->AddLine(D2D1::Point2F(150, 0));
        pSink->AddLine(D2D1::Point2F(150, 5));
//        pSink->AddLine(D2D1::Point2F(-150, 5));
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        hr = pSink->Close();
	}

    SafeRelease(&pSink);
*/

// ******************************************
	
	//This block creates the geometry used in the RHCS vs LHCS demo and should be moved to a separate file .


	ID2D1GeometrySink *pSink = NULL;
    if (SUCCEEDED(hr)){        // Create a path geometry for the right and left hand 
								// CS demo i void ViewDW::getD2DforPage(HWND handle, int pageIndex, D2D1_RECT_F rect ).
       hr = pD2DFactory_->CreatePathGeometry(&pRight_vs_Left); 
        // Use the geometry sink to write to the path geometry.
       hr = pRight_vs_Left->Open(&pSink);
	}
    if (SUCCEEDED(hr)){
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);//   D2D1_FILL_MODE_WINDING = 1,
		D2D1_POINT_2F Orig ={0.0f};
		//using namespace DirectX;		XMFLOAT2 vct2;

//		DirectX::XMFLOAT2 vct2( 100.0f, 415.0f );
		//DirectX::XMFLOAT2 vct2( 0.0f, 0.0f );
		//DirectX::XMVECTOR dest;
  //      dest = DirectX::XMLoadFloat2(&vct2);
//		Orig.x = 
//		XMVECTOR DirectX::XMLoadFloat2( CONST XMFLOAT2 *pSource);
		//Orig.x = DirectX::XMVectorGetX( dest );
		//Orig.y = DirectX::XMVectorGetY( dest );

		float axisLength = 120.0f;
		float arrowLength = 10.0f;
        pSink->BeginFigure( Orig, D2D1_FIGURE_BEGIN_HOLLOW );
		pSink->AddLine(D2D1::Point2F( Orig.x + axisLength, Orig.y ));
        pSink->AddLine(D2D1::Point2F( Orig.x + axisLength - arrowLength, Orig.y +											arrowLength/2));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->BeginFigure( D2D1::Point2F( Orig.x + axisLength, Orig.y ),
					D2D1_FIGURE_BEGIN_HOLLOW );  
		pSink->AddLine(D2D1::Point2F( Orig.x + axisLength - arrowLength , Orig.y - 
			arrowLength/2));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->BeginFigure( Orig, D2D1_FIGURE_BEGIN_HOLLOW );  
		pSink->AddLine(D2D1::Point2F( Orig.x, Orig.y - axisLength));
        pSink->AddLine(D2D1::Point2F( Orig.x - arrowLength/2, Orig.y - axisLength											+ arrowLength));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->BeginFigure( D2D1::Point2F( Orig.x , Orig.y - axisLength											), D2D1_FIGURE_BEGIN_HOLLOW );  
		pSink->AddLine(D2D1::Point2F( Orig.x + arrowLength/2  , Orig.y - axisLength	
			+ arrowLength));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->BeginFigure( Orig, D2D1_FIGURE_BEGIN_HOLLOW );  
		pSink->AddLine(D2D1::Point2F( Orig.x + 0.7*axisLength, Orig.y - 0.3*axisLength));
		pSink->AddLine(D2D1::Point2F( Orig.x + 0.6*axisLength, Orig.y - 0.28*axisLength));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->BeginFigure( D2D1::Point2F( Orig.x + 0.71*axisLength, Orig.y - 0.30*axisLength),
			D2D1_FIGURE_BEGIN_HOLLOW );  
		pSink->AddLine(D2D1::Point2F( Orig.x + 0.69*axisLength, Orig.y - 0.25*axisLength));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);

        hr = pSink->Close();
	}
	 SafeRelease(&pSink);

	// End RHCS vs LHCS demo geometry

// ***************************************
/*
    SafeRelease(&pSink);
    if (SUCCEEDED(hr)){        // Create a path geometry.
       hr = pD2DFactory_->CreatePathGeometry(&pBoxGeometry); 
        // Use the geometry sink to write to the path geometry. 3f8898
       hr = pBoxGeometry->Open(&pSink);
	}
    if (SUCCEEDED(hr)){
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);//   D2D1_FILL_MODE_WINDING = 1,
        pSink->BeginFigure(
            D2D1::Point2F(0, 0),
            D2D1_FIGURE_BEGIN_FILLED );  //  D2D1_FIGURE_BEGIN_HOLLOW = 1,
        pSink->AddLine(D2D1::Point2F(150, 0));
        pSink->AddLine(D2D1::Point2F(150, 5));
//        pSink->AddLine(D2D1::Point2F(-150, 5));
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        hr = pSink->Close();
	}
    SafeRelease(&pSink);
*/
/*
    if (SUCCEEDED(hr)){        // Create a path geometry.
       hr = pD2DFactory_->CreatePathGeometry(&pFeatherGeometry); 
        // Use the geometry sink to write to the path geometry.
       hr = pFeatherGeometry->Open(&pSink);
	}
    if (SUCCEEDED(hr)){
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);//   D2D1_FILL_MODE_WINDING = 1,
        pSink->BeginFigure(
            D2D1::Point2F(100, 100),
            D2D1_FIGURE_BEGIN_FILLED );  //  D2D1_FIGURE_BEGIN_HOLLOW = 1,
        pSink->AddLine(D2D1::Point2F(150, 0));
		 pSink->AddArc ( D2D1::ArcSegment(
                        D2D1::Point2F(440, 255), // end point
                        D2D1::SizeF(85, 85),
                        0.0f, // rotation angle
                        D2D1_SWEEP_DIRECTION_CLOCKWISE,
                        D2D1_ARC_SIZE_SMALL
                        ));            
//		pSink->AddArc(D2D1::Point2F(150), 10, 30, 1, 1 );
//		typedef struct D2D1_ARC_SEGMENT
//{
//    D2D1_POINT_2F point;
//    D2D1_SIZE_F size;
//    FLOAT rotationAngle;
 //  D2D1_SWEEP_DIRECTION sweepDirection;
 //   D2D1_ARC_SIZE arcSize;
//
//} D2D1_ARC_SEGMENT;
        pSink->AddLine(D2D1::Point2F(150, 5));
//        pSink->AddLine(D2D1::Point2F(-150, 5));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        hr = pSink->Close();
	}

    SafeRelease(&pSink);
*/

    if (SUCCEEDED(hr)){
		hr = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&pDWriteFactory_) );
	}
    if (SUCCEEDED(hr)){        // Create WIC factory.
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pWICFactory)
            );
    }
   if (SUCCEEDED(hr)){ // This is just for the page numbering  - It needs to be done
						// only once so it is put here instead of in drawDW
    hr = pDWriteFactory_->CreateTextFormat( 
		L"Arial",
        NULL,  
		DWRITE_FONT_WEIGHT_HEAVY,
        DWRITE_FONT_STYLE_ITALIC,
        DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
        L"en-us",
        &pVectorFormat_);

    hr = pDWriteFactory_->CreateTextFormat( 
		L"Arial",
        NULL,  
		DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
		17.0f,
        L"en-us",
        &pTextFormat_);

	origin = D2D1::Point2F(g_siMargin, 0.0f);
	pageNumberOrigin = D2D1::Point2F(0.0f, 0.0f);
//	pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
    pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    pTextFormat_->SetIncrementalTabStop(30.0f);

    hr = pDWriteFactory_->CreateTextFormat( 
		L"Arial",
        NULL,  
		DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
		12.0f,
        L"en-us",
        &pMenuFormat_);
//	pMenuFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		pMenuFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);


   }
	if (SUCCEEDED(hr)){
    hr = pDWriteFactory_->CreateTextFormat( 
		L"Times New Roman",
        NULL,  
		DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
		72.0f,
        L"en-us",
        &pDW_TextFormat);
	}
	pDW_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	return hr;

}

// called on WM_CLOSE
void ViewDW::WriteUTF16(){  // save the page number to file DWOGL22{z}  22 = pg num
#ifdef DEBUG_GB
	log(L"void ViewDW::WriteUTF16()");
#endif
//	if( bLoadFile ){
		ofstream file2(activeFile.c_str(), std::ios::out | std::ios::binary);    // 
		if(!file2){
	//		MessageBox(NULL, TEXT("Could not open for out put"), activeFile.c_str(), MB_OK);
		 }
		else {
			MessageBox(NULL, TEXT("Write"), activeFile.c_str(), MB_OK);
			int index = 0;
			index = book.find(L"DWOGL", index ); 	// token for direct write openGL file
			fbCount  = book.find(L"{z}", fbCount );
			if( index != wstring::npos && fbCount != wstring::npos){ 
				book.replace(index + 5 , fbCount - index - 5, to_wstring( pageNumber -1 ));
			}
			file2.write((char *) book.c_str(), book.length()*sizeof(wchar_t));
		}

//	}
}
wstring ViewDW::ReadUTF16(const wstring &filename){
#ifdef DEBUG_GB
	log(L"wstring ViewDW::ReadUTF16(const wstring & filename)");
#endif
	activeFile.clear();
	activeFile.append(filename);
	int iUniTest = IS_TEXT_UNICODE_SIGNATURE; // 0xFEFF at beginning of file.
	int cTextLength_;											//  byte order mask
	wstring error = L"Can not open file";
    ifstream file(filename.c_str(), std::ios::binary);    // std::ios::binary for Windows
	if(!file){
		MessageBox(NULL, TEXT("Could not open"), filename.c_str(), MB_ICONINFORMATION);
		error = L"Could not open this file..";
		log(error);
		bLoadFile = FALSE;
		return error; }
    std::stringstream ss;
    ss << file.rdbuf() << '\0';
//	 log(wstring((wchar_t *)ss.str().c_str()));
	cTextLength_ = (UINT32)strlen(ss.str().c_str());
	if(IsTextUnicode(ss.str().c_str(), cTextLength_, &iUniTest)){
	//	MessageBox(NULL, TEXT("Have Unicode"), filename.c_str(), MB_ICONINFORMATION);
		bLoadFile = TRUE;
		return wstring((wchar_t *)ss.str().c_str());}
	else{
		MessageBox(NULL, TEXT("Not UNICODE"), filename.c_str(), MB_ICONINFORMATION);
		error = L"This file is not unicode.";
		log(error);
		bLoadFile = FALSE;
		return error;}

// Closing of streams:  Done inplicitly by the streams dtor.  So an explicit call to
//	close() /is needed only if the file must be closed before reaching the end of the
//	scope  in  which it was declared. 
} 
bool ViewDW::openDW_file( HWND handle, int reloadFlag ){  // returns TRUE to controller telling it to 
#ifdef DEBUG_GB		// InvalidateRect if we have a good file
	log(L"ViewDW::openDW_file(HWND handle, int reloadFlag )");
#endif

	if( reloadFlag == 0 ){  //reloadFlag == 0 means its not a reload need the Open File dlg.  
		TCHAR szFile[140];
		ZeroMemory( &ofn , sizeof( ofn));
		ofn.lStructSize = sizeof ( ofn );
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof( szFile );
//		ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
		ofn.lpstrFilter = TEXT("Text\0*.TXT\0\All\0*.*\0");
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0 ;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
		
		GetOpenFileName( &ofn );
		reload_Filename.clear();
	    reload_Filename = ofn.lpstrFile;}  // Save the file name for reopen call

		if( reload_Filename.find( L".obj" ) != wstring::npos ){
		MessageBox( NULL, ofn.lpstrFile, 
				TEXT("Have a .obj file here"), MB_OK );
		//bmFileName.clear();
		//bmFileName = ofn.lpstrFile;
		//bBitMap = TRUE; 
		} 

//	MessageBox( NULL, ofn.lpstrFile, TEXT("File Name"), MB_OK );
//	const wstring filename = ofn.lpstrFile;
/*
	if( filename.find( L".bmp" ) != wstring::npos ){
		MessageBox( NULL, ofn.lpstrFile, 
				TEXT("Have a .bmp file here"), MB_OK );
		bmFileName.clear();
		bmFileName = ofn.lpstrFile;
		bBitMap = TRUE; } 
	else if( filename.find( L".jpg" ) != wstring::npos ){
		MessageBox( NULL, ofn.lpstrFile, 
				TEXT("Have a .jpg file here"), MB_OK );
		bmFileName.clear();
		bmFileName = ofn.lpstrFile;
		bBitMap = TRUE; }
	else if( filename.find( L".JPG" ) != wstring::npos ){
		MessageBox( NULL, ofn.lpstrFile, 
				TEXT("Have a .jpg file here"), MB_OK );
		bmFileName.clear();
		bmFileName = ofn.lpstrFile;
		bBitMap = TRUE; }
	else if( filename.find( L".png" ) != wstring::npos ){
		MessageBox( NULL, ofn.lpstrFile, 
				TEXT("Have a .jpg file here"), MB_OK );
		bmFileName.clear();
		bmFileName = ofn.lpstrFile;
		bBitMap = TRUE; }

	else {
*/ 

	//MessageBox( NULL, reload_Filename.c_str(), 
	//			TEXT("filename.c_str()"), MB_OK );
	bLoadFile = TRUE;
	test.clear();
//	test = ReadUTF16( ofn.lpstrFile );  // If there is a problemo will set bLoadFile = FALSE.
	test = ReadUTF16( reload_Filename.c_str() );  // If there is a problemo will set bLoadFile = FALSE.

	if( bLoadFile ){  // Have a file, now check the format.
		if( test.find(L"fonttbl") == wstring::npos ){
			//MessageBox( NULL, ofn.lpstrFile, 
			//	TEXT("File is not a DW reader file format!"), MB_OK );
			MessageBox( NULL, reload_Filename.c_str(), 
				TEXT("File is not a DW reader file format!"), MB_OK );

			bLoadFile = FALSE;  return FALSE;}
		else { loadResourceText();  return TRUE; } 
	} // have a DW reader file format, so load it up!
	else {MessageBox( NULL, reload_Filename.c_str(), TEXT("Oops; Could not read!"), MB_OK );
	return FALSE;}
//}

//	ReadUTF16(ofn.lpstrFile);(const string & filename)
/*
	hFile = CreateFile (ofn.lpstrFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF){
			MessageBox( NULL, ofn.lpstrFile, TEXT ("Valid"), MB_OK );
			PWSTR pszFileText;
			pszFileText = (PWSTR)GlobalAlloc(GPTR, dwFileSize + 2 );
//			test = (PWSTR)GlobalAlloc(GPTR, dwFileSize + 2 );

			if(pszFileText != NULL)
			{
				DWORD dwOutNumRead;
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwOutNumRead, NULL))
					CloseHandle(hFile);

				wsprintf(szBuffer, TEXT("Bytes read %i"),dwOutNumRead);
				//	wsprintf(szBuffer, TEXT("File size %i"),dwFileSize);
				MessageBox(NULL, szBuffer, ofn.lpstrFile,NULL);
				pszFileText[dwFileSize] = 0; // Add null terminator
				pszFileText[dwFileSize + 1] = 0;
				iUniTest = IS_TEXT_UNICODE_SIGNATURE; // 0xFEFF at beginning of file.
												//  byte order mask
				cTextLength_ = (UINT32)lstrlen(pszFileText);
				if(IsTextUnicode(pszFileText, cTextLength_, &iUniTest))
				{MessageBox(NULL, TEXT("Have Unicode"), ofn.lpstrFile,MB_ICONINFORMATION);}				
				//delete [] wszText_;
				//wszText_ = new wchar_t[cTextLength_ + 1];
				//wcscpy_s(wszText_, cTextLength_ + 1, pszFileText);
				test = pszFileText;
				// log(test);
				bLoadFile = TRUE;
				loadResourceText();
//				GlobalFree(pszFileText);
			}
		}
	}
*/
}
void ViewDW::loadResourceText(){
#ifdef DEBUG_GB
	log(L"ViewDW::loadResourceText()");
#endif
	if( !bLoadFile ){ // loading from resource
/*
	PTSTR pFileName;
	int eCode = 0;
	int resID = 0;
	pFileName =  TEXT("Load resource");
	TCHAR Buf[140];
	wsprintf(Buf, TEXT("Yes = book, No = test"));
*/
//#define TEXTFILE			256 //  Needs be > 255 ? 
//#define TESTFILE			259 //  Needs be > 255 ? 
//#define IDR_BOOKTXT		257
//#define IDR_TESTTXT		258
//IDR_BOOKTXT TEXTFILE DISCARDABLE "hellortf.txt"
//IDR_TESTTXT TESTFILE DISCARDABLE "test.txt"

	//eCode =  MessageBox(NULL, Buf, pFileName, MB_YESNOCANCEL);
	//if( eCode == IDYES ){  eCode = 257; resID = 256; }  //  load IDR_BOOKTXT = 257
	//else if( eCode == IDNO ){  eCode = 258;  resID = 259; } // load IDR_TESTTXT = 258
	//else if( eCode == IDCANCEL ){  eCode = 257; resID = 256; } //  load IDR_BOOKTXT = 257

    HMODULE hModule = ::GetModuleHandle( NULL );
    HRSRC rc = ::FindResource( hModule, MAKEINTRESOURCE( IDR_BOOKTXT ),
				MAKEINTRESOURCE( TEXTFILE ));

 //   HRSRC rc = ::FindResource( hModule, MAKEINTRESOURCE( eCode ),
	//				MAKEINTRESOURCE( resID ));
    HGLOBAL rcData = ::LoadResource( hModule, rc );
	book.clear();
	book.append( (PWSTR)LockResource( rcData ) ); }
	else{  // loading from file 
		SetWindowText( mainHandle, reload_Filename.c_str());  // Put the file name on the title bar
		book.erase();
		book.append( test ); 
		bLoadFile = FALSE;}

// Check if direct write openGL file, last page  ///////////////////////////////////////////

	int index = 0;
	wstring temp;
	fbCount = 0;
	index  = book.find(L"DWOGL", index );
	fbCount  = book.find(L"{z}", fbCount );
	// token for direct write openGL file
	if( index != wstring::npos && fbCount != wstring::npos){ 
		temp.clear();
		temp.append(book, index + 5 , fbCount - index - 5);
		pageNumber = stoi(temp) + 1;
//MessageBox(NULL, TEXT("Have valid file format"),  TEXT("Have valid file format"), MB_OK);
	}
	else{
MessageBox(NULL, TEXT("Invalid file format"),  TEXT("Invalid file format"), MB_OK);
	}


//	get the number of fonts and read them into wstring array fontNames[number of fonts]

	readFontTable();
	font.clear();
	font = fontNames[0];


// Check for \mode format errors
// Find the number of pages pageCount
	index = 0;
	pageCount = 0;
	while( index  > -1 ){
//		index = book.find( L"\\fs", index);
		index = book.find( L"\\mode", index);
		if( index < 0 ) break;
//		if( index == checkForEscape( index, -1 )){
			temp.clear();
//			temp.append(book, index + 3 , 4);
			temp.append(book, index + 5 , 2);
			if( temp != L"1 " && temp != L"1\\" && temp != L"2 " && temp != L"2\\"){
				oops.clear();
				oops = L"\\mode1 error";
				oops.append( book, index,  12 );
				errorMessage( pageCount, index , oops);
				}
			pageCount++;
//		}
		index++;
	}  // while( index  > -1 )

// Find the number of pages; pageCount
	index = 0;
	pageCount = 0;
	while( index  > -1 )
	{
	index = book.find( L"\\mode", index);
	if( index < 0 ) break;
	pageCount++;
	index++;
	}

	if( pageCount < 1 ){
		oops.clear( );
		oops = L"There are no formatted pages in this file";
		errorMessage( 0, 0 , oops);
	}
	// Check to see if user DWOGL73{z} is in range
	if( pageCount < pageNumber ){
		oops.clear();
		oops.append( book, 0,  12 );
		errorMessage( 0, 0 , oops);
		pageNumber = 0;}

	log(L"pageCount =   %i", pageCount);
	pages.clear();
	pages.resize( pageCount );
	pageMode.clear();
	pageMode.resize( pageCount );
	formattedWords.resize( pageCount );

// Paginate, ie;  read book into pages[pageCount] 
// Also count the number of \mode2 i.e.; GL_MODE pages into glModeCount

	int start = 0;
	int glModeCount  = 0;
	index = 0;
	index = book.find( L"12345", index);
	for( int i = 0; i < pageCount; i++ ){
		start = index + 5 ;
//		index = book.find( L"\\mode1", index);
		index = book.find( L"\\mode", index);
		if( book[ index + 5 ] == L'1' ) pageMode[i] = DW_MODE;
		else if( book[ index + 5 ] == L'2' ){
			pageMode[i] = GL_MODE;
			glModeCount++;  
			}
//		log(L"pageMode[%i] =   %i", i, pageMode[i]);
		pages[i].clear();
		pages[i].append( book, start, index - start);
		index++;
	}

//	Create the list of page numbers and glRoutine numbers in 
//	glRoutineNumberPageNumber[ routine number ] = page number

    log(L"glModeCount = %i", glModeCount);
	index = 1; 
	glRoutineNumberPageNumber.clear();
	glRoutineNumberPageNumber.resize( glModeCount + 1 );
	glRoutineNumberPageNumber[0] = 0;  //  The cover is always GL_MODE
	for( int i = 1; i < pageCount; i++ ){
		if( pageMode[i] == GL_MODE && index <= glModeCount ){
			glRoutineNumberPageNumber[index] = i;
			log(L"glRoutineNumberPageNumber[%i] = %i", index, i);
			index++;
		}
	}

// Check for singleton DW_MODE blocks; i.e.; must have at least 2 DW_MODES in a row
	for( int i = 1; i < pageCount - 1; i++ ){

		if(			pageMode[i] == DW_MODE 
			 && pageMode[i - 1] != DW_MODE  
			 && pageMode[i + 1] != DW_MODE ) {
		oops = L"There are singleton DW_MODE blocks pages in this file";
		errorMessage( i, 0, oops);}
	}

// Count the italic  blocks

	isItalOnPage.clear();
	isItalOnPage.resize( pageCount, -1 );
	pageItalIsOn.clear();
	pageItalIsOn.resize( pageCount);
	fbCount = 0;
	for( int i = 0; i < pageCount; i++ ){
		index = pages[i].find(L"\\i");
		if( index != wstring::npos ){ 
			isItalOnPage[i] = fbCount;
			pageItalIsOn[fbCount] = i;
			fbCount++;
	//		log("isItalOnPage[%i] = %i ", i ,isItalOnPage[i]);
		}
	}


	int totalPages = fbCount;
	int i;
	int naughtCount = 0;
	italics.clear();	
	if( totalPages > 0 ){ 
		italics.resize( totalPages );
		for( int j = 0; j < totalPages; j++ ){
			i = pageItalIsOn[j];
			index = 0;
			fbCount = 0;
			naughtCount = 0;
			while( index > - 1 ){  
			index = pages[i].find( L"\\i", index);
			if( pages[i][ index + 2 ] == L'0' ) naughtCount++;
			if(index  < 0 ) break;
			if( pages[i][ index + 2 ] != L'0' && pages[i][ index + 2 ] != L'\\'){ // have \i
				if( pages[i][ index + 2 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			if( pages[i][ index + 2 ] == L'0' && pages[i][ index + 3 ] != L'\\'){ // have \i0
				if( pages[i][ index + 3 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			fbCount++;
			index++;
			}
		if( fbCount % 2 != 0 ||	fbCount !=  2*naughtCount ){
			oops.clear();
			oops = L"Odd ital count!";
			errorMessage( i, fbCount, oops );}
		italics[j].total = fbCount/2;
		italics[j].txStart.clear();
		italics[j].txStart.resize(fbCount/2);
		italics[j].txEnd.clear();
		italics[j].txEnd.resize(fbCount/2);
	//	log(L"italics[%i].total =  %i",  j,  italics[j].total);
		}  // for j
	}  // if( fbCount > 0 )


	isBoldOnPage.clear();
	isBoldOnPage.resize( pageCount, -1 );
	pageBoldIsOn.clear();
	pageBoldIsOn.resize( pageCount, -1 );
	fbCount = 0;
	for( int i = 0; i < pageCount; i++ ){
		index = pages[i].find(L"\\b");
		if( index != wstring::npos ){ 
			isBoldOnPage[i] = fbCount;
			pageBoldIsOn[fbCount] = i;
			fbCount++;
	//		log("isBoldOnPage[%i] = %i ", i ,isBoldOnPage[i]);
		}
	}
	//log("isBoldOnPage[0] = %i ", isBoldOnPage[0] );
	//log(" pages w bolds = %i ", fbCount );
	totalPages = fbCount;

	bold.clear();	
	if( totalPages > 0 ){ 
		bold.resize( totalPages );
		for( int j = 0; j < totalPages; j++ ){
			i = pageBoldIsOn[j];
			index = 0;
			fbCount = 0;
			naughtCount = 0;
			while( index > - 1 ){  
			index = pages[i].find( L"\\b", index);
			if( pages[i][ index + 2 ] == L'0' ) naughtCount++;
			if(index  < 0 ) break;
			if( pages[i][ index + 2 ] != L'0' && pages[i][ index + 2 ] != L'\\'){ // have \b
				if( pages[i][ index + 2 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			if( pages[i][ index + 2 ] == L'0' && pages[i][ index + 3 ] != L'\\'){ // have \b0
				if( pages[i][ index + 3 ] != L' ' ){ 
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			fbCount++;
			index++;
			}
			if( fbCount % 2 != 0 ||	fbCount !=  2*naughtCount ){
				oops.clear();
				oops = L"Odd bold count!";
				errorMessage( i, fbCount, oops );}
			bold[j].total = fbCount/2;
			bold[j].txStart.clear();
			bold[j].txStart.resize(fbCount/2);
			bold[j].txEnd.clear();
			bold[j].txEnd.resize(fbCount/2);
		//	log(L"bold[%i].total =  %i",  j,  bold[j].total);
		}  // for j
	}  // 	if( totalPages > 0 )


// Count the underline  blocks

		ulines.clear();
		ulines.resize( pageCount );
		for( int i = 0; i < pageCount; i++ ){
		index = 0;
		fbCount = 0;
		naughtCount = 0;
		while( index > - 1 ){  
			index = pages[i].find( L"\\l", index) ;
			if( pages[i][ index + 2 ] == L'0' ) naughtCount++;
			if(index  < 0 ) break;
			if( pages[i][ index + 2 ] != L'0' && pages[i][ index + 2 ] != L'\\'){ // have \l
				if( pages[i][ index + 2 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			if( pages[i][ index + 2 ] == L'0' && pages[i][ index + 3 ] != L'\\'){ // have \l0
				if( pages[i][ index + 3 ] != L' ' ){ 
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			fbCount++;
			index++;
			}
		if( fbCount % 2 != 0 ||	fbCount  != 2*naughtCount ) errorMessage( i, fbCount, oops );
		ulines[i].total = fbCount;
		ulines[i].txStart.clear();
		ulines[i].txStart.resize(fbCount);
		ulines[i].txEnd.clear();
		ulines[i].txEnd.resize(fbCount);
//		log(L"ulines[%i].total =  %i",  i,  ulines[i].total);
		}

// Count the strike-through  blocks
		strikes.clear();
		strikes.resize( pageCount );
		for( int i = 0; i < pageCount; i++ ){
		index = 0;
		fbCount = 0;
		naughtCount = 0;
		while( index > - 1 ){  
			index = pages[i].find( L"\\s", index) ;
			if( pages[i][ index + 2 ] == L'0' ) naughtCount++;
			if(index  < 0 ) break;
			if( pages[i][ index + 2 ] != L'0' && pages[i][ index + 2 ] != L'\\'){ // have \s
				if( pages[i][ index + 2 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			if( pages[i][ index + 2 ] == L'0' && pages[i][ index + 3 ] != L'\\'){ // have \s0
				if( pages[i][ index + 3 ] != L' ' ){ 
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			fbCount++;
			index++;
			}
		if( fbCount % 2 != 0 ||	fbCount  !=  2*naughtCount) errorMessage( i, fbCount, oops );
		strikes[i].total = fbCount;
		strikes[i].txStart.clear(); 
		strikes[i].txStart.resize(fbCount);
		strikes[i].txEnd.clear();
		strikes[i].txEnd.resize(fbCount);
//		log(L"strikes[%i].total =  %i",  i,  strikes[i].total);
		}

	isImageOnPage.clear();
	isImageOnPage.resize(pageCount, -1 );
	fbCount = 0;
	for( int i = 0; i < pageCount; i++ ){
	index = 0;
	while( index > - 1 ){  
		index = pages[i].find( L"\\c", index) ;
		if(index  < 0 ) break;
		isImageOnPage[i] = fbCount;
//		log("isImageOnPage[%i] = %i", i, isImageOnPage[i] );
		fbCount++;
		index++;
		}
	}
//	log(" total images ========================== %i " ,fbCount);
	imageCount = fbCount;
	pageImages.clear();
	if( imageCount > 0) pageImages.resize( imageCount );


// Count the pageTable blocks & get the nu
	indexTablesOnPage.clear();
	indexTablesOnPage.resize( pageCount, -1 );
	pageTablesAreOn.clear();
	pageTablesAreOn.resize( pageCount, -1 );
	fbCount = 0;
	for( int i = 0; i < pageCount; i++ ){
		index = pages[i].find(L"\\z");
		if( index != wstring::npos ){ 
			indexTablesOnPage[i] = fbCount;
			pageTablesAreOn[fbCount] = i;
			fbCount++;
		//	log("indexTablesOnPage[%i] = %i ", i ,indexTablesOnPage[i]);
		}
	}

	totalPages = fbCount;
	tables.clear();	
	if( totalPages > 0 ){ 
		tables.resize( totalPages );
		for( int j = 0; j < totalPages; j++ ){
			i = pageTablesAreOn[j];
			index = 0;
			fbCount = 0;			
			while( index > - 1 ){ 
				index = pages[i].find( L"\\z", index);
				if(index  < 0 ) break;
				fbCount++;
				index++;
				}
			tables[j].page = i;
			tables[j].total = fbCount;
			tables[j].rows.clear();
			tables[j].rows.resize(fbCount);
			tables[j].cols.clear();
			tables[j].cols.resize(fbCount);
			tables[j].cellWidth.clear();
			tables[j].cellWidth.resize(fbCount);
			tables[j].colWidth.clear();
			tables[j].colWidth.resize(fbCount);
			tables[j].top.clear();
			tables[j].top.resize(fbCount);
		//	log(L"tables[%i].total =  %i", j,  tables[j].total);
		}  // for j
	}  // if( fbCount > 0 )


	for( int j = 0; j < totalPages; j++ ){
		i = pageTablesAreOn[j];
		index = 0;
		fbCount = 0;
			while( index > - 1 ){ 					
				index = pages[i].find( L"\\w", index) ;
				if(index  < 0 ) break;
				fbCount++;
				index++;
			}         
		tables[j].totalWords = fbCount;
		tables[j].cellText.clear();
		tables[j].cellText.resize( fbCount );
	//	log(L"tables[%i].totalWords =  %i",  j,  fbCount);
	}


// Count the size blocks
//			if( fbChange[ fbIndex + 2 ] != L's' && iswdigit(fbChange[fbIndex + 2 ])) {


	isSizeOnPage.clear();
	isSizeOnPage.resize( pageCount, -1 );
	fbCount = 0;
	for( int i = 0; i < pageCount; i++ ){
		index = pages[i].find(L"\\$");
		if( index != wstring::npos ){ 
			isSizeOnPage[i] = fbCount; 
			fbCount++;
//			log("isBoldOnPage[%i] = %i ", i ,isBoldOnPage[i]);
		}
	}
		sizes.clear();
		sizes.resize( pageCount );
		for( int i = 0; i < pageCount; i++ ){
		index = 0;
		fbCount = 0;
		while( index > - 1 ){  
			index = pages[i].find( L"\\$", index) ; //  have \$xx
			if(index  < 0 ) break; // check for trailing space
			if( !iswdigit(pages[i][ index + 2])){ 
				oops.clear();
				oops.append(pages[i], index, 20 );
				errorMessage( i, fbCount + 1, oops );}
			if( iswdigit(pages[i][ index + 2]) && pages[i][ index + 4] != L'\\'){ 
				if( pages[i][ index + 4 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
				}
			}
			fbCount++;
			index++;
			}
		sizes[i].total = fbCount;
		sizes[i].txStart.clear();
		sizes[i].txStart.resize(fbCount);
		sizes[i].txEnd.clear();
		sizes[i].txEnd.resize(fbCount);
		sizes[i].points.clear();
		sizes[i].points.resize(fbCount);
//		log(L"sizes[%i].total =  %i",  i,  sizes[i].total);
		}

// Count the names blocks
		names.clear();
		names.resize( pageCount );
		int fontNumber = 0;
		for( int i = 0; i < pageCount; i++ ){
			index = 0;
			fbCount = 0;
			while( index > - 1 ){  
				index = pages[i].find( L"\\f", index); 
				if(index  < 0 ) break;
				if( iswdigit(pages[i][ index + 2 ]) && pages[i][ index + 3 ] != L'\\'){ 
					if( pages[i][ index + 3 ] != L' ' ){
					oops.clear();
					oops.append( pages[i], index,  12 );
					log(oops);
					errorMessage( i, fbCount, oops );
					}
				}
				temp.clear(); 
				temp.append(pages[i], index + 2  , 1);
			//	log(temp);
				fontNumber = stoi(temp);
			//	log(fontNames[fontNumber]);
				fbCount++;
				index++;
			}
		names[i].total = fbCount;
		names[i].txStart.clear();
		names[i].txStart.resize(fbCount);
		names[i].txEnd.clear();
		names[i].txEnd.resize(fbCount);
		names[i].fontNo.clear();
		names[i].fontNo.resize(fbCount);
//		log(L"names[%i].total =  %i",  i,  names[i].total);
		}

// Count the format blocks - 

		int slashIndex;
		for( int i = 0; i < pageCount; i++ ){
		start = 0;
		index = 0;
		fbCount = 0;
		slashIndex = 0;
		while( index > -1 ){
			index = pages[i].find( L"\\", index);
	//		log(L"count format blocks");
			start = index;
			if(index  < 0 ) break;
		//	if( index == checkForEscape( index, i )){
				index = pages[i].find( L" ", index);
				temp.clear(); 
				temp.append(pages[i], start , index - start);
		//		log(temp);
				while( start > - 1 ){  // check for bad blocks  
					slashIndex = start;
					start = temp.find( L"\\", start);
					if(start  < 0 ) break;
					if( start - slashIndex > 4 ) { // should be no blocks bigger than \fsxx
						errorMessage( i, fbCount, oops );  // 
					}
					start++;
				}
				fbCount++;
		//	} // if( index == checkForEscape(index)){
		//	else index++;  // have escape sequence 
			index++;
		} // while( index > - 1 )
	//	log(L"page %i fbCount =   %i",i,fbCount);
		formattedWords[i] = fbCount;
	}

// Count the D2D blocks
//  d2Dblock[i].total is a count of the number of D2D routines so case 1: in getD2DforPage
//	is the first D2D routine in the book.
		d2Dblock.clear();
		d2Dblock.resize( pageCount );
		start = 0;
		for( int i = 0; i < pageCount; i++ ){
			index = 0;
			fbCount = 0;
			while( index > - 1 ){  
				index = pages[i].find( L"\\d2D", index); 
				if(index  < 0 ) break;
				if( pages[i][ index + 4 ] != L' ' && pages[i][ index + 4 ] != L'\\'){	
					oops.clear();
					oops.append( pages[i], index,  12 );
					errorMessage( i, fbCount, oops );
					}
				fbCount++;
				start++;
				index++;
			}
		d2Dblock[i].total = fbCount;
		if( d2Dblock[i].total != 0 ) d2Dblock[i].total = start;
		//	log(L"d2Dblock[%i].total =  %i",  i,  fbCount);
		}
	fbCount = 0;
	nameCount = 0;
	italCount = 0;
	boldCount = 0;
	sizeCount = 0;
	ulineCount = 0;
	strikeCount = 0;
	tableCount = 0;
	cellCount = 0;
	imageTally = 0;

	int windex;
	int tempIndex = 0;
	for( int i = 0; i < pageCount; i++ ){
		index = 0;
		for( int j = 0; j < formattedWords[i]; j++ ){  // number of format bocks on each page
			index = pages[i].find( L"\\", index);	// find fb start
			if( index == wstring::npos ) {log(L"break page %i",i); break;}
		//	if( index == checkForEscape(index, i)){
//			log(L"index  =    %i", index);
				windex = index;
				windex = pages[i].find( L" ", windex);
//				log(L"windex  =    %i", windex);
				fbChange.clear();
				fbChange.append( pages[i], index, windex - index );
			//		log(fbChange);
				if(index  >= 0 && windex >= 1 )
					{
					if( pages[i].size() >  windex  + 1 )
						pages[i].erase( index, windex - index + 1 );
					}
				setTextState( i, index );
		//}
		//	else{ index++;  j--; pages[i].erase( index,  1 );} // have escape char here
		//	index++;  // commented out 5-2-15		
		} // j loop
	//erase any remaining last fbs
	//tempIndex = 0;
	//tempIndex  = pages[i].find( L"\\", tempIndex );
	//log(L"page %i last / + 1  =    %c" ,i, pages[i][tempIndex + 1] );
	//log(L"page %i size =    %i", i, pages[i].size());
	//	if( tempIndex > 0 ) {pages[i].erase( tempIndex , pages[i].size() - tempIndex );}
	} // i loop

	makePage( NEXT_ID, NULL );
//	displayMenu( MESSAGE_BOX, MESSAGE_ID );
//	bMessage = TRUE;

}// end void ViewDW::loadResourceText(){
void ViewDW::setTextState(int pageIndex, int wordIndex){
#ifdef DEBUG_GB
//	log(L"ViewDW::setTextState(int pageIndex = %i, word# = %i, index %i)", 
//	pageIndex, wordNumber, wordIndex);
#endif

	int fbIndex;
	wstring temp;

// --------------------- Images  ----------------------

	int arrayIndex = isImageOnPage[pageIndex];
	if( imageCount > 0 && arrayIndex >= 0){
		fbIndex = fbChange.find(L"\\c");
		if( fbIndex != wstring::npos ){
			pageImages[arrayIndex].name.clear();
			pageImages[arrayIndex].name.append( fbChange, 2 , fbChange.size() - 14);
			log(pageImages[arrayIndex].name );
			temp.clear();
			temp.append( fbChange, fbChange.size() - 12 , 3);
			pageImages[arrayIndex].originX = stoi(temp);
			temp.clear();
			temp.append( fbChange, fbChange.size() - 9 , 3);
			pageImages[arrayIndex].originY = stoi(temp);
			temp.clear();
			temp.append( fbChange, fbChange.size() - 6 , 3);
			pageImages[arrayIndex].width = stoi(temp);
			temp.clear();
			temp.append( fbChange, fbChange.size() - 3 , 3);
			pageImages[arrayIndex].height = stoi(temp);
			imageTally++;			
			if( imageTally == imageCount ) imageCount = 0;
		}		
	}

// --------------------- Bolds  ----------------------

	arrayIndex = isBoldOnPage[pageIndex];
		if( arrayIndex > -1){
		fbIndex = fbChange.find(L"\\b");
		if( fbIndex != wstring::npos ){
			if( fbChange[ fbIndex + 2 ] != '0' ){
				bold[arrayIndex].txStart[boldCount] = wordIndex;
			//	log(L"Start bold block page %i index %i", pageIndex, wordIndex);
			}
			else{
				bold[arrayIndex].txEnd[boldCount ] = wordIndex;
			//	log(L"End bold block page %i index %i", pageIndex, wordIndex);
				boldCount++;
			}
			if( boldCount == bold[arrayIndex].total  )  boldCount= 0;				
		}
	}

// --------------------- Italics  ----------------------

	arrayIndex = isItalOnPage[pageIndex];
		if( arrayIndex > -1){
		fbIndex = fbChange.find(L"\\i");
		if( fbIndex != wstring::npos ){
			if( fbChange[ fbIndex + 2 ] != '0' ){
				italics[arrayIndex].txStart[italCount] = wordIndex;
			//	log(fbChange);
		//		log(L"Start italic block page %i index %i", pageIndex, wordIndex);
			}
			else{
				italics[arrayIndex].txEnd[italCount ] = wordIndex;
		//		log(L"End italic block page %i index %i", pageIndex, wordIndex);
			//	log(fbChange);
				italCount++;
			}
			if( italCount == italics[arrayIndex].total  ) italCount = 0;				
		}
	}

// --------------------- Tables  ----------------------
	//std::vector<int> indexTablesOnPage;
	//std::vector<int> pageTablesAreOn;
	arrayIndex = indexTablesOnPage[pageIndex];
	if( arrayIndex > -1){
	fbIndex = fbChange.find(L"\\z");
	if( fbIndex != wstring::npos ){
	//		log(L"Have table %i here", tableCount);
		temp.clear();
		temp.append( fbChange, fbIndex + 2, 2 );
		tables[arrayIndex].rows[tableCount] = stoi(temp);
		rows = tables[arrayIndex].rows[tableCount];
		temp.clear();
		temp.append( fbChange, fbIndex + 4, 2 );
		tables[arrayIndex].cols[tableCount] = stoi(temp);
		cols = tables[arrayIndex].cols[tableCount];
		colCount = 0;
		tableWordCount = 0;
		tables[arrayIndex].colWidth[tableCount].resize(tables[arrayIndex].cols[tableCount]);
		
		}
	fbIndex = fbChange.find(L"\\w");
	if( fbIndex != wstring::npos ){
		tables[arrayIndex].cellText[cellCount].append( fbChange, 2, fbChange.size() - 2);
	//	log(L"cellCount = %i  c_1 = %i, colCount = %i, tableCount = %i" ,cellCount,  cols, colCount, tableCount);
		if( cellCount <  cols ){
		tables[arrayIndex].colWidth[tableCount][ colCount] = tables[arrayIndex].cellText[ cellCount].size();
	//	log(L"tables[%i].colWidth[%i][%i] =%i", arrayIndex, tableCount, colCount,
	//		tables[arrayIndex].colWidth[tableCount][ colCount ]);
		}
		else {
//		log(L"cellCount = %i  c_1 = %i, colCount = %i " ,cellCount,  c_1, colCount);

		if(tables[arrayIndex].cellText[cellCount].size() > tables[arrayIndex].colWidth[tableCount][ colCount ] )
		{
			tables[arrayIndex].colWidth[tableCount][ colCount] = tables[arrayIndex].cellText[cellCount].size();
		}
	//	log(L"tables[ arrayIndex ].cellText[ cellCount ].size()");
	//	log(L"tables[%i].cellText[%i].size() = ", arrayIndex, cellCount);
//			tables[arrayIndex].cellText[cellCount].size());

	//	log(L"tables[arrayIndex].colWidth[tableCount][ colCount ]");
	//	log(L"tables[%i].colWidth[%i][ %i] = %i", arrayIndex, tableCount, colCount, tables[arrayIndex].colWidth[tableCount][ colCount]);
		
		}  // else
		colCount++;
		if( colCount == cols ) colCount = 0;
		cellCount++;			
		if (cellCount  ==  tables[arrayIndex].totalWords ) cellCount = 0;
		tableWordCount++;
		if (tableWordCount  ==  rows*cols){ tableWordCount = 0; tableCount++; }
		if (tableCount  ==  tables[arrayIndex].total) tableCount = 0;

		}// if( fbIndex != wstring::npos ){
	}

// ---------------------  Sizes  ----------------------

	if( sizes[pageIndex].total > 0 ){
		fbIndex = fbChange.find(L"\\$"); //
		if( fbIndex != wstring::npos ){
//			log(L"Have font size here");
			temp.clear();
			temp.append( fbChange, fbIndex + 2, 2  );
		//	log( temp );
			sizes[pageIndex].points[sizeCount] = stoi(temp)/2;
	//		log( L"sizes[pageIndex].points[sizeCount] =  %i", sizes[pageIndex].points[sizeCount]);
//			log( "sizeCount =  %i", sizeCount);
			sizes[pageIndex].txStart[sizeCount] = wordIndex;
//	log("page[%i}start[%i] = %i", pageIndex, sizeCount, sizes[pageIndex].txStart[sizeCount]);
//	log("pages[%i][%i] = %c",pageIndex , wordIndex, pages[pageIndex][wordIndex]);
			if( sizeCount > 0){	
				sizes[pageIndex].txEnd[sizeCount - 1 ] = sizes[pageIndex].txStart[sizeCount] ;
	//			sizes[pageIndex].txEnd[sizeCount - 1 ] = sizes[pageIndex].txStart[sizeCount] - 1;
//	log("page[%i}end[%i] = %i", pageIndex, sizeCount - 1,
//				sizes[pageIndex].txEnd[sizeCount - 1 ]);

			}
			if( sizeCount == sizes[pageIndex].total - 1 ){	//end of count
				sizes[pageIndex].txEnd[sizeCount]  =  pages[pageIndex].size() ;
//	log("page[%i}end[%i] = %i", pageIndex, sizeCount, sizes[pageIndex].txEnd[sizeCount]);
				sizeCount = 0;
			}
			else sizeCount++;
		}
	}
//	log(L"sizeCount = %i", sizeCount);

// ---------------------  Names  ----------------------
	if( names[pageIndex].total > 0 ){
		fbIndex = fbChange.find(L"\\f");
		if( fbIndex != wstring::npos ){
			if( fbChange[ fbIndex + 2 ] != L's' && iswdigit(fbChange[fbIndex + 2 ])) {
	//			log(L"Font name here");
				temp.clear();
				temp.append( fbChange, fbIndex + 2, 1  );
			//	log( temp );
				names[pageIndex].fontNo[nameCount] = stoi(temp);
			
		//		log( L"nameCount =  %i", nameCount);
		//		log(fontNames[names[pageIndex].fontNo[nameCount]]);
				names[pageIndex].txStart[nameCount] = wordIndex;
//	log("page[%i}Nstart[%i] = %i", pageIndex, nameCount, names[pageIndex].txStart[nameCount]);
				if( nameCount > 0){			
				names[pageIndex].txEnd[nameCount - 1 ] = names[pageIndex].txStart[nameCount]  ;

//	log("page[%i}Nend[%i] = %i", pageIndex, nameCount - 1,
//		names[pageIndex].txEnd[nameCount - 1]);
				}
				if( nameCount == names[pageIndex].total - 1 ){ // at end of page
				names[pageIndex].txEnd[nameCount]  =  pages[pageIndex].size();
//	log("page[%i}Nnend[%i] = %i", pageIndex, nameCount, names[pageIndex].txEnd[nameCount]);
				nameCount = 0;
				}
				else nameCount++;
			}
		}
	}
//	log(L"nameCount = %i", nameCount);	

// ---------------------  Underlines  ----------------------
	if( ulines[pageIndex].total > 0 ){
		fbIndex = fbChange.find(L"\\l");
		if( fbIndex != wstring::npos ){
			if( fbChange[ fbIndex + 2 ] != '0' ){ 
			//	log(L"Have underline here = %i, wordIndex = %i", ulineCount, wordIndex);		
				ulines[pageIndex].txStart[ulineCount] = wordIndex;}
			else{
			//	log(L"Have no underline here = %i, wordIndex = %i",
			//	ulineCount - 1, wordIndex);
				ulines[pageIndex].txEnd[ulineCount - 1 ] = wordIndex;
				}
			ulineCount++;
			if( ulineCount  ==  ulines[pageIndex].total ) ulineCount = 0;
		}
	}
//	log(L"ulineCount = %i", ulineCount);

// ---------------------  Strike-throughs  -----------------
	if( strikes[pageIndex].total > 0 ){
		fbIndex = fbChange.find(L"\\s");
		if( fbIndex != wstring::npos ){
			if( fbChange[ fbIndex + 2 ] != '0' ){ 
			//	log(L"Have strike-through here = %i, wordIndex = %i", strikeCount, wordIndex);	
				strikes[pageIndex].txStart[strikeCount] = wordIndex;}
			else{
		//		log(L"Have no strike-through here = %i, wordIndex = %i",
		//			strikeCount - 1, wordIndex);
				strikes[pageIndex].txEnd[strikeCount - 1 ] = wordIndex;
				}
			strikeCount++;
			if( strikeCount  ==  strikes[pageIndex].total ) strikeCount = 0;
		}
	}
//	log(L"strikeCount = %i", strikeCount);

}
// Read the header and  write the font names into vector<wstring> fontNames[ number of fonts ]
void ViewDW::readFontTable()
{
#ifdef DEBUG_GB
	log(L"void ViewDW::readFontTable()");
#endif
// First find the end of the fonttbl
// First append header into wstring temp and read that instead of whole file

	wstring temp;
	int endIndex = 0;
	int startIndex = 0;
	startIndex = book.find( L"\\fonttbl", startIndex);
	endIndex = book.find( L"\\endfontbl", endIndex);
	temp.append(book, startIndex, endIndex - startIndex);
//	log(temp);

// Find number of fonts fontCount

	endIndex = 0;
	int fontCount = 0;		// position of last $ token
	while( endIndex  > -1 )
	{
		endIndex = temp.find( L"charset", endIndex);
		if( endIndex < 0 ) break;
		fontCount++;	endIndex++;
	}
//	log(L"fontCount = %i", fontCount);

// Read font names into vector<wstring> fontNames;
	fontNames.clear();
	fontNames.resize(fontCount);
	startIndex = 0;
	endIndex = 0;
	for( int i = 0; i < fontCount; i++ ){
		startIndex = temp.find(L"fcharset", startIndex) + 10;
		endIndex = startIndex;
		endIndex = temp.find(L";}", endIndex);
		fontNames[i].clear();
		fontNames[i].append( temp, startIndex, endIndex -  startIndex);
//		log(fontNames[i]);
		startIndex++, endIndex++;
	}

}

void ViewDW::setTextLayout(int pageIndex, int side){
#ifdef DEBUG_GB
//	log(L"ViewDW::setTextLayout() pageIndex = %i", pageIndex);
#endif


//	float width  = pTextLayout_->GetMaxWidth();
//	float height = pTextLayout_->GetMaxHeight();
//		log(L"ViewDW::setTextLayout() width = %f", width);

	HRESULT hr = S_OK;
	DWRITE_TEXT_RANGE tRange;
	tRange.startPosition = 0;
	tRange.length = pnum.size();
	pPageNumberLayout_->SetFontSize( 12.0f, tRange);
	//tRange.length = find.size();
	//pFindLayout_->SetFontSize( 12.0f, tRange);



// ---------------------  Italics  --------------------

	if( isItalOnPage[pageIndex] > -1 ){
		for( int i = 0; i < italics[isItalOnPage[pageIndex]].total; i++ ){
		tRange.startPosition = italics[isItalOnPage[pageIndex]].txStart[i];
		tRange.length = italics[isItalOnPage[pageIndex]].txEnd[i] - 
							italics[isItalOnPage[pageIndex]].txStart[i];
//		log(L"italics[%i].length[%i] = %i" , isItalOnPage[pageIndex], i, tRange.length);
		if( side == LEFT_PAGE ) pTextLayout_->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, tRange);
		if( side == RIGHT_PAGE ) pRightLayout_->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, tRange);
		}
	}

// ---------------------  Bolds  ----------------------
	if( isBoldOnPage[pageIndex] > -1 ){
		for( int i = 0; i < bold[isBoldOnPage[pageIndex]].total; i++ ){
		tRange.startPosition = bold[isBoldOnPage[pageIndex]].txStart[i];
		tRange.length = bold[isBoldOnPage[pageIndex]].txEnd[i] - 
							bold[isBoldOnPage[pageIndex]].txStart[i];
//		log(L"testBold[%i].length[%i] = %i" , isBoldOnPage[pageIndex], i, tRange.length);
//		pTextLayout_->SetFontWeight(DWRITE_FONT_WEIGHT_BLACK, tRange);
		if( side == LEFT_PAGE ) pTextLayout_->SetFontWeight(DWRITE_FONT_WEIGHT_EXTRA_BOLD, tRange);
		if( side == RIGHT_PAGE ) pRightLayout_->SetFontWeight(DWRITE_FONT_WEIGHT_EXTRA_BOLD, tRange);
		}
	}

// ---------------------  Underlines  ------------------

	for( int i = 0; i < ulines[pageIndex].total ; i++ ){
		tRange.startPosition = ulines[pageIndex].txStart[i];
		tRange.length = ulines[pageIndex].txEnd[i] - ulines[pageIndex].txStart[i];
//		log(L"ulines[%i].length[%i] = %i" , pageIndex, i, tRange.length);
		if( side == LEFT_PAGE ) pTextLayout_->SetUnderline(TRUE, tRange);
		if( side == RIGHT_PAGE ) pRightLayout_->SetUnderline(TRUE, tRange);
	}

// ---------------------  Strike-throughs  -------------

	for( int i = 0; i < strikes[pageIndex].total ; i++ ){
		tRange.startPosition = strikes[pageIndex].txStart[i];
		tRange.length = strikes[pageIndex].txEnd[i] - strikes[pageIndex].txStart[i];
//		log(L"strikes[%i].length[%i] = %i" , pageIndex, i, tRange.length);
		if( side == LEFT_PAGE ) pTextLayout_->SetStrikethrough(TRUE, tRange);
		if( side == 2 ) pRightLayout_->SetStrikethrough(TRUE, tRange);
//		pTextLayout_->SetDrawingEffect(pDarkGrayBrush_, tRange);
	}

// ---------------------  Sizes  ----------------------

	for( int i = 0; i < sizes[pageIndex].total ; i++ ){
		tRange.startPosition = sizes[pageIndex].txStart[i];
		tRange.length = sizes[pageIndex].txEnd[i] - sizes[pageIndex].txStart[i];
//		log(L"sizes[%i].length[%i] = %i, range = %i" , pageIndex, i, tRange.length
//			, tRange);
//		log(L"sizes[pageIndex].points[i] = %f" , sizes[pageIndex].points[i]);
		if( side == LEFT_PAGE ) pTextLayout_->SetFontSize(sizes[pageIndex].points[i], tRange);
		if( side == RIGHT_PAGE ) pRightLayout_->SetFontSize(sizes[pageIndex].points[i], tRange);
	}

// ---------------------  Names  ----------------------

	for( int i = 0; i < names[pageIndex].total ; i++ ){
		tRange.startPosition = names[pageIndex].txStart[i];
		tRange.length = names[pageIndex].txEnd[i] - names[pageIndex].txStart[i];
//		log(L"names[%i].length[%i] = %i" , pageIndex, i, tRange.length);
		font = fontNames[names[pageIndex].fontNo[i]];
		if( side == LEFT_PAGE ) pTextLayout_->SetFontFamilyName( (PWSTR)font.c_str(), tRange);
		if( side == RIGHT_PAGE ) pRightLayout_->SetFontFamilyName( (PWSTR)font.c_str(), tRange);
	}
}

HRESULT ViewDW::makePage( int key, int number ){
#ifdef DEBUG_GB
	log(L"ViewDW::makePage(int key) pageNumber = %i, key=%i", number, key);
#endif
	
	HRESULT hr = S_OK;
	     if( key != FIND_MENU ) findIndex = -1;
	     if( key == VK_UP && pageNumber > 1 ) pageNumber--;			// Prior
	else if( key == VK_DOWN	 && pageNumber < pageCount ) pageNumber++;	// Next	
	else if( key == VK_HOME )	pageNumber = 1;							// Home
	else if( key == TOC_ID )	pageNumber = 4;							// Table of Contents
	else if( key == INDEX_ID )	pageNumber =  pageCount - 3;			// Index
	else if( key == VK_END )	pageNumber =  pageCount;				// End
//	else if( key == BACK_ID )	// <-	To be implemented				// Back
//	else if( key == FOR_ID )	// ->									// Forward
	else if( key == FIND_MENU &&  number != GET_TEXT) pageNumber =  number + 1;

	// User hit enter in active menu: display pageNumber
	else if( key == PAGE_MENU ){ 
		bMenu = FALSE;
		if( number > 0 ) pageNumber =  number; }
//	else return hr;  // July reformation

//	pnum = to_wstring( pageNumber - 1);  
//	page.clear();
//	page.append(pages[pageNumber - 1]);

	return hr;
}

void  ViewDW::textMetrix( int mouseX, int  mouseY,	D2D1_SIZE_F DWSize ){ //caller: from ViewDW::drawDW
#ifdef DEBUG_GB
	log(L"ViewDW::textMetrix( int mouseX %i, int  mouseY %i )",  mouseX, mouseY);
#endif
	int pageIndex;
	gbRightPage  = FALSE;
	BOOL trail  = FALSE;
	gbInside = FALSE;      //  Also us as flag of a GET_TEXT case in findstring 
	DWRITE_HIT_TEST_METRICS txMetrix = {0};

	find.clear();
	find = L"Find: ";

	if(	float(( mouseX*96 )/dpiScaleY_) < DWSize.width/2 ){
		pTextLayout_->HitTestPoint( float(( mouseX*96 )/dpiScaleY_) - g_siMargin, 
			float(( mouseY*96 )/dpiScaleY_), &trail, &gbInside, &txMetrix);}
	else if( currentMode == DW_MODE ){
		pRightLayout_->HitTestPoint( float(( mouseX*96 )/dpiScaleY_) - g_siMargin/2 -
			DWSize.width/2,	float(( mouseY*96 )/dpiScaleY_), &trail, &gbInside, &txMetrix);
		gbRightPage = TRUE;}
	if( gbInside ){

		int indexRight;
		int indexLeft;

		indexRight = indexLeft = txMetrix.textPosition;
		log(L"indexRight = txMetrix.textPosition  = %i;", txMetrix.textPosition);
		if( gbRightPage ) pageIndex = pageNumber;
		else pageIndex = pageNumber - 1;
		while( iswalnum(pages[pageIndex][indexRight]) > 0 ) indexRight++;
		while( iswalnum(pages[pageIndex][indexLeft]) > 0 ) indexLeft--;

		if( indexRight != wstring::npos && indexLeft != wstring::npos && indexRight > indexLeft )		{
			//find.clear();
			//find = L"Find: ";
			find.append( pages[pageIndex], indexLeft + 1, indexRight - indexLeft - 1 );
			findIndex = indexLeft + 1;
		}
		else findIndex = -1;
		//log(L"find.size = %i",find.size());
		//log(find);
	}

	getTextX = -1;  // flag drawDW to not call this function unless text is clicked on
}
void ViewDW::getFindRect( int posLeft, int posRight, int side ){ // caller: ControllerDW::lButtonDown
#ifdef DEBUG_GB
	log(L"void ViewDW::getFindRect()");
#endif

	DWRITE_HIT_TEST_METRICS findMetrix = {0};
	float hitX = 0; float hitY = 0;

	if( gbRightPage ){
		pRightLayout_->HitTestTextPosition( posLeft, FALSE, &hitX, &hitY, &findMetrix);
		gfindRect.left  = hitX + g_siMargin;
		pRightLayout_->HitTestTextPosition( posRight, TRUE, &hitX, &hitY, &findMetrix);	
		gfindRect.right  = hitX + g_siMargin;	
	}
	else { 
		pTextLayout_->HitTestTextPosition( posLeft, FALSE, &hitX, &hitY, &findMetrix);
		gfindRect.left  = hitX + g_siMargin;
		pTextLayout_->HitTestTextPosition( posRight, TRUE, &hitX, &hitY, &findMetrix);	
		gfindRect.right  = hitX + g_siMargin;	
	}
	gfindRect.top  = findMetrix.top;
	gfindRect.bottom  = findMetrix.top + findMetrix.height;
}
int ViewDW::findString( int direction ){ // caller: ControllerDW::lButtonDown
#ifdef DEBUG_GB
	log(L"int ViewDW::findString()" );
#endif
	int pageIndex;
	static int previousPage = pageNumber;
	static int index = 0;
	if( gbInside){ index = findIndex; gbInside = FALSE; }  //  Have GET_TEXT string
	if( gbRightPage ){ pageIndex = pageNumber; log(L"gbRightPage"); 
			previousPage = pageNumber + 1;}
	else { pageIndex = pageNumber - 1; log(L"!gbRightPage");
			previousPage = pageNumber;} 

	if (find.size() < 7 ) { log(L"find is empty" ); return pageNumber; }

	else{
		std::wstring temp;
		temp = find;
		temp.erase( 0, 6);
		if( direction == FIND_PREVIOUS ){
			for( int i = pageIndex; i > -1; i-- ){
				if( previousPage != pageNumber ){ index = pages[i].size();
				previousPage = pageNumber; }
				else if( index > 0 ) index--; 
				index = pages[i].rfind( temp, index );
				if( index == wstring::npos ) findIndex = - 1;
				else { findIndex = index;
				return i; }
			} // for( int i
		} // if( direction = FIND_PREVIOUS ){
		else if( direction == FIND_NEXT ){
			if( currentMode == GL_MODE ){
				for( int i = pageIndex; i < pageCount; i++ ){
	log(L"previousPage %i  pageNumber  %i pageIndex %i",previousPage, pageNumber, pageIndex );
	//				if( previousPage != pageNumber ){
					if( previousPage != pageIndex + 1 ){
	//					index = 0; previousPage = pageNumber; }
						index = 0; previousPage = pageNumber; }
					else if( index > 0 ) index++; 
					index = pages[i].find( temp, index );
					if( index == wstring::npos ){ findIndex = - 1; index = 0; }
					else { findIndex = index;
					log(L"return i  %i ",i );
					return i; }
				} // for( int i
			} // if( currentMode == GL_MODE )
			else if( currentMode == DW_MODE ){
				for( int i = pageIndex; i < pageCount; i++ ){
	log(L"previousPage %i  pageNumber  %i pageIndex %i",previousPage, pageNumber, pageIndex );
	//				if( previousPage != pageNumber ){
					if( previousPage != pageIndex + 1 ){
	//					index = 0; previousPage = pageNumber; }
						index = 0; previousPage = pageNumber; }
					else if( index > 0 ) index++; 
					index = pages[i].find( temp, index );
					if( index == wstring::npos ){ findIndex = - 1; index = 0; }
					else { findIndex = index;
					log(L"return i  %i ",i );
					return i; }
				} // for( int i
			} // if( currentMode == GL_MODE )
		} // if( direction = FIND_NEXT )

		return pageIndex;
	} // else
}
void ViewDW::getChar(HWND handle, WPARAM findChar,  int lowWord, int highWord){
#ifdef DEBUG_GB
	log(L"void ViewDW::getChar( WPARAM findChar = %i,  int lowWord = %i, int highWord = %i)", findChar, lowWord, highWord);

#endif
	int digit;
	static int pageNumber = -1;
	static int charCount = 0;
	static int pageDigit[3];

//	int pageNumberEntered;
//called from int ControllerMain::getChar(  WPARAM message,  int lowWord, int highWord)
//when user hits a key on the KB -> triggers a WM_CHAR from windows

//  the keys 0 - 9  send the values 48 thru 57 in WPARAM findChar
//  A backspace sends 8 in findChar

// charCount is init to 0 then reset to 0 when user presses enter in page menu

	if( bPage ){
		if( findChar == 28 && pageNumber > -1){
			log(L"User entered %i"), pageNumber;
			charCount = 0;
			pageDigit[0] = pageDigit[1] = pageDigit[1] = 0;
			makePage( PAGE_MENU, pageNumber + 1 );
			pageNumber = -1;
			return;
			}
		if ( findChar == 8 ){
			log(L"User hit <-Backspace");
			if( charCount > 0){
				log(L"User hit <-Backspace remove last digit");
				charCount--;
				}	
			else{
				log(L"User hit <-Backspace but nothing to remove, just return");
				return;  // have backspace; remove last digit
				}
			}
		if( findChar > 47 && findChar < 58 ){
			log(L"User entered a digit 0 - 9");
			digit = findChar - 48;
			}	
		else if ( findChar != 8 ){
			log(L"User entered a non-digit character ignore and return ");
			return;
		}
		if( charCount < 3 ){
			if( findChar == 8  ) pageDigit[charCount] = 0;
			else{ 
				pageDigit[charCount] = digit; 
				charCount++;} 
			}
		if( charCount == 0 ) pageNumber = -1; // tell display menu to display "?"
		else if( charCount == 1 ) pageNumber = pageDigit[0] ;
		else if( charCount == 2 ) pageNumber = 10*pageDigit[0] + pageDigit[1];
		else if( charCount == 3 ) pageNumber = 100*pageDigit[0] + 10*pageDigit[1] + pageDigit[2];
		log(L"pageNumber = %i", pageNumber);
		if( pageNumber >= pageCount) pageNumber = pageCount - 1; // Catch attemps to go past end of book.
		displayMenu( PAGE_NUMBER_MENU, pageNumber);
		log(L"pageNumber = %i", pageNumber);
		InvalidateRect( handle, NULL, FALSE );
	} // if 
	return;
}

void ViewDW::getCharGL( WPARAM findChar ){
#ifdef DEBUG_GB
	log(L"void ViewDW::getCharGL(WPARAM findChar = %i )", findChar );
//called from int ControllerMain::getChar(  message, int menuItem, int x, int y)
#endif
	if ( findChar == 8 && find.size() > 6 ){ // i.e. find string =  L"Find: "
log(L"findChar == 8 && find.size() > 6 )");

	find.erase(find.size() - 1, 1);}
	else if (findChar != 8 ){
	wstring temp;
	temp = findChar; find.append(temp);}
	log(find);
}
void ViewDW::colorBox(D2D1_SIZE_F size )
{
#ifdef DEBUG_GB
	log(L"void ViewDW::colorBox()" );
#endif
}
void ViewDW::colorSliders(D2D1_SIZE_F size, int color , int mouseY){
#ifdef DEBUG_GB
	log(L"void ViewDW::colorSliders(D2D1_SIZE_F size )" );
#endif
	D2D1_ELLIPSE redButton, greenButton, blueButton;
	D3DCOLORVALUE sliderColor = {1.0f, 0.0f, 0.0f, 1.0f};
//	sliderColor.a = 1.0f;
//	typedef struct D3DCOLORVALUE
//{
//    FLOAT r;
//    FLOAT g;
//    FLOAT b;
//    FLOAT a;
	redButton.point.x = 10.5*size.width/(iMenuSize + 1);
//	redButton.point.y =  ( mouseY*96 )/dpiScaleY_; // + 8.0f  
	redButton.point.y =  ( mouseY )/dpiScaleY_; // + 8.0f  

	log(L"colorSliders height %f  mouseY %i" , size.height, mouseY);
	redButton.radiusX = 12.0f;
	redButton.radiusY = 6.0f;
	greenButton.point.x = redButton.point.x + size.width/(iMenuSize + 1);;
	greenButton.point.y = 8.0f;
	greenButton.radiusX = 12.0f;
	greenButton.radiusY = 6.0f;
	blueButton.point.x = greenButton.point.x + size.width/(iMenuSize + 1);;
	blueButton.point.y = 8.0f;
	blueButton.radiusX = 12.0f;
	blueButton.radiusY = 6.0f;
//sliderColor.r += mouseY*0.0078125 ;//  255 - mouseY;
//sliderColor.g += mouseY*0.0078125 ;//  255 - mouseY;
	sliderColor.r = 1.0f -  mouseY*0.0038125 ;//  255 - mouseY;
	pRedBrush_->SetColor(sliderColor);  //0xff0000

//	pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));  //0xff0000
	pRT_->FillEllipse( &redButton, pRedBrush_);
	pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Green));
	pRT_->FillEllipse( &greenButton, pRedBrush_);
	pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::LightBlue));
	pRT_->FillEllipse( &blueButton, pRedBrush_);


//			pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Green));

}
D2D1_COLOR_F ViewDW::BGcolor( int page )
{
#ifdef DEBUG_GB
//	log(L"D2D1_COLOR_F ViewDW::BGcolor( int page )" );
#endif
	float colFactor = (float)(((float)page)/((float)pageCount));
	float brushFactor =  0.05f;
	float grayFactor =  0.2f;
	D2D1_COLOR_F bgColor;
	D2D1_COLOR_F bgBrushColor;
//	UINT red;
	CHAR red;

	red = colFactor*255;
	int x = sizeof(red);

	 if( colFactor > 0.833 ){
//		log(L"colFactor > 5/6 %f", colFactor );
		bgColor.r = 1.0f;
		bgColor.g = 1.0f - ((1.000f - colFactor) / 0.166);
		bgColor.b = 0.0f;}
	else if( colFactor > 0.666 ){
//		log(L"colFactor > 4/6 %f", colFactor );
		bgColor.r = ((0.833f - colFactor) / 0.166);
		bgColor.g = 1.0f;
		bgColor.b = 0.0f;}
	else if( colFactor > 0.500 ){
//		log(L"colFactor > 3/6 %f", colFactor );
		bgColor.r = 0.0f;
		bgColor.g = 1.08f;
		bgColor.b = 1.0f - ((0.666f - colFactor) / 0.166);}
	 else if( colFactor > 0.333 ){
//		log(L"colFactor > 2/6 %f", colFactor );
		bgColor.r = 0.0f;
		bgColor.g = ((0.500f - colFactor) / 0.166);
		bgColor.b = 1.0f;}
	 else if( colFactor > 0.166 ){
	//	log(L"colFactor > 1/6 %f", colFactor );
		bgColor.r = ((0.333f - colFactor) / 0.166);
		bgColor.g = 0.0f;
		bgColor.b = 1.0f;}
	 else if( colFactor > 0.000 ){
//		log(L"colFactor > 0/6 %f", colFactor );
		bgColor.r = 0.6f*1.0f + grayFactor;
		bgColor.g = 0.0f + grayFactor;
		bgColor.b = 0.2f*(1.0f - ((0.166f - colFactor) / 0.166)) + grayFactor;
		bgColor.g = bgColor.b;
//		bgColor.b = 0.2f*(1.0f - ((0.166f - colFactor) / 0.166)) + grayFactor;	 
	 }

	//bgColor.r = colFactor;
	//bgColor.g = colFactor;
	//bgColor.b = colFactor;
	//bgColor.a = colFactor;

	bgBrushColor.r = colFactor + brushFactor;
	bgBrushColor.g = colFactor + brushFactor;
	bgBrushColor.b = colFactor + brushFactor;
	bgBrushColor.a = 1.0f;

//	pDarkGrayBrush_->SetColor( bgBrushColor );

	bgBrushColor.r = 1.0f - colFactor;
	bgBrushColor.g = 1.0f - colFactor;
	bgBrushColor.b = 1.0f - colFactor;
	bgBrushColor.a = 1.0f;
//	pBlackBrush_->SetColor( bgBrushColor );
	brushFactor = bgColor.r + bgColor.g + bgColor.b;
	colFactor = 3 - brushFactor;
//	log(L"page color = %f  text color = %f red = %i  size, %i",  brushFactor, colFactor, red, x);
	bgColor.r = 0.1f;
	bgColor.g = 0.1f;
	bgColor.b = 0.1f;
	bgColor.a = 1.0;


	return bgColor;
}
void ViewDW::colorWheel( D2D1_SIZE_F size )
{
#ifdef DEBUG_GB
	log(L"void ViewDW::colorWheel()" );
#endif
	//D2D1_COLOR_F bgColor;
	//bgColor.r = 0.4f;
	//bgColor.g = 0.4f;
	//bgColor.b = 0.4f;
	//bgColor.a = 0.0f;
    //FLOAT position;
   //D2D1_COLOR_F color;
   // FLOAT r;
   // FLOAT g;
   // FLOAT b;
   // FLOAT a;
//static CHOOSECOLOR cc ; 
//static COLORREF crCustColors[ 16] ;


	D2D1_POINT_2F center;
	center.x = size.width/2;
	center.y = size.height/2;
	for( float i = 0; i < 360; i++){
			float red = 0;
			float green = 0;
			float blue = 0;
			if( i > 240  ){
				red = ( i - 240 )/120;
				green = ( 360 - i )/120;
				blue = 0.0f;
			}
			if( i <= 240 && i > 120  ){
				red = 0.0f;
				green = ( i - 120 )/120;
				blue =( 240 - i )/120;;
			}
			if( i <= 120  ){
				red = (120 - i )/120;
				green = 0.0f;
				blue =  i/120;;
			}
			colWheelStop[0].position  =  0.0f;
			colWheelStop[1].position  =  1.0f;
//			colWheelStop[2].position  =  0.0f;
			colWheelStop[1].color.r  =  red;
			colWheelStop[1].color.g  =  green;
			colWheelStop[1].color.b  =  blue;
			colWheelStop[1].color.a  =  1.0f;
			colWheelStop[0].color.r  =  0.0f;
			colWheelStop[0].color.g  =  0.0f;
			colWheelStop[0].color.b  =  0.0f;
			colWheelStop[0].color.a  =  1.0f;
			//colWheelStop[2].color.r  =  0.0f;
			//colWheelStop[2].color.g  =  1.0f;
			//colWheelStop[2].color.b  =  0.0f;
			//colWheelStop[2].color.a  =  1.0f;

			//if( i float red = 
//		for( int j = 0; j < 7; j++){
			//colWheelStop[0].position  =  1.0f;
			//colWheelStop[1].position  =  0.5f;
			//colWheelStop[2].position  =  0.0f;
			//colWheelStop[0].color.r  =  1.0f;
			//colWheelStop[0].color.g  =  0.0f;
			//colWheelStop[0].color.b  =  0.0f;
			//colWheelStop[0].color.a  =  1.0f;
			//colWheelStop[1].color.r  =  0.5f;
			//colWheelStop[1].color.g  =  0.5f;
			//colWheelStop[1].color.b  =  0.0f;
			//colWheelStop[1].color.a  =  1.0f;
			//colWheelStop[2].color.r  =  0.0f;
			//colWheelStop[2].color.g  =  1.0f;
			//colWheelStop[2].color.b  =  0.0f;
			//colWheelStop[2].color.a  =  1.0f;

//		}

    const D2D1::Matrix3x2F rot = D2D1::Matrix3x2F::Rotation(i);
    const D2D1::Matrix3x2F trans = D2D1::Matrix3x2F::Translation(size.width/2, size.height/2);

    pRT_->SetTransform( rot*trans );

	//center.x = 0;
	//center.y = 0;
//	pRT_->SetTransform( D2D1::Matrix3x2F::Rotation( 50, center ));
//	pRT_->SetTransform( D2D1::Matrix3x2F::Translation( size.width/2, size.height/2));
	CreateGradientResources();
	pRT_->FillGeometry(pPathGeometry, pLinearGradientBrush);
	}
//	pRT_->FillGeometry(pPathGeometry, pBlackBrush_);
}
HRESULT ViewDW::CreateGradientResources()
{
#ifdef DEBUG_GB
	log(L"ViewDW::CreateGradientResources(HWND handle)");
#endif
	HRESULT hr;
    ID2D1GradientStopCollection *pGradientStops = NULL;
    // Create a linear gradient.
/*
    FLOAT position;
    D2D1_COLOR_F color;

    static const D2D1_GRADIENT_STOP stops[] =
    {
        {   1.f,  { 0.f, 1.f, 1.f, 0.25f }  },
        {   0.f,  { 1.0f, 0.f, 1.f, 1.f }  },    // blue?
    };
    hr = pRT_->CreateGradientStopCollection(
        stops,
        ARRAYSIZE(stops),
        &pGradientStops
        );
*/
    hr = pRT_->CreateGradientStopCollection(
        colWheelStop,
        ARRAYSIZE(colWheelStop),
        &pGradientStops
        );
    if (SUCCEEDED(hr))
    {	SafeRelease(&pLinearGradientBrush);
        hr = pRT_->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(-150, 0),
                D2D1::Point2F(0, 150)),
            D2D1::BrushProperties(),
            pGradientStops,
            &pLinearGradientBrush
            );
        pGradientStops->Release();
    }
 
	if( FAILED( hr ) ) return hr;

    return S_OK;
}
/*
HRESULT ViewDW::drawPaper()
{
#ifdef DEBUG_GB
	log(L"ViewDW::drawPaper()" );
#endif

//	float margin;
    D2D1_SIZE_F sizeRT= pRT_->GetSize();
	margin = 0.05*sizeRT.width;
//		log(L"drawPaper  %f  %f", sizeRT.width,  sizeRT.height);
    ID2D1GeometrySink *pSink = NULL;
        // Create a path geometry.
		SafeRelease(&pPathGeometry);
	HRESULT	 hr = pD2DFactory_->CreatePathGeometry(&pPathGeometry); 
    if (SUCCEEDED(hr))
    {
        // Use the geometry sink to write to the path geometry.
        hr = pPathGeometry->Open(&pSink);
    }
    if (SUCCEEDED(hr))
    {
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);

        pSink->BeginFigure(
            D2D1::Point2F(margin, margin),
            D2D1_FIGURE_BEGIN_FILLED   //  D2D1_FIGURE_BEGIN_HOLLOW = 1,
            );

        pSink->AddLine(D2D1::Point2F(sizeRT.width - margin, margin));
        pSink->AddLine(D2D1::Point2F(sizeRT.width - margin, sizeRT.height - margin));
        pSink->AddLine(D2D1::Point2F(margin, sizeRT.height - margin));
        pSink->AddLine(D2D1::Point2F(margin, margin));
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED); 
        hr = pSink->Close();
    }
   
    SafeRelease(&pSink);

	return hr = S_OK;

}
*/

void ViewDW::animateControl(WPARAM button, HWND handle,  int x , int y ){

	// Called when doing the Cartesian CS D2D Demo, or getD2DforPage routines in general
#ifdef DEBUG_GB							
	log(L"ViewDW::animateControl(  HWND handle,  int  x = %i)", x);
#endif

	if( button == 1  ){ // WM_LBUTTONDOWN
		bLeftButtonDown = TRUE;
		bLeftButtonUp= FALSE;
	}
	else if( button == 0 ){ // WM_LBUTTONUP
		bLeftButtonDown = FALSE;
		bLeftButtonUp = TRUE;
	}
	else if( button == 2 ){ // WM_LBUTTONUP
		bRightButtonDown = TRUE;
		bRightButtonUp = FALSE;
	}
		InvalidateRect( handle, NULL, FALSE );
}
void ViewDW::labelGraphics(const WCHAR *string,  D2D1_POINT_2F loc, D2D1_COLOR_F color, float size, int length ){
#ifdef DEBUG_GB							
//	log(L"ViewDW::labelGraphics(const WCHAR *string,  D2D1_POINT_2F loc ");
#endif
	DWRITE_TEXT_RANGE range; range.startPosition = 0; range.length = length + 1;

	SafeRelease(&pGraphicMenuLayout_);
	//pDWriteFactory_->CreateTextLayout( string, 1, pVectorFormat_, 20, 20,
	//			&pGraphicMenuLayout_ );
	pDWriteFactory_->CreateTextLayout( string, length, pVectorFormat_,  2*length*size,2*size,
				&pGraphicMenuLayout_ );

	pGraphicMenuLayout_->SetFontSize(size, range);
	pGraphicMenuLayout_->SetFontWeight(DWRITE_FONT_WEIGHT_LIGHT, range);
	pRedBrush_->SetColor(color);  // Yellow
	pRT_->DrawTextLayout( loc, pGraphicMenuLayout_, pRedBrush_ );
}
void ViewDW::getD2DforPage(HWND handle, int pageIndex, D2D1_RECT_F rect ){
#ifdef DEBUG_GB							
//	log(L"void ViewDW::getD2DforPage(int pageIndex = %i )", pageIndex);
#endif

//	log(L"d2Dblock[ pageIndex ].total = %i", d2Dblock[ pageIndex ].total);
//  d2Dblock[i].total is a count of the number of D2D routines so case 1: is the first
// 	D2D routine in the book.
    switch(	d2Dblock[ pageIndex ].total)
    {
	case 1:{    //Left Hand and Right Hand Coodinates 
		D2D1_COLOR_F blueCS		=	D2D1::ColorF(0x3f8898);
		D2D1_COLOR_F brightBlu  =	D2D1::ColorF(0x3fc8c8);
		D2D1_COLOR_F yellowCS   =	D2D1::ColorF(0x98883f);
		D2D1_COLOR_F brightYel  =	D2D1::ColorF(0xdcc03f);

		static FLOAT displace = 0.0f;
		static FLOAT angle = 0.0f;
		D2D1::Matrix3x2F trans;
		D2D1::Matrix3x2F rot;
		D2D1::Matrix3x2F scaleMat;


		float width = rect.right - rect.left;
		D2D1_POINT_2F scale;
			scale.x = 0.00104*width;			// scale wide/956
			scale.y = 0.000955*rect.bottom;	// scale high/1047

		//  Draw the Blueish CS
		pRedBrush_->SetColor(blueCS); // Blue

		trans = D2D1::Matrix3x2F::Translation( rect.left  + 0.4f*width, 0.5*rect.bottom);
		scaleMat = D2D1::Matrix3x2F::Scale( scale.y, scale.x, D2D1::Point2F(0.0f, 0.0f));
		rot = D2D1::Matrix3x2F::Rotation( angle, D2D1::Point2F(0.0f, 0.0f));
		pRT_->SetTransform( scaleMat*rot*trans );
		pRT_->DrawGeometry(pRight_vs_Left, pRedBrush_);

		trans = D2D1::Matrix3x2F::Translation( rect.left + displace*scale.x  + 0.2f*width, 0.5*rect.bottom);
		scaleMat = D2D1::Matrix3x2F::Scale( scale.x, scale.y, D2D1::Point2F(0.0f, 0.0f));
		pRedBrush_->SetColor(yellowCS);  // Yellow
		pRT_->SetTransform( scaleMat*trans );
		pRT_->DrawGeometry(pRight_vs_Left, pRedBrush_);		
		pRT_->SetTransform(D2D1::IdentityMatrix());
		if( gbArrived ){
			D2D1_POINT_2F loc;
			loc.x = rect.left + displace*scale.x + 0.2f*width + scale.x*130;
			loc.y = 0.5*rect.bottom - 11*scale.y;
			labelGraphics(L"x",  loc, brightYel, 20.0*scale.y, 1);
			loc.x = rect.left + displace*scale.x + 0.2f*width + scale.x*90;
			loc.y = 0.5*rect.bottom - 62*scale.y;
			labelGraphics(L"y",  loc, brightYel, 20.0*scale.y, 1);
			loc.x = rect.left + displace*scale.x + 0.195f*width;
			loc.y = 0.5*rect.bottom - 145*scale.y;
			labelGraphics(L"z", loc, brightYel, 20.0*scale.y, 1);
			loc.x = rect.left + displace*scale.x + 0.23f*width;
			loc.y = 0.5*rect.bottom + 15*scale.y;
			labelGraphics(L"RHS", loc, brightYel, 20.0*scale.y, 3);

			loc.x = rect.left  + 0.4f*width - scale.x*140;
			loc.y = 0.5*rect.bottom - 12*scale.y;
			labelGraphics(L"x",  loc, brightBlu, 20.0*scale.y, 1);
			loc.x = rect.left  + 0.4f*width - scale.x*50;
			loc.y = 0.5*rect.bottom - 98*scale.y;
			labelGraphics(L"y",  loc, brightBlu, 20.0*scale.y, 1);
			loc.x = rect.left  + 0.39f*width;
			loc.y = 0.5*rect.bottom - 145*scale.y;
			labelGraphics(L"z", loc, brightBlu, 20.0*scale.y, 1);
			loc.x = rect.left  + 0.3f*width;;
			loc.y = 0.5*rect.bottom + 15*scale.y;
			labelGraphics(L"LHS", loc, brightBlu, 20.0*scale.y, 3);
		}

		if( displace < 322 && !gbArrived){
			SetTimer( handle, 7, 30, NULL);
			angle = angle + (float)(1.570796*(displace/300));
			displace++;}

		else{ 
			KillTimer(handle,7);  
			gbArrived = TRUE;
			angle = 270.0;
			InvalidateRect( handle, NULL, FALSE );
		}

		break;
		}
	case 2:{ 
	
	    //log(L"rect.top, right, bottom, left  = %f , %f, %f , %f )", rect.top, rect.right,											rect.bottom,rect.left);
		static DWORD startTime;
		static D2D1_POINT_2F start;
		deltaT = 0;
	if( bLeftButtonDown ){ // WM_LBUTTONDOWN
		velocity.x = velocity.y  = 0.0f;
		GetCursorPos( &mousePos );
		ScreenToClient( DWhandle, &mousePos);
		start.x = mousePos.x*dpiScaleX_;
		start.y = mousePos.y*dpiScaleY_;
		ePoint.x = start.x;
		ePoint.y = start.y;
		startTime = GetTickCount();
//		log(L"start.x = %f start.y = %f startTime = %d)", start.x , start.y, startTime);}
	}
	else if( bLeftButtonUp ){ // WM_LBUTTONUP
			bLeftButtonUp = FALSE;
		SetTimer( handle, 7, 30, NULL);
		GetCursorPos( &mousePos );
		ScreenToClient( DWhandle, &mousePos);

		end.x = mousePos.x*dpiScaleX_;
		end.y = mousePos.y*dpiScaleY_;
		deltaT = GetTickCount() - startTime;
		velocity.x = (end.x - start.x )/deltaT;
		velocity.y = (end.y - start.y )/deltaT;

//		log(L"case: 2 bLeftButtonUp (  velocity.x  = %f, velocity.y  = %f)",											velocity.x, velocity.y);		
	}

		pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Cornsilk));  

		pRT_->FillRectangle(&rect, pRedBrush_);
		pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

		ePoint.x += 25*velocity.x;
		ePoint.y += 25*velocity.y;

		if( ((end.x - ePoint.x )*(end.x - ePoint.x ) + (end.y - ePoint.y )*(end.y - ePoint.y ))< 25 ){
			ePoint.x = end.x;
			ePoint.y = end.y;
			KillTimer(handle,7);
	//		bLeftButtonUp = FALSE;
		}
		cannonBall.radiusX = 15.0f;
		cannonBall.radiusY = 15.0f;

		cannonBall.point = ePoint;
        pRT_->FillEllipse( cannonBall, CS_AxisBrush_ );

		break;
		}

	case 3:{  
	//   log(L"rect.top, right, bottom, left  = %f , %f, %f , %f )", rect.top, rect.right,											rect.bottom,rect.left);
	   log(L"ViewDW::getD2DforPage case 3: ");
			static D2D1_POINT_2F tempx, tempy, tickx, ticky;
			float axisMargin = 0.1f;
//		if( gbWindowResized){log(L"gbWindowResized");
			CSorigin.x = 0.5*( rect.left +  rect.right);
			CSorigin.y =  0.5*rect.bottom;
			Xaxis.x  = axisMargin*rect.left +  (1-axisMargin)*rect.right;
			tempx.x =   (1-axisMargin)*rect.left +  axisMargin*rect.right;
			Xaxis.y = CSorigin.y;
			tempx.y =  Xaxis.y;
			Yaxis.x = CSorigin.x;
			Yaxis.y = axisMargin*rect.bottom;
			tempy.y =  (1-axisMargin)*rect.bottom;
			tempy.x = Yaxis.x;
			gbWindowResized = FALSE;
	//		}
			pRT_->DrawLine( tempy, Yaxis, CS_AxisBrush_, 0.5f);
			pRT_->DrawLine( tempx, Xaxis, CS_AxisBrush_, 0.5f);
	//if(	float(( mouseX*96 )/dpiScaleY_) < DWSize.width/2 ){
	//	pTextLayout_->HitTestPoint( float(( mouseX*96 )/dpiScaleY_) - g_siMargin, 
	//		float(( mouseY*96 )/dpiScaleY_), &trail, &gbInside, &txMetrix);}
			// Draw menu
		graphicMenu.clear( );
		graphicMenu = L"Unit Vectors";
		SafeRelease(&pFindLayout_);
		pDWriteFactory_->CreateTextLayout( (PWSTR)graphicMenu.c_str(),  
			(UINT32)lstrlen((PWSTR)graphicMenu.c_str() ), pMenuFormat_, 150.0f,											30.0f, &pFindLayout_ );
		D2D1_POINT_2F menuOrigin;
		menuOrigin.x = CSorigin.x;
		menuOrigin.y = rect.bottom - 20;
				pRT_->DrawTextLayout( menuOrigin, pFindLayout_, pBlackBrush_ );

			for( int i = 1; i < 10; i++ ){
					ticky.x = CSorigin.x - 5;
					ticky.y = i*axisMargin*rect.bottom;
					tickx.y = i*axisMargin*rect.bottom;
					tickx.x = CSorigin.x + 5;
					if( i != 5 )
					pRT_->DrawLine( ticky, tickx, CS_AxisBrush_, 0.5f);					
				}
			for( int i = 1; i < 10; i++ ){
					ticky.y = CSorigin.y - 5;
					ticky.x = rect.left + i*axisMargin*(rect.right - rect.left);
					tickx.x =  rect.left + i*axisMargin*(rect.right - rect.left);
					tickx.y = CSorigin.y + 5;
					if( i != 5 )
					pRT_->DrawLine( ticky, tickx, CS_AxisBrush_, 0.5f);					
				}

			if( bLeftButtonDown ){log(L"case 3  LBDN");
				gbSaveRadiusVector = FALSE;
				SetTimer( handle, 7, 30, NULL);
				GetCursorPos( &mousePos );
				ScreenToClient( DWhandle, &mousePos);
				end.x = mousePos.x*dpiScaleX_;
				end.y = mousePos.y*dpiScaleY_;
				r = sqrt((CSorigin.x - end.x)*(CSorigin.x - end.x) +
						(CSorigin.y - end.y)*(CSorigin.y - end.y));
				cosR = (end.x -  CSorigin.x )/r;
				sinR = (CSorigin.y - end.y )/r;
				arrowLeft.x = end.x - 20*( cosR*0.96 + sinR*0.27 );
				arrowLeft.y = end.y + 20*( sinR*0.96 - cosR*0.27 );
				arrowRight.x = end.x - 20*( cosR*0.96 - sinR*0.27 );
				arrowRight.y = end.y + 20*( sinR*0.96 + cosR*0.27 );
				if( r > 15 ){
					pRT_->DrawLine( arrowLeft, end, radiusBrush_, 1.0f);
					pRT_->DrawLine( arrowRight, end, radiusBrush_, 1.0f);}
				pRT_->DrawLine( CSorigin, end, radiusBrush_, 1.0f); // added

			} // if( bLeftButtonDown )

			else if( bLeftButtonUp ){
				gbSaveRadiusVector = TRUE;
				bLeftButtonUp = FALSE;
				GetCursorPos( &mousePos );
				ScreenToClient( DWhandle, &mousePos);
				end.x = mousePos.x*dpiScaleX_;
				end.y = mousePos.y*dpiScaleY_;
				scale.x  = ( end.x - CSorigin.x )/( Xaxis.x - CSorigin.x );
				scale.y  = ( CSorigin.y - end.y )/( CSorigin.y - Yaxis.y );
				log(L"case 3  LBUP  scale.x = %f" ,scale.x);
				if( r > 15 ){
					pRT_->DrawLine( arrowLeft, end, radiusBrush_, 1.0f);
					pRT_->DrawLine( arrowRight, end, radiusBrush_, 1.0f);}
				pRT_->DrawLine( CSorigin, end, radiusBrush_, 1.0f); // added

			} // else if( bLeftButtonUp )
			else if( gbSaveRadiusVector ){//log(L" scale.x = %f",scale.x);
								KillTimer( handle, 7);
				end.x = CSorigin.x + scale.x*(Xaxis.x - CSorigin.x);
			log(L"end.x = scale.x*Xaxis.x - origin.x = %f",scale.x*(Xaxis.x - origin.x));
				end.y = CSorigin.y + scale.y*(Yaxis.y - CSorigin.y);
				r = sqrt((CSorigin.x - end.x)*(CSorigin.x - end.x) +
						(CSorigin.y - end.y)*(CSorigin.y - end.y));
				cosR = (end.x -  CSorigin.x )/r;
				sinR = (CSorigin.y - end.y )/r;
				arrowLeft.x = end.x - 20*( cosR*0.96 + sinR*0.27 );
				arrowLeft.y = end.y + 20*( sinR*0.96 - cosR*0.27 );
				arrowRight.x = end.x - 20*( cosR*0.96 - sinR*0.27 );
				arrowRight.y = end.y + 20*( sinR*0.96 + cosR*0.27 );
				if( r > 15 ){
					pRT_->DrawLine( arrowLeft, end, radiusBrush_, 1.0f);
					pRT_->DrawLine( arrowRight, end, radiusBrush_, 1.0f);}
				pRT_->DrawLine( CSorigin, end, radiusBrush_, 1.0f); // added
				static const WCHAR radiusVect[] =  TEXT("r");
				D2D1_RECT_F rad;
				rad.top = end.y;
				rad.left = end.x;
				rad.right = rad.left + 20;
				rad.bottom = rad.top + 20;

				pRT_->DrawText( radiusVect, ARRAYSIZE(radiusVect) - 1, pVectorFormat_,
						rad, pBlackBrush_ );
			} // else if( gbSaveRadiusVector )
		break;
		}
/*
	case 3:{

			static const WCHAR sc_helloWorld[] =  TEXT("here is an equation \u222b \u221a \u0394 \u0395 ");

       m_pRenderTarget->DrawText(
            sc_helloWorld,
            ARRAYSIZE(sc_helloWorld) - 1,
            m_pTextFormat,
            D2D1::RectF(50, 730, 600, 430),
            m_pBlackBrush
            );

		rect.top = 2*rtSize.height/3.0f;
		rect.left = 0.0f;
		rect.bottom = rtSize.height;
		rect.right = rtSize.width;
		wstring dwText = L"Hello\nDirectWrite!";
		DWRITE_TEXT_RANGE tRange;
		tRange.startPosition = 0;
		tRange.length = dwText.size();
		pRT_->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));
		pRT_->DrawText(
			(PWSTR)dwText.c_str(),        // The string to render.
			(UINT32)lstrlen((PWSTR)dwText.c_str() ),    // The string's length.
			pDW_TextFormat,    // The text format.
			rect,       // The region of the window where the text will be rendered.
			pBlackBrush_ );
		break;
		}
		case 4:{
	wstring sizeText;
	wstring temp;
	float dpiX, dpiY;
    // Returns the size of the render target in DIPs.
	D2D1_RECT_F rect = {0};
	D2D1_RECT_U pixRect = {0};
	D2D1_SIZE_F DIPsize  = pRT_->GetSize();  // for variable size rectangle
	rect.top = 0.0f;
	rect.left = 0.0f;
	rect.bottom =DIPsize.height;
	rect.right = DIPsize.width;
//	log(L"size.width = %f, size.height =%f",DIPsize.width,DIPsize.height);  // 688 x 800 DIPs

	pRT_->DrawLine(
		D2D1::Point2F( rect.left  , rect.bottom/2.0f),
		D2D1::Point2F( rect.right , rect.bottom/2.0f),
        pRedBrush_,
        0.5f
        );
	pRT_->DrawLine(
		D2D1::Point2F( rect.right/2 , rect.top),
		D2D1::Point2F( rect.right/2 ,  rect.bottom),
        pRedBrush_,
        0.5f
        );

	sizeText.clear();
	sizeText = to_wstring(DIPsize.width);
	rect.top = DIPsize.height/2.0f - 10.0f;
	rect.left = DIPsize.width/2.0f  -50.0f;
	rect.bottom = rect.top + 20;
	rect.right = rect.left + 100;
	pRT_->Clear(D2D1::ColorF(D2D1::ColorF::Sienna));
	pRT_->DrawText(
		(PWSTR)sizeText.c_str(), 
		(UINT32)lstrlen((PWSTR)sizeText.c_str() ), 
		pTextFormat_,
		rect,
		pBlackBrush_ );
	rect.top = 2*DIPsize.height/3.0f;
	rect.left = DIPsize.width/6.0f;
	rect.bottom = 5*DIPsize.height/6.0f;
	rect.right = 5*DIPsize.width/6.0f;
	pRT_->DrawRectangle(&rect, pBlackBrush_, 0.5f, NULL);
	D2D1_RECT_F textRect = {0};
	sizeText.clear();
	sizeText = L" Width of d2D child window = ";
	temp = to_wstring(DIPsize.width);
	sizeText.append(temp);
	temp = L" DIP's\n Height of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(DIPsize.height);
	sizeText.append(temp);
	temp = L" DIP's\n";
	sizeText.append(temp);

	D2D1_SIZE_U pixSize  = pRT_->GetPixelSize();  // 

	temp = L" Width of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(pixSize.width);
	sizeText.append(temp);
	temp = L" pixels's\n Height of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(pixSize.height);
	sizeText.append(temp);
	temp = L" pixels's\n";
	sizeText.append(temp);

	pRT_->GetDpi( &dpiX, &dpiY);

	temp = L" Width of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(dpiX);
	sizeText.append(temp);
	temp = L" DPI's\n Height of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(dpiY);
	sizeText.append(temp);
	temp = L" DPI's";
	sizeText.append(temp);

	pRT_->DrawText(
		(PWSTR)sizeText.c_str(),        // The string to render.
		(UINT32)lstrlen((PWSTR)sizeText.c_str() ),    // The string's length.
		pTextFormat_,    // The text format.
		rect,       // The region of the window where the text will be rendered.
		pBlackBrush_     // The brush used to draw the text.
	);
	break;

		} // case
	*/
		case 54:{
			break;
		}
		case 55:{
			break;
		}
		case 56:{
			break;
		}
		case 57:{
			break;
		}
		case 58:{
			break;
		}

	default: break;
	} // switch
}
/*
void ViewDW::drawD2D(){
#ifdef DEBUG_GB							
	log(L"ViewDW::drawD2D()");
#endif
	wstring sizeText;
	wstring temp;
	float dpiX, dpiY;
    // Returns the size of the render target in DIPs.
	D2D1_RECT_F rect = {0};
	D2D1_RECT_U pixRect = {0};
	D2D1_SIZE_F DIPsize  = pRT_->GetSize();  // for variable size rectangle
	rect.top = 0.0f;
	rect.left = 0.0f;
	rect.bottom =DIPsize.height;
	rect.right = DIPsize.width;
//	log(L"size.width = %f, size.height =%f",DIPsize.width,DIPsize.height);  // 688 x 800 DIPs

	pRT_->DrawLine(
		D2D1::Point2F( rect.left  , rect.bottom/2.0f),
		D2D1::Point2F( rect.right , rect.bottom/2.0f),
        pRedBrush_,
        0.5f
        );
	pRT_->DrawLine(
		D2D1::Point2F( rect.right/2 , rect.top),
		D2D1::Point2F( rect.right/2 ,  rect.bottom),
        pRedBrush_,
        0.5f
        );


	sizeText.clear();
	sizeText = to_wstring(DIPsize.width);
	rect.top = DIPsize.height/2.0f - 10.0f;
	rect.left = DIPsize.width/2.0f  -50.0f;
	rect.bottom = rect.top + 20;
	rect.right = rect.left + 100;
	pRT_->DrawTextW(
		(PWSTR)sizeText.c_str(),        // The string to render.
		(UINT32)lstrlen((PWSTR)sizeText.c_str() ),    // The string's length.
		pTextFormat_,    // The text format.
		rect,       // The region of the window where the text will be rendered.
		pBlackBrush_     // The brush used to draw the text.
	);
	rect.top = 2*DIPsize.height/3.0f;
	rect.left = DIPsize.width/6.0f;
	rect.bottom = 5*DIPsize.height/6.0f;
	rect.right = 5*DIPsize.width/6.0f;
	pRT_->DrawRectangle(&rect, pBlackBrush_, 0.5f, NULL);
	D2D1_RECT_F textRect = {0};
	sizeText.clear();
	sizeText = L" Width of d2D child window = ";
	temp = to_wstring(DIPsize.width);
	sizeText.append(temp);
	temp = L" DIP's\n Height of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(DIPsize.height);
	sizeText.append(temp);
	temp = L" DIP's\n";
	sizeText.append(temp);

	D2D1_SIZE_U pixSize  = pRT_->GetPixelSize();  // 

	temp = L" Width of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(pixSize.width);
	sizeText.append(temp);
	temp = L" pixels's\n Height of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(pixSize.height);
	sizeText.append(temp);
	temp = L" pixels's\n";
	sizeText.append(temp);

	pRT_->GetDpi( &dpiX, &dpiY);

	temp = L" Width of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(dpiX);
	sizeText.append(temp);
	temp = L" DPI's\n Height of d2D child window = ";
	sizeText.append(temp);
	temp = to_wstring(dpiY);
	sizeText.append(temp);
	temp = L" DPI's";
	sizeText.append(temp);

	pRT_->DrawText(
		(PWSTR)sizeText.c_str(),        // The string to render.
		(UINT32)lstrlen((PWSTR)sizeText.c_str() ),    // The string's length.
		pTextFormat_,    // The text format.
		rect,       // The region of the window where the text will be rendered.
		pBlackBrush_     // The brush used to draw the text.
	);
}
*/
void ViewDW::newDrawTables( tableParams table, int tableType, D2D1_SIZE_F size ){
#ifdef DEBUG_GB							//       3          4
// log(L"ViewDW::newDrawTables( pageNumber = %i)", pageNumber);
#endif

// tables[arrayIndex].colWidth[tableCount][ colCount ]

	D2D1_RECT_F rect = {0};
	D2D1_SIZE_F tableSize; 
	D2D1_SIZE_F tableCellSize; 
	float tableFontSize = 16.0f;
	if( tableType == 0 ) { tableFontSize = 12.0f; }
	rect.left = g_siMargin;
	D2D1_POINT_2F origin = {rect.left,rect.left};
	DWRITE_TEXT_RANGE tRange;
	tRange.startPosition = 0;
//	 size.width   = size.width/2.0f; 

if( tableType == 2 ){  //  Help Menu Pop - Up
	rect.right = size.width;
	rect.left = size.width - 150.0f;
	rect.top = 0.0f;
	rect.bottom = iMenuSize*16;
	D2D1_POINT_2F origin = { rect.left, 0.0f };
	pRT_->DrawRectangle(&rect, pBlackBrush_, 2.0f, NULL);
	pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF(0xa06020)));
	pRT_->FillRectangle(&rect, pRedBrush_);
	for( int i = 0; i < iMenuSize; i++ ){  // iMenuSize  == total number of menu items
		origin.y = 16*i;
		SafeRelease(&pTableLayout_);
		pDWriteFactory_->CreateTextLayout(
		(PWSTR)table.cellText[i].c_str(),
		(UINT32)lstrlen((PWSTR)table.cellText[i].c_str()),  
		pTextFormat_,  					
		rect.left,
		origin.y + 16,  //rect.bottom,
		&pTableLayout_ );      // The IDWriteTextLayout interface pointer.
		tRange.length = table.cellText[i].size();
		pTableLayout_->SetFontSize( 12.0f, tRange );
		pRT_->DrawTextLayout( origin , pTableLayout_, pBlackBrush_ );}
	}

else {
	if(tableType != 0 ){
		for( int i = 0; i < table.total; i++ ){
			for( int k = 0; k <table.cols[i]; k++ ){
				table.cellWidth[ i ] = table.cellWidth[ i ] + table.colWidth[i][ k ];
			}
		} // i = number of tables on page
	}

rect.top = 70.0f; // need to set in fblock read section rect.top = table[pageNumber].top[0];
	if(  tableType == 0  )  rect.top = 0.0f;   // Have menu table
	else rect.top = 70.0f;  // Top of first table on page
int	start = 0;
	for( int i = 0; i < table.total; i++ ){
		float addCell;
		tableCellSize.width = 0.13f*tableFontSize*table.cellWidth[ i ];
//		tableSize.width = tableCellSize.width*table.cols[i];
		tableSize.width = table.cellWidth[ i ]*9.0f;
		tableSize.height = tableFontSize*table.rows[i];
		tableCellSize.height = tableFontSize;
		rect.bottom = rect.top + tableSize.height;
		rect.right = rect.left + tableSize.width; 
		if( tableType == 0 ){ rect.right = size.width; }  // menu here
		else pRT_->DrawRectangle(&rect, pBlackBrush_, 0.375f, NULL);
		for( int j = 0; j < table.rows[i]; j++ ){
			origin.y = rect.top + j*tableCellSize.height;
			addCell = 0.0f;
			for( int k = 0; k < table.cols[i]; k++ ){
				if( tableType == 0 ) {  // display menu
					tableCellSize.width = size.width/(iMenuSize + 1);
					origin.x  =  (1 +  k)*tableCellSize.width; 
				}
				else{	
					origin.x = rect.left  + addCell;
					addCell = addCell + table.colWidth[i][ k ]*9.0f;
				}
				SafeRelease(&pTableLayout_);
				pDWriteFactory_->CreateTextLayout(
				(PWSTR)table.cellText[ k + start ].c_str(),
				(UINT32)lstrlen((PWSTR)table.cellText[ k + start ].c_str() ),  
				pTextFormat_,  					
				rect.right,
				12.0f,  //rect.bottom,
				&pTableLayout_ );      // The IDWriteTextLayout interface pointer.
				tRange.length = table.cellText[ k + start ].size();
				pTableLayout_->SetFontSize( 12.0f, tRange);
				if( tableType == 0 ){
					if( table.top[i] == k + start ) // Highlight
					pBlackBrush_->SetColor(D2D1::ColorF(D2D1::ColorF(0xF4B480)));
					else pBlackBrush_->SetColor(D2D1::ColorF(D2D1::ColorF(0xa06020)));
				}
				else pBlackBrush_->SetColor(D2D1::ColorF(D2D1::ColorF(0xF4B480)));

				pRT_->DrawTextLayout( origin , pTableLayout_, pBlackBrush_ );
			} // k loop
			start = start + table.cols[i];
		} // j loop
		addCell = 0;
		if(  tableType == 0  ) table.cols[0] = iMenuSize + 1;  // menu add 1 for page number
		for( int j = 1; j < table.cols[i]; j++ ){
			if ( tableType == 0 ) {
				rect.left = 0.0f;
				addCell = j*size.width/(iMenuSize + 1); }
			//pRT_->DrawLine(		// used to debug menu positions on display	
			//	D2D1::Point2F(  addCell , rect.top ),
			//	D2D1::Point2F(  addCell  , rect.bottom ),
   //             pRedBrush_,
   //             1.375f
			//	);}
			else {addCell = addCell + table.colWidth[i][ j-1 ]*9.0f; }
			//pRT_->DrawLine(	// used to debug menu positions on display		
			//	D2D1::Point2F( rect.left +  addCell , rect.top ),
			//	D2D1::Point2F( rect.left +  addCell  , rect.bottom ),
   //             pRedBrush_,
   //             1.375f
			//	);}
		} // j loop
		if(  tableType == 0  ) table.cols[0] = iMenuSize ;  // menu
		for( int j = 1; j < table.rows[i]; j++ ){
			pRT_->DrawLine(
				D2D1::Point2F( rect.left ,   j*tableCellSize.height + rect.top),
				D2D1::Point2F( rect.right,   j*tableCellSize.height + rect.top),
                pRedBrush_,
                1.375f
                );
		} // j loop
		rect.top = rect.top + tableSize.height + 100.0f;
	} // i loop 
	// reset color from highlight color
	 pBlackBrush_->SetColor(D2D1::ColorF(D2D1::ColorF(0xF4B480)));
	}
}

HRESULT ViewDW::CreateDeviceResources(HWND handle)
{
#ifdef DEBUG_GB
//	log(L"ViewDW::CreateDeviceResources(HWND handle)");
#endif
    HRESULT hr = S_OK;
	if(!pRT_){
 
		RECT rc;
		GetClientRect(handle, &rc);
//		log(L" Creating pRT %%%%%%%%%%%%%%%%%%%% %ix%i.", rc.right, rc.bottom);
		D2D1_SIZE_U Usize = D2D1::SizeU( (rc.right - rc.left ), (rc.bottom - rc.top) );
//		log(L"drawDW Usize size %ix%i.", Usize.width, Usize.height);
		hr = pD2DFactory_->CreateHwndRenderTarget( D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties( handle, Usize ), &pRT_ );
//		pRT_->CreateSolidColorBrush(D2D1::ColorF(0xFF0000),  &pRedBrush_ );//Red = 0xFF0000,
		pRT_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pRedBrush_ );
		pRT_->CreateSolidColorBrush(D2D1::ColorF(0xF4B480),  &pBlackBrush_ );
		pRT_->CreateSolidColorBrush(D2D1::ColorF(0.09f, 0.07f, 0.07f, 1.0f ),
		&pDarkGrayBrush_);
		pRT_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), &CS_AxisBrush_ );
		pRT_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::BlueViolet),  &radiusBrush_ );

	}
/*
//wstring picture =L".\\leibniz.gif";
wstring picture =L".\\";
//	picture.append(bitmapFile[pageNumber]);
	if(bBitMap){
		picture.clear();
		picture =L".\\";
		picture.append(bitmapFile[pageNumber - 1]);
		log(L" pageNumber %i", pageNumber);
		log(bitmapFile[pageNumber - 1]);
		log(picture);

		hr = LoadBitmapFromFile(
			pRT_,
			m_pWICFactory,
	        L".\\sampleImage.jpg",
			200,
			100,
			&m_pBitmap
			);}
//			(PWSTR)picture.c_str(),

//		    (PWSTR)bitmapFile[pageNumber].c_str(),
//			L".\\earth.bmp",
//			L".\\leibniz.gif",
//			L".\\descates.jpg",
*/
 /*       if (SUCCEEDED(hr))
        {
            ID2D1GradientStopCollection *pGradientStops = NULL;
            // Create a linear gradient.
            static const D2D1_GRADIENT_STOP stops[] =
            {
                {   1.f,  { 0.f, 1.f, 1.f, 0.25f }  },
                {   0.f,  { 1.0f, 0.f, 1.f, 1.f }  },    // blue?
            };

            hr = pRT_->CreateGradientStopCollection(
                stops,
                ARRAYSIZE(stops),
                &pGradientStops
                );
            if (SUCCEEDED(hr))
            {
                hr = pRT_->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(-150, 0),
                        D2D1::Point2F(0, 150)),
                    D2D1::BrushProperties(),
                    pGradientStops,
                    &pLinearGradientBrush
                    );
                pGradientStops->Release();
            }
        }
*/
	if( FAILED( hr )) return hr;

    return S_OK;
}

void ViewDW::drawDW( HWND handle ){
#ifdef DEBUG_GB
//	log(L"ViewDW::drawDW(HWND handle)  pageNumber -1 = %i, pageCount = %i, ", pageNumber - 1,  pageCount);
#endif
	pnum = to_wstring( pageNumber - 1); 
	int glRoutineNumber = -1;
	if( pageNumber - 1 == BOOK_COVER || pageMode[ pageNumber - 1 ] == GL_MODE){
		if( currentMode != GL_MODE || currentPage != pageNumber - 1 ){
			currentMode = GL_MODE;
			currentPage = pageNumber - 1;
			for( int i = 0; i < glRoutineNumberPageNumber.size(); i++){
				if( glRoutineNumberPageNumber[ i ] == currentPage ){
					glRoutineNumber = i;
log(L"ViewDW::drawDW(HWND handle)  glRoutineNumber =  = %i, ", glRoutineNumber);
					break;
				}
			}
			SendMessage( mainHandle, WM_SIZE, GL_MODE,  glRoutineNumber ); 
			// This will send a WM_SIZE msg to ControllerMain.cpp, which will send the 
			// glRoutineNumber on to ModelGl
		}
	}
			//  Last page of book
	else if( pageNumber - 1 == pageCount || pageMode[ pageNumber - 1 ] == DW_MODE ){
		if( currentMode != DW_MODE ){
			currentMode = DW_MODE;
			SendMessage( mainHandle, WM_SIZE, DW_MODE, DW_MODE ); }
	}

    if (SUCCEEDED( CreateDeviceResources(handle)) )
    {     //        D2D1_WINDOW_STATE_OCCLUDED = 0x0000001,

	//if (g_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)
	//	return hr;
/* Note  If the window was occluded the last time that EndDraw was called, the next time that the render target calls CheckWindowState, it will return D2D1_WINDOW_STATE_OCCLUDED regardless of the current window state. If you want to use CheckWindowState to determine the current window state, you should call CheckWindowState after every EndDraw call and ignore its return value. This call will ensure that your next call to CheckWindowState state will return the actual window state.
return SUCCEEDED(hr) ? 0 : 1;

*/
	//	if (0 == (D2D1_WINDOW_STATE_OCCLUDED & pRT_->CheckWindowState()))
    //    {
	D2D1_RECT_F rect = {0};
	D2D1_SIZE_F rtSize;
	rtSize = pRT_->GetSize();
// log(L"ViewDW::drawDW(HWND handle)  rtSize.width= %f", rtSize.width);
	float width, height;   // Do not allow these to go negative  < 1  !! 
	if( currentMode == DW_MODE){
		if( rtSize.width  > 3*g_siMargin ) width = rtSize.width/2 - 1.5f*g_siMargin;
		else  width = rtSize.width;  // *
		if( rtSize.height > 2*g_siMargin ) height = rtSize.height - 2.0f*g_siMargin;
		else  height = rtSize.height; // *
	}
	else if( currentMode == GL_MODE){
		if( rtSize.width > 2*g_siMargin ) width = rtSize.width - 2*g_siMargin;
		else  width = rtSize.width;
		if( rtSize.height > 2*g_siMargin ) height = rtSize.height -2*g_siMargin;
		else  height = rtSize.height; 	
	}
//	log(L"drawDW width x height %fx%f.", rtSize.width, rtSize.height);
//	if( currentMode == GL_MODE){
		SafeRelease(&pTextLayout_);
		pDWriteFactory_->CreateTextLayout(
			(PWSTR)pages[pageNumber - 1].c_str(),  
			(UINT32)lstrlen((PWSTR)pages[pageNumber - 1].c_str()),
			pTextFormat_, width, height, &pTextLayout_ );

	if( pageNumber < pageCount && currentMode == DW_MODE){ // Don't go past last page, or beyond range of pageMode[].
		SafeRelease(&pRightLayout_);
		pDWriteFactory_->CreateTextLayout(
			(PWSTR)pages[pageNumber].c_str(),  
			(UINT32)lstrlen((PWSTR)pages[pageNumber].c_str()),
			pTextFormat_, width, height, &pRightLayout_ );
	}
	
	//SafeRelease(&pFindLayout_);
	//pDWriteFactory_->CreateTextLayout(
 //       (PWSTR)find.c_str(),  
 //       (UINT32)lstrlen((PWSTR)find.c_str() ), 
 //       pTextFormat_,   
 //       rtSize.width,  
 //       20.0f,
 //       &pFindLayout_ );

	SafeRelease(&pMenuLayout_);
	pDWriteFactory_->CreateTextLayout( (PWSTR)menu.c_str(),  
        (UINT32)lstrlen((PWSTR)menu.c_str() ), 
        pMenuFormat_,   
        40.0f,  
        20.0f,
        &pMenuLayout_ );

	SafeRelease(&pPageNumberLayout_);
	pDWriteFactory_->CreateTextLayout( (PWSTR)pnum.c_str(), 
        (UINT32)lstrlen((PWSTR)pnum.c_str() ), pTextFormat_, rtSize.width,
        20.0f, &pPageNumberLayout_ );

	SafeRelease(&m_pBitmap);
	SafeRelease(&m_pBitmapRight);
	if(	 isImageOnPage[pageNumber - 1] >= 0 ){ 
       HRESULT hr = LoadResourceBitmap( pRT_,  m_pWICFactory,
            (PWSTR)pageImages[isImageOnPage[pageNumber - 1]].name.c_str(),
			L"Image", pageImages[isImageOnPage[pageNumber - 1]].width,
			pageImages[isImageOnPage[pageNumber - 1]].height, &m_pBitmap );
		if (SUCCEEDED(hr)) bBitMap = TRUE;
		else  bBitMap = FALSE;		
	}
	if( pageNumber < pageCount && currentMode == DW_MODE){ // Don't go past last page, or beyond																//range of isImageOnPage[].
		if(	 isImageOnPage[ pageNumber ] >= 0 ) { 
			HRESULT hr = LoadResourceBitmap( pRT_,  m_pWICFactory,
				(PWSTR)pageImages[isImageOnPage[pageNumber]].name.c_str(),
				L"Image", pageImages[isImageOnPage[pageNumber ]].width,
				pageImages[isImageOnPage[ pageNumber ]].height, &m_pBitmapRight );
			if (SUCCEEDED(hr)) bBitMapRight = TRUE;
			else  bBitMapRight = FALSE;		
		}
	}
//		HRESULT hr = LoadBitmapFromFile(
//			pRT_,
//			m_pWICFactory,
//			(PWSTR)bitmapFile[pageNumber - 1].c_str(),
////	        L".\\earth.bmp ",
//			200,
//			0,
//			&m_pBitmap
//			);

	setTextLayout( pageNumber - 1, LEFT_PAGE );
		// Don't go past last page
   if( pageNumber < pageCount && currentMode == DW_MODE )setTextLayout( pageNumber, RIGHT_PAGE );

    pRT_->BeginDraw(); 
    pRT_->SetTransform(D2D1::IdentityMatrix());
//	pRT_->Clear(D2D1::ColorF(D2D1::ColorF::Black));
//	pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	pRedBrush_->SetColor(BGcolor(pageNumber));
		rect.left = 0.0f;  rect.right = rtSize.width;
		rect.top = 0.0f;   rect.bottom = rtSize.height;
	pRT_->FillRectangle(&rect, pRedBrush_);

/*	if( !bColor ){
	float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		for (int y = 0; y < r1*20; y++){
			float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float r3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float r4 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            pRT_->DrawLine(
				D2D1::Point2F(0.0f, r1*rtSize.height),
				D2D1::Point2F(rtSize.width, r1*rtSize.height),
                pDarkGrayBrush_,
                r4*2.0f
                );
            pRT_->DrawLine(
				D2D1::Point2F(r2*rtSize.width, 0.0),
				D2D1::Point2F(r2*rtSize.width, rtSize.height),
                pDarkGrayBrush_,
                r2*2.0f
                );
		}
	} // 	if( !bColor ) 
*/
	if( bBitMap ){ bBitMap = FALSE;
		D2D1_SIZE_F bmSize = m_pBitmap->GetSize();
		pRT_->DrawBitmap( m_pBitmap,
		D2D1::RectF(
		(float)pageImages[isImageOnPage[pageNumber - 1]].originX,
		(float)pageImages[isImageOnPage[pageNumber - 1]].originY, 
		(float)pageImages[isImageOnPage[pageNumber - 1]].originX + bmSize.width, 
		(float)pageImages[isImageOnPage[pageNumber - 1]].originY +  bmSize.height));}
	if( bBitMapRight ){ bBitMapRight = FALSE;
		D2D1_SIZE_F bmSizeRight = m_pBitmapRight->GetSize();
		pRT_->DrawBitmap( m_pBitmapRight,
		D2D1::RectF(
		(float)pageImages[isImageOnPage[pageNumber]].originX + width + g_siMargin,
		(float)pageImages[isImageOnPage[pageNumber]].originY, 
		(float)pageImages[isImageOnPage[pageNumber]].originX + bmSizeRight.width + width + g_siMargin, 
		(float)pageImages[isImageOnPage[pageNumber]].originY + bmSizeRight.height));}


	if( indexTablesOnPage[pageNumber - 1] > -1 )
		{ newDrawTables( tables[indexTablesOnPage[pageNumber - 1]], 1, rtSize);}
	graphicWindow = NONE;
	if( d2Dblock[pageNumber - 1].total > 0 ){   // Left page in graphic
		rect.left = 0.0f; rect.right = rtSize.width/2.0f;
		graphicWindow = LEFT;
		getD2DforPage( handle, pageNumber - 1, rect );}
	if( pageNumber < pageCount ){ // Don't go past last page, or beyond range of pageMode[]. 
		if( currentMode == DW_MODE && d2Dblock[ pageNumber ].total > 0 ){
			rect.left = rtSize.width/2.0f;  rect.right = rtSize.width;
			graphicWindow = RIGHT;
			getD2DforPage( handle, pageNumber, rect );}
	}
	if( pageNumber < pageCount ){ // Don't go past last page, or beyond range of pageMode[].
		if( d2Dblock[pageNumber - 1].total == 0 && d2Dblock[ pageNumber ].total == 0){
			graphicWindow = NONE;
				KillTimer(handle,7);
		}
	}
	if( bMenu) newDrawTables( menuTable, 0, rtSize );
	if( bFind == TRUE ){ 
		newDrawTables( findTable, 0 , rtSize );
		if( getTextX > -1 ) textMetrix( getTextX, getTextY,  rtSize);

		if( findIndex > -1 ){
			DWRITE_TEXT_RANGE findRange = {0};
			findRange.length = find.size() - 6;
			findRange.startPosition = findIndex;
			pDarkGrayBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
			if( gbRightPage ) pRightLayout_->SetDrawingEffect(pDarkGrayBrush_, findRange );
			else pTextLayout_->SetDrawingEffect(pDarkGrayBrush_, findRange );
			if( getTextX == -1 )
				getFindRect( findIndex, findIndex + findRange.length - 1, gbRightPage);
			if( gbRightPage ) {gfindRect.right += rtSize.width/2 - g_siMargin/2;
			gfindRect.left += rtSize.width/2 - g_siMargin/2;}
//			gbRightPage = FALSE;
			pRT_->DrawRectangle( &gfindRect, pDarkGrayBrush_, 0.5f, NULL);	//}
			log(L"ViewDW::drawDW(HWND handle)  findIndex = %i findRange.length = %i ", findIndex,					findRange.length); }
		SafeRelease(&pFindLayout_);
		pDWriteFactory_->CreateTextLayout( (PWSTR)find.c_str(),  
			(UINT32)lstrlen((PWSTR)find.c_str() ), pTextFormat_, rtSize.width, 20.0f,
			&pFindLayout_ );
		DWRITE_TEXT_RANGE findRange = {0};
		findRange.length = find.size();
		pFindLayout_->SetFontSize( 12.0f, findRange);
		pRT_->DrawTextLayout( origin, pFindLayout_, pBlackBrush_ );
	}

	origin.x = g_siMargin;

	    //STDMETHOD(SetLineBreakpoints)(
     //   UINT32 textPosition,
     //   UINT32 textLength,
     //   _In_reads_(textLength) DWRITE_LINE_BREAKPOINT const* lineBreakpoints
     //   ) PURE;

	pRT_->DrawTextLayout( origin, pTextLayout_, pBlackBrush_ );
	if( pageNumber < pageCount && currentMode == DW_MODE ){ // Don't go past last page
		origin.x = width  +  2.0f*g_siMargin;
		pRT_->DrawTextLayout( origin, pRightLayout_, pBlackBrush_ );
	}

	if( pageNumber > 1 )  pRT_->DrawTextLayout( pageNumberOrigin,	
								pPageNumberLayout_, pBlackBrush_ );
	if(currentMode == DW_MODE && pageNumber > 1){
		pnum.clear();
		pnum = to_wstring( pageNumber);
		rect.top = 0;
		rect.bottom = 20.0f;
		rect.right = rtSize.width;
		rect.left = rtSize.width - (UINT32)lstrlen((PWSTR)pnum.c_str())*9.0f;

		pRT_->DrawText( (PWSTR)pnum.c_str(), (UINT32)lstrlen((PWSTR)pnum.c_str()),
				   pMenuFormat_, rect,
				   pBlackBrush_ , D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL );}

	if( bHelp ){ newDrawTables( helpMenu, 2, rtSize ); bHelp = FALSE; }
	if( bColor ){ newDrawTables( colorMenu, 0, rtSize );
				 colorSliders( rtSize, NULL, slidersY ); }
	if( bFile ){ newDrawTables( fileMenu, 0, rtSize );
				 //colorSliders( rtSize, NULL, slidersY );
	}
//	if( bMessage ) drawMessageBox( rtSize );
//	if( bMessage )  newDrawTables( menuTable, 0 , rtSize );
	if (D2DERR_RECREATE_TARGET == pRT_->EndDraw()) DiscardDeviceResources();
//	pRT_->CheckWindowState();
//		} // if CheckWindowState()))
	} // if SUCCEEDED

}
int ViewDW::size( int width, int height){
#ifdef DEBUG_GB
	log(L"ViewDW::size %ix%i", width, height );
#endif
	gbWindowResized = TRUE;  
	// width and height is in pixels
    if (pRT_){		
		if(FAILED( pRT_->Resize(D2D1::SizeU(width, height)))){
			DiscardDeviceResources();
			return 0; }}
		log(L"pRT_ ViewDW::size %ix%i.", width, height );
/*        D2D1_SIZE_F size  = pRT_->GetSize();
		log(L"ViewDW::size %fx%f", size.width, size.height );
				//if( size.width >  3*g_siMargin ) size.width -= 2*g_siMargin;
				////else size.width = 0; //2*g_siMargin;
				//if( size.height >  2*g_siMargin ) size.height -= 2*g_siMargin;
		if (pTextLayout_)
		{
	//			log(L"ViewDW::size have pTextLayout_"  );
			pTextLayout_->SetMaxWidth(size.width);
			pTextLayout_->SetMaxHeight(size.height);
		}
		if (pPageNumberLayout_)
		{
	//			log(L"ViewDW::size have pPageNumberLayout_"  );
			pPageNumberLayout_->SetMaxWidth(size.width);
			pPageNumberLayout_->SetMaxHeight(size.height);
		}
		if (pTableLayout_)
		{
	//			log(L"ViewDW::size have pTableLayout_"  );
			pTableLayout_->SetMaxWidth(size.width);
			pTableLayout_->SetMaxHeight(size.height);
		}		
	} // if pRT_
	return 0;
	*/
}
///////////////////////////////////////////////////////////////////////////////
// initialize all controls
///////////////////////////////////////////////////////////////////////////////
void ViewDW::initControls(HWND handle)
{
#ifdef DEBUG_GB
	log(L"ViewDW::initControls(HWND handle)" );
#endif
	DWhandle = handle;

	menuTable.cellText.resize(iMenuSize);
	menuTable.total = 1;
	menuTable.rows.clear();
	menuTable.rows.resize(1);
	menuTable.rows[0] = 1;
	menuTable.cols.clear();
	menuTable.cols.resize(1);
	menuTable.cols[0] = iMenuSize;
	menuTable.cellWidth.clear();
	menuTable.cellWidth.resize(iMenuSize);
//	std::vector<std::vector<int>> colWidth;
	//menuTable.colWidth.clear();
	//menuTable.colWidth.resize(iMenuSize);
	menuTable.top.clear();
	menuTable.top.resize(1);
	menuTable.top[0] = -1.0f;
	menuTable.cellText[0]  = L"File";
	menuTable.cellText[1]  = L"Page";
	menuTable.cellText[2]  = L"Prior";
	menuTable.cellText[3]  = L"Next";
	menuTable.cellText[4]  = L"Cover";
	menuTable.cellText[5]  = L"TOC";
	menuTable.cellText[6]  = L"Index";
	menuTable.cellText[7]  = L"End";
	menuTable.cellText[8]  = L"<-";
	menuTable.cellText[9]  = L"->";
	menuTable.cellText[10]  = L"Help";
	menuTable.cellText[11]  = L"Color";
	menuTable.cellText[12]  = L"Find";


	helpMenu.cellText.resize(iMenuSize);
	helpMenu.total = 1;
	helpMenu.rows.clear();
	helpMenu.rows.resize(1);
	helpMenu.rows[0] = 1;
	helpMenu.cols.clear();
	helpMenu.cols.resize(1);
	helpMenu.cols[0] = iMenuSize;
	helpMenu.cellWidth.clear();
	helpMenu.cellWidth.resize(iMenuSize);
	helpMenu.top.clear();
	helpMenu.top.resize(1);
	helpMenu.top[0] = -1.0f;
	helpMenu.cellText[0]  = L"File: Space + f";
	helpMenu.cellText[1]  = L"Page: Space + p";
	helpMenu.cellText[2]  = L"Prior: Up arrow";
	helpMenu.cellText[3]  = L"Next: Down arrow";
	helpMenu.cellText[4]  = L"Cover: Home";
	helpMenu.cellText[5]  = L"TOC: Space + t";
	helpMenu.cellText[6]  = L"Index: Space + i";
	helpMenu.cellText[7]  = L"End";
	helpMenu.cellText[8]  = L"<-   Left arrow";
	helpMenu.cellText[9]  = L"->   Right arrow";
	helpMenu.cellText[10]  = L"Help: Space + h";
	helpMenu.cellText[11]  = L"Color: Space + c";
	helpMenu.cellText[12]  = L"Find: Space + w";

	findTable.cellText.resize(iMenuSize);
	findTable.total = 1;
	findTable.rows.clear();
	findTable.rows.resize(1);
	findTable.rows[0] = 1;
	findTable.cols.clear();
	findTable.cols.resize(1);
	findTable.cols[0] = iMenuSize;
	findTable.cellWidth.clear();
	findTable.cellWidth.resize(iMenuSize);
	findTable.top.clear();
	findTable.top.resize(1);
	findTable.top[0] = -1.0f;
	findTable.cellText[0]  = L"";
	findTable.cellText[1]  = L"";
	findTable.cellText[2]  = L"";
	findTable.cellText[3]  = L"";
	findTable.cellText[4]  = L"";
	findTable.cellText[5]  = L"";
	findTable.cellText[6]  = L"";
	findTable.cellText[7]  = L"";
	findTable.cellText[8]  = L"";
	findTable.cellText[9]  = L"";
	findTable.cellText[10]  = L"Previous";
	findTable.cellText[11]  = L"Next";
	findTable.cellText[12]  = L"Cancel";

	fileMenu.cellText.resize(iMenuSize);
	fileMenu.total = 1;
	fileMenu.rows.clear();
	fileMenu.rows.resize(1);
	fileMenu.rows[0] = 1;
	fileMenu.cols.clear();
	fileMenu.cols.resize(1);
	fileMenu.cols[0] = iMenuSize;
	fileMenu.cellWidth.clear();
	fileMenu.cellWidth.resize(iMenuSize);
	fileMenu.top.clear();
	fileMenu.top.resize(1);
	fileMenu.top[0] = -1.0f;
	fileMenu.cellText[0]  = L"Reload";
	fileMenu.cellText[1]  = L"Load";
	fileMenu.cellText[2]  = L"Cancel";
	fileMenu.cellText[3]  = L"";
	fileMenu.cellText[4]  = L"";
	fileMenu.cellText[5]  = L"";
	fileMenu.cellText[6]  = L"";
	fileMenu.cellText[7]  = L"";
	fileMenu.cellText[8]  = L"";
	fileMenu.cellText[9]  = L"";
	fileMenu.cellText[10]  = L"";
	fileMenu.cellText[11]  = L"";
	fileMenu.cellText[12]  = L"";

	colorMenu.cellText.resize(iMenuSize);
	colorMenu.total = 1;
	colorMenu.rows.clear();
	colorMenu.rows.resize(1);
	colorMenu.rows[0] = 1;
	colorMenu.cols.clear();
	colorMenu.cols.resize(1);
	colorMenu.cols[0] = iMenuSize;
	colorMenu.cellWidth.clear();
	colorMenu.cellWidth.resize(iMenuSize);
	colorMenu.top.clear();
	colorMenu.top.resize(1);
	colorMenu.top[0] = -1.0f;
	colorMenu.cellText[0]  = L"";
	colorMenu.cellText[1]  = L"";
	colorMenu.cellText[2]  = L"";
	colorMenu.cellText[3]  = L"";
	colorMenu.cellText[4]  = L"";
	colorMenu.cellText[5]  = L"";
	colorMenu.cellText[6]  = L"";
	colorMenu.cellText[7]  = L"";
	colorMenu.cellText[8]  = L"";
	colorMenu.cellText[9]  = L"";
	colorMenu.cellText[10]  = L"";
	colorMenu.cellText[11]  = L"";
	colorMenu.cellText[12]  = L"Cancel";

}

/*
void ViewDW::animate(bool flag)
{
#ifdef DEBUG_GB
	log(L"ViewDW::animate(bool flag)" );
#endif
    if(flag)
        buttonAnimate.setText(L"Stop");
    else
        buttonAnimate.setText(L"Animate");
}*/

int ViewDW::checkForEscape(int index, int page){
#ifdef DEBUG_GB
	log(L"ViewDW::checkForEscape(int index = %i page = %i )", index, page );
#endif
	//if( !index ) return index;
	wstring temp;
	temp.clear();
	if( page < 0 ) // have book
	temp.append( book, index - 1, 2 );
	else  if( index > 0 ) temp.append(pages[page], index + 1, 1 );
	// check for escape character
	log(temp);
	if( temp == L"\\"){
		log(L"escape  escape  escape  escape  escape");
		log(temp);
//		oops.clear();
//		oops.append( book, index ,  12 );
//		errorMessage( pageCount, index , oops);
		return index + 1;
	}
	return index;
}
void ViewDW::drawMessageBox( D2D1_SIZE_F size ){
#ifdef DEBUG_GB
	log(L"ViewDW::drawMessageBox()" );
#endif
/*
	POINT mousePos;
	D2D1_ROUNDED_RECT rndRect; rndRect.radiusX = 15.0f; rndRect.radiusY = 15.0f;
	GetPhysicalCursorPos( &mousePos );
//	GetCursorPos( &mousePos );
	ScreenToClient( DWhandle, &mousePos);

	//rndRect.rect.left = mousePos.x;
	//rndRect.rect.right = rndRect.rect.left + 50;
	//rndRect.rect.top = mousePos.y;
	//rndRect.rect.bottom = rndRect.rect.top + 50;


	rndRect.rect.left = 50;
	rndRect.rect.right = 250;
	rndRect.rect.top = 250;
	rndRect.rect.bottom = 350;

	pRedBrush_->SetColor(D2D1::ColorF(D2D1::ColorF::Sienna));
	pRT_->FillRoundedRectangle( &rndRect, pRedBrush_);

	if( mousePos.x < rndRect.rect.right &&  mousePos.x > rndRect.rect.left &&
	    mousePos.y > rndRect.rect.top &&  mousePos.y < rndRect.rect.bottom )
		bMessage = FALSE;
	else displayMenu( MESSAGE_BOX, MESSAGE_ID );
*/

}
void ViewDW::errorMessage(int page, int word, wstring msg){
#ifdef DEBUG_GB
	log(L"ViewDW::errorMessage(int page, int word)" );
#endif
//	PTSTR pFileName;
	int eCode = 0;
//	pFileName =  TEXT("Format Error");
	TCHAR Buf[140];
	wsprintf(Buf, TEXT("Format Error: Page %i Word %i"),  page, word );
//	eCode =  MessageBox(NULL, Buf, msg.c_str(), MB_RETRYCANCEL);

	eCode  =  MessageBox(NULL, msg.c_str(), Buf, MB_RETRYCANCEL);
	if( eCode == IDRETRY ){
	//	MessageBox(0, L"IDRETRY", L"IDRETRY",  MB_OK);
	//	bLoadFile = FALSE;
		loadResourceText();}

//	{MessageBox(NULL, TEXT("Have Unicode"), pFileName,MB_ICONINFORMATION);}	
		
			//	wsprintf(Buf, TEXT("wParam %i"), wParam);
			//	MessageBox(NULL, TEXT("KEYDOWN"), Buf, MB_ICONINFORMATION);
//        ::MessageBox(0, L"Problem inGlew land", L"Error", MB_ICONEXCLAMATION | MB_OK);

				//	wsprintf(szBuffer, TEXT("File size %i"),dwFileSize);
//					MessageBox(NULL, szBuffer, pFileName,NULL);
}

void ViewDW::displayMenu( int action, int menuItem ){
#ifdef DEBUG_GB
	log(L"ViewDW::displayMenu( action %i menuItem %i )", action, menuItem);
#endif

switch( action ){
	case FILE_MENU:  
		bFile = TRUE;
		bMenu = FALSE;
		fileMenu.top[0] = menuItem;  // Highlight the menu item
		break;
	case REMOVE_MENU:  //  User hit enter in active menu bMenuActive true in procedure.cpp
		log(L"ViewDW::displayMenu( case REMOVE_MENU:");
	 	bMenu = FALSE;
		bFind = FALSE;
		bColor = FALSE;
		bFile = FALSE;
		break;
	case PAGE_MENU:
		log(L"ViewDW::displayMenu( case PAGE_MENU:");
		bMenu = TRUE;
		bPage = TRUE;
		menuTable.cellText[PAGE_ID] = L"?";
		menuTable.top[0] = PAGE_ID;  // highlight page menu ?
		break;
	//case PAGE_MENU:
	//	log(L"ViewDW::displayMenu( case PAGE_MENU:");
	//	bMenu = TRUE;
	//	bPage = TRUE;
	//	if( charCount == 0 ) menuTable.cellText[PAGE_ID]  = L"?";  
	//	else menuTable.cellText[PAGE_ID] = to_wstring( menuItem );
	//	menuTable.cellText[PAGE_ID] = L"?";
	//	menuTable.top[0] = PAGE_ID;  // highlight page menu ?
	//	break;

	case PAGE_NUMBER_MENU:
		log(L"ViewDW::displayMenu( menuItem = pageNO = %i", menuItem);
		bMenu = TRUE;
		menuTable.top[0] = PAGE_ID;
		if( menuItem < 0 ) menuTable.cellText[PAGE_ID]  = L"?";  
		else menuTable.cellText[PAGE_ID] = to_wstring( menuItem );
		break;
	case HELP_MENU: bHelp = TRUE; // Pop up the help menu  -4
		break;
	case RGB_MENU:  // Pop up the color menu -5
		bColor = TRUE;
		bMenu = FALSE;
		colorMenu.top[0] = menuItem;
		break;
	case RGB_SLIDER:  
		log(L"ViewDW::displayMenu( case RGB_SLIDER:");
		break;
	case FIND_MENU:  // Pop up the find menu -3, highlight Previous, Next, Cancel
		log(L"ViewDW::displayMenu( case FIND_MENU: menuItem = %i", menuItem);
	//	if( menuItem == FIND_ID){
			if( !bFind ){ find.clear(); find = L"Find: ";}
			bFind = TRUE; 
			bMenu = FALSE;
			findTable.top[0] = menuItem; //  This will be FIND:, UP_ID, DOWN_ID, or CANCEL_ID
		//	getTextX = -1;  // flag not a texMetrix call
			break;
		//}  
/*		
		if( menuItem == FIND_PREVIOUS || menuItem == FIND_NEXT){
		if( !bFind ){ find.clear(); find = L"Find: ";}
		bFind = TRUE; 
		bMenu = FALSE;
		findTable.top[0] = menuItem; //  This will be FIND:, UP_ID, DOWN_ID, or CANCEL_ID
//		getTextX = -1;  // flag not a texMetrix call
		break;
		}  
*/
	case MAIN_MENU:
		menuTable.cellText[PAGE_ID]  = L"Page";
		findIndex = -1;   
		bMenu = TRUE;
		bMessage = FALSE;
		bFind = FALSE;
		bColor = FALSE;
		bFile = FALSE;
		bPage = FALSE;
		menuTable.top[0] = menuItem;  
		break;
	case MESSAGE_BOX:
		bMessage = TRUE;
		break;
	}			

}

HRESULT ViewDW::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
#ifdef DEBUG_GB
//	log(L"ViewDW::LoadResourceBitmap )");
#endif
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;
    HMODULE hModule = ::GetModuleHandle( NULL ); // my add
    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW( hModule, resourceName, resourceType);
    HRESULT hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(hModule, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(hModule, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
        
    }
    if (SUCCEEDED(hr))
    {
          // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
			// returns the original size of the bit map.
            hr = pSource->GetSize(&originalWidth, &originalHeight);
		//	log("originalWidth = %i, originalHeight = %i" ,originalWidth, originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                    if (SUCCEEDED(hr))
                    {
                        hr = pConverter->Initialize(
                            pScaler,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapDitherTypeNone,
                            NULL,
                            0.f,
                            WICBitmapPaletteTypeMedianCut
                            );
                    }
                }
            }
        }
        else
        {
                    
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
        //create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}