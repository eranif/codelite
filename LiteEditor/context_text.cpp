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

ContextBase *ContextText::NewInstance(LEditor *container)
{
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
	LexerConfPtr lexPtr;
	
	// Read the configuration file
	if (EditorConfigST::Get()->IsOk()) {
		lexPtr = EditorConfigST::Get()->GetLexer(wxT("Text"));
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(wxSCI_LEX_NULL);
	rCtrl.StyleClearAll();
	
	DoApplySettings( lexPtr );
}
