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
	wxString wcSearchedState;
	if (state & StateConflict) {
		wcSearchedState << wxT("conflicted");
	}

	if (state & StateModified) {
		wcSearchedState << wxT(" modified added merged deleted ");
	}

	if (state & StateUnversioned) {
		wcSearchedState << wxT(" unversioned ignored ");
	}
    
    wxString repSearchedState;
    if (state & StateOutOfDate) {
        repSearchedState << wxT(" modified ");
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
                        if (!path.IsEmpty()) {
                            wxXmlNode *status = XmlUtils::FindFirstByTagName(child, wxT("wc-status"));
                            if (status) {
                                wxString item = XmlUtils::ReadString(status, wxT("item"), wxEmptyString);
                                if (wcSearchedState.Contains(item) && files.Index(path) == wxNOT_FOUND) {
                                    files.Add(path);
                                }
                            }
                            status = XmlUtils::FindFirstByTagName(child, wxT("repos-status"));
                            if (status) {
                                wxString item = XmlUtils::ReadString(status, wxT("item"), wxEmptyString);
                                if (repSearchedState.Contains(item) && files.Index(path) == wxNOT_FOUND) {
                                    files.Add(path);
                                }
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

wxString SvnXmlParser::GetRevision(const wxString& input)
{
	wxStringInputStream stream(input);
	wxXmlDocument doc(stream);
	if (!doc.IsOk()) {
		//wxLogMessage(input);
		return wxEmptyString;
	}

	wxXmlNode *root = doc.GetRoot();
	if (root) {
		wxXmlNode *node = root->GetChildren();
		while ( node ) {
			if (node->GetName() == wxT("entry")) {
				return XmlUtils::ReadString(node, wxT("revision"), wxEmptyString);
			}
			node = node->GetNext();
		}
	}
	return wxEmptyString;
}
