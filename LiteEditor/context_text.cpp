#include "context_text.h"
#include <vector>
#include "editor_config.h"
#include "cl_editor.h"

ContextText::ContextText(LEditor *container) 
: ContextBase(container)
{
	ApplySettings();
}

ContextText::~ContextText()
{
}

ContextBase *ContextText::NewInstance(LEditor *container){
	return new ContextText(container);
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::AutoIndent(const wxChar &nChar)
{
	ContextBase::AutoIndent(nChar);
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::CodeComplete()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::CompleteWord()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::GotoDefinition()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::GotoPreviousDefintion()
{
}

void ContextText::ApplySettings()
{
	// Initialise default style settings
	SetName(wxT("Text"));

	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------

	// Set the key words and the lexer
	wxString keyWords;
	std::list<StyleProperty> styles;

	// Read the configuration file
	if(EditorConfigST::Get()->IsOk()){
		styles = EditorConfigST::Get()->GetLexer(wxT("Text"))->GetProperties();
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(wxSCI_LEX_NULL);
	rCtrl.StyleClearAll();

	std::list<StyleProperty>::iterator iter = styles.begin();
	for(iter != styles.end(); iter != styles.end(); iter++)
	{
		int size = (*iter).GetFontSize();
		wxString face = (*iter).GetFaceName();
		bool bold = (*iter).IsBold();

		wxFont font;
		if((*iter).GetId() == wxSCI_STYLE_CALLTIP){
			font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		} else {
			font = wxFont(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		}

		if((*iter).GetId() == 0){ //default
			rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT, font);
			rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT, (*iter).GetFontSize());
			rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT, (*iter).GetFgColour());
			rCtrl.StyleSetBackground(wxSCI_STYLE_DEFAULT, (*iter).GetBgColour());
			rCtrl.StyleSetBackground(wxSCI_STYLE_LINENUMBER, (*iter).GetBgColour());
			rCtrl.StyleSetSize(wxSCI_STYLE_LINENUMBER, (*iter).GetFontSize());
		}

		rCtrl.StyleSetFont((*iter).GetId(), font);
		rCtrl.StyleSetSize((*iter).GetId(), (*iter).GetFontSize());
		rCtrl.StyleSetForeground((*iter).GetId(), (*iter).GetFgColour());
		rCtrl.StyleSetBackground((*iter).GetId(), (*iter).GetBgColour());
	}	
}
