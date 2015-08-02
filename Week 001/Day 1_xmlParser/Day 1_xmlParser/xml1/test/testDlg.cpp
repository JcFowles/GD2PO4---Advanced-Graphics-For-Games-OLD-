// testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CtestDlg dialog




CtestDlg::CtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}

BEGIN_MESSAGE_MAP(CtestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CtestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CtestDlg message handlers

BOOL CtestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	//m_tree.
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtestDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtestDlg::OnBnClickedButton1()
{
	//_TCHAR *test = (_TCHAR *)calloc(256,sizeof(_TCHAR));
	//lstrcpy(test,_T("012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234"));
	//int k = 0;
	//while (k < 100)
	//{
	//	test = concat(test,_T('k'));
	//	k++;
	//}
	//return;
	CFileDialog *dlg = new CFileDialog(true);
	CString pathName, CszXML = _T("");
	if(dlg->DoModal() == IDOK )
	{
		 pathName = dlg->GetPathName();
	}
	delete (dlg);
	CFile *fp = new CFile(pathName,CFile::modeRead);
	_TCHAR c_arr[101] = {0};
	while(true)
	{
		memset(c_arr,0,100);
		if(fp->Read(c_arr,100)<100)
		{
			CszXML.Append(c_arr);
			break;
		}
		CszXML.Append(c_arr);
	}


		fp->Close();
	delete(fp);
	Cxml *xml = new  Cxml();
	xml->ParseString(CszXML.GetBuffer());
	m_tree.DeleteAllItems();
	CNode *root = xml->GetRootNode();
	fill(NULL,root);
	
	delete(xml);
}

void CtestDlg::fill(HTREEITEM tree_node, CNode* xml_node)
{
	CAttribute *a = NULL;
	CString szText= _T("");
	if(xml_node == NULL)
		return;
	szText.Append(xml_node->GetName());
	szText.Append( _T(" - "));
	szText.Append(xml_node->GetValue());
	while((a=xml_node->GetNextAttribute()) != NULL)
	{
		szText.Append(_T(" _ "));
		szText.Append(a->GetName());
		szText.Append(_T(" = "));
		szText.Append(a->GetValue());
	}
	HTREEITEM hLocal = NULL;
	if(tree_node == NULL)
	{
		hLocal = m_tree.InsertItem(szText);
	}
	else
	{
		hLocal = m_tree.InsertItem(szText,tree_node);
	}
	while(true)
	{
		CNode *chld = xml_node->GetNextChild();
		if(chld == NULL)
			return;
		fill(hLocal,chld);
	}
}
