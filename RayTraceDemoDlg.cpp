
// RayTraceDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RayTraceDemo.h"
#include "RayTraceDemoDlg.h"
#include "afxdialogex.h"

#include "DemoScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRayTraceDemoDlg dialog

//OutputConsole g_Console; // move to Console.cpp
float g_fParam = 0.2F;
CDemoScene g_Scene;

// Rendering process callback function
class CRenderingCB : public IRenderWindowSink
{
public:
	CRenderingCB() { }
	virtual ~CRenderingCB() { }

public:
	virtual void OnNotifyRenderProgress(int row, int total_row)
	{
		float fRadio = (float)(row + 1) / total_row;
		fRadio *= 100.0f;
		CString sRadio;
		sRadio.Format(_T("rendering process: %.2f%%"), fRadio);
		theApp.GetMainWnd()->SetWindowText(sRadio);
	}

};

CRenderingCB g_RenderWndSink;

//
CRayTraceDemoDlg::CRayTraceDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RAYTRACEDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRayTraceDemoDlg::~CRayTraceDemoDlg()
{
	CMemBuffer::Unload();
}

//
void CRayTraceDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRayTraceDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRayTraceDemoDlg message handlers

BOOL CRayTraceDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
//	SetTimer(1000, 3000, NULL);

	// TODO: Add extra initialization here
	CMemBuffer::Load(theApp.m_hInstance, _T("F:/Laplata_CG_Library/RayTraceDemo/mem400x400.bmp"), 400, 400);

//	SetTimer(1000, 20000, NULL);
	g_Scene.SetRenderWndSink(&g_RenderWndSink);
	g_Scene.SetConsole(&g_Console);

	g_Scene.Setup();
	g_Scene.BuildScene();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRayTraceDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRayTraceDemoDlg::OnPaint()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CPaintDC dc(this);

	CMemBuffer	memBuf(&dc, 400, 400);

 	CDC *pSurface = memBuf.GetMemDC();
// 	for (int i = 0; i < 10; ++i)
// 	{
// 		pSurface->SetPixel(i, 10, RGB(255, 0, 0));
// 	}
//	memBuf.Clear(RGB(0, 0, 0));

// 	Vec3f	v0(3, 0, 0);
// 	Vec3f	v1(0, 4, 0);
// 	Vec3f	v2(0, 0, 5);
// 	Vec3f vResult = v0 + v1;
// 	vResult += v2;
// 	CString sOut;
// 	sOut.Format(TEXT("v0 + v1 + v2 = (%.2f,%.2f,%.2f)"), vResult.X(), vResult.Y(), vResult.Z());
// 	pSurface->TextOut(20, 20, sOut);
// 	sOut.Empty();
// 	sOut.Format(TEXT("and the length is: %.2f"), vResult.Length());
// 	pSurface->TextOut(20, 50, sOut);

// 	int nx = 400;
// 	int ny = 400;
// 	int y = 0;
// 	for (int j = ny - 1; j >= 0; j--)
// 	{
// 		for (int i = 0; i < nx; i++)
// 		{
// 			Color3f col((float)i / (float)nx, (float)j / (float)ny, g_fParam);
// 			int r = (int)(255.99 * col.R());
// 			int g = (int)(255.99 * col.G());
// 			int b = (int)(255.99 * col.B());
// 
// 			pSurface->SetPixel(i, y, RGB(r, g, b));
// 		}
// 		++y;
// 	}

	g_Scene.SetSurface(pSurface);

	//
	g_Console.Write(TEXT("start frame rendering...\n"));
	DWORD dwStart = ::GetTickCount();

	g_Scene.RenderScene();

	unsigned long dwEnd = ::GetTickCount();
	CString sDura;
	sDura.Format(TEXT("end frame endering!\ndelta per frame is: %ums\n"), dwEnd - dwStart);
	g_Console.Write(sDura.GetBuffer());

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRayTraceDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//
BOOL CRayTraceDemoDlg::OnEraseBkgnd(CDC* pDC)
{
//	CRect rc;
//	GetClientRect(&rc);
//	pDC->FillSolidRect(&rc, RGB(255, 255, 255));
	return TRUE;
}

void CRayTraceDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000)
	{
		g_Scene.UpdateScene(0.0f);

		::InvalidateRect(m_hWnd, NULL, FALSE);
	}
}
