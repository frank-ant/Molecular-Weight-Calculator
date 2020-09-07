// ==========================================================================
// PrintListCtrl.h
// 
// Author : Marquet Mike
//          mike.marquet@altavista.net
//
// Last Modified : 12/01/2000
// by            : MMA
// ==========================================================================

#if !defined(AFX_PRINTLISTCTRL_H__D5C2BEC1_C744_11D3_A0E2_004005555C30__INCLUDED_)
#define AFX_PRINTLISTCTRL_H__D5C2BEC1_C744_11D3_A0E2_004005555C30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// class CPrintListCtrl

class CPrintListCtrl  
 {
  protected :
              HWND        m_hListCtrl;
              HDC         m_hPrintDC;
              BOOL        m_bResizeColumnsToFitContent;
              BOOL        m_bPrintHead;
              BOOL        m_bPrintFoot;
              BOOL        m_bTruncateColumn;
              BOOL        m_bPrintColumnSeparator;
              BOOL        m_bPrintLineSeparator;
              BOOL        m_bPrintHeadBkgnd;
              BOOL        m_bPrintFootBkgnd;
              char        m_szFtHead[255];
              char        m_szFtFoot[255];
              char        m_szFtList[255];
              int         m_iScreenFtHead;
              int         m_iScreenFtFoot;
              int         m_iScreenFtList;
              COLORREF    m_clrHeadBkgnd;
              COLORREF    m_clrFootBkgnd;
              SYSTEMTIME  m_stST;
              HGLOBAL     m_hDevMode;
			  

              void ClearPrintDC();
              void CreateDevModeObject();

              void DeleteDevModeObject();
              
              int GetFootHeight(HFONT hFootFont);
              int GetLengthyStringFromColumn(int iCol);
              int GetTextSize(LPCTSTR lpszText, SIZE *pstSize = NULL);
              
              void PrintFoot(int iOffsetY, HFONT hFootFont, int iPageCounter);
              int  PrintHead(int iOffsetY, HFONT hHeadFont, LPCTSTR lpszTitle, int iPageCounter);
              int  PrintListCtrlHeader(int iOffsetY, HFONT hListFont, int iColCount, int *piColWidthTable, int *piColFormatTable, char **pszColTitleTable);
              int  PrintListCtrlLine(int iPageWidth, int iPageHeight, int iOffsetY, HFONT hListFont, HWND hListCtrl, int iLine, int *piColWidthTable, int *piColFormatTable, BOOL *pbNewPage);

              BOOL ResizeColumnToFitContent(int iCol);
              BOOL ResizeColumnsToFitContent();

              int ScreenHeightToPrinterHeight(int iScreenHeight);
              int ScreenWidthToPrinterWidth(int iScreenWidth);

  public :
           CPrintListCtrl();
           virtual ~CPrintListCtrl();

           BOOL Print(HWND hParent, LPCTSTR lpszDocName = "PrintListCtrl", BOOL bDefault = FALSE);
           BOOL PrintSetup(HWND hParent, BOOL bDefault = FALSE);

           BOOL SetDefaultPrinterSettings(HWND hParent = NULL);

           BOOL SetPrinterOrientation(BOOL bLandscape = FALSE);
           BOOL SetPrinterPaperSize(int iValue = DMPAPER_A4); // See DEVMODE structure for possible parameters

           // INLINE
           BOOL  GetPrintColumnSeparator()      { return m_bPrintColumnSeparator; }
           BOOL  GetPrintFoot()                 { return m_bPrintFoot; }
           BOOL  GetPrintHead()                 { return m_bPrintHead; }
           BOOL  GetPrintLineSeparator()        { return m_bPrintLineSeparator; }

           BOOL  GetResizeColumnsToFitContent() { return m_bResizeColumnsToFitContent; }

           HWND  GetListCtrl()                  { return m_hListCtrl; }

           BOOL  GetTruncateColumn()            { return m_bTruncateColumn; }

           char *GetHeadFontName()              { return m_szFtHead; }
           char *GetFootFontName()              { return m_szFtFoot; }
           char *GetListFontName()              { return m_szFtList; }

           int   GetHeadFontHeight()            { return m_iScreenFtHead; }
           int   GetFootFontHeight()            { return m_iScreenFtFoot; }
           int   GetListFontHeight()            { return m_iScreenFtList; }

           void PrintColumnSeparator(BOOL bValue)      { m_bPrintColumnSeparator = bValue; }
           void PrintFoot(BOOL bValue)                 { m_bPrintFoot = bValue; }
           void PrintHead(BOOL bValue)                 { m_bPrintHead = bValue; }
           void PrintLineSeparator(BOOL bValue)        { m_bPrintLineSeparator = bValue; }

           void ResizeColumnsToFitContent(BOOL bValue) { m_bResizeColumnsToFitContent = bValue; }

           void SetListCtrl(HWND hListCtrl)            { m_hListCtrl = hListCtrl; }

           void TruncateColumn(BOOL bValue)            { m_bTruncateColumn = bValue; }

           void PrintFootBkgnd(BOOL bValue, COLORREF clrValue = RGB(255,255,255))
            {
             m_bPrintFootBkgnd = bValue;
             m_clrFootBkgnd    = clrValue;
            }

           void PrintHeadBkgnd(BOOL bValue, COLORREF clrValue = RGB(255,255,255))
            {
             m_bPrintHeadBkgnd = bValue;
             m_clrHeadBkgnd    = clrValue;
            }

           void SetHeadFont(LPCTSTR lpszFontName = "Arial", int iFontHeight = 10)
            {
             strcpy(m_szFtHead, lpszFontName);
             m_iScreenFtHead = iFontHeight;
            }

           void SetFootFont(LPCTSTR lpszFontName = "Arial", int iFontHeight = 10)
            {
             strcpy(m_szFtFoot, lpszFontName);
             m_iScreenFtFoot = iFontHeight;
            }

           void SetListFont(LPCTSTR lpszFontName = "Arial", int iFontHeight = 10)
            {
             strcpy(m_szFtList, lpszFontName);
             m_iScreenFtList = iFontHeight;
            }

		   CListBox	  *m_cHeadListBox;	// added by Frank Antolasic
 };

#endif // !defined(AFX_PRINTLISTCTRL_H__D5C2BEC1_C744_11D3_A0E2_004005555C30__INCLUDED_)
#pragma once



