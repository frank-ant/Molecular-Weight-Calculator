#pragma once


// PrintList_Setup dialog

class PrintList_Setup : public CDialog
{
	DECLARE_DYNAMIC(PrintList_Setup)

public:
	PrintList_Setup(CWnd* pParent = NULL);   // standard constructor
	virtual ~PrintList_Setup();
	RECT m_RectSample;
	LOGFONT lf,lfFooter,lfList;
	int lfFontSize, lfFooterFontSize, lfListFontSize;

	CButton	m_Sample;
	BOOL	m_CheckCS;
	BOOL	m_CheckLS;
	BOOL	m_CheckPH;
	BOOL	m_CheckPL;
	BOOL	m_CheckRC;
	BOOL	m_CheckTC;
	BOOL	m_CheckPF;
	BOOL	m_CheckPFR;
	BOOL	m_CheckPHR;

// Dialog Data
	enum { IDD = IDD_PRINT_LISTCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonheader();
	afx_msg void OnBnClickedButtonlistf();
	afx_msg void OnBnClickedButtonfooter();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckph();
	afx_msg void OnBnClickedCheckpf();
	afx_msg void OnBnClickedCheckpl();
	afx_msg void OnBnClickedCheckls();
	afx_msg void OnBnClickedCheckcs();
};
