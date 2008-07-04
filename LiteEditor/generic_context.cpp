//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : generic_context.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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
void ContextGeneric::CodeComplete(long pos)
{
	wxUnusedVar(pos);
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
