#include "generic_context.h"
#include <vector>
#include "editor_config.h"
#include "cl_editor.h"

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

ContextBase *ContextGeneric::NewInstance(LEditor *container)
{
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
	LexerConfPtr lexPtr;
	// Read the configuration file
	if (EditorConfigST::Get()->IsOk()) {
		lexPtr = EditorConfigST::Get()->GetLexer(m_name);
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(lexPtr->GetLexerId());

	for (int i = 0; i <= 4; ++i) {
		wxString keyWords = lexPtr->GetKeyWords(i);
		keyWords.Replace(wxT("\n"), wxT(" "));
		keyWords.Replace(wxT("\r"), wxT(" "));
		rCtrl.SetKeyWords(i, keyWords);
	}

	DoApplySettings( lexPtr );
}

void ContextGeneric::OnFileSaved()
{
}
