/***************************************************************
 * Name:      TextShape.cpp
 * Purpose:   Implements static text shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/TextShape.h"
#include "wx/wxsf/ShapeCanvas.h"

XS_IMPLEMENT_CLONABLE_CLASS(wxSFTextShape, wxSFRectShape);

wxSFTextShape::wxSFTextShape(void)
: wxSFRectShape()
{
    m_Font = sfdvTEXTSHAPE_FONT;
    m_Font.SetPointSize(12);

	m_nLineHeight = 12;

    m_TextColor = sfdvTEXTSHAPE_TEXTCOLOR;
    m_sText = wxT("Text");

    m_Fill = *wxTRANSPARENT_BRUSH;
    m_Border = *wxTRANSPARENT_PEN;
	
	m_nRectSize = wxRealPoint(0, 0);

	MarkSerializableDataMembers();

    UpdateRectSize();
}

wxSFTextShape::wxSFTextShape(const wxRealPoint& pos, const wxString& txt, wxSFDiagramManager* manager)
: wxSFRectShape(pos, wxRealPoint(0, 0), manager)
{
    m_Font = sfdvTEXTSHAPE_FONT;
    m_Font.SetPointSize(12);

	m_nLineHeight = 12;

    m_TextColor = sfdvTEXTSHAPE_TEXTCOLOR;
    m_sText = txt;

    m_Fill = *wxTRANSPARENT_BRUSH;
    m_Border = *wxTRANSPARENT_PEN;

	MarkSerializableDataMembers();

    UpdateRectSize();
}

wxSFTextShape::wxSFTextShape(const wxSFTextShape& obj)
: wxSFRectShape(obj)
{
    m_Font = obj.m_Font;

    m_TextColor = obj.m_TextColor;
    m_sText = obj.m_sText;

	MarkSerializableDataMembers();

    UpdateRectSize();
}

wxSFTextShape::~wxSFTextShape()
{

}

void wxSFTextShape::MarkSerializableDataMembers()
{
    XS_SERIALIZE_EX(m_Font, wxT("font"), sfdvTEXTSHAPE_FONT);
    XS_SERIALIZE_EX(m_TextColor, wxT("color"), sfdvTEXTSHAPE_TEXTCOLOR);
    XS_SERIALIZE(m_sText, wxT("text"));
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFTextShape::Update()
{
    UpdateRectSize();
    wxSFShapeBase::Update();
}

void wxSFTextShape::Scale(double x, double y, bool children)
{
    // HINT: overload it for custom actions...

	if((x > 0) && (y > 0))
	{
	    double s = 1;

        if(x == 1) s = y;
        else if (y == 1) s = x;
	    else if(x >= y) s = x;
	    else
            s = y;

        double size = (double)m_Font.GetPointSize()*s;
        if(size < 5)size = 5;

		m_Font.SetPointSize((int)size);
		UpdateRectSize();

        // call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}

void wxSFTextShape::OnHandle(wxSFShapeHandle& handle)
{
    // HINT: overload it for custom actions...

    wxRealPoint prevSize = GetRectSize();

    // perform standard operations
	switch(handle.GetType())
	{
	case wxSFShapeHandle::hndLEFT:
		OnLeftHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHT:
		OnRightHandle(handle);
		break;

	case wxSFShapeHandle::hndTOP:
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndBOTTOM:
		OnBottomHandle(handle);
		break;

    default:
        break;
	}

    wxRealPoint newSize = m_nRectSize;

    double sx = newSize.x / prevSize.x;
    double sy = newSize.y / prevSize.y;
    Scale(sx, sy);

	switch(handle.GetType())
	{
	case wxSFShapeHandle::hndLEFT:
		{
		    double dx = m_nRectSize.x - prevSize.x;
            MoveBy(-dx, 0);

            SerializableList::compatibility_iterator node = GetFirstChildNode();
            while(node)
            {
                ((wxSFShapeBase*)node->GetData())->MoveBy(-dx, 0);
                node = node->GetNext();
            }
		}
		break;

	case wxSFShapeHandle::hndTOP:
		{
		    double dy = m_nRectSize.y - prevSize.y;
            MoveBy(0, -dy);

            SerializableList::compatibility_iterator node = GetFirstChildNode();
            while(node)
            {
                ((wxSFShapeBase*)node->GetData())->MoveBy(0, -dy);
                node = node->GetNext();
            }
		}
		break;

    default:
        break;
	}
	
	wxSFShapeBase::OnHandle( handle );
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

wxSize wxSFTextShape::GetTextExtent()
{
    wxCoord w = -1, h = -1;
    if(m_pParentManager && GetParentCanvas())
    {
        wxClientDC dc((wxWindow*)GetParentCanvas());

        // calculate text extent
        if( wxSFShapeCanvas::IsGCEnabled() )
        {
            #if wxUSE_GRAPHICS_CONTEXT
			double wd = -1, hd = -1, d = 0, e = 0;

			wxGraphicsContext *pGC = wxGraphicsContext::Create( dc );
            pGC->SetFont( m_Font, *wxBLACK );
			
			// we must use string tokenizer to inspect all lines of possible multiline text
			h = 0;
			wxString sLine;
			
			wxStringTokenizer tokens( m_sText, wxT("\n\r"), wxTOKEN_RET_EMPTY );
			while( tokens.HasMoreTokens() )
			{
				sLine = tokens.GetNextToken();
				pGC->GetTextExtent( sLine, &wd, &hd, &d, &e );
				
				h += (hd + e);
				if( (wd + d) > w ) w = (wd + d);
			}
			m_nLineHeight = (hd + e);
			
            pGC->SetFont( wxNullFont, *wxBLACK );
            #endif
        }
        else
        {
            dc.SetFont(m_Font);
            dc.GetMultiLineTextExtent(m_sText, &w, &h, &m_nLineHeight);
            dc.SetFont(wxNullFont);
        }
    }
    else
    {
        w = (int)m_nRectSize.x;
        h = (int)m_nRectSize.y;

        wxStringTokenizer tokens(m_sText, wxT("\n\r"), wxTOKEN_RET_EMPTY);
        m_nLineHeight = int(m_nRectSize.y/tokens.CountTokens());
    }

    /*if( wxSFShapeCanvas::IsGCEnabled() )
    {
        return wxSize((wxCoord)(wd + d), (wxCoord)(hd + e));
    }
    else*/
        return wxSize(w, h);
}

