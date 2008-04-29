#include "context_base.h"
#include <vector>
#include "editor_config.h"
#include "cl_editor.h"

ContextBase::ContextBase(LEditor *container)
		: m_container(container)
		, m_name(wxEmptyString)
{
}

ContextBase::ContextBase(const wxString &name)
		: m_name(name)
{
}

ContextBase::~ContextBase()
{
}

//provide basic indentation
void ContextBase::AutoIndent(const wxChar &ch)
{
	if (ch == wxT('\n')) {
		//just copy the previous line indentation
		LEditor &rCtrl = GetCtrl();
		int indentSize = rCtrl.GetIndent();
		int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
		int prevLine = line - 1;
		//take the previous line indentation size
		int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
		rCtrl.SetLineIndentation(line, prevLineIndet);
		//place the caret at the end of the line
		int dummy = rCtrl.GetLineIndentation(line);
		if (rCtrl.GetUseTabs()) {
			dummy = dummy / indentSize;
		}
		rCtrl.SetCaretAt(rCtrl.GetCurrentPos() + dummy);
	}
}

void ContextBase::DoApplySettings(LexerConfPtr lexPtr)
{
	LEditor &rCtrl = GetCtrl();

	rCtrl.StyleClearAll();
	rCtrl.SetStyleBits(rCtrl.GetStyleBitsNeeded());

	std::list<StyleProperty> styles = lexPtr->GetProperties();
	std::list<StyleProperty>::iterator iter = styles.begin();
	for (; iter != styles.end(); iter++) {
		StyleProperty st = (*iter);
		int size = st.GetFontSize();
		wxString face = st.GetFaceName();
		bool bold = st.IsBold();

		if ( st.GetId() == -1 ) {
			// fold margin foreground colour
			rCtrl.SetFoldMarginColour(true, st.GetBgColour());
			rCtrl.SetFoldMarginHiColour(true, st.GetFgColour());
		} else if( st.GetId() == -2 ) {
			// selection colour
			rCtrl.SetSelForeground(true, st.GetFgColour());
			rCtrl.SetSelBackground(true, st.GetBgColour());
		} else {


			wxFont font;
			if (st.GetId() == wxSCI_STYLE_CALLTIP) {
				font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
			} else {
				font = wxFont(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
			}

			if (st.GetId() == 0) { //default
				rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT, font);
				rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT, (*iter).GetFontSize());
				rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT, (*iter).GetFgColour());
				rCtrl.StyleSetBackground(wxSCI_STYLE_DEFAULT, (*iter).GetBgColour());
				rCtrl.StyleSetSize(wxSCI_STYLE_LINENUMBER, (*iter).GetFontSize());
				rCtrl.SetFoldMarginColour(true, (*iter).GetBgColour());
				rCtrl.SetFoldMarginHiColour(true, (*iter).GetBgColour());
			}

			rCtrl.StyleSetFont(st.GetId(), font);
			rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
			rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
			rCtrl.StyleSetBackground(st.GetId(), (*iter).GetBgColour());
		}
	}

}
