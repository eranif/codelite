//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : context_text.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
