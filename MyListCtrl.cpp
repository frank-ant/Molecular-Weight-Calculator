// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MyListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyListCtrl

MyListCtrl::MyListCtrl()
{
	nSortedCol = 0;			// changed from -1
	bSortAscending = TRUE;
}

MyListCtrl::~MyListCtrl()
{
}

bool MyListCtrl::SortNumericItems( int nCol, bool bAscending,int low/*=0*/, int high/*=-1*/  )
{
	CWaitCursor Cursor;

    if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
         return FALSE;
                             
	if( high == -1 ) high = GetItemCount() - 1;
	int lo = low;   
	int hi = high;
    int midItem;
                             
	if( hi <= lo ) return FALSE;
                             
	midItem = atoi(GetItemText( (lo+hi)/2, nCol ));
                             
	// loop through the list until indices cross
	while( lo <= hi )
	{
	// rowText will hold all column text for one row
		CStringArray rowText;
                                     
	 // find the first element that is greater than or equal to 
	// the partition element starting from the left Index.
		if( bAscending )
		while( ( lo < high ) && (atoi(GetItemText(lo, nCol)) < midItem ) )
			++lo;           
		else
		while( ( lo < high ) && (atoi(GetItemText(lo, nCol)) > midItem ) )
			++lo;
                                     
	// find an element that is smaller than or equal to 
	// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && (atoi(GetItemText(hi, nCol)) > midItem ) )
				 --hi;           
		else
			 while( ( hi > low ) && (atoi(GetItemText(hi, nCol)) < midItem ) )
				 --hi;
                                                     
	// if the indexes have not crossed, swap                
	 // and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if(atoi(GetItemText(lo, nCol)) != atoi(GetItemText(hi, nCol)) )
			{                               
	// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
                                     
				int i;
				for( i=0; i < nColCount; i++) 
					rowText[i] = GetItemText(lo, i);
                                     
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
								     LVIS_FOCUSED |  LVIS_SELECTED |
								       LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
			    lvitemhi = lvitemlo;
			    lvitemhi.iItem = hi;
                                     
			    GetItem( &lvitemlo );
			    GetItem( &lvitemhi );
        
			   for( i=0; i< nColCount; i++)
				   SetItemText(lo, i, GetItemText(hi, i) );
                                     
			   lvitemhi.iItem = lo;
			   SetItem( &lvitemhi );
                                     
		       for( i=0; i< nColCount; i++)
			   SetItemText(hi, i, rowText[i]);
                                     
		      lvitemlo.iItem = hi;
		      SetItem( &lvitemlo );
			}
                                             
			++lo;
			--hi;
		}
	}                           
 // If the right index has not reached the left side of array
 // must now sort the left partition.
    if( low < hi )
    SortNumericItems( nCol, bAscending , low, hi);
                             
 // If the left index has not reached the right side of array
 // must now sort the right partition.
    if( lo < high )
    SortNumericItems( nCol, bAscending , lo, high );
                             
    return TRUE;
}


// SortTextItems        - Sort the list based on column text
// Returns              - Returns true for success
// nCol                 - column that contains the text to be sorted
// bAscending           - indicate sort order
// low                  - row to start scanning from - default row is 0
// high                 - row to end scan. -1 indicates last row
bool MyListCtrl::SortTextItems( int nCol, bool bAscending, int low /*= 0*/, int high /*= -1*/ )
{
	CWaitCursor Cursor;
   if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
                return FALSE;

   if( high == -1 ) high = GetItemCount() - 1;

   int lo = low;
   int hi = high;
   CString midItem;

   if( hi <= lo ) return FALSE;

   midItem = GetItemText( (lo+hi)/2, nCol );

        // loop through the list until indices cross
   while( lo <= hi )
   {
        // rowText will hold all column text for one row
        CStringArray rowText;
        // find the first element that is greater than or equal to 
        // the partition element starting from the left Index.
        if( bAscending )
   while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
           ++lo;
                else
   while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
           ++lo;

        // find an element that is smaller than or equal to 
       // the partition element starting from the right Index.
        if( bAscending )
   while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
           --hi;
                else
   while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
           --hi;

       // if the indexes have not crossed, swap
       // and if the items are not equal
        if( lo <= hi )
        {
   // swap only if the items are not equal
	    if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
		{
           // swap the rows
           LV_ITEM lvitemlo, lvitemhi;
           int nColCount = 
                   ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
           rowText.SetSize( nColCount );
           int i;
           for( i=0; i<nColCount; i++)
                   rowText[i] = GetItemText(lo, i);
           lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
           lvitemlo.iItem = lo;
           lvitemlo.iSubItem = 0;
           lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
								LVIS_FOCUSED |  LVIS_SELECTED | 
								LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

           lvitemhi = lvitemlo;
           lvitemhi.iItem = hi;

           GetItem( &lvitemlo );
           GetItem( &lvitemhi );

           for( i=0; i<nColCount; i++)
                   SetItemText(lo, i, GetItemText(hi, i));

           lvitemhi.iItem = lo;
           SetItem( &lvitemhi );

           for( i=0; i<nColCount; i++)
                   SetItemText(hi, i, rowText[i]);

           lvitemlo.iItem = hi;
           SetItem( &lvitemlo );
		}

			++lo;
		   --hi;
	   }
   }

        // If the right index has not reached the left side of array
        // must now sort the left partition.
    if( low < hi )
          SortTextItems( nCol, bAscending , low, hi);

        // If the left index has not reached the right side of array
        // must now sort the right partition.
    if( lo < high )
           SortTextItems( nCol, bAscending , lo, high );

    return TRUE;
}




