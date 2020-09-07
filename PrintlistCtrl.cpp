// ==========================================================================
// PrintListCtrl.cpp
// 
// Author : Marquet Mike
//          mike.marquet@altavista.net
//
// Last Modified : 12/01/2000
// by            : MMA
// ==========================================================================

// ==========================================================================
// Les Includes
// ==========================================================================

#include "stdafx.h"
#include <commctrl.h>
#include <commdlg.h>
#include <malloc.h>
#include "PrintListCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ==========================================================================
// Les Defines
// ==========================================================================

#define HEIGHT_SEPARATOR1   20
#define HEIGHT_SEPARATOR2   10
#define HEIGHT_SEPARATOR3    6
#define HEIGHT_SEPARATOR4    2
#define HEIGHT_SEPARATOR5   10

/////////////////////////////////////////////////////////////////////////////
// class CPrintListCtrl

CPrintListCtrl::CPrintListCtrl()
 {
  m_hListCtrl                  = NULL;
  m_hPrintDC                   = NULL;
  m_bResizeColumnsToFitContent = FALSE;
  m_bPrintHead                 = TRUE;
  m_bPrintFoot                 = TRUE;
  m_bTruncateColumn            = TRUE;
  m_bPrintColumnSeparator      = TRUE;
  m_bPrintLineSeparator        = FALSE;
  m_bPrintHeadBkgnd            = FALSE;
  m_bPrintFootBkgnd            = FALSE;
  m_iScreenFtHead              = 10;
  m_iScreenFtFoot              = 10;
  m_iScreenFtList              = 10;
  m_clrHeadBkgnd               = RGB(255,255,255);
  m_clrFootBkgnd               = RGB(255,255,255);
  m_hDevMode                   = NULL;
  m_cHeadListBox			   = NULL;

  strcpy(m_szFtHead, "Arial");
  strcpy(m_szFtFoot, "Arial");
  strcpy(m_szFtList, "Arial");

  CreateDevModeObject();
 }

// --------------------------------------------------------------------------

CPrintListCtrl::~CPrintListCtrl()
 {
  ClearPrintDC();
  DeleteDevModeObject();
  m_cHeadListBox = NULL;
 }

// --------------------------------------------------------------------------

void CPrintListCtrl::ClearPrintDC()
 {
  if (m_hPrintDC) DeleteDC(m_hPrintDC);

  m_hPrintDC = NULL;
 }

// --------------------------------------------------------------------------

void CPrintListCtrl::CreateDevModeObject()
 {
  DeleteDevModeObject();

  m_hDevMode = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DEVMODE));
  
  DEVMODE *pstDevMode = (DEVMODE *)GlobalLock(m_hDevMode);

  pstDevMode->dmSize = sizeof(DEVMODE);

  GlobalUnlock(m_hDevMode);
 }

// --------------------------------------------------------------------------

