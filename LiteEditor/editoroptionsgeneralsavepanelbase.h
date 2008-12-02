///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editoroptionsgeneralsavepanelbase__
#define __editoroptionsgeneralsavepanelbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorOptionsGeneralSavePanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorOptionsGeneralSavePanelBase : public wxPanel
{
	private:

	protected:
		wxCheckBox* m_checkBoxTrimLine;
		wxCheckBox* m_checkBoxAppendLF;

	public:
		EditorOptionsGeneralSavePanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorOptionsGeneralSavePanelBase();

};

#endif //__editoroptionsgeneralsavepanelbase__
