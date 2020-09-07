// PrintList_Setup.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PrintList_Setup.h"
#include ".\printlist_setup.h"


// PrintList_Setup dialog

IMPLEMENT_DYNAMIC(PrintList_Setup, CDialog)
PrintList_Setup::PrintList_Setup(CWnd* pParent /*=NULL*/)
	: CDialog(PrintList_Setup::IDD, pParent)
{
	m_CheckCS = FALSE;
	m_CheckLS = FALSE;
	m_CheckPH = FALSE;
	m_CheckPL = FALSE;
	m_CheckRC = FALSE;
	m_CheckTC = FALSE;
	m_CheckPF = FALSE;
	m_CheckPFR = FALSE;
	m_CheckPHR = FALSE;
	m_CheckCS = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckCS", 1);
	m_CheckLS = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckLS", 1);
	m_CheckPH = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPH", 1);
	m_CheckPL = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPL", 1);
	m_CheckRC = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckRC", 1);
	m_CheckTC = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckTC", 1);
	m_CheckPF = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPF", 1);
	m_CheckPFR = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPFR", 0);
	m_CheckPHR = AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPHR", 0);

	lfFontSize= lfFooterFontSize= lfListFontSize =12;
}

PrintList_Setup::~PrintList_Setup()
{
}

void PrintList_Setup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAMPLE, m_Sample);
	DDX_Check(pDX, IDC_CHECKCS, m_CheckCS);
	DDX_Check(pDX, IDC_CHECKLS, m_CheckLS);
	DDX_Check(pDX, IDC_CHECKPH, m_CheckPH);
	DDX_Check(pDX, IDC_CHECKPL, m_CheckPL);
	DDX_Check(pDX, IDC_CHECKRC, m_CheckRC);
	DDX_Check(pDX, IDC_CHECKTC, m_CheckTC);
	DDX_Check(pDX, IDC_CHECKPF, m_CheckPF);
	DDX_Check(pDX, IDC_CHECKPFR, m_CheckPFR);
	DDX_Check(pDX, IDC_CHECKPHR, m_CheckPHR);
}


BEGIN_MESSAGE_MAP(PrintList_Setup, CDialog)
	ON_BN_CLICKED(IDC_BUTTONHEADER, OnBnClickedButtonheader)
	ON_BN_CLICKED(IDC_BUTTONLISTF, OnBnClickedButtonlistf)
	ON_BN_CLICKED(IDC_BUTTONFOOTER, OnBnClickedButtonfooter)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECKPH, OnBnClickedCheckph)
	ON_BN_CLICKED(IDC_CHECKPF, OnBnClickedCheckpf)
	ON_BN_CLICKED(IDC_CHECKPL, OnBnClickedCheckpl)
	ON_BN_CLICKED(IDC_CHECKLS, OnBnClickedCheckls)
	ON_BN_CLICKED(IDC_CHECKCS, OnBnClickedCheckcs)
END_MESSAGE_MAP()


// PrintList_Setup message handlers

void PrintList_Setup::OnBnClickedButtonheader()
{
		CFontDialog FontDialog(&lf);
	if (FontDialog.DoModal() != IDOK ) return;
	FontDialog.GetCurrentFont(&lf);
	lfFontSize=FontDialog.GetSize()/10;

	AfxGetApp()->WriteProfileString("Spectrum Globals","MIHeader Font Face",lf.lfFaceName);
	AfxGetApp()->WriteProfileInt("Spectrum Globals","MIHeader Font height",lf.lfHeight);
	AfxGetApp()->WriteProfileInt("Spectrum Globals","MIHeader Font pointsize",FontDialog.GetSize()/10);
	Invalidate();
}

void PrintList_Setup::OnBnClickedButtonlistf()
{
	CFontDialog FontDialog(&lfList);
	if (FontDialog.DoModal() != IDOK ) return;
	FontDialog.GetCurrentFont(&lfList);
	lfListFontSize=FontDialog.GetSize()/10;

	AfxGetApp()->WriteProfileString("Spectrum Globals","MIList Font Face",lfList.lfFaceName);
	AfxGetApp()->WriteProfileInt("Spectrum Globals","MIList Font height",lfList.lfHeight);
	AfxGetApp()->WriteProfileInt("Spectrum Globals","MIList Font pointsize",FontDialog.GetSize()/10);
	
	Invalidate();
}

void PrintList_Setup::OnBnClickedButtonfooter()
{
	CFontDialog FontDialog(&lfFooter);
	if (FontDialog.DoModal() != IDOK ) return;
	FontDialog.GetCurrentFont(&lfFooter);
	lfFooterFontSize=FontDialog.GetSize()/10;
	AfxGetApp()->WriteProfileString("Spectrum Globals","MIFooter Font Face",lfFooter.lfFaceName);
	AfxGetApp()->WriteProfileInt("Spectrum Globals","MIFooter Font height",lfFooter.lfHeight);
	AfxGetApp()->WriteProfileInt("Spectrum Globals","MIFooter Font pointsize",FontDialog.GetSize()/10);

	Invalidate();
}