void CPrintListCtrl::DeleteDevModeObject()
 {
  if (m_hDevMode) GlobalFree(m_hDevMode);

  m_hDevMode = NULL;
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::GetFootHeight(HFONT hFootFont)
 {
  LOGFONT stLF;

  GetObject(hFootFont, sizeof(stLF), &stLF);

  return stLF.lfHeight + (2 * HEIGHT_SEPARATOR4);
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::GetLengthyStringFromColumn(int iCol)
 {
  if (!m_hListCtrl) return FALSE;

  HWND hHeaderCtrl = (HWND)SendMessage(m_hListCtrl, LVM_GETHEADER, 0L, 0L);

  if (!hHeaderCtrl) return FALSE;

  int iColCount  = (int)SendMessage(hHeaderCtrl, HDM_GETITEMCOUNT, 0L, 0L);
  int iLineCount = (int)SendMessage(m_hListCtrl, LVM_GETITEMCOUNT, 0L, 0L);
  int iMax       = 0;

  if (iCol < 0 || iCol >= iColCount) return -1;

  HDITEM stHDI;
  char   szBuf[255];

  stHDI.mask       = HDI_TEXT;
  stHDI.pszText    = szBuf;
  stHDI.cchTextMax = sizeof(szBuf);

  if (SendMessage(hHeaderCtrl, HDM_GETITEM, (WPARAM)(int)iCol, (LPARAM)(LPHDITEM)&stHDI))
   {
    if ((int)strlen(szBuf) > iMax) iMax = strlen(szBuf);
   }

  for (int L=0; L<iLineCount; L++)
   {
    LVITEM stLVI;

    stLVI.iSubItem   = iCol;
    stLVI.pszText    = szBuf;
    stLVI.cchTextMax = sizeof(szBuf);

    if (SendMessage(m_hListCtrl, LVM_GETITEMTEXT, (WPARAM)(int)L, (LPARAM)(LPLVITEM)&stLVI))
     {
      if ((int)strlen(szBuf) > iMax) iMax = strlen(szBuf);
     }
   }

  return iMax;
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::GetTextSize(LPCTSTR lpszText, SIZE *pstSize)
 {
  int  iWidth;
  SIZE stSize;

  GetTextExtentPoint32(m_hPrintDC, lpszText, strlen(lpszText), &stSize);

  iWidth = stSize.cx;
  
  if (!pstSize) return iWidth;

  memcpy(pstSize, &stSize, sizeof(SIZE));

  return iWidth;
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::Print(HWND hParent, LPCTSTR lpszDocName, BOOL bDefault)
 {
  if (!m_hPrintDC && !PrintSetup(hParent,bDefault)) return FALSE;

  if (!m_hPrintDC) return FALSE;

  if (!m_hListCtrl) return FALSE;

  HWND hHeaderCtrl = (HWND)SendMessage(m_hListCtrl, LVM_GETHEADER, 0L, 0L);

  if (!hHeaderCtrl) return FALSE;

  if (m_bResizeColumnsToFitContent) ResizeColumnsToFitContent();

  int iColCount  = (int)SendMessage(hHeaderCtrl, HDM_GETITEMCOUNT, 0L, 0L);
  int iLineCount = (int)SendMessage(m_hListCtrl, LVM_GETITEMCOUNT, 0L, 0L);

  int   *piColWidthTable  = new int[iColCount];
  int   *piColFormatTable = new int[iColCount];
  char **pszColTitleTable = new char *[iColCount];

  // Get system time
  GetSystemTime(&m_stST);

  // Create all fonts to use for printing
  int iPrinterFtHead = ScreenHeightToPrinterHeight(m_iScreenFtHead);
  int iPrinterFtList = ScreenHeightToPrinterHeight(m_iScreenFtList);
  int iPrinterFtFoot = ScreenHeightToPrinterHeight(m_iScreenFtFoot);

  HFONT hHeadFont = CreateFont(iPrinterFtHead, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, m_szFtHead);
  HFONT hListFont = CreateFont(iPrinterFtList, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, m_szFtList);
  HFONT hFootFont = CreateFont(iPrinterFtFoot, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, m_szFtFoot);

  if (!hHeadFont || !hListFont || !hFootFont)
   {
    if (hHeadFont) DeleteObject(hHeadFont);
    if (hListFont) DeleteObject(hListFont);
    if (hFootFont) DeleteObject(hFootFont);

    return FALSE;
   }

  // Get size of a character in list font
  HFONT hOldFont = (HFONT)SelectObject(m_hPrintDC, hListFont);
  char  szChar[] = "X";
  SIZE  stSize;

  GetTextExtentPoint32(m_hPrintDC, szChar, strlen(szChar), &stSize);

  SelectObject(m_hPrintDC, hOldFont);

  // Get all information about column size, alignment, ...
  for (int C=0; C<iColCount; C++)
   {
    HDITEM stHDI;
    char   szTitle[255];

    memset(szTitle, 0, sizeof(szTitle));

    stHDI.mask       = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
    stHDI.cchTextMax = sizeof(szTitle);
    stHDI.pszText    = szTitle;

    if (!SendMessage(hHeaderCtrl, HDM_GETITEM, (WPARAM)(int)C, (LPARAM)(LPHDITEM)&stHDI)) continue;

    int iMax = GetLengthyStringFromColumn(C) + 1;

    //piColWidthTable[C] = ScreenWidthToPrinterWidth(stHDI.cxy) + 20;

    piColWidthTable[C]  = stHDI.cxy ? stSize.cx * iMax : 0;
    
    if ((stHDI.fmt & HDF_CENTER) == HDF_CENTER) piColFormatTable[C] = DT_CENTER;
    else if ((stHDI.fmt & HDF_RIGHT) == HDF_RIGHT) piColFormatTable[C] = DT_RIGHT;
         else piColFormatTable[C] = DT_LEFT;

    if (stHDI.cchTextMax > 0 && stHDI.pszText)
     {
      pszColTitleTable[C] = new char[stHDI.cchTextMax];

      ASSERT(pszColTitleTable[C]);

      lstrcpy(pszColTitleTable[C], stHDI.pszText);
     }
   }

  // Get page dimension
  int iPageWidth  = GetDeviceCaps(m_hPrintDC, HORZRES);
  int iPageHeight = GetDeviceCaps(m_hPrintDC, VERTRES);

  // Start printing
  DOCINFO stDI;

  stDI.cbSize       = sizeof(stDI);
  stDI.fwType       = 0;
  stDI.lpszDatatype = 0;
  stDI.lpszOutput   = 0;
  stDI.lpszDocName  = lpszDocName;

  StartDoc(m_hPrintDC, &stDI);

  BOOL bNewPage     = TRUE;
  int  iPageCounter = 0;
  int  iOffsetY     = 0;

  for (int L=0; L<iLineCount; L++)
   {
    if (bNewPage)
     {
      StartPage(m_hPrintDC);
      bNewPage = FALSE;
      iOffsetY = 0;
      iPageCounter++;

	  iPageWidth  = GetDeviceCaps(m_hPrintDC, HORZRES);
      iPageHeight = GetDeviceCaps(m_hPrintDC, VERTRES);

      if (m_bPrintHead)
       {
        iOffsetY = PrintHead(iOffsetY, hHeadFont, lpszDocName, iPageCounter);
       }

      iOffsetY = PrintListCtrlHeader(iOffsetY, hListFont, iColCount, piColWidthTable, piColFormatTable, pszColTitleTable);

      if (m_bPrintFoot)
       {
        PrintFoot(iPageHeight - GetFootHeight(hFootFont), hFootFont, iPageCounter);

        iPageHeight -= (GetFootHeight(hFootFont) + HEIGHT_SEPARATOR5);
       }
     }

    iOffsetY = PrintListCtrlLine(iPageWidth, iPageHeight, iOffsetY, hListFont, m_hListCtrl, L, piColWidthTable, piColFormatTable, &bNewPage);

    if (iOffsetY == -1 && bNewPage)
     {
      EndPage(m_hPrintDC);
      L--;
     }
   }

  if (!bNewPage) EndPage(m_hPrintDC);

  EndDoc(m_hPrintDC);

  // Free all allocated memories and objects
  if (hHeadFont) DeleteObject(hHeadFont);
  if (hListFont) DeleteObject(hListFont);
  if (hFootFont) DeleteObject(hFootFont);

  if (pszColTitleTable)
   {
    for (int C=0; C<iColCount; C++)
     if (pszColTitleTable[C]) delete [] pszColTitleTable[C];

    delete [] pszColTitleTable;
   }

  if (piColFormatTable) delete [] piColFormatTable;

  if (piColWidthTable) delete [] piColWidthTable;

  return TRUE;
 }

// --------------------------------------------------------------------------

void CPrintListCtrl::PrintFoot(int iOffsetY, HFONT hFootFont, int iPageCounter)
 {
  LOGFONT stLF;

  GetObject(hFootFont, sizeof(stLF), &stLF);

  int   iPageWidth = GetDeviceCaps(m_hPrintDC, HORZRES);
  char  szBuf[255];
  RECT  stRect;
  RECT  stFrameRect;
  HFONT hOldFont = (HFONT)SelectObject(m_hPrintDC, hFootFont);
  int   iOldMode = SetBkMode(m_hPrintDC, TRANSPARENT);

  stFrameRect.left   = 0;
  stFrameRect.top    = iOffsetY;
  stFrameRect.right  = iPageWidth - 1;
  stFrameRect.bottom = iOffsetY + stLF.lfHeight + (2 * HEIGHT_SEPARATOR4);

  if (m_bPrintFootBkgnd)
   {
    HBRUSH hBrush    = CreateSolidBrush(m_clrFootBkgnd);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(m_hPrintDC, hBrush);
  
    FillRect(m_hPrintDC, &stFrameRect, hBrush);

    SelectObject(m_hPrintDC, hOldBrush);
   }

  // Print page counter
  stRect.left   = 2;
  stRect.top    = iOffsetY;
  stRect.right  = iPageWidth / 2;
  stRect.bottom = iOffsetY + stLF.lfHeight + (2 * HEIGHT_SEPARATOR4);

  sprintf(szBuf, "Page : %d", iPageCounter);

  DrawText(m_hPrintDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

  // Print date and time
  stRect.left   = iPageWidth / 2;
  stRect.top    = iOffsetY;
  stRect.right  = iPageWidth - 3;
  stRect.bottom = iOffsetY + stLF.lfHeight + (2 * HEIGHT_SEPARATOR4);

  sprintf(szBuf, "%02d/%02d/%04d  %02d:%02d:%02d", m_stST.wDay,
                                                   m_stST.wMonth,
                                                   m_stST.wYear,
                                                   m_stST.wHour,
                                                   m_stST.wMinute,
                                                   m_stST.wSecond);

  DrawText(m_hPrintDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

  // Print frame rectangle
  FrameRect(m_hPrintDC, &stFrameRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

  // Restore objects
  SetBkMode(m_hPrintDC, iOldMode);

  SelectObject(m_hPrintDC, hOldFont);
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::PrintHead(int iOffsetY, HFONT hHeadFont, LPCTSTR lpszTitle, int iPageCounter)
 {
  LOGFONT stLF, stTitleLF;
  int     iPageWidth, iPageHeight;
  HFONT   hTitleFont;

  GetObject(hHeadFont, sizeof(stLF), &stLF);

  memcpy(&stTitleLF, &stLF, sizeof(LOGFONT));

  stTitleLF.lfHeight = stTitleLF.lfHeight * 2;

  hTitleFont = CreateFontIndirect(&stTitleLF);

  iPageWidth  = GetDeviceCaps(m_hPrintDC, HORZRES);
  iPageHeight = GetDeviceCaps(m_hPrintDC, VERTRES);

  RECT  stRect;
  char  szBuf[255];
  HFONT hOldFont;
  int   iTopFrame = iOffsetY;

  int iOldMode = SetBkMode(m_hPrintDC, TRANSPARENT);

  // Print head background if wanted
  RECT stFrameRect;

  stFrameRect.left   = 0;
  stFrameRect.top    = iTopFrame;
  stFrameRect.right  = iPageWidth - 1;
  stFrameRect.bottom = iOffsetY + (2 * stLF.lfHeight) + stTitleLF.lfHeight;

  if (m_bPrintHeadBkgnd)
   {
    HBRUSH hBrush    = CreateSolidBrush(m_clrHeadBkgnd);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(m_hPrintDC, hBrush);
  
    FillRect(m_hPrintDC, &stFrameRect, hBrush);

    SelectObject(m_hPrintDC, hOldBrush);
   }

  // Print page counter
  stRect.left   = 0;
  stRect.top    = iOffsetY;
  stRect.right  = iPageWidth;
  stRect.bottom = iOffsetY + stLF.lfHeight;

  sprintf(szBuf, "Page : %d", iPageCounter);

  hOldFont = (HFONT)SelectObject(m_hPrintDC, hHeadFont);
  
  if (!m_bPrintFoot) DrawText(m_hPrintDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
  
  iOffsetY += stLF.lfHeight;

  // Print title
  stRect.left   = 0;
  stRect.top    = iOffsetY;
  stRect.right  = iPageWidth;
  stRect.bottom = iOffsetY + stTitleLF.lfHeight;

  SelectObject(m_hPrintDC, hTitleFont);
  DrawText(m_hPrintDC, lpszTitle, strlen(lpszTitle), &stRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  iOffsetY += stTitleLF.lfHeight;

  // Print header list box  Frank Antolasic added
  if(m_cHeadListBox) {
	iOffsetY += stTitleLF.lfHeight;
	CString temp;
	stRect.left   = 0;
    stRect.top    = iOffsetY;
    stRect.right  = iPageWidth;
    stRect.bottom = iOffsetY + stTitleLF.lfHeight;

	int count = m_cHeadListBox->GetCount();
	for(int i = 0; i < count; i++) {
		m_cHeadListBox->GetText(i, temp);
		DrawText(m_hPrintDC, temp, strlen(temp), &stRect, DT_SINGLELINE | DT_VCENTER );
		iOffsetY += stTitleLF.lfHeight;
		stRect.top    = iOffsetY;
		stRect.bottom = iOffsetY + stTitleLF.lfHeight;
	}
	    
  }


  // Print page counter
  stRect.left   = 0;
  stRect.top    = iOffsetY;
  stRect.right  = iPageWidth;
  stRect.bottom = iOffsetY + stLF.lfHeight;

  sprintf(szBuf, "%02d/%02d/%04d  %02d:%02d:%02d", m_stST.wDay,
                                                   m_stST.wMonth,
                                                   m_stST.wYear,
                                                   m_stST.wHour,
                                                   m_stST.wMinute,
                                                   m_stST.wSecond);

  SelectObject(m_hPrintDC, hHeadFont);
  
  if (!m_bPrintFoot) DrawText(m_hPrintDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
  
  iOffsetY += stLF.lfHeight;

  // Print frame
  FrameRect(m_hPrintDC, &stFrameRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

  // Restore default values
  SetBkMode(m_hPrintDC, iOldMode);

  SelectObject(m_hPrintDC, hOldFont);

  DeleteObject(hTitleFont);

  return (iOffsetY + HEIGHT_SEPARATOR1);
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::PrintListCtrlHeader(int iOffsetY, HFONT hListFont, int iColCount, int *piColWidthTable, int *piColFormatTable, char **pszColTitleTable)
 {
  LOGFONT stLF;
  RECT    stRect;
  int     iOffsetX    = 0;
  HFONT   hOldFont    = (HFONT)SelectObject(m_hPrintDC, hListFont);
  int     iPageWidth  = GetDeviceCaps(m_hPrintDC, HORZRES);
  int     iPageHeight = GetDeviceCaps(m_hPrintDC, VERTRES);

  GetObject(hListFont, sizeof(stLF), &stLF);

  // Print columns titles
  for (int C=0; C<iColCount; C++)
   {
    if (m_bTruncateColumn && (iOffsetX + piColWidthTable[C]) > iPageWidth) break;

    if (iOffsetX > iPageWidth) break;

    if (!piColWidthTable[C]) continue;

    stRect.left   = iOffsetX;
    stRect.top    = iOffsetY;
    stRect.right  = iOffsetX + piColWidthTable[C] - 1;
    stRect.bottom = iOffsetY + stLF.lfHeight;

    DrawText(m_hPrintDC, pszColTitleTable[C], strlen(pszColTitleTable[C]), &stRect, DT_SINGLELINE | DT_VCENTER | piColFormatTable[C]);

    iOffsetX += piColWidthTable[C];

    // Print column separator if wanted
    if (m_bPrintColumnSeparator && C > 0)
     {
      MoveToEx(m_hPrintDC, stRect.left-2, stRect.top, NULL);
      LineTo(m_hPrintDC, stRect.left-2, stRect.bottom + HEIGHT_SEPARATOR2);
     }
   }

  iOffsetY += stLF.lfHeight;

  // Print separator line
  HPEN hPen    = CreatePen(PS_SOLID, 3, RGB(0,0,0));
  HPEN hOldPen = (HPEN)SelectObject(m_hPrintDC, hPen);

  MoveToEx(m_hPrintDC, 0, iOffsetY, NULL);
  LineTo(m_hPrintDC, iPageWidth, iOffsetY);

  // Restore default values
  SelectObject(m_hPrintDC, hOldPen);

  SelectObject(m_hPrintDC, hOldFont);

  return (iOffsetY + HEIGHT_SEPARATOR2);
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::PrintListCtrlLine(int iPageWidth, int iPageHeight, int iOffsetY, HFONT hListFont, HWND hListCtrl, int iLine, int *piColWidthTable, int *piColFormatTable, BOOL *pbNewPage)
 {
  *pbNewPage = FALSE;

  if (!hListCtrl) return -1;

  HWND hHeaderCtrl = (HWND)SendMessage(hListCtrl, LVM_GETHEADER, 0L, 0L);

  if (!hHeaderCtrl) return -1;

  int iColCount   = (int)SendMessage(hHeaderCtrl, HDM_GETITEMCOUNT, 0L, 0L);
  int iLineCount  = (int)SendMessage(hListCtrl, LVM_GETITEMCOUNT, 0L, 0L);

  LOGFONT stLF;

  GetObject(hListFont, sizeof(stLF), &stLF);

  if ((iOffsetY + stLF.lfHeight) > iPageHeight)
   {
    *pbNewPage = TRUE;

    return -1;
   }

  int   iOffsetX = 0;
  HFONT hOldFont = (HFONT)SelectObject(m_hPrintDC, hListFont);

  for (int C=0; C<iColCount; C++)
   {
    if (m_bTruncateColumn && (iOffsetX + piColWidthTable[C]) > iPageWidth) break;
    
    if (iOffsetX > iPageWidth) break;

    if (!piColWidthTable[C]) continue;

    LVITEM stLVI;
    char   szBuf[255];
    RECT   stRect;

    memset(szBuf, 0, sizeof(szBuf));

    stLVI.iSubItem   = C;
    stLVI.pszText    = szBuf;
    stLVI.cchTextMax = sizeof(szBuf);

    SendMessage(hListCtrl, LVM_GETITEMTEXT, (WPARAM)(int)iLine, (LPARAM)(LPLVITEM)&stLVI);

    stRect.left   = iOffsetX;
    stRect.top    = iOffsetY;
    stRect.right  = iOffsetX + piColWidthTable[C];
    stRect.bottom = iOffsetY + stLF.lfHeight;

    //int iWidth = GetTextSize(szBuf);
    
    //if (iWidth > (stRect.right - stRect.left)) strcpy(szBuf,"...");

    DrawText(m_hPrintDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_VCENTER | piColFormatTable[C]);

    iOffsetX += piColWidthTable[C];

    // Print column separator if wanted
    if (m_bPrintColumnSeparator && C > 0)
     {
      MoveToEx(m_hPrintDC, stRect.left-2, stRect.top, NULL);
      LineTo(m_hPrintDC, stRect.left-2, stRect.bottom + HEIGHT_SEPARATOR3);
     }
   }

  iOffsetY += stLF.lfHeight;

  // Print line separator if wanted

  if (m_bPrintLineSeparator)
   {
    MoveToEx(m_hPrintDC, 0, iOffsetY + (int)(HEIGHT_SEPARATOR3 / 2), NULL);
    LineTo(m_hPrintDC, iPageWidth, iOffsetY + (int)(HEIGHT_SEPARATOR3 / 2));
   }

  SelectObject(m_hPrintDC, hOldFont);

  return (iOffsetY + HEIGHT_SEPARATOR3);
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::PrintSetup(HWND hParent, BOOL bDefault)
 {
  PRINTDLG stPD;

  memset(&stPD, 0, sizeof(stPD));

  stPD.lStructSize = sizeof(stPD);
  stPD.hwndOwner   = hParent;
  stPD.Flags       = PD_ALLPAGES | PD_DISABLEPRINTTOFILE |
                     PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
  stPD.hDevNames   =  NULL;
  stPD.hDevMode    = bDefault ? NULL : m_hDevMode;

  if (bDefault)
   {
    stPD.Flags |= PD_RETURNDEFAULT;
   }

  BOOL bRet = PrintDlg(&stPD);

  if (bRet == FALSE && CommDlgExtendedError()) return FALSE;

  if (bRet == FALSE && !CommDlgExtendedError()) return TRUE;

  ClearPrintDC();

  m_hPrintDC = stPD.hDC;

  return (m_hPrintDC != NULL);
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::ResizeColumnToFitContent(int iCol)
 {
  if (!m_hListCtrl) return FALSE;

  HWND hHeaderCtrl = (HWND)SendMessage(m_hListCtrl, LVM_GETHEADER, 0L, 0L);

  if (!hHeaderCtrl) return FALSE;
  
  int iColCount = (int)SendMessage(hHeaderCtrl, HDM_GETITEMCOUNT, 0L, 0L);

  if (iCol < 0 || iCol >= iColCount) return FALSE;

  return SendMessage(m_hListCtrl, LVM_SETCOLUMNWIDTH, (WPARAM)(int)iCol, MAKELPARAM((int)LVSCW_AUTOSIZE_USEHEADER,0));
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::ResizeColumnsToFitContent()
 {
  if (!m_hListCtrl) return FALSE;

  HWND hHeaderCtrl = (HWND)SendMessage(m_hListCtrl, LVM_GETHEADER, 0L, 0L);

  if (!hHeaderCtrl) return FALSE;
  
  int iColCount = (int)SendMessage(hHeaderCtrl, HDM_GETITEMCOUNT, 0L, 0L);

  for (int C=0; C<iColCount; C++)
   {
    ResizeColumnToFitContent(C);
   }

  return TRUE;
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::ScreenHeightToPrinterHeight(int iScreenHeight)
 {
  if (!m_hPrintDC) return -1;

  HDC hScreenDC = GetDC(NULL);

  if (!hScreenDC) return -1;

  int iPrinterHeight = (iScreenHeight * GetDeviceCaps(m_hPrintDC,LOGPIXELSY)) / GetDeviceCaps(hScreenDC,LOGPIXELSY);

  ReleaseDC(NULL, hScreenDC);

  return iPrinterHeight;
 }

// --------------------------------------------------------------------------

int CPrintListCtrl::ScreenWidthToPrinterWidth(int iScreenWidth)
 {
  if (!m_hPrintDC) return -1;

  HDC hScreenDC = GetDC(NULL);

  if (!hScreenDC) return -1;

  int iPrinterWidth = (iScreenWidth * GetDeviceCaps(m_hPrintDC,LOGPIXELSX)) / GetDeviceCaps(hScreenDC,LOGPIXELSX);

  ReleaseDC(NULL, hScreenDC);

  return iPrinterWidth;
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::SetDefaultPrinterSettings(HWND hParent)
 {
  ClearPrintDC();

  return PrintSetup(hParent,TRUE);
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::SetPrinterOrientation(BOOL bLandscape)
 {
  if (!m_hDevMode) return FALSE;

  DEVMODE *pstDevMode = (DEVMODE *)GlobalLock(m_hDevMode);

  pstDevMode->dmFields |= DM_ORIENTATION;

  pstDevMode->dmOrientation = bLandscape ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;

  GlobalUnlock(m_hDevMode);

  return TRUE;
 }

// --------------------------------------------------------------------------

BOOL CPrintListCtrl::SetPrinterPaperSize(int iValue)
 {
  if (!m_hDevMode) return FALSE;

  DEVMODE *pstDevMode = (DEVMODE *)GlobalLock(m_hDevMode);

  pstDevMode->dmFields |= DM_PAPERSIZE;

  pstDevMode->dmPaperSize = iValue;

  GlobalUnlock(m_hDevMode);

  return TRUE;
 }

