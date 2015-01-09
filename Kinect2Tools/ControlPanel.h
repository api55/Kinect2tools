
// ControlPanel.h : header file
//

#pragma once
#include "afxwin.h"
#include "KinectViewer.h"
#include "afxcmn.h"


// ControlPanel dialog
class ControlPanel : public CDialogEx
{
// Construction
public:
	ControlPanel(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KINECT2TOOLS_DIALOG };
	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	void ControlPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	KinectViewer* viewer;
public:
	afx_msg void OnStartClicked();
	afx_msg void OnBnClickedbutton();
	afx_msg void OnBnClickedCancel();
	CSliderCtrl min_x_slider;
	CSliderCtrl max_x_slider;
	CSliderCtrl min_y_slider;
	CSliderCtrl max_y_slider;
	CSliderCtrl min_z_slider;
	CSliderCtrl max_z_slider;
};