BOOL PrintList_Setup::OnInitDialog() 
{
	
	CDialog::OnInitDialog();
	
	CFont tempfont;
	
	tempfont.CreateStockObject(SYSTEM_FONT);
	tempfont.GetObject(sizeof(LOGFONT), &lf);
	tempfont.GetObject(sizeof(LOGFONT), &lfFooter);
	tempfont.GetObject(sizeof(LOGFONT), &lfList);
	
	GetDlgItem(IDC_SAMPLE)->GetWindowRect(&m_RectSample);
	ScreenToClient(&m_RectSample);

	strcpy(lf.lfFaceName, AfxGetApp()->GetProfileString("Spectrum Globals","MIHeader Font Face","Times New Roman") );
	lf.lfHeight =  AfxGetApp()->GetProfileInt("Spectrum Globals","MIHeader Font height", 12);
	strcpy(lfFooter.lfFaceName, AfxGetApp()->GetProfileString("Spectrum Globals","MIFooter Font Face","Times New Roman") );
	lfFooter.lfHeight =  AfxGetApp()->GetProfileInt("Spectrum Globals","MIFooter Font height", 12);
	strcpy(lfList.lfFaceName, AfxGetApp()->GetProfileString("Spectrum Globals","MIList Font Face","Times New Roman") );
	lfList.lfHeight =  AfxGetApp()->GetProfileInt("Spectrum Globals","MIList Font height", 12);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PrintList_Setup::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CFont Font, Footer, List;
	COLORREF line=0;

	Font.CreateFontIndirect(&lf);
	Footer.CreateFontIndirect(&lfFooter);
	List.CreateFontIndirect(&lfList);
	dc.SelectObject(&Font);
	dc.SetBkMode(TRANSPARENT);
	UINT OldFlag = dc.SetTextAlign(TA_TOP | TA_CENTER);

	CPen Pen;
	Pen.CreatePen(PS_SOLID, 2, line);
	dc.SelectObject(Pen);

	if(m_CheckPH) {
		dc.TextOut(m_RectSample.left+((m_RectSample.right - m_RectSample.left)/2), m_RectSample.top+10, "Elemental Composition");
		dc.TextOut(m_RectSample.left+((m_RectSample.right - m_RectSample.left)/2), m_RectSample.top+20, "Possible Comp");
	}
	if(m_CheckPF) {
		dc.SetTextAlign(OldFlag);
		OldFlag = dc.SetTextAlign(TA_BOTTOM | TA_CENTER);
		dc.SelectObject(&Footer);
		dc.TextOut(m_RectSample.left+((m_RectSample.right - m_RectSample.left)/2), m_RectSample.bottom-10, "Page :          1/1/2005");
	}

	OldFlag = dc.SetTextAlign(TA_TOP | TA_CENTER);
	dc.SelectObject(&List);
	if(m_CheckCS) {
		dc.TextOut(m_RectSample.left+((m_RectSample.right - m_RectSample.left)/2), m_RectSample.top+((m_RectSample.bottom - m_RectSample.top)/2+1), "| C6H6 | 200 | 0.11111 |");

	}
	else dc.TextOut(m_RectSample.left+((m_RectSample.right - m_RectSample.left)/2), m_RectSample.top+((m_RectSample.bottom - m_RectSample.top)/2+1), " C6H6  200  0.11111 ");

	if(m_CheckLS) {
	dc.MoveTo(m_RectSample.left+5, m_RectSample.top+((m_RectSample.bottom - m_RectSample.top)/2));
	dc.LineTo(m_RectSample.right-5,m_RectSample.top+((m_RectSample.bottom - m_RectSample.top)/2));
	dc.MoveTo(m_RectSample.left+5, m_RectSample.top+((m_RectSample.bottom - m_RectSample.top)/2)+30);
	dc.LineTo(m_RectSample.right-5,m_RectSample.top+((m_RectSample.bottom - m_RectSample.top)/2)+30);

	}
	dc.SetTextAlign(OldFlag);
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void PrintList_Setup::OnDestroy() 
{
	CDialog::OnDestroy();
	
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckCS", m_CheckCS);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckLS", m_CheckLS);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckPH", m_CheckPH);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckPL", m_CheckPL);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckRC", m_CheckRC);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckTC", m_CheckTC);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckPF", m_CheckPF);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckPFR", m_CheckPFR);
	 AfxGetApp()->WriteProfileInt("Spectrum Globals","CheckPHR", m_CheckPHR);
}


void PrintList_Setup::OnBnClickedCheckph()
{
	if(m_CheckPH) m_CheckPH = FALSE;
	else m_CheckPH = TRUE;
	Invalidate();
}

void PrintList_Setup::OnBnClickedCheckpf()
{
	if(m_CheckPF) m_CheckPF = FALSE;
	else m_CheckPF = TRUE;
	Invalidate();
}

void PrintList_Setup::OnBnClickedCheckpl()
{
	if(m_CheckPL) {
		m_CheckPL = FALSE;
		m_RectSample.left +=10;
		m_RectSample.right -=10;
		GetDlgItem(IDC_SAMPLE)->MoveWindow(&m_RectSample);
	}
	else {
		m_CheckPL = TRUE;
		m_RectSample.left -=10;
		m_RectSample.right +=10;
		GetDlgItem(IDC_SAMPLE)->MoveWindow(&m_RectSample);
	}
	Invalidate();
}

void PrintList_Setup::OnBnClickedCheckls()
{
	if(m_CheckLS) m_CheckLS = FALSE;
	else m_CheckLS = TRUE;
	Invalidate();
}

void PrintList_Setup::OnBnClickedCheckcs()
{
	if(m_CheckCS) m_CheckCS = FALSE;
	else m_CheckCS = TRUE;
	Invalidate();
}
