#if !defined(AFX_MYLISTCTRL_H__756BA2C0_38A3_11D4_AE30_0000C0FD4775__INCLUDED_)
#define AFX_MYLISTCTRL_H__756BA2C0_38A3_11D4_AE30_0000C0FD4775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyListCtrl window

class MyListCtrl : public CListCtrl
{
// Construction
public:
	MyListCtrl();
	bool SortNumericItems(int nCol, bool bAscending,int low/*=0*/, int high/*=-1*/  );
	bool SortTextItems(int nCol, bool bAscending, int low /*= 0*/, int high /*= -1*/);
	void OnHeaderClicked(NMHDR *pNMHDR, LRESULT* pResult);
	void SetColumnNumeric( int iCol );
	const bool IsColumnNumeric( int iCol ) const;

	int nSortedCol;
	bool bSortAscending;
	CUIntArray    m_aNumericColumns;
	CUIntArray    m_aDoubleColumns;
	CUIntArray    m_aFormulaColumns;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyListCtrl)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	const bool IsColumnDouble( int iCol ) const;
	bool SortFormulaItems( int nCol, bool bAscending,int low/*=0*/, int high/*=-1*/  );
	bool SortDoubleItems( int nCol, bool bAscending,int low/*=0*/, int high/*=-1*/  );
	void SetColumnFormula( int iCol );
	void SetColumnDouble( int iCol );
	virtual ~MyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(MyListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__756BA2C0_38A3_11D4_AE30_0000C0FD4775__INCLUDED_)
