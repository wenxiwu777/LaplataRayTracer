
// RayTraceDemoDlg.h : header file
//

#pragma once


// CRayTraceDemoDlg dialog
class CRayTraceDemoDlg : public CDialogEx
{
// Construction
public:
	CRayTraceDemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CRayTraceDemoDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RAYTRACEDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
};