BEGIN_MESSAGE_MAP(MyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(MyListCtrl)
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCLICKA, 0,OnHeaderClicked)
	ON_NOTIFY(HDN_ITEMCLICKW, 0,OnHeaderClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MyListCtrl message handlers


void MyListCtrl::OnHeaderClicked(NMHDR *pNMHDR, LRESULT* pResult)
{
//	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

//    if( phdn->iButton == 0 )
//    {
      // User clicked on header using left mouse button
//	    if( phdn->iItem == nSortedCol )
  //           bSortAscending = !bSortAscending;
//        else
//             bSortAscending = TRUE;
//             nSortedCol = phdn->iItem;
//             if(IsColumnNumeric(nSortedCol )) SortNumericItems( nSortedCol, bSortAscending,0,-1);
//			 else if(IsColumnDouble(nSortedCol )) SortDoubleItems( nSortedCol, bSortAscending,0,-1);
//			 else SortTextItems( nSortedCol, bSortAscending,0,-1);
//    }

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR); 
	HDITEM HeaderItem; 
	HeaderItem.mask = HDI_FORMAT | HDI_BITMAP; 
	CHeaderCtrl* HeaderCtrl = GetHeaderCtrl(); 
	HeaderCtrl->GetItem(pNMLV->iItem, &HeaderItem); 
	if (HeaderItem.hbm != 0) { 
		DeleteObject(HeaderItem.hbm); 
		HeaderItem.hbm = 0; 
	} 
	HeaderItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 
	bSortAscending = (nSortedCol != pNMLV->iItem) ? true : !bSortAscending; 
	HeaderItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(bSortAscending ? IDB_UP : IDB_DOWN), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 
	HeaderCtrl->SetItem(pNMLV->iItem, &HeaderItem); 
	if (nSortedCol != pNMLV->iItem) { 
		HeaderCtrl->GetItem(nSortedCol, &HeaderItem); 
		HeaderItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
		if (HeaderItem.hbm != 0) { 
			DeleteObject(HeaderItem.hbm); 
			HeaderItem.hbm = 0; 
		} 
		HeaderCtrl->SetItem(nSortedCol, &HeaderItem); 
		nSortedCol = pNMLV->iItem; 
	} 

    if(IsColumnNumeric(nSortedCol )) SortNumericItems( nSortedCol, bSortAscending,0,-1);
	else if(IsColumnDouble(nSortedCol )) SortDoubleItems( nSortedCol, bSortAscending,0,-1);
	else SortTextItems( nSortedCol, bSortAscending,0,-1);
   

    *pResult = 0;
}

/*
Utility function to set a column that will contain only numeric values.
Speeds up the sorting if this is set on the right columns.
*/
void MyListCtrl::SetColumnNumeric( int iCol )
{
	m_aNumericColumns.Add( iCol );
}

/*
Utility function to tell you if the given column is set as numeric.
*/
const bool MyListCtrl::IsColumnNumeric( int iCol ) const
{
	for( int i = 0; i < m_aNumericColumns.GetSize(); i++ )
	{	
		if( m_aNumericColumns.GetAt( i ) == (UINT)iCol )
			return true;
	}
	return false;
}

void MyListCtrl::SetColumnDouble(int iCol)
{
	m_aDoubleColumns.Add( iCol );
}

void MyListCtrl::SetColumnFormula(int iCol)
{
	m_aFormulaColumns.Add( iCol );
}

