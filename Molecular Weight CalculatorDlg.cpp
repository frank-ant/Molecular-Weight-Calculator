// Molecular Weight CalculatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Molecular Weight CalculatorDlg.h"
#include "Atomic_elements.h"   // elm[]  def
#include "accMassListing.h"    // iso[] def
#include "groupListing.h"      // groupList[] def
#include "PrintList_Setup.h"
#include ".\molecular weight calculatordlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutMwtDlg dialog used for App About

class CAboutMwtDlg : public CDialog
{
public:
	CAboutMwtDlg();

// Dialog Data
	enum { IDD = IDD_MwtABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutMwtDlg::CAboutMwtDlg() : CDialog(CAboutMwtDlg::IDD)
{
}

void CAboutMwtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutMwtDlg, CDialog)
END_MESSAGE_MAP()


//////// CMolecularWeightCalculatorDlg dialog////////////////////////////////////////////



CMolecularWeightCalculatorDlg::CMolecularWeightCalculatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMolecularWeightCalculatorDlg::IDD, pParent)
	, m_RadioCharge(0)
	, m_radioChargeState(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nel = sizeof(elm) / sizeof(element);
	niso = sizeof(iso) / sizeof(isotope);
	nGroupElements = sizeof(groupList) / sizeof(Group);
	eadd = 0;
	iadd = 0;
	SingleElement = NULL;
	FormulaError = 0;
	setpointers();
	/*m_RadioCharge = 1;*/
	newPeak = NULL;

	//{{AFX_DATA_INIT(CMolecularWeightCalculatorDlg)
	m_EditFormula = AfxGetApp()->GetProfileString("ISO", "Formula", "C6H6");
	m_EditIntensityLimit = AfxGetApp()->GetProfileString("ISO", "IntenLimit", "0.000001");
	//}}AFX_DATA_INIT
}

CMolecularWeightCalculatorDlg::~CMolecularWeightCalculatorDlg()
{
	AfxGetApp()->WriteProfileString("ISO", "Formula", m_EditFormula);
	AfxGetApp()->WriteProfileString("ISO", "IntenLimit", m_EditIntensityLimit);
	for(int i = 0; i < AllElements.GetSize(); i++) {
		SingleElement = (FormulaElement *)AllElements.GetAt(i);
		delete SingleElement;		
	}
	if(newPeak) delete [] newPeak;
}

void CMolecularWeightCalculatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMolecularWeightCalculatorDlg)
	DDX_Text(pDX, IDC_EDITFORMULA, m_EditFormula);
	DDV_MaxChars(pDX, m_EditFormula, 1024);
	DDX_Control(pDX, IDC_LIST2, m_HeadingListBox);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Text(pDX, IDC_EDIT_INTENSITY_LIMIT, m_EditIntensityLimit);
	DDV_MaxChars(pDX, m_EditIntensityLimit, 28);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO1NEUTRAL, m_NeutralCtrl);
}

BEGIN_MESSAGE_MAP(CMolecularWeightCalculatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONCalculate, OnBUTTONCalculate)
	ON_BN_CLICKED(IDC_BUTTONPrint, OnBUTTONPrint)
	ON_BN_CLICKED(IDC_BUTTONSaveAs, OnBUTTONSaveAs)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_COPY_BUTTON, OnCopyButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_PRINT_SETUP, OnBnClickedPrintSetup)
	ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HELPBUTTON, OnBnClickedHelpbutton)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTONPRINTTABLE, OnBnClickedButtonprinttable)
	ON_BN_CLICKED(IDC_COPY_SPECTRUM, OnBnClickedCopySpectrum)
	ON_BN_CLICKED(IDC_RADIO2POSITIVE, &CMolecularWeightCalculatorDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3NEGATIVE, &CMolecularWeightCalculatorDlg::OnBnClickedRadio3negative)
	ON_BN_CLICKED(IDC_RADIO1NEUTRAL, &CMolecularWeightCalculatorDlg::OnBnClickedRadio1neutral)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMolecularWeightCalculatorDlg message handlers

void CMolecularWeightCalculatorDlg::OnOK() 
{
	UpdateData();
	AfxGetApp()->WriteProfileString("ISO", "Formula", m_EditFormula);
	AfxGetApp()->WriteProfileString("ISO", "IntenLimit", m_EditIntensityLimit);
	CDialog::OnOK();
}


void CMolecularWeightCalculatorDlg::CheckFormula()
{
	int FirstBracket=0, FirstDot=0;
	WorkingFormula = m_EditFormula;
	
	ZeroAllElements();
	
	while(FirstBracket > -1) {
		FirstBracket = WorkingFormula.Find('(');
		if(FirstBracket > -1) HasBrackets();
	}

	while(FirstDot > -1) {
		FirstDot = WorkingFormula.Find('.');
		if(FirstDot > -1) HasDots();
	}
	AddToElementArray(WorkingFormula);
	ExtractFormula();
}


void CMolecularWeightCalculatorDlg::CreateColumns()
{
	LV_COLUMN lv;
	int i = 1;

	m_ListBox.ModifyStyle(NULL, LVS_REPORT);
	m_ListBox.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP);
	lv.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;
	
	// Create 4 columns in the list control and
	// give each column a header string.
		lv.cx = 15 * m_ListBox.GetStringWidth("12345678") / 10;
		lv.pszText = "Mass:  ";
		m_ListBox.InsertColumn(i, &lv);
		m_ListBox.SetColumnDouble(i++ );
		
		lv.cx = 15 * m_ListBox.GetStringWidth("12345678") / 10;
		lv.pszText = "Intensity";
		m_ListBox.InsertColumn(i, &lv);
		m_ListBox.SetColumnDouble(i++ );

		lv.cx = 15 * m_ListBox.GetStringWidth("133333");
		lv.pszText = "Isotope Pattern";
		m_ListBox.InsertColumn(i++, &lv);
		
}
// CMolecularWeightCalculatorDlg message handlers

