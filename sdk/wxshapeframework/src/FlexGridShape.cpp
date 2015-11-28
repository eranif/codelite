/***************************************************************
 * Name:      FlexGridShape.cpp
 * Purpose:   Implements flexible grid shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-09-27
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/FlexGridShape.h"

XS_IMPLEMENT_CLONABLE_CLASS(wxSFFlexGridShape, wxSFGridShape);

wxSFFlexGridShape::wxSFFlexGridShape() : wxSFGridShape()
{
}

wxSFFlexGridShape::wxSFFlexGridShape(const wxRealPoint& pos, const wxRealPoint& size, int rows, int cols, int cellspace, wxSFDiagramManager* manager)
: wxSFGridShape(pos, size, rows, cols, cellspace, manager)
{
}

wxSFFlexGridShape::wxSFFlexGridShape(const wxSFFlexGridShape& obj) : wxSFGridShape(obj)
{
}

wxSFFlexGridShape::~wxSFFlexGridShape()
{

}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFFlexGridShape::DoChildrenLayout()
{
    if( !m_nCols || !m_nRows ) return;

    wxSFShapeBase *pShape;
    int nIndex, nRow, nCol, nTotalX, nTotalY;
    size_t i;

    wxRect nCurrRect;

    // initialize size arrays
    m_arrRowSizes.SetCount( m_nRows );
    m_arrColSizes.SetCount( m_nCols );
    for( i = 0; i < (size_t)m_nRows; i++ ) m_arrRowSizes[i] = 0;
    for( i = 0; i < (size_t)m_nCols; i++ ) m_arrColSizes[i] = 0;

    nIndex = nCol = nTotalX = nTotalY = 0;
    nRow = -1;

    // prepare a storage for processed shapes pointers
    m_arrChildShapes.SetCount( m_arrCells.GetCount() );

    // get maximum size of all managed (child) shapes per row and collumn
    for( i = 0; i < m_arrCells.GetCount(); i++ )
    {
        pShape = (wxSFShapeBase*)GetChild(m_arrCells[i]);
        if( pShape )
        {
            // store used shape pointer for further processing (optimization for speed)
            m_arrChildShapes[i] = pShape;

            if( nIndex++ % m_nCols == 0 )
            {
                nCol = 0; nRow++;
            }
            else
                nCol++;

            nCurrRect = pShape->GetBoundingBox();

            // update maximum rows and columns sizes
            if( (pShape->GetHAlign() != halignEXPAND) && (nCurrRect.GetWidth() > m_arrColSizes[nCol]) ) m_arrColSizes[nCol] = nCurrRect.GetWidth();
            if( (pShape->GetVAlign() != valignEXPAND) && (nCurrRect.GetHeight() > m_arrRowSizes[nRow]) ) m_arrRowSizes[nRow] = nCurrRect.GetHeight();
        }
    }

    // put managed shapes to appropriate positions
    nIndex = nCol = 0;
    nRow = -1;

    for( i = 0; i < m_arrCells.GetCount(); i++ )
    {
        pShape = m_arrChildShapes[i];
        if( pShape )
        {
            if( nIndex++ % m_nCols == 0 )
            {
                nCol = 0; nTotalX = 0; nRow++;
                if( nRow > 0 ) nTotalY += m_arrRowSizes[ nRow-1 ];
            }
            else
            {
                nCol++;
                if( nCol > 0 ) nTotalX += m_arrColSizes[ nCol-1 ];
            }

            FitShapeToRect( pShape, wxRect( nTotalX + (nCol+1)*m_nCellSpace,
                                            nTotalY + (nRow+1)*m_nCellSpace,
                                            m_arrColSizes[ nCol ], m_arrRowSizes[ nRow ] ) );
        }
    }
}
