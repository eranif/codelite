//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : debuggertip.cpp              
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
 #if 0
#include "debuggertip.h"
#include "tiptree.h"
#include "wx/stattext.h"
#include "wx/statline.h"
#include "wx/dcbuffer.h"
#include "manager.h"
#include "editor.h"

DebuggerTip::DebuggerTip(wxWindow *parent, const wxString &expression, TreeNode<wxString, NodeData> *tree, long pos)
: wxPopupWindow(parent) 
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	Initialize(expression, tree, pos);
}

DebuggerTip::~DebuggerTip()
{
}

void DebuggerTip::Initialize(const wxString &expression, TreeNode<wxString, NodeData> *tree, long pos)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(topSizer);
	
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	wxPanel *mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER );
	mainPanel->SetSizer(sz);
	
	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	
	wxStaticText *text = new wxStaticText(mainPanel, wxID_ANY, wxT("Expression: "));
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetWeight(wxBOLD);
	
	text->SetFont(font);
	hsz->Add(text, 0, wxEXPAND|wxALL, 5);
	
	wxStaticText *expr = new wxStaticText(mainPanel, wxID_ANY, expression);
	hsz->Add(expr, 0, wxEXPAND|wxALL, 5);
	sz->Add(hsz, 0, wxEXPAND);
	
	wxStaticLine *staticline = new wxStaticLine(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	sz->Add(staticline, 0, wxEXPAND|wxALL, 5);
	
	m_localVarsTree = new TipTree(mainPanel, wxID_ANY, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxNO_BORDER);
	m_localVarsTree->BuildTree(tree);
	sz->Add(m_localVarsTree, 1, wxALL|wxEXPAND, 5);
	sz->SetMinSize(200, 200);
	
	topSizer->Add(sz, 1, wxEXPAND|wxALL, 1);
	sz->Fit(mainPanel);
	topSizer->Fit(this);
	
	//set the tip window at the caret place
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	if(editor){
		wxPoint pt = editor->PointFromPosition(pos);
		//pt is in wxScintilla coordinates, need to convert them into
		//the screen coordinates
		wxPoint displayPt = editor->ClientToScreen(pt);
		this->Position(displayPt, wxSize(0, 0));
	}
}
#endif
