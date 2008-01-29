#include "generic_context.h"
#include <vector>
#include "editor_config.h"
#include "editor.h"

ContextGeneric::ContextGeneric(LEditor *container, const wxString &name) 
: ContextBase(container)
{
	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------
	SetName(name);
	ApplySettings();
}


ContextGeneric::~ContextGeneric()
{
}

ContextBase *ContextGeneric::NewInstance(LEditor *container){
	return new ContextGeneric(container, GetName());
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::AutoIndent(const wxChar &nChar)
{
	ContextBase::AutoIndent(nChar);
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::CodeComplete()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::CompleteWord()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::GotoDefinition()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::GotoPreviousDefintion()
{
}

void ContextGeneric::ApplySettings()
{
	// Set the key words and the lexer
	std::list<StyleProperty> styles;
	LexerConfPtr lexPtr;
	// Read the configuration file
	if(EditorConfigST::Get()->IsOk()){
		lexPtr = EditorConfigST::Get()->GetLexer(m_name);
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(lexPtr->GetLexerId());

	wxString keyWords = lexPtr->GetKeyWords();
	keyWords.Replace(wxT("\n"), wxT(" "));
	keyWords.Replace(wxT("\r"), wxT(" "));
	rCtrl.SetKeyWords(0, keyWords);
	rCtrl.StyleClearAll();
	
	styles = lexPtr->GetProperties();
	std::list<StyleProperty>::iterator iter = styles.begin();
	for(; iter != styles.end(); iter++)
	{
		StyleProperty st = (*iter);
		int size = st.GetFontSize();
		wxString face = st.GetFaceName();
		bool bold = st.IsBold();
		
		wxFont font(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);

		if(st.GetId() == 0){ //default
			rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT, font);
			rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT, (*iter).GetFontSize());
			rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT, (*iter).GetFgColour());
			rCtrl.StyleSetBackground(wxSCI_STYLE_DEFAULT, (*iter).GetBgColour());
			rCtrl.StyleSetBackground(wxSCI_STYLE_LINENUMBER, (*iter).GetBgColour());
			rCtrl.StyleSetSize(wxSCI_STYLE_LINENUMBER, (*iter).GetFontSize());
		}

		rCtrl.StyleSetFont(st.GetId(), font);
		rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
		rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
		rCtrl.StyleSetBackground(st.GetId(), (*iter).GetBgColour());
	}
}

void ContextGeneric::OnFileSaved()
{
	//by default just colour the page
	GetCtrl().Colourise(0, wxSCI_INVALID_POSITION);
}
