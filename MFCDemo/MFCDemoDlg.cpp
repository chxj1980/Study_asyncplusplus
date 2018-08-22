
// MFCDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCDemo.h"
#include "MFCDemoDlg.h"
#include "afxdialogex.h"

#include <async++.h>

#ifdef _DEBUG
#pragma comment(lib,"../debug/Async++.lib")
#else
#pragma comment(lib,"../release/Async++.lib")
#endif // _DEBUG


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCDemoDlg dialog



CMFCDemoDlg::CMFCDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCDemoDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCDemoDlg message handlers

BOOL CMFCDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCDemoDlg::OnPaint()
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
HCURSOR CMFCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void SetProgressTo100(CProgressCtrl* pProce1)
{
	pProce1->SetRange(0, 100);
	pProce1->SetStep(1);
	for (int i = 0; i <= 100; i++)
	{
		Sleep(20);
		pProce1->SetPos(i);
	}
}

void SetProgressTo0(CProgressCtrl* pProce1)
{
	pProce1->SetRange(0, 100);
	pProce1->SetStep(1);
	for (int i = 100; i >= 0; i--)
	{
		Sleep(20);
		pProce1->SetPos(i);
	}
}

void CMFCDemoDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CProgressCtrl* pProce1 = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS1);
	CProgressCtrl* pProce2 = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS2);
	CProgressCtrl* pProce3 = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS3);


	async::spawn([pProce1]{
		SetProgressTo100(pProce1);
	});


	async::spawn([pProce2]{
		SetProgressTo100(pProce2);
	});


	async::spawn([pProce3]{
		SetProgressTo100(pProce3);
	});

}

CProgressCtrl* gProArray[3];

void CMFCDemoDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CProgressCtrl* pProce4 = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS4);
	CProgressCtrl* pProce5 = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS5);
	CProgressCtrl* pProce6 = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS6);
	if (pProce4->GetPos() == 0)
	{
		auto t1 = async::spawn([pProce4]{
			SetProgressTo100(pProce4);
		});
		auto t2 = t1.then([pProce5] {
			SetProgressTo100(pProce5);
		});

		auto t3 = t2.then([pProce6] {
			SetProgressTo100(pProce6);
		});
	}
	else
	{
		auto t1 = async::spawn([pProce6]{
			SetProgressTo0(pProce6);
		});

		auto t2 = t1.then([pProce5] {
			SetProgressTo0(pProce5);
		});

		auto t3 = t2.then([pProce4] {
			SetProgressTo0(pProce4);
		});
	}
}
