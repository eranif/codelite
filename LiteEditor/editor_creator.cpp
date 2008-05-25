//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editor_creator.cpp              
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
 #include "editor_creator.h"
#include "cl_editor.h"
#include "wx/window.h"
#include "frame.h"


EditorCreator::EditorCreator()
: m_parent(NULL)
{
}


EditorCreator::~EditorCreator()
{
}

void EditorCreator::SetParent(wxWindow *parent)
{
	m_parent = parent;
}

LEditor *EditorCreator::NewInstance()
{
	return new LEditor(Frame::Get()->GetNotebook(), wxID_ANY, wxSize(1, 1), wxEmptyString, wxEmptyString, false);
}

void EditorCreator::Add(LEditor *editor)
{
	m_queue.push_back(editor);
}

size_t EditorCreator::GetQueueCount()
{
	return m_queue.size();
}
