/***************************************************************
 * Name:      EditTextShape.cpp
 * Purpose:   Implements editable text shape class
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

#include <wx/textctrl.h>
#include "wx/wxsf/EditTextShape.h"
#include "wx/wxsf/ShapeCanvas.h"

static int textCtrlId = wxNewId();

XS_IMPLEMENT_CLONABLE_CLASS(wxSFEditTextShape, wxSFTextShape);

BEGIN_EVENT_TABLE(wxSFContentCtrl, wxTextCtrl)
	EVT_KILL_FOCUS(wxSFContentCtrl::OnKillFocus)
	EVT_KEY_DOWN(wxSFContentCtrl::OnKeyDown)
END_EVENT_TABLE()

//----------------------------------------------------------------------------------//
// wxSFContentCtrl control class
//----------------------------------------------------------------------------------//

wxSFContentCtrl::wxSFContentCtrl(wxWindow* parent, wxWindowID id, wxSFEditTextShape* parentShape, const wxString& content, wxPoint pos, wxSize size, int style)
: wxTextCtrl(parent, id, content, pos, size, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxNO_BORDER | style)
{
	m_pParent = parent;
	m_pParentShape = parentShape;
	m_sPrevContent = content;

	SetInsertionPointEnd();
	if(m_pParentShape)
	{
		wxSFTextShape* pTextShape = (wxSFTextShape*)m_pParentShape;

		// update the font size in accordance to the canvas scale
		wxFont font = pTextShape->GetFont();
		font.SetPointSize(int(font.GetPointSize() * m_pParentShape->GetParentCanvas()->GetScale()));

		SetFont(font);
		SetBackgroundColour(wxColour(200, 200, 200));
		SetFocus();
	}
}

void wxSFContentCtrl::OnKillFocus(wxFocusEvent& event)
{
	wxUnusedVar( event );
	
	//Quit();
}

void wxSFContentCtrl::OnKeyDown(wxKeyEvent& event)
{
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		Quit( sfCANCEL_TEXT_CHANGES );
		break;
	case WXK_TAB:
		Quit( sfAPPLY_TEXT_CHANGES );
		break;
	case WXK_RETURN:
		// enter new line if SHIFT key was pressed together with the ENTER key
		if( wxGetKeyState( WXK_SHIFT ) )
		{
			event.Skip();
		}
		else
			Quit( sfAPPLY_TEXT_CHANGES );
		break;
	default:
		event.Skip();
	}
}

void wxSFContentCtrl::Quit(bool apply)
{
	Hide();
	
	if(m_pParentShape)
	{
		m_pParentShape->m_pTextCtrl = NULL;
		m_pParentShape->SetStyle(m_pParentShape->m_nCurrentState);

		// save canvas state if the textctrl content has changed...
		if( apply && ( m_sPrevContent != GetValue() ) )
		{
			m_pParentShape->SetText(GetValue());
			m_sPrevContent = GetValue();
			
		    // inform parent shape canvas about text change...
            m_pParentShape->GetParentCanvas()->OnTextChange(m_pParentShape);
		    m_pParentShape->GetParentCanvas()->SaveCanvasState();
		}

		m_pParentShape->Update();
		m_pParentShape->GetParentCanvas()->Refresh();
	}

	Destroy();
}

//----------------------------------------------------------------------------------//
// wxSFDetachedContentCtrl control class
//----------------------------------------------------------------------------------//

wxSFDetachedContentCtrl::wxSFDetachedContentCtrl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 350,100 ), wxTE_MULTILINE );
	m_pText->SetMinSize( wxSize( 350,100 ) );
	
	mainSizer->Add( m_pText, 1, wxALL|wxEXPAND, 5 );
	
	wxStdDialogButtonSizer* buttonSizer;
	wxButton* buttonSizerOK;
	wxButton* buttonSizerCancel;
	buttonSizer = new wxStdDialogButtonSizer();
	buttonSizerOK = new wxButton( this, wxID_OK );
	buttonSizer->AddButton( buttonSizerOK );
	buttonSizerCancel = new wxButton( this, wxID_CANCEL );
	buttonSizer->AddButton( buttonSizerCancel );
	buttonSizer->Realize();
	mainSizer->Add( buttonSizer, 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

wxSFDetachedContentCtrl::~wxSFDetachedContentCtrl()
{
}

//----------------------------------------------------------------------------------//
// wxSFEditTextShape shape class
//----------------------------------------------------------------------------------//

wxSFEditTextShape::wxSFEditTextShape(void)
: wxSFTextShape()
{
	m_pTextCtrl = NULL;
	m_fForceMultiline = sfdvEDITTEXTSHAPE_FORCEMULTILINE;
	m_nEditType = sfdvEDITTEXTSHAPE_EDITTYPE;

	XS_SERIALIZE_EX(m_fForceMultiline, wxT("multiline"), sfdvEDITTEXTSHAPE_FORCEMULTILINE);
	XS_SERIALIZE_INT_EX(m_nEditType, wxT("edittype"), sfdvEDITTEXTSHAPE_EDITTYPE);
}

wxSFEditTextShape::wxSFEditTextShape(const wxRealPoint& pos, const wxString& txt, wxSFDiagramManager* manager)
: wxSFTextShape(pos, txt, manager)
{
	m_pTextCtrl = NULL;
	m_fForceMultiline = sfdvEDITTEXTSHAPE_FORCEMULTILINE;
	m_nEditType = sfdvEDITTEXTSHAPE_EDITTYPE;

	XS_SERIALIZE_EX(m_fForceMultiline, wxT("multiline"), sfdvEDITTEXTSHAPE_FORCEMULTILINE);
	XS_SERIALIZE_INT_EX(m_nEditType, wxT("edittype"), sfdvEDITTEXTSHAPE_EDITTYPE);
}

wxSFEditTextShape::wxSFEditTextShape(const wxSFEditTextShape& obj)
: wxSFTextShape(obj)
{
	m_pTextCtrl = NULL;
	m_fForceMultiline = obj.m_fForceMultiline;
	m_nEditType = obj.m_nEditType;

	XS_SERIALIZE_EX(m_fForceMultiline, wxT("multiline"), sfdvEDITTEXTSHAPE_FORCEMULTILINE);
	XS_SERIALIZE_INT_EX(m_nEditType, wxT("edittype"), sfdvEDITTEXTSHAPE_EDITTYPE);
}

wxSFEditTextShape::~wxSFEditTextShape(void)
{
	if(m_pTextCtrl)delete m_pTextCtrl;
}
//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

void wxSFEditTextShape::EditLabel()
{
	if( GetParentCanvas() )
	{
		int dx, dy;
		wxRealPoint shpPos = GetAbsolutePosition();
		double scale = GetParentCanvas()->GetScale();
		GetParentCanvas()->CalcUnscrolledPosition(0, 0, &dx, &dy);
		
		switch( m_nEditType )
		{
			case editINPLACE:
			{
				wxRect shpBB = GetBoundingBox();
				int style = 0;

				if( m_fForceMultiline || m_sText.Contains(wxT("\n")) )
				{
					style = wxTE_MULTILINE;
					// set minimal control size
				}

				if( (m_sText == wxEmptyString) || ((style == wxTE_MULTILINE) && (shpBB.GetWidth() < 50)) )shpBB.SetWidth(50);

				m_pTextCtrl = new wxSFContentCtrl(GetParentCanvas(), textCtrlId, this, m_sText, wxPoint(int((shpPos.x * scale) - dx), int((shpPos.y * scale) - dy)), wxSize(int(shpBB.GetWidth() * scale), int(shpBB.GetHeight() * scale)), style);

				m_nCurrentState = GetStyle();
				RemoveStyle(sfsSIZE_CHANGE);
			}
			break;
			
			case editDIALOG:
			{
				wxString sPrevText = GetText();
				
				wxSFDetachedContentCtrl m_pTextDlg( GetParentCanvas() );
				
				//m_pTextDlg.Move( wxPoint(int((shpPos.x * scale) - dx), int((shpPos.y * scale) - dy)) );
				m_pTextDlg.SetContent( sPrevText );
				
				if( m_pTextDlg.ShowModal() == wxID_OK )
				{
					if( m_pTextDlg.GetContent() != sPrevText )
					{
						SetText( m_pTextDlg.GetContent() );
						
						GetParentCanvas()->OnTextChange( this );
						GetParentCanvas()->SaveCanvasState();
						
						Update();
						GetParentCanvas()->Refresh( false );
					}
				}
			}
			break;
			
			default:
				break;
		}
	}
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFEditTextShape::OnLeftDoubleClick(const wxPoint& pos)
{
	// HINT: override it if neccessary...
	
	wxUnusedVar( pos );

    EditLabel();
}

bool wxSFEditTextShape::OnKey(int key)
{
    // HINT: override it if neccessary...

    switch(key)
    {
        case WXK_F2:
            if(IsActive() && IsVisible())
            {
                EditLabel();
            }
            break;

        default:
            break;
    }

    return wxSFShapeBase::OnKey(key);
}
