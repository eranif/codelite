//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svnxmlparser.cpp              
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
 #include "svnxmlparser.h"
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include "xmlutils.h"
#include <wx/log.h>

SvnXmlParser::SvnXmlParser()
{
}

SvnXmlParser::~SvnXmlParser()
{
}

void SvnXmlParser::GetFiles(const wxString &input, wxArrayString &files, FileState state)
{
	wxString searchedState;
	if (state & StateConflict) {
		searchedState << wxT("conflicted");
	}

	if (state & StateModified) {
		searchedState << wxT(" modified added merged deleted ");
	}

	if (state & StateUnversioned) {
		searchedState << wxT(" unversioned ignored ");
	}

	wxStringInputStream stream(input);
	wxXmlDocument doc(stream);
	if (!doc.IsOk()) {
		//wxLogMessage(input);
		return;
	}

	wxXmlNode *root = doc.GetRoot();
	if (root) {
		wxXmlNode *node = root->GetChildren();
		while ( node ) {
			if (node->GetName() == wxT("target")) {
				wxXmlNode *child = node->GetChildren();
				while (child) {
					if (child->GetName() == wxT("entry")) {
						wxString path = XmlUtils::ReadString(child, wxT("path"), wxEmptyString);
						wxXmlNode *status = XmlUtils::FindFirstByTagName(child, wxT("wc-status"));
						if (status) {
							wxString item = XmlUtils::ReadString(status, wxT("item"), wxEmptyString);
							if (path.IsEmpty() == false && searchedState.Contains(item)) {
								files.Add(path);
							}
						}
					}
					child = child->GetNext();
				}
			}
			node = node->GetNext();
		}
	}
}
