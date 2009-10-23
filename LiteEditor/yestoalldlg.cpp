///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  8 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "yestoalldlg.h"

///////////////////////////////////////////////////////////////////////////

YesToAllDlg::YesToAllDlg( wxWindow* parent, const wxString& message /*=wxT("This space unintentionally left blank")*/, const bool setcheck /*=false*/, 
							const wxString& title /*=wxT("Are you sure?")*/, wxWindowID id /*=wxID_ANY*/, const wxPoint& pos /*=wxDefaultPosition*/, 
							const wxSize& size /*=wxSize( wxDefaultSize )*/, long style /*=wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )*/ )
								: YesToAllBaseDlg( parent, id, title, pos, size, style )
{
	SetMessage(message);
	m_checkBoxAll->SetValue(setcheck);
}

void YesToAllDlg::SetMessage(const wxString& message)
{
	m_staticMsg->SetLabel(message);
	GetSizer()->Layout();
	GetSizer()->Fit(this);
}

void YesToAllDlg::SetCheckboxText(const wxString& text)
{
	m_checkBoxAll->SetLabel(text);
	GetSizer()->Layout();
	GetSizer()->Fit(this);
}