bool MyListCtrl::SortDoubleItems(int nCol, bool bAscending, int low, int high)
{
	CWaitCursor Cursor;

    if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
         return FALSE;
                             
	if( high == -1 ) high = GetItemCount() - 1;
	int lo = low;   
	int hi = high;
    double midItem;
                             
	if( hi <= lo ) return FALSE;
                             
	midItem = atof(GetItemText( (lo+hi)/2, nCol ));
                             
	// loop through the list until indices cross
	while( lo <= hi )
	{
	// rowText will hold all column text for one row
		CStringArray rowText;
                                     
	 // find the first element that is greater than or equal to 
	// the partition element starting from the left Index.
		if( bAscending )
		while( ( lo < high ) && (atof(GetItemText(lo, nCol)) < midItem ) )
			++lo;           
		else
		while( ( lo < high ) && (atof(GetItemText(lo, nCol)) > midItem ) )
			++lo;
                                     
	// find an element that is smaller than or equal to 
	// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && (atof(GetItemText(hi, nCol)) > midItem ) )
				 --hi;           
		else
			 while( ( hi > low ) && (atof(GetItemText(hi, nCol)) < midItem ) )
				 --hi;
                                                     
	// if the indexes have not crossed, swap                
	 // and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if(atof(GetItemText(lo, nCol)) != atof(GetItemText(hi, nCol)) )
			{                               
	// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
                                     
				int i;
				for( i=0; i < nColCount; i++) 
					rowText[i] = GetItemText(lo, i);
                                     
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
								     LVIS_FOCUSED |  LVIS_SELECTED |
								       LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
			    lvitemhi = lvitemlo;
			    lvitemhi.iItem = hi;
                                     
			    GetItem( &lvitemlo );
			    GetItem( &lvitemhi );
        
			   for( i=0; i< nColCount; i++)
				   SetItemText(lo, i, GetItemText(hi, i) );
                                     
			   lvitemhi.iItem = lo;
			   SetItem( &lvitemhi );
                                     
		       for( i=0; i< nColCount; i++)
			   SetItemText(hi, i, rowText[i]);
                                     
		      lvitemlo.iItem = hi;
		      SetItem( &lvitemlo );
			}
                                             
			++lo;
			--hi;
		}
	}                           
 // If the right index has not reached the left side of array
 // must now sort the left partition.
    if( low < hi )
    SortDoubleItems( nCol, bAscending , low, hi);
                             
 // If the left index has not reached the right side of array
 // must now sort the right partition.
    if( lo < high )
    SortDoubleItems( nCol, bAscending , lo, high );
                             
    return TRUE;
}

bool MyListCtrl::SortFormulaItems(int nCol, bool bAscending, int low, int high)
{
	CWaitCursor Cursor;
   if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
                return FALSE;

   if( high == -1 ) high = GetItemCount() - 1;

   int lo = low;
   int hi = high;
   CString midItem;

   if( hi <= lo ) return FALSE;

   midItem = GetItemText( (lo+hi)/2, nCol );

        // loop through the list until indices cross
   while( lo <= hi )
   {
        // rowText will hold all column text for one row
        CStringArray rowText;
        // find the first element that is greater than or equal to 
        // the partition element starting from the left Index.
        if( bAscending )
   while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
           ++lo;
                else
   while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
           ++lo;

        // find an element that is smaller than or equal to 
       // the partition element starting from the right Index.
        if( bAscending )
   while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
           --hi;
                else
   while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
           --hi;

       // if the indexes have not crossed, swap
       // and if the items are not equal
        if( lo <= hi )
        {
   // swap only if the items are not equal
	    if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
		{
           // swap the rows
           LV_ITEM lvitemlo, lvitemhi;
           int nColCount = 
                   ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
           rowText.SetSize( nColCount );
           int i;
           for( i=0; i<nColCount; i++)
                   rowText[i] = GetItemText(lo, i);
           lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
           lvitemlo.iItem = lo;
           lvitemlo.iSubItem = 0;
           lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
								LVIS_FOCUSED |  LVIS_SELECTED | 
								LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

           lvitemhi = lvitemlo;
           lvitemhi.iItem = hi;

           GetItem( &lvitemlo );
           GetItem( &lvitemhi );

           for( i=0; i<nColCount; i++)
                   SetItemText(lo, i, GetItemText(hi, i));

           lvitemhi.iItem = lo;
           SetItem( &lvitemhi );

           for( i=0; i<nColCount; i++)
                   SetItemText(hi, i, rowText[i]);

           lvitemlo.iItem = hi;
           SetItem( &lvitemlo );
		}

			++lo;
		   --hi;
	   }
   }

        // If the right index has not reached the left side of array
        // must now sort the left partition.
    if( low < hi )
          SortTextItems( nCol, bAscending , low, hi);

        // If the left index has not reached the right side of array
        // must now sort the right partition.
    if( lo < high )
           SortTextItems( nCol, bAscending , lo, high );

    return TRUE;
}

const bool MyListCtrl::IsColumnDouble(int iCol) const
{
	for( int i = 0; i < m_aDoubleColumns.GetSize(); i++ )
	{	
		if( m_aDoubleColumns.GetAt( i ) == (UINT)iCol )
			return true;
	}
	return false;

}

BOOL MyListCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	HDITEM HeaderItem; 
	HeaderItem.mask = HDI_FORMAT | HDI_BITMAP; 
	CHeaderCtrl* HeaderCtrl = GetHeaderCtrl(); 
	HeaderCtrl->GetItem(nSortedCol, &HeaderItem); 
	if (HeaderItem.hbm != 0) { 
		DeleteObject(HeaderItem.hbm); 
		HeaderItem.hbm = 0; 
	} 
	HeaderItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 
	HeaderItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(bSortAscending ? IDB_UP : IDB_DOWN), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 
	HeaderCtrl->SetItem(nSortedCol, &HeaderItem); 
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


