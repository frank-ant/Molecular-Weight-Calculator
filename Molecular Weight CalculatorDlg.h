// Molecular Weight CalculatorDlg.h : header file
//

#pragma once
#include "PrintListCtrl.h"
#include "MyListCtrl.h"
#include "afxwin.h"


#define ADDBASE 100        /* above the natural elements */
#define MAXADD 10          /* space for user-defined 'elements' */
#define MAXAT  50


typedef struct isotope_s {
	double m; 
	float fr;
 }  isotope;    /* mass, abundance */
typedef struct Element_s{
	char *sym;
	int niso; 
	isotope *p;
	double iosMass;
	float AverageMass;
 } element;    /* symbol, no. of isotopes, ptr , averageMass*/
typedef struct Atom_s{
	int atno;
	int count;
 }    atom;
typedef struct Peak_s{
	double mass;
	float intens;
	float multiplicity;
	double LowMass;
	double HighMass;
	float spread;
 }  peak;
typedef struct Formula_Element_s{
	CString symbol;
	CString element;
	CString NoString;
	int NoAtoms;
 }  FormulaElement;
typedef struct Group_s{
	char *symbol;
	char *formula;
	char *name;
	} Group;



// CMolecularWeightCalculatorDlg dialog
class CMolecularWeightCalculatorDlg : public CDialog
{
// Construction
public:
	CMolecularWeightCalculatorDlg(CWnd* pParent = NULL);	// standard constructor
	~CMolecularWeightCalculatorDlg();
	void OnPaint(CDC *);
	void PrintPage(CDC *, int , int );
	void FindIsotopes();
	void ExpandSymbol();
	void HasDots();
	void HasBrackets();
	void AddToAllElements();
	void ExtractFormula();
	void AddToElementArrayM(CString aFormula, int MFactor);
	void AddToElementArray(CString aFormula);
	void CheckFormula();
	void HandleSaveAsText(CString filename);
	void CreateColumns();
	void CalculateIsotopes();

	int FormulaError;
	element	addel[MAXADD];      	/* e.g. isotope enriched ones, etc. */
	isotope	addiso[5 * MAXADD];
	atom	atoms[MAXAT];    	/* up to MAXAT different atoms in one formula */
	int 	natoms;                 /* number of atoms */
	int 	eadd, iadd;        	/* added elements & isotopes */
	int nel;
	int niso;
	int nGroupElements;
	char buf[81];
	CStringArray IsoPattern;
	CObArray AllElements;
	FormulaElement * SingleElement;
	CPrintListCtrl m_cPrintListCtrl;
	RECT m_RectSample;
	CString WorkingFormula, OriginalFormula;
	peak *newPeak;
	float maxintens;
	int nnew;

/* --- Function prototypes --- */

	void 	setpointers (void);
	void 	addelement(void);
	int 	squob(char *s);
	void 	foutput(int a, int n);
	isotope	imin(int atno);
	isotope	imax(int atno);
	int 	atno(char *s);
	int 	formula(char *in);
// Dialog Data
	enum { IDD = IDD_MOLECULARWEIGHTCALCULATOR_DIALOG };
	CString	m_EditFormula;
	CListBox	m_HeadingListBox;
	MyListCtrl	m_ListBox;
	CString	m_EditIntensityLimit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual void OnOK();
	afx_msg void OnBUTTONCalculate();
	afx_msg void OnBUTTONPrint();
	afx_msg void OnBUTTONSaveAs();
	afx_msg void OnCopyButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	void ZeroAllElements(void);
	afx_msg void OnBnClickedPrintSetup();
	afx_msg void OnBnClickedHelp();
	afx_msg void OnBnClickedHelpbutton();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonprinttable();
	void FillCalculatedValues(void);
	afx_msg void OnBnClickedCopySpectrum();
	afx_msg void OnBnClickedRadio2();
	CButton m_NeutralCtrl;
	int m_RadioCharge;
	afx_msg void OnBnClickedRadio3negative();
	afx_msg void OnBnClickedRadio1neutral();
	int m_radioChargeState;
	void Fill_ListBox(int NumberIso, float maxintens);
	// Adds the mass of an electron to each isotope mass
	void AddElectronMass(void);
	// Subtracfts the mass of an electron from the isotope mass
	void SubtractElectronMass(void);
	void DrawYAxis(CDC * dc);
};