void wxSFTextShape::UpdateRectSize()
{
    wxSize tsize = GetTextExtent();

	if(tsize.IsFullySpecified())
	{
		if(tsize.x <= 0) tsize.x = 1;
		if(tsize.y <= 0) tsize.y = 1;

		m_nRectSize.x = (double)tsize.x;
		m_nRectSize.y = (double)tsize.y;
	}
}

void wxSFTextShape::SetText(const wxString& txt)
{
    m_sText = txt;
    UpdateRectSize();
}

void wxSFTextShape::SetFont(const wxFont& font)
{
    m_Font = font;
    UpdateRectSize();
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFTextShape::DrawNormal(wxDC& dc)
{
    wxSFRectShape::DrawNormal(dc);
    DrawTextContent(dc);
}

void wxSFTextShape::DrawHover(wxDC& dc)
{
    wxSFRectShape::DrawHover(dc);
    DrawTextContent(dc);
}

void wxSFTextShape::DrawHighlighted(wxDC& dc)
{
    wxSFRectShape::DrawHighlighted(dc);
    DrawTextContent(dc);
}

void wxSFTextShape::DrawShadow(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxColour nCurrColor = m_TextColor;
	m_TextColor = GetParentCanvas()->GetShadowFill().GetColour();
	wxRealPoint nOffset = GetParentCanvas()->GetShadowOffset();

	MoveBy(nOffset);
	DrawTextContent(dc);
	MoveBy(-nOffset.x, -nOffset.y);

	m_TextColor = nCurrColor;
}

void wxSFTextShape::OnLeftHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

    m_nRectSize.x -= ((double)handle.GetPosition().x - GetAbsolutePosition().x);
	//m_nRectSize.x -= (double)handle.GetDelta().x;
}

void wxSFTextShape::OnTopHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	m_nRectSize.y -= ((double)handle.GetPosition().y - GetAbsolutePosition().y);
	//m_nRectSize.y -= (double)handle.GetDelta().y;
}

void wxSFTextShape::OnBottomHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...
	
	m_nRectSize.y = handle.GetPosition().y - GetAbsolutePosition().y;
	
}

void wxSFTextShape::OnRightHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...
	
	m_nRectSize.x =handle.GetPosition().x - GetAbsolutePosition().x;
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//
void wxSFTextShape::DrawTextContent(wxDC& dc)
{
	wxString line;
	int i = 0;

	dc.SetBrush(m_Fill);
	dc.SetBackgroundMode(wxTRANSPARENT);

    /*#ifdef __WXMSW__
    if(m_Fill.GetStyle() == wxTRANSPARENT)
    {
        dc.SetBackgroundMode(wxTRANSPARENT);
    }
    else
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(m_Fill.GetColour());
    }
    #endif*/

    dc.SetTextForeground(m_TextColor);
	dc.SetFont(m_Font);

    wxRealPoint pos = GetAbsolutePosition();

	// draw all text lines
	wxStringTokenizer tokens(m_sText, wxT("\n\r"), wxTOKEN_RET_EMPTY);
    while(tokens.HasMoreTokens())
    {
        line = tokens.GetNextToken();
		dc.DrawText(line, (int)pos.x, (int)pos.y + i*m_nLineHeight);
		i++;
	}

    dc.SetFont(wxNullFont);

	dc.SetBrush(wxNullBrush);
}