BOOL CMolecularWeightCalculatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_SAMPLE)->GetWindowRect(&m_RectSample);
	ScreenToClient(&m_RectSample);
	
	/*m_NeutralCtrl.CheckRadioButton(IDC_RADIO1NEUTRAL,IDC_RADIO3NEGATIVE,IDC_RADIO1NEUTRAL);*/
	CreateColumns();
	m_cPrintListCtrl.SetListCtrl(m_ListBox.GetSafeHwnd()); 
	m_cPrintListCtrl.ResizeColumnsToFitContent(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckRC", 0));
	m_cPrintListCtrl.TruncateColumn(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckTC", 1));
	m_cPrintListCtrl.PrintColumnSeparator(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckCS", 1));
	m_cPrintListCtrl.PrintLineSeparator(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckLS", 0));
	m_cPrintListCtrl.PrintHead(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPH", 1));
	m_cPrintListCtrl.SetHeadFont(AfxGetApp()->GetProfileString("Spectrum Globals","MIHeader Font Face","Times New Roman"), 
								 AfxGetApp()->GetProfileInt("Spectrum Globals","MIHeader Font pointsize", 12));
	m_cPrintListCtrl.SetFootFont(AfxGetApp()->GetProfileString("Spectrum Globals","MIFooter Font Face","Times New Roman"),
								 AfxGetApp()->GetProfileInt("Spectrum Globals","MIFooter Font pointsize", 10));
	m_cPrintListCtrl.SetListFont(AfxGetApp()->GetProfileString("Spectrum Globals","MIList Font Face","Times New Roman"),
								 AfxGetApp()->GetProfileInt("Spectrum Globals","MIList Font pointsize", 10));
	m_cPrintListCtrl.SetPrinterOrientation(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPL", 0));
	m_cPrintListCtrl.PrintHeadBkgnd(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPHR", 0), RGB(255,0,0));
	m_cPrintListCtrl.PrintFootBkgnd(AfxGetApp()->GetProfileInt("Spectrum Globals","CheckPFR", 0), RGB(255,0,0));
	m_cPrintListCtrl.m_cHeadListBox = &m_HeadingListBox;
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu = "Help on Molecular weight calculator";
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
	m_NeutralCtrl.CheckDlgButton(IDC_RADIO1NEUTRAL,1);
	OnBUTTONCalculate();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMolecularWeightCalculatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutMwtDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}



// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMolecularWeightCalculatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMolecularWeightCalculatorDlg::HandleSaveAsText(CString filename)
{
	CString tempString;
	CFile * outfile = new CFile(filename,CFile::modeWrite | CFile::modeCreate);
	int count;

	if(!outfile) {
		MessageBox( "Invalid drive or directory",
		 "File error", MB_OK | MB_ICONQUESTION);
		 return;
		}
	CArchive ar(outfile,CArchive::store);

	for(int i=0; i<m_HeadingListBox.GetCount();i++) {
			m_HeadingListBox.GetText(i,tempString);
			tempString.Replace(':','\t');
			if(tempString.Find("Mass Percent")) tempString += "\n";
			ar.WriteString(tempString);
	}

	ar.WriteString("\n");
	ar.WriteString("Isotope Pattern Report for   : ");
	ar.WriteString(OriginalFormula);
	ar.WriteString("     -> ");
	ar.WriteString(m_EditFormula);
	ar.WriteString("\nMass          Intensity            Isotope Pattern \n");
	ar.WriteString("-------------------------------------------------------------------\n");

	count = m_ListBox.GetItemCount();
	for(int i = 0; i < count; i++) {
		filename = m_ListBox.GetItemText(i,0);
		filename += "    ";
		tempString = m_ListBox.GetItemText(i,1);
		while(tempString.GetLength() < 13) tempString += " ";
		filename += tempString;
		tempString = m_ListBox.GetItemText(i,2);
		filename += tempString;
		filename += "\n";
		ar.WriteString(filename);
	}
	ar.Close();
	if(outfile) delete outfile;
}

void CMolecularWeightCalculatorDlg::OnBUTTONSaveAs() 
{
	CString filename, fileExt;

	filename = "iso distribution of ";
	filename += OriginalFormula;
	fileExt = "txt";
	
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	LPCTSTR lpszFilter = "All Files (*.*)|*.*"
						 "|Text files (*.txt)|*.txt|";
						 
	CFileDialog * dlg = new CFileDialog(FALSE, fileExt,filename,dwFlags,lpszFilter);

	 if (dlg->DoModal() == IDOK) {
		  filename = dlg->GetPathName(); 
		  fileExt = dlg->GetFileExt();
		  HandleSaveAsText(filename);
	 }	
	 if (dlg) delete dlg;
}


void CMolecularWeightCalculatorDlg::OnBUTTONCalculate() 
{
	UpdateData();
	OriginalFormula = m_EditFormula;
	IsoPattern.RemoveAll();
	CheckFormula();
	SetDlgItemText(IDC_STATIC_FORMULA, m_EditFormula);
	strcpy(buf,m_EditFormula);
	FindIsotopes();
	if(!FormulaError) Invalidate();
	else {
		FormulaError = 0;
		m_EditFormula = "";
		return;
	}
	FillCalculatedValues();
	m_ListBox.SortDoubleItems(0,1,0,-1);
	
}


void CMolecularWeightCalculatorDlg::setpointers (void)      /* set pointers in el entries to start of isotopes */
{                            /* for that element in the iso table */
	int i;
	isotope *p = iso;

	for (i = 0;  i < nel;  i++)	{
		elm[i].p = p;
		p += elm[i].niso;
	}
}


int CMolecularWeightCalculatorDlg::squob(char *buf)      /* squeeze out the blanks in s and return the length */
			  /* of the resultant string not counting terminating null*/
{
	int i,j;
	char c;

	i=j=0;
	while ( (c = buf[i++]) != '\0')
		if (c != ' ') buf[j++]=c;
	buf[j]='\0';
	return(j);
}


void CMolecularWeightCalculatorDlg::foutput(int oz, int nr)
{
	atoms[natoms].atno = oz;		/* Ordnungszahl */
	atoms[natoms].count = nr;		/* No. of atoms of this element */
	natoms++;
	if (natoms >= MAXAT) {
		MessageBox("Formula too long","Formula Error",MB_OK);
		return;
	}
}


isotope CMolecularWeightCalculatorDlg::imin(int atno)        /* isotope of lowest mass of element atno */
{
	if (atno < ADDBASE)
	  return (*(elm[atno-1].p));  /* element 1 is in table entry 0, etc. */
	else return(*(addel[atno-ADDBASE].p));
}


isotope CMolecularWeightCalculatorDlg::imax(int atno)        /* isotope of highest mass of element atno */
{
	if (atno < ADDBASE)
	  return( *(elm[atno - 1].p + elm[atno -1].niso - 1));
	else return(*(addel[atno-ADDBASE].p + addel[atno -ADDBASE].niso - 1));
}


int CMolecularWeightCalculatorDlg::atno(char *str)  			/* return atom # or 0 if str not valid element symbol */
{
	int i;

	for (i = 0;  i < nel;  i++)             /* try natural elements first */
	   if (0 == strcmp(elm[i].sym, str))	/* if symbol is found in elem. table */
		return(i + 1);			/* return correct atom number */

	for (i = 0;  i < eadd;  i++)		/* try 'user-def.' elements */
	  if (0 == strcmp(addel[i].sym, str))	/* if symbol is found in 'user-def.'elem. table */
		return(i + ADDBASE);            /* return a high 'atomic #' on user-defined elements */

	return 0;				/* this is 'else' */
}


int CMolecularWeightCalculatorDlg::formula(char *in)
/* Determine if input is valid formula. Set composition in table *atoms.
   return 1 if OK, 0 if not call foutput() */
{
	int 	a, n,      /* to handle (element, count) pairs. a is El.No., n is the number */
		state;     /* number of chars read */
	char 	ch, buf[3];

	state = a = n= 0;				/* here, no char was read */

	while(0 != (ch = *(in++)))		/* while not end of string */
	{
	switch(state)
		{
		case 0:
			if(isupper(ch))		/* uppercase letter ? */
				{
				buf[0] = ch;		/* take first char of element symbol */
				buf[1] = 0;			/* ASCII zero to terminate string */
				state = 1;			/* i.e. 1st char was read */
				}
			else goto error;
			break;

		case 1:
			if (isdigit(ch))			/* is it a number ? */
				{
				if(0 == (a = atno(buf))) goto error;	/* if NOT an element */
				n = ch - '0';		/* just like 'atoi()' */
				state = 2;			/* 2nd char was read */
				}
			else if(islower(ch))
				{
				buf[1] = ch;       	 	/* take char as 2nd letter */
				buf[2] = 0;			/* terminate string */
				if(0 == (a = atno(buf))) goto error;	/* see above */
				state = 3;
				}
			else if(isupper(ch))
				{
				if(0 == (a = atno(buf))) goto error;	/* see above */
				n = 1;                      /* 1st char read */
				foutput(a, n);
				buf[0] = ch;		/* take first char of element symbol */
				buf[1] = 0;			/* ASCII zero to terminate string */
				state = 1;			/* i.e. 1st char was read */
				}
			else goto error;
			break;

		case 2:
			if (isdigit(ch))                    /* is it a number */
			n = 10 * n + ch - '0';		/* YES -> get value (tens) */
			else if(isupper(ch))  {
				foutput(a, n);
				buf[0] = ch;		/* take first char of element symbol */
				buf[1] = 0;			/* ASCII zero to terminate string */
				state = 1;			/* i.e. 1st char was read */
			}
			else goto error;
			break;

		case 3:
			if (isdigit(ch))      		/* is it a number */
			{				/* YES */
				n = ch - '0';			/* get value */
				state = 2;
			}
			else if (isupper(ch)) {
				if(0 == (a = atno(buf))) goto error;
				n = 1;
				foutput(a, n);
				buf[0] = ch;			/* take first char of element symbol */
				buf[1] = 0;			/* ASCII zero to terminate string */
				state = 1;			/* i.e. 1st char was read */
			}
			else  goto error;
			break;

		}  /* end of case */
	}          /* end of while */

	if (state == 1 || state == 3) n = 1;
	if (state != 0)	{
		if(0 == (a = atno(buf))) goto error;
		foutput(a, n);
		return(1);
	}

error:
	FormulaError = 1;
	MessageBox("Bad Formula","Formula Error",MB_OK);
	return(0);
}



void CMolecularWeightCalculatorDlg::addelement(void)            /* user-defined element */
{
/*	int i, j, k, first;
	float f, sumpc;
	char buf1[81], buf2[81], *p = NULL;

	while(1)
	{
		printf("Enter symbol for element: ");
		fgets(buf1, 80, stdin);
		for(i = 0;  i < strlen(buf1);  i++)
			if (buf1[i] == '\n' || buf1[i] == '\r')
				buf1[i] = ' ';
		squob(buf1);
		buf1[2] = 0;
		if (!isupper(buf1[0]) || (buf1[1] != 0 && (!islower(buf1[1]))))
			{
			printf("Bad symbol\n\n");
			continue;
			}
		if(0 == atno(buf1))   */  /* not already known symbol */
/*			{
			addel[eadd].sym = (char *)malloc(3);
			strcpy(addel[eadd].sym, buf1);
			break;
			}
		printf("This symbol is already in use.\n\n");
		}
	printf("\nEnter mass - percent abundance pairs, one pair per line, with space between.\n");
	printf("Enter an empty line to finish.\n");

startiso:
	addel[eadd].niso = 0;
	first = 1;
	sumpc = 0.0;
	while(1)
	{
	printf(">");
	fgets(buf1, 80, stdin);
	if (strlen(buf1) <= 1)
		{
		if(sumpc > 99.5 && sumpc < 100.5) break;
		else if(sumpc <= 99.5)
			{
			printf("Sum of isotope percentages is %.1f, press Y if ok, N to enter more:", sumpc);
			fgets(buf2, 80, stdin);
			if (buf2[0] == 'y' || buf2[0] == 'Y') break;
			else continue;
			}
		else
			{
			printf("Sum of isotope percentages is %.1f, press Y if ok, N to start over:", sumpc);
			fgets(buf2, 80, stdin);
			if (buf2[0] == 'y' || buf2[0] == 'Y') break;
			else goto startiso;
			}
		}
	j = strtol(buf1, &p, 10);

	if (j > 0 && j < 300)
		addiso[iadd].m = j;
	else if(j <= 0)
		{
		printf("Negative or zero mass. Re-enter line.");
		continue;
		}
	else
		{
		printf("Mass > 300. Type Y to confirm, or N to re-enter line. ");
		fgets(buf2, 80, stdin);
		k = buf2[0];
		if (k != 'y' && k != 'Y')
			continue;
		addiso[iadd].m = j;
		}

	f = atof(p);
	if (f > 0 && f <= 100)
		{
		addiso[iadd].fr = f / 100;
		sumpc += f;
		}
	else
		{
		printf("Impossible percentage. Re-enter line\n");
		continue;
		}

	addel[eadd].niso++;
	if(first)
		{
		addel[eadd].p = addiso + iadd;
		first = 0;
		}
	iadd++;
   }
eadd++;*/
}



void CMolecularWeightCalculatorDlg::CalculateIsotopes()
{

}

void CMolecularWeightCalculatorDlg::FindIsotopes()
{
	int i, j, q, nold=1/*, nnew*/;
	double m, tempmass,newmin=0, newmax=0, totalInten;
	register int k;
	int ii, ix, fraction, ns=0 /* ListBoxIndex*/;
	/*char stars[71];*/
	float fr,/* maxintens*/ sumintens;
	float Limit = atof(m_EditIntensityLimit.GetBuffer(28));
	if(Limit > 0) Limit /= 1000;
	isotope s;
	element e;
	peak *old = NULL;
	/*peak *newPeak = NULL;*/
	peak *pp = NULL;
	
	/*CString tempString = "Isotope Pattern for : ";*/

	fraction = 0; 
	nnew = 0;
	buf[80] = 0;					/* terminate string */
	squob(buf);					/* clean up */
	natoms = 0;					/* init. */
	/*tempString += buf;*/
	/*if(m_RadioCharge == -1) tempString += " -";
	if(m_RadioCharge == 1) tempString += " +";
	m_HeadingListBox.ResetContent( );*/
	m_ListBox.DeleteAllItems();
	/*m_HeadingListBox.AddString(tempString);*/
	/*ListBoxIndex = m_ListBox.GetItemCount();*/
	
	if (!formula(buf))  {
		m_ListBox.InsertItem(0, "Invalid Formula");
		return;   /* if NOT a valid formula */
	}

	if(!newPeak) 
		newPeak = new peak[5000];
	old = new peak[5000];
	if ((newPeak == NULL)||(old == NULL)) {
		MessageBox("Out of Memory!","MEM Error",MB_OK);
		return;
	}
	for (k = 0;  k < 5000;  k++) {
		newPeak[k].intens = 1;		/* init. */
		newPeak[k].mass = 0;
		newPeak[k].multiplicity = 0;
		newPeak[k].spread = 0;
		old[k].intens = 1;		/* init. */
		old[k].mass = 0;
		old[k].multiplicity = 0;
		old[k].spread = 0;
	}
	
	int index = 0;
	for (i = 0;  i < natoms;  i++){				/* for all elements */
		if (atoms[i].atno < ADDBASE) e = elm[atoms[i].atno - 1];
		else e = addel[atoms[i].atno - ADDBASE];
		for(j = 0;  j < atoms[i].count;  j++){    /* for all atoms of an element */
			index = 0;
			for (k = 0;  k < e.niso;  k++) {          /* for all isotopes */
				m = (k + e.p)->m;		/* mass */
				fr = (k + e.p)->fr;		/* inty */	
				for (q = ns;  q < nold;  q++)  {
					tempmass = m+old[q].mass;
					ii = index;
					while (ii) {
						if((tempmass < newPeak[ii-1].mass+0.4)&&(tempmass > newPeak[ii-1].mass-0.4)) {  // same mass just increase intensity
							if(tempmass != newPeak[ii-1].mass) {
								if(tempmass > newPeak[ii-1].HighMass) {
									newPeak[ii-1].HighMass = tempmass;
									newmax = (fr*old[q].intens);
									newmin = newPeak[ii-1].intens;
								}
								if(tempmass < newPeak[ii-1].LowMass) {
									newPeak[ii-1].LowMass = tempmass;
									newmin = (fr*old[q].intens);
									newmax = newPeak[ii-1].intens;
								}
								totalInten = newmax + newmin;
								newPeak[ii-1].multiplicity++;
								newPeak[ii-1].spread = newPeak[ii-1].HighMass - newPeak[ii-1].LowMass;
								newPeak[ii-1].mass = newPeak[ii-1].LowMass + (newPeak[ii-1].spread * newmax/totalInten); 
								newPeak[ii-1].spread = 0;
								newPeak[ii-1].HighMass = newPeak[ii-1].mass ;
								newPeak[ii-1].LowMass = newPeak[ii-1].mass ;
							}
							newPeak[ii-1].intens += (fr*old[q].intens);
							break;
						}
						ii--;
					}
					if(!ii) {
						newPeak[index].mass = m+old[q].mass;		/* shift mass */
						newPeak[index].HighMass = newPeak[index].LowMass = newPeak[index].mass;
						newPeak[index++].intens = fr*old[q].intens; 	/* add inty */
					}
				}
				nnew = index;
			}
			if(index > 10) {
				ns = -1;
				for(ix=0; ix < index; ix++) {
					if( newPeak[ix].intens > Limit) {
						nnew = ix+1;
						if(ns<0) ns = ix;
					}
				}
				index = nnew; 
			}
			pp = newPeak;    // swap pointers
			newPeak = old;
			old = pp;
			nold = index;
		}
	}
	pp = newPeak;    // swap pointers
	newPeak = old;
	old = pp;
	nold = index;

	maxintens = sumintens = 0;
	for (ii=0;  ii< nnew;  ii++)	{	/* find max. */
		sumintens += newPeak[ii].intens;			
		if(newPeak[ii].mass == 0) {
			nnew = ii-1;
			break;
		}
		if (newPeak[ii].intens > maxintens)	maxintens = newPeak[ii].intens;
	}
	for (ii = 0;  ii < nnew;  ii++)	{	/* calculate fraction/percent */
		newPeak[ii].intens /= maxintens;
		if (fraction) newPeak[ii].intens /= sumintens;
		else newPeak[ii].intens *= 100;  /* they are already normalized to max=1 */
	}
	if (fraction) maxintens /= sumintens;
	else maxintens = 100;
	
	if(m_RadioCharge == -1) AddElectronMass();
	if(m_RadioCharge == 1) SubtractElectronMass();
	Fill_ListBox(nnew, maxintens);
	//for(ii = 0;  ii < nnew;  ii++)	{
	//	ns = .5 + 60.0 * newPeak[ii].intens / maxintens;	/* no. of stars */
	//	for (k = 0;  k < ns;  k++) stars[k] = '*';
	//	stars[ns] = 0;
	//	tempString.Format("%1.6f", newPeak[ii].mass);
	//	m_ListBox.InsertItem(ListBoxIndex+ii, tempString);
	//	tempString.Format("%1.6f", newPeak[ii].intens);
	//	m_ListBox.SetItemText(ListBoxIndex+ii,1,tempString);
	//	m_ListBox.SetItemText(ListBoxIndex+ii,2,stars);
	//
	//	tempString.Format(" %12.6f %15.6f   |%s" ,newPeak[ii].mass, newPeak[ii].intens, stars);
	//	IsoPattern.Add(tempString);
	//}
	delete [] old;
	/*delete [] newPeak;*/
}


void CMolecularWeightCalculatorDlg::OnPaint() 
{
	CString massstring, intensitystring;
	float xfactor, yfactor = 0;
	double f_inten, f_mass;
	float xpos, ypos, lastxpos = 0;
	int mass, LastPrintMass, skipmass = 0, massrange,i;
	CSize extent;
	CRect AxisBox;
	CPen *Pen=NULL, *OldPen=NULL;

	int NoPeaks = m_ListBox.GetItemCount();
	if(NoPeaks < 2) {
		massstring = m_ListBox.GetItemText(0,0);
		if(massstring == "Invalid Formula") {
			CDialog::OnPaint();
			return;
		}
	}
	massstring = m_ListBox.GetItemText(0,0);
	mass = atoi(massstring);
	massstring = m_ListBox.GetItemText(NoPeaks-1,0);
	LastPrintMass = atoi(massstring);
	massrange = LastPrintMass - mass+6;

	CPaintDC dc(this); // device context for painting
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_CENTER | TA_RIGHT );
	COLORREF crColor = RGB(0,0,255);
	Pen = new CPen(PS_SOLID ,2, crColor);
	extent = dc.GetTextExtent("100");
	AxisBox.left = m_RectSample.left+(extent.cx);
	AxisBox.bottom = m_RectSample.bottom - (extent.cy);
	AxisBox.right = m_RectSample.right - extent.cx;
	AxisBox.top = m_RectSample.top + (extent.cy*2);

											// draw axis lines
	/*dc.MoveTo(m_RectSample.left+20, m_RectSample.bottom-20);
	dc.LineTo(m_RectSample.left+20, m_RectSample.top+20);
	dc.MoveTo(m_RectSample.left+20, m_RectSample.bottom-20);
	dc.LineTo(m_RectSample.right-10, m_RectSample.bottom-20);*/
	ypos = (AxisBox.bottom - AxisBox.top)/10;
	xpos = ypos/2;
	
	DrawYAxis(&dc);
	//for( i=0;i<11;i++) {		// do yaxis
	//	dc.MoveTo(m_RectSample.left+20, m_RectSample.bottom-20-(i*ypos));
	//	dc.LineTo(m_RectSample.left+15,m_RectSample.bottom-20-(i*ypos));
	//	if(i) {  // do inermediate steps
	//		dc.MoveTo(m_RectSample.left+20, m_RectSample.bottom-20-(i*ypos)+xpos);
	//		dc.LineTo(m_RectSample.left+17,m_RectSample.bottom-20-(i*ypos)+xpos);
	//	}
	//	intensitystring = itoa(i*10,massstring.GetBuffer(5),10);
	//	dc.TextOut(m_RectSample.left+3, m_RectSample.bottom-27-(i*ypos),intensitystring);
	//}
	dc.TextOut(AxisBox.right, m_RectSample.bottom-5,"m/z");	
	xfactor = (AxisBox.right - AxisBox.left);
	xfactor /= massrange;
	yfactor = (AxisBox.bottom - AxisBox.top);
	yfactor /= 100;
	massstring = m_ListBox.GetItemText(0,0);
	mass = atoi(massstring)-1;

	for(i = 0; i < massrange;i++){		// do x axis
		if(massrange > 100) if((mass+i)%10) continue;
		if(massrange > 200) if((mass+i)%20) continue;
		xpos = AxisBox.left+(i*xfactor);
		dc.MoveTo(xpos, AxisBox.bottom);
		dc.LineTo(xpos, AxisBox.bottom+7);
		massstring = itoa(mass+i,intensitystring.GetBuffer(5),10);
		extent = dc.GetTextExtent(massstring);
		if(xpos > extent.cx + lastxpos) {
			lastxpos = xpos + 4;
			dc.TextOut(xpos, AxisBox.bottom+10,massstring);	
		}
	}

	OldPen = dc.SelectObject(Pen);
	LastPrintMass = mass;
	for( i=0; i < NoPeaks; i++ ) {
		massstring = m_ListBox.GetItemText(i,0);	// mass
		f_mass = atof(massstring);
		xpos = AxisBox.left + (xfactor*(f_mass-LastPrintMass));
		intensitystring = m_ListBox.GetItemText(i,1);	//intensity
		f_inten = atof(intensitystring);
		ypos = AxisBox.bottom - (f_inten*yfactor);
		dc.MoveTo(xpos, AxisBox.bottom);
		dc.LineTo(xpos,ypos);	
		if(f_inten == 100) {
			massstring.Format("%4.4f",f_mass);
			dc.TextOut(xpos,ypos-16,massstring);
			if(f_inten == 100) {
				massstring = buf;
				if(m_RadioCharge == -1) massstring += " -";
				if(m_RadioCharge == 1) massstring += " +";
				dc.TextOut(xpos,ypos-extent.cy-16,massstring);
			}
		}
	}
	if(Pen) {
		dc.SelectObject(OldPen);
		delete Pen;
	}
}

void CMolecularWeightCalculatorDlg::DrawYAxis(CDC * dc)
{
	CString massstring, intensitystring;
	CSize extent = dc->GetTextExtent("100");
	float ypos, xpos, increment;
	CRect AxisBox;

	AxisBox.left = m_RectSample.left+(extent.cx);
	AxisBox.bottom = m_RectSample.bottom - (extent.cy);
	AxisBox.right = m_RectSample.right - extent.cx;
	AxisBox.top = m_RectSample.top + (extent.cy*2);

	ypos = (AxisBox.bottom - AxisBox.top);
	ypos /= 10;
	xpos = ypos/2;

	dc->MoveTo(AxisBox.left, AxisBox.bottom);  // draw axis line
	dc->LineTo(AxisBox.left, AxisBox.top);
	dc->MoveTo(AxisBox.left, AxisBox.bottom);
	dc->LineTo(AxisBox.right, AxisBox.bottom);

	for( int i=0;i<11;i++) {		// do yaxis
		increment = i*ypos;
		dc->MoveTo(AxisBox.left, AxisBox.bottom-increment);
		dc->LineTo(AxisBox.left-5,AxisBox.bottom-increment);
		if(i) {  // do inermediate steps
			dc->MoveTo(AxisBox.left, AxisBox.bottom-increment+xpos);
			dc->LineTo(AxisBox.left-3,AxisBox.bottom-increment+xpos);
		}
		intensitystring = itoa(i*10,massstring.GetBuffer(5),10);
		dc->TextOut(m_RectSample.left+5, AxisBox.bottom-increment-(extent.cy/2),intensitystring);
	}
}

//  Basically for printing //////////////////////////
void CMolecularWeightCalculatorDlg::OnPaint(CDC * dc)
{
	CString massstring, intensitystring;
	float xfactor, yfactor = 0, MaxInten = 0;
	double f_inten, f_mass;
	float xpos, ypos, lastxpos=0;
	int mass, LastPrintMass, massrange, PrintPosY, offsett, i;
	CSize extent;
	CPen *Pen=NULL, *OldPen = NULL;
	CRect RectSample;

	extent = dc->GetTextExtent("dddd");
	offsett = extent.cx;
	RectSample.bottom = dc->GetDeviceCaps(VERTRES)-extent.cy;
	RectSample.right = dc->GetDeviceCaps(HORZRES)-extent.cx;
	RectSample.left = extent.cx;
	RectSample.top = extent.cy;/*RectSample.bottom/2;*/

	int NoPeaks = m_ListBox.GetItemCount();
	if(NoPeaks < 1) {
		return;
	}
	massstring = m_ListBox.GetItemText(0,0);
	mass = atoi(massstring);
	massstring = m_ListBox.GetItemText(NoPeaks-1,0);
	LastPrintMass = atoi(massstring);
	massrange = LastPrintMass - mass+6;

	massstring = "Isotope Model for : ";
	massstring += OriginalFormula;
	if(m_RadioCharge == -1) massstring += " -";
	if(m_RadioCharge == 1) massstring += " +";
	dc->TextOut(RectSample.left+extent.cx, 1,massstring);

	/*extent = dc->GetTextExtent("dddd");*/
	PrintPosY = 1+ extent.cy;
	//for( i=0; i<IsoPattern.GetSize(); i++) {		// print text box
	//	massstring = IsoPattern.GetAt(i);
	//	dc->TextOut(RectSample.left-extent.cx, PrintPosY, massstring);
	//	PrintPosY += extent.cy;
	//}

	RectSample.top = PrintPosY;
	for( i=0; i<m_HeadingListBox.GetCount();i++) {
			m_HeadingListBox.GetText(i,massstring);
			extent = dc->GetTextExtent(massstring);
			dc->TextOut(RectSample.right-extent.cx, RectSample.top+(extent.cy*i),massstring);
	}

	dc->SetBkMode(TRANSPARENT);
	dc->SetTextAlign(TA_CENTER | TA_RIGHT );
	COLORREF crColor = RGB(0,0,0);
	Pen = new CPen(PS_SOLID ,extent.cy/10, crColor);
	OldPen = dc->SelectObject(Pen);
	extent = dc->GetTextExtent("dd");

	dc->MoveTo(RectSample.left+extent.cx, RectSample.bottom-extent.cy);
	dc->LineTo(RectSample.left+extent.cx, RectSample.top+extent.cy);
	dc->MoveTo(RectSample.left+extent.cx, RectSample.bottom-extent.cy);
	dc->LineTo(RectSample.right-extent.cx, RectSample.bottom-extent.cy);
	ypos = (RectSample.bottom-(extent.cy*2) - RectSample.top)/10;

	for(i=0;i<11;i++) {		// do yaxis
		dc->MoveTo(RectSample.left+extent.cx, RectSample.bottom-extent.cy-(i*ypos));
		dc->LineTo(RectSample.left+5,RectSample.bottom-extent.cy-(i*ypos));
		intensitystring = itoa(i*10,massstring.GetBuffer(5),10);
		dc->TextOut(RectSample.left-extent.cx, RectSample.bottom-extent.cy-(extent.cy/2)-(i*ypos),intensitystring);
	}
	for(i=1;i<21;i++) {			// do yaxis intemediate steps
		dc->MoveTo(RectSample.left+extent.cx, RectSample.bottom-extent.cy-(i*ypos/2));
		dc->LineTo(RectSample.left+(extent.cx/2),RectSample.bottom-extent.cy-(i*ypos/2));

	}
	NoPeaks = IsoPattern.GetSize();
	for( i=0; i < NoPeaks; i++ ) {
		intensitystring = m_ListBox.GetItemText(i,1);
		yfactor = atof(intensitystring);
		if(yfactor > MaxInten) MaxInten = yfactor;
	}
	if(NoPeaks < 1) return;
	xfactor = (RectSample.right-(extent.cx*2) - RectSample.left);
	xfactor /= massrange;
	yfactor = (RectSample.bottom-(extent.cy*2) - RectSample.top);
	yfactor /= 100;
	massstring = m_ListBox.GetItemText(0,0);
	mass = atoi(massstring) - 1;
//	massstring = itoa(mass,intensitystring.GetBuffer(5),10);
	dc->TextOut(RectSample.right, RectSample.bottom,"m/z");	
	LastPrintMass = mass-1;
	for(i = 0; i < NoPeaks; i++){		// do x axis
		massstring = m_ListBox.GetItemText(i,0);	// mass
		f_mass = atof(massstring);
		mass = f_mass;
		
		if(massrange > 20) if((mass)%2) continue;
		if(massrange > 100) if((mass)%10) continue;
		if(massrange > 200) if((mass)%20) continue;
		
		xpos = offsett + m_RectSample.left + (xfactor*(mass-LastPrintMass));
		dc->MoveTo(xpos, RectSample.bottom-extent.cy);
		dc->LineTo(xpos, RectSample.bottom-(extent.cy/2));
		massstring = itoa(mass,intensitystring.GetBuffer(5),10);
		extent = dc->GetTextExtent(massstring);
		if(xpos > extent.cx + lastxpos) {
			lastxpos = xpos + 4;
			dc->TextOut(xpos, RectSample.bottom-(extent.cy/2),massstring);	
		}
	}
	if(Pen) {
		dc->SelectObject(OldPen);
		delete Pen;
	}
	crColor = RGB(0,0,255);
	Pen = new CPen(PS_SOLID ,extent.cy/8, crColor);
	OldPen = dc->SelectObject(Pen);
//	massstring = m_ListBox.GetItemText(0,0);
//	mass = atoi(massstring) - 1;
//	LastPrintMass = mass-1;
	for( i=0; i < NoPeaks; i++ ) {
		massstring = m_ListBox.GetItemText(i,0);	// mass
		f_mass = atof(massstring);
		xpos = offsett + m_RectSample.left + (xfactor*(f_mass-LastPrintMass));
		intensitystring = m_ListBox.GetItemText(i,1);	//intensity
		f_inten = atof(intensitystring);
		ypos = RectSample.bottom - extent.cy - (f_inten*yfactor);
		dc->MoveTo(xpos, RectSample.bottom - extent.cy);
		dc->LineTo(xpos,ypos);		
		if(f_inten == 100) {
			massstring.Format("%4.4f",f_mass);
			if(m_RadioCharge == -1) massstring += " -";
			if(m_RadioCharge == 1) massstring += " +";
			dc->TextOut(xpos,ypos-extent.cy,massstring);
		}
	}
	
	if(Pen) {
		dc->SelectObject(OldPen);
		delete Pen;
	}
}

void CMolecularWeightCalculatorDlg::HasDots()
{
	CString ExpandedFormula, MultString;
	int FirstDot;
	int i=1;
	char Num='1';

	FirstDot = WorkingFormula.Find('.');
	while(Num ) {
		Num = WorkingFormula.GetAt(FirstDot+i);
			if(isdigit(Num)) {
				i++;
			}
			else {
				Num = 0;
				if(MultString.GetLength() < 1) MultString += '1';
				break;
			}
		MultString += Num;
		if(FirstDot+i == WorkingFormula.GetLength()) break;
	}
	ExpandedFormula = WorkingFormula.Mid(FirstDot+i);
	AddToElementArrayM(ExpandedFormula, atoi(MultString));
	WorkingFormula.Delete(FirstDot,ExpandedFormula.GetLength()+MultString.GetLength()+1);

}


void CMolecularWeightCalculatorDlg::HasBrackets()
{
	CString ExpandedFormula, MultString;
	int FirstBracket, nextBracket, closeBracket;
	int i, nested=0;
	char Num;

	FirstBracket = WorkingFormula.Find('(');
	if(FirstBracket < 0) return;
	nextBracket = WorkingFormula.Find('(',FirstBracket+1);
	closeBracket = WorkingFormula.Find(')');
	if(nextBracket < 0) nextBracket = closeBracket;
	while(nextBracket < closeBracket) {
		nested = 1;
		FirstBracket = nextBracket;
		nextBracket = WorkingFormula.Find('(',FirstBracket+1);
		if(nextBracket < 0) nextBracket = closeBracket;
	}
	if(closeBracket < 0) {
		WorkingFormula.Delete(FirstBracket);
		return;
	}
	ExpandedFormula = WorkingFormula.Mid(FirstBracket+1,closeBracket-FirstBracket-1);
	for(i = 1; i + closeBracket < WorkingFormula.GetLength(); i++) {
		Num = WorkingFormula.GetAt(closeBracket+i);
		if(isdigit(Num)) MultString += Num;
		else break;		
	}
	AddToElementArrayM(ExpandedFormula, atoi(MultString));
	WorkingFormula.Delete(FirstBracket,(closeBracket+i)-FirstBracket);

	if(nested) {
		ExtractFormula();
		WorkingFormula.Insert(FirstBracket,m_EditFormula);
		ZeroAllElements();
	}
}


void CMolecularWeightCalculatorDlg::AddToElementArray(CString aFormula)
{
	char c;
	if( aFormula.GetLength() < 1 ) return;
	if(SingleElement) delete SingleElement;   //
	SingleElement = new FormulaElement();
	SingleElement->NoAtoms = 0;

	for(int i = 0; i < aFormula.GetLength(); i++) {
		c = aFormula.GetAt(i);
		if(isupper(c)) {
			if(!SingleElement->NoAtoms) {
				SingleElement->element += c;
				SingleElement->NoAtoms = 1;
			}
			else {
				SingleElement->symbol = SingleElement->element + SingleElement->NoString;
				SingleElement->NoAtoms = atoi(SingleElement->NoString);
				if(!SingleElement->NoAtoms) SingleElement->NoAtoms = 1;
				AddToAllElements();
				SingleElement = new FormulaElement();
				SingleElement->element += c;
				SingleElement->NoAtoms = 1;
			}
		}
		else if(islower(c)) SingleElement->element += c;
		else if(isdigit(c)) SingleElement->NoString += c;

	}
	SingleElement->symbol = SingleElement->element + SingleElement->NoString;
	SingleElement->NoAtoms = atoi(SingleElement->NoString);
	if(!SingleElement->NoAtoms) SingleElement->NoAtoms = 1;
	AddToAllElements();

}

void CMolecularWeightCalculatorDlg::AddToElementArrayM(CString aFormula, int MFactor)
{
	char c, temp[28];
//	int newElement = 0;
	if(!MFactor) MFactor = 1;

	if(SingleElement) delete SingleElement;
	SingleElement = new FormulaElement();
	SingleElement->NoAtoms = 0;


	for(int i = 0; i < aFormula.GetLength(); i++) {
		c = aFormula.GetAt(i);
		if(isupper(c)) {
			if(!SingleElement->NoAtoms) {
				SingleElement->element += c;
				SingleElement->NoAtoms = 1;
			}
			else {
				if(SingleElement->NoString.GetLength()) SingleElement->NoAtoms *= atoi(SingleElement->NoString);
				SingleElement->NoAtoms *= MFactor;
				if(SingleElement->NoAtoms >1 ) SingleElement->NoString = itoa(SingleElement->NoAtoms,temp,10);
				SingleElement->symbol = SingleElement->element + SingleElement->NoString;
				AddToAllElements();
				SingleElement = new FormulaElement();
				SingleElement->element += c;
				SingleElement->NoAtoms = 1;
			}
		}
		else if(islower(c)) SingleElement->element += c;
		else if(isdigit(c)) SingleElement->NoString += c;

	}
	if(SingleElement->NoString.GetLength()) SingleElement->NoAtoms *= atoi(SingleElement->NoString);
	SingleElement->NoAtoms *= MFactor;
	if(SingleElement->NoAtoms >1 ) SingleElement->NoString = itoa(SingleElement->NoAtoms,temp,10);
	SingleElement->symbol = SingleElement->element + SingleElement->NoString;
	AddToAllElements();

}

void CMolecularWeightCalculatorDlg::ExtractFormula()
{
	CString tempFormula = "";

	for(int i = 0; i < AllElements.GetSize(); i++) {
		SingleElement = (FormulaElement *)AllElements.GetAt(i);
		tempFormula += SingleElement->symbol;		
	}
	if(tempFormula.GetLength() > 0) m_EditFormula = tempFormula;
	SingleElement = NULL;
}

void CMolecularWeightCalculatorDlg::AddToAllElements()
{
	FormulaElement *tempSingleElement=NULL;
	char temp[28];

	for(int i = 0; i < AllElements.GetSize(); i++) {
		tempSingleElement = (FormulaElement *)AllElements.GetAt(i);
		if(SingleElement->element == tempSingleElement->element) {
			SingleElement->NoAtoms += tempSingleElement->NoAtoms;
			SingleElement->NoString = itoa(SingleElement->NoAtoms,temp,10);
			SingleElement->symbol = SingleElement->element + SingleElement->NoString;
			AllElements.InsertAt(i,(CObject *)SingleElement);
			AllElements.RemoveAt(i+1);
			delete tempSingleElement;  // clean up
			SingleElement = NULL;
			return;
		}
	}
	if(atno(SingleElement->element.GetBuffer(28))) {
		AllElements.Add((CObject *)SingleElement);
		SingleElement = NULL;
	}
	else {	// expand symbol
		ExpandSymbol();
	}
	if(SingleElement != NULL) delete SingleElement;
}

void CMolecularWeightCalculatorDlg::ZeroAllElements(void)
{
	while ( AllElements.GetSize() > 0) {
		SingleElement = (FormulaElement *)AllElements.GetAt(0);
		AllElements.RemoveAt(0);
		delete SingleElement;
		SingleElement = NULL;
	}
}


void CMolecularWeightCalculatorDlg::ExpandSymbol()
{
	CString aFormula;
	for(int i=0; i < nGroupElements;i++) {
		if(SingleElement->element == groupList[i].symbol) {
			aFormula = groupList[i].formula;
			AddToElementArrayM(aFormula,atoi(SingleElement->NoString));
			SingleElement = NULL;
			break;
		}
	}
	
}

// copy to clipboard
void CMolecularWeightCalculatorDlg::OnCopyButton() 
{
	CString source;
	int size = 0, i;
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		for(i=0; i<IsoPattern.GetSize(); i++) {
			source = IsoPattern.GetAt(i);
			size += source.GetLength() + 2;
		}
		size += OriginalFormula.GetLength() +26 ;
		for(i=0; i<m_HeadingListBox.GetCount();i++) {
			m_HeadingListBox.GetText(i,source);
			size += source.GetLength() + 2;
		}
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
		buffer = (char*)GlobalLock(clipbuffer);
		buffer[0] = NULL;
		
		for(i=0; i<m_HeadingListBox.GetCount();i++) {
			m_HeadingListBox.GetText(i,source);
			source.Replace(':','\t');
			strcat(buffer, LPCSTR(source));
			if(source.Find("Mass Percent")) strcat(buffer, "\n");
		}
		strcat(buffer, "\n");
		for(i=0; i<m_ListBox.GetItemCount(); i++) {
			source = m_ListBox.GetItemText(i,0);
			strcat(buffer, LPCSTR(source));
			strcat(buffer, "\t");
			source = m_ListBox.GetItemText(i,1);
			strcat(buffer, LPCSTR(source));
			strcat(buffer, "\t");
			source = m_ListBox.GetItemText(i,2);
			strcat(buffer, LPCSTR(source));
			strcat(buffer, "\n");
		}

//		for(i=0; i<IsoPattern.GetSize(); i++) {
//			source = IsoPattern.GetAt(i);
//			strcat(buffer, LPCSTR(source));
//			strcat(buffer, "\n");
//		}
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
	
}


void CMolecularWeightCalculatorDlg::OnBUTTONPrint() 
{
//	if(!m_cPrintListCtrl.Print(GetSafeHwnd(), "Isotope Pattern Listing"))
//	 MessageBox("Failed to Print List Control");
	HDC    hdcPrn ;

		 // Instantiate a CPrintDialog.
	CPrintDialog *printDlg = new CPrintDialog(FALSE, PD_ALLPAGES | PD_RETURNDC, NULL);

		 // Initialize some of the fields in PRINTDLG structure.
	int count = 1; //m_ListBoxCtrl.GetCount();
	printDlg->m_pd.nMinPage = printDlg->m_pd.nMaxPage = (count/50)+1;
	printDlg->m_pd.nFromPage = printDlg->m_pd.nToPage = (count/50)+1;
	
		 // Display Windows print dialog box.
	printDlg->DoModal();
		 // Obtain a handle to the device context.
	hdcPrn = printDlg->GetPrinterDC();
	if (hdcPrn != NULL)
	{
		CDC *pDC = new CDC;
		pDC->Attach (hdcPrn);      // attach a printer DC
		pDC->StartDoc("test");  // begin a new print job
									 // for Win32 use
									 // CDC::StartDoc(LPDOCINFO) override
		for (int i=0; i < printDlg->m_pd.nToPage; i++) {
			PrintPage(pDC, i, count);
		}
		pDC->EndDoc();             // end a print job
		pDC->Detach();             // detach the printer DC
		delete pDC;
	 }
	 delete printDlg; 
}


void CMolecularWeightCalculatorDlg::PrintPage(CDC *pDC, int /*PageNo*/, int /*count*/)
{
//	char   pbuf[256];
//	int j = 2;
//	int Pagebottom = pDC->GetDeviceCaps(VERTRES);

	pDC->StartPage();          // begin a new page
//	CSize extent = pDC->GetTextExtent("12345");
//	itoa(PageNo+1, pbuf,10);
//	pDC->TextOut(extent.cx*5, Pagebottom - (extent.cy), "Page Number");
//	pDC->TextOut(extent.cx*9, Pagebottom - (extent.cy), pbuf);
	
	OnPaint(pDC);
	pDC->EndPage();
}

void CMolecularWeightCalculatorDlg::OnLvnItemchangedList1(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CMolecularWeightCalculatorDlg::OnBnClickedPrintSetup()
{
	PrintList_Setup dlg(this);
	if(dlg.DoModal() == IDOK) {
		m_cPrintListCtrl.ResizeColumnsToFitContent(dlg.m_CheckRC);
		m_cPrintListCtrl.TruncateColumn(dlg.m_CheckTC);
		m_cPrintListCtrl.PrintColumnSeparator(dlg.m_CheckCS);
		m_cPrintListCtrl.PrintLineSeparator(dlg.m_CheckLS);
		m_cPrintListCtrl.PrintHead(dlg.m_CheckPH);
		m_cPrintListCtrl.PrintFoot(dlg.m_CheckPF);
		m_cPrintListCtrl.SetHeadFont(dlg.lf.lfFaceName, dlg.lfFontSize);
		m_cPrintListCtrl.SetFootFont(dlg.lfFooter.lfFaceName,dlg.lfFooterFontSize);
		m_cPrintListCtrl.SetListFont(dlg.lfList.lfFaceName,dlg.lfListFontSize);
		m_cPrintListCtrl.SetPrinterOrientation(dlg.m_CheckPL);
		m_cPrintListCtrl.PrintHeadBkgnd(dlg.m_CheckPHR, RGB(255,0,0));
		m_cPrintListCtrl.PrintFootBkgnd(dlg.m_CheckPFR, RGB(255,0,0));
	}
}

void CMolecularWeightCalculatorDlg::OnBnClickedHelp()
{
	CAboutMwtDlg dlgAbout;
	dlgAbout.DoModal();
}

void CMolecularWeightCalculatorDlg::OnBnClickedHelpbutton()
{
	CAboutMwtDlg dlgAbout;
	dlgAbout.DoModal();	
}

void CMolecularWeightCalculatorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if((cx == 0)&&(cy == 0)) return;
	if((m_RectSample.left ==0)&&( m_RectSample.right == 0)) return;
	m_RectSample.bottom = cy-10;
	m_RectSample.right = cx;
	m_RectSample.left = 10;
	CRect rect;
	CWnd* temp = GetDlgItem(IDC_LIST1);
	if(temp != NULL) {
		temp->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = cx;
		m_ListBox.MoveWindow(rect);
		Invalidate();
	}
}

void CMolecularWeightCalculatorDlg::OnBnClickedButtonprinttable()
{
	if(!m_cPrintListCtrl.Print(GetSafeHwnd(), "Molecular Weight Listing"))
	 MessageBox("Failed to Print List Control");
}

void CMolecularWeightCalculatorDlg::FillCalculatedValues(void)
{
	CString tempString = "Isotope Pattern for : ";
	tempString += buf;
	if(m_RadioCharge == -1) tempString += " -";
	if(m_RadioCharge == 1) tempString += " +";
	m_HeadingListBox.ResetContent( );
	m_HeadingListBox.AddString(tempString);

	double AverageMass=0, AverageMassEl=0, PercentM;
	FormulaElement *tempSingleElement=NULL;

	CString MassString;
	tempString = "Monoisotopic Mwt : ";
	for(int i = 0; i < AllElements.GetSize(); i++) {
		tempSingleElement = (FormulaElement *)AllElements.GetAt(i);
		for(int j=0; j < nel; j++) {
			if(elm[j].sym == tempSingleElement->element) {
				PercentM = elm[j].iosMass;
				PercentM *= tempSingleElement->NoAtoms;
				AverageMassEl += PercentM;
				/*MassString.Format(" %9.8f",AverageMassEl);*/
				break;
			}
		}	
	}

	if(m_RadioCharge == 1) AverageMassEl -= 0.000548579909067;
	if(m_RadioCharge == -1) AverageMassEl += 0.000548579909067;
	MassString.Format(" %9.8f",AverageMassEl);
	tempString += MassString;
	SetDlgItemText(IDC_STATICMolecularWeight,tempString);
	m_HeadingListBox.AddString(tempString);
	
	AverageMass=0;
	tempString = "Average Mwt        : ";
	for(int i = 0; i < AllElements.GetSize(); i++) {
		tempSingleElement = (FormulaElement *)AllElements.GetAt(i);
		for(int j=0; j < nel; j++) {
			if(elm[j].sym == tempSingleElement->element) {
				AverageMass += (tempSingleElement->NoAtoms * elm[j].AverageMass);
				break;
			}
		}	
	}
	MassString.Format(" %5.4f",AverageMass);
	tempString += MassString;
	SetDlgItemText(IDC_STATICAverageMwt,tempString);
	m_HeadingListBox.AddString(tempString);
	
	tempString = "Mass Percent      : ";
	m_HeadingListBox.AddString(tempString);
	tempString = "";
	for(int i = 0; i < AllElements.GetSize(); i++) {
		tempSingleElement = (FormulaElement *)AllElements.GetAt(i);
		for(int j=0; j < nel; j++) {
			if(elm[j].sym == tempSingleElement->element) {
				AverageMassEl = (tempSingleElement->NoAtoms * elm[j].AverageMass);
				PercentM = AverageMassEl/AverageMass;
				PercentM *= 100;
				tempString += tempSingleElement->element;
				MassString.Format(" %5.2f",PercentM);
				tempString += MassString;
				tempString += "%,  ";
				break;
			}
		}	
	}
	tempString.TrimRight(_T(", "));
	SetDlgItemText(IDC_STATICMassPercent,tempString);
	m_HeadingListBox.AddString(tempString);
}

void CMolecularWeightCalculatorDlg::OnBnClickedCopySpectrum()
{
	CMetaFileDC * m_pMetaDC = new CMetaFileDC();
	m_pMetaDC->CreateEnhanced(GetDC(),NULL,NULL,"whatever");
	CClientDC clientDC(this) ; 
	m_pMetaDC->m_hAttribDC = clientDC.m_hDC; 
	OnPaint(m_pMetaDC);
	HENHMETAFILE hMF = m_pMetaDC->CloseEnhanced();
	OpenClipboard();
	EmptyClipboard();
	::SetClipboardData(CF_ENHMETAFILE,hMF);
	CloseClipboard();
	delete m_pMetaDC;
}


void CMolecularWeightCalculatorDlg::OnBnClickedRadio2()  //positive
{
	if(m_RadioCharge == 1) return; // already posive
	else if(m_RadioCharge == -1){   // add mass of electron to all masses
		SubtractElectronMass();
		SubtractElectronMass();
	}
	else if(m_RadioCharge == 0){   // add mass of electron to all masses
		SubtractElectronMass();
	}
	m_RadioCharge = 1;
	m_ListBox.DeleteAllItems();
	Fill_ListBox(nnew, maxintens);
	FillCalculatedValues();
	Invalidate();
}


void CMolecularWeightCalculatorDlg::OnBnClickedRadio3negative()  //negative
{
	if(m_RadioCharge == -1) return; // already neg
	else if(m_RadioCharge == 1){   // add mass of electron to all masses
		AddElectronMass();
		AddElectronMass();
	}
	else if(m_RadioCharge == 0){   // add mass of electron to all masses
		AddElectronMass();
	}
	m_RadioCharge = -1;
	m_ListBox.DeleteAllItems();
	Fill_ListBox(nnew, maxintens);
	FillCalculatedValues();
	Invalidate();
}


void CMolecularWeightCalculatorDlg::OnBnClickedRadio1neutral() //nuetral
{
	if(m_RadioCharge == 0) return; // already netral
	else if(m_RadioCharge == 1) AddElectronMass();
	
	else if(m_RadioCharge == -1) SubtractElectronMass();
	
	m_RadioCharge = 0;
	m_ListBox.DeleteAllItems();
	Fill_ListBox(nnew, maxintens);
	FillCalculatedValues();
	Invalidate();
}




void CMolecularWeightCalculatorDlg::Fill_ListBox(int nnew, float maxintens)
{
	int ii, ns, k;
	CString tempString;
	char stars[71];
	int ListBoxIndex = m_ListBox.GetItemCount();
	IsoPattern.RemoveAll();

	for(ii = 0;  ii < nnew;  ii++)	{
		ns = .5 + 60.0 * newPeak[ii].intens / maxintens;	/* no. of stars */
		for (k = 0;  k < ns;  k++) stars[k] = '*';
		stars[ns] = 0;
		tempString.Format("%1.6f", newPeak[ii].mass);
		m_ListBox.InsertItem(ListBoxIndex+ii, tempString);
		tempString.Format("%1.6f", newPeak[ii].intens);
		m_ListBox.SetItemText(ListBoxIndex+ii,1,tempString);
		m_ListBox.SetItemText(ListBoxIndex+ii,2,stars);
	
		tempString.Format(" %12.6f %15.6f   |%s" ,newPeak[ii].mass, newPeak[ii].intens, stars);
		IsoPattern.Add(tempString);
	}
}


// Adds the mass of an electron to each isotope mass
void CMolecularWeightCalculatorDlg::AddElectronMass(void)
{
	for(int i = 0;  i < nnew;  i++)	{
			newPeak[i].mass += 0.000548579909067;
	}
}


// Subtracfts the mass of an electron from the isotope mass
void CMolecularWeightCalculatorDlg::SubtractElectronMass(void)
{
	for(int i = 0;  i < nnew;  i++)	{
			newPeak[i].mass -= 0.000548579909067;
	}
}


