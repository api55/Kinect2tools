
// ControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Kinect2Tools.h"
#include "ControlPanel.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// ControlPanel dialog

void ControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, x_min, min_x_slider);
	DDX_Control(pDX, x_max, max_x_slider);
	DDX_Control(pDX, y_min, min_y_slider);
	DDX_Control(pDX, y_max, max_y_slider);
	DDX_Control(pDX, z_min, min_z_slider);
	DDX_Control(pDX, z_max, max_z_slider);
}


ControlPanel::ControlPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(ControlPanel::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BEGIN_MESSAGE_MAP(ControlPanel, CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(start_button, &ControlPanel::OnStartClicked)
	ON_BN_CLICKED(update_button, &ControlPanel::OnBnClickedbutton)
	ON_BN_CLICKED(IDCANCEL, &ControlPanel::OnBnClickedCancel)
END_MESSAGE_MAP()


// ControlPanel message handlers

BOOL ControlPanel::OnInitDialog()
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

	// TODO: Add extra initialization here
	viewer = new KinectViewer();
	min_x_slider.SetRange(0,1000);
	min_y_slider.SetRange(0, 1000);
	min_z_slider.SetRange(0, 1000);
	max_x_slider.SetRange(0, 1000);
	max_y_slider.SetRange(0, 1000);
	max_z_slider.SetRange(0, 1000);
	max_x_slider.SetPos(1000);
	max_y_slider.SetPos(1000);
	max_z_slider.SetPos(1000);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void ControlPanel::OnSysCommand(UINT nID, LPARAM lParam)
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

void ControlPanel::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ControlPanel::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void ControlPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString t;
	double position = 0.0;
	switch (pScrollBar->GetDlgCtrlID()){
	case x_min:
		position = ((double)min_x_slider.GetPos()) / 10.0;
		t.Format(_T("%0.1f %%"), position);
		SetDlgItemText(x_min_label, t);
		viewer->min_x = position;
		break;
	case x_max:
		position = ((double)max_x_slider.GetPos()) / 10.0;
		t.Format(_T("%0.1f %%"), position);
		SetDlgItemText(x_max_label, t);
		viewer->max_x = position;
		break;
	case y_min:
		position = ((double)min_y_slider.GetPos()) / 10.0;
		t.Format(_T("%0.1f %%"), position);
		SetDlgItemText(y_min_label, t);
		viewer->min_y = position;
		break;
	case y_max:
		position = ((double)max_y_slider.GetPos()) / 10.0;
		t.Format(_T("%0.1f %%"), position);
		SetDlgItemText(y_max_label, t);
		viewer->max_y = position;
		break;
	case z_min:
		position = ((double)min_z_slider.GetPos()) / 10.0;
		t.Format(_T("%0.1f %%"), position);
		SetDlgItemText(z_min_label, t);
		viewer->min_z = position;
		break;
	case z_max:
		position = ((double)max_z_slider.GetPos()) / 10.0;
		t.Format(_T("%0.1f %%"), position);
		SetDlgItemText(z_max_label, t);
		viewer->max_z = position;
		break;
	default:
		break;
	}
}
void ControlPanel::OnStartClicked()
{
	// TODO: Add your control notification handler code here
	
	viewer = new KinectViewer();
	GetDlgItem(start_button)->EnableWindow(false);
}


void ControlPanel::OnBnClickedbutton()
{
	// TODO: Add your control notification handler code here
	if (!viewer){
		return;
	}
	while (true){
		viewer->update();
	}
}




void ControlPanel::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnCancel();
	CString t;
	t.Format(_T("nothing to do %d"), 1);
	SetDlgItemText(z_max_label, t);
}
