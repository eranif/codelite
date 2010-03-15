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
#include "svnxml.h"
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include "xmlutils.h"
#include <wx/log.h>
#include "wx_xml_compatibility.h"

SvnXML::SvnXML()
{
}

SvnXML::~SvnXML()
{
}

void SvnXML::GetFiles(const wxString &input, wxArrayString& modifiedFiles, wxArrayString &conflictedFiles, wxArrayString &unversionedFiles, wxArrayString& newFiles, wxArrayString& deletedFiles)
{
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
                                if (item == wxT("modified") || item == wxT("merged")) {
                                    modifiedFiles.Add(path);
                                } else
								if (item == wxT("conflicted")) {
                                    conflictedFiles.Add(path);
                                } else
								if (item == wxT("added")) {
                                    newFiles.Add(path);
                                } else
								if (item == wxT("deleted")) {
                                    deletedFiles.Add(path);
                                } else
								if (item == wxT("unversioned")) {
                                    unversionedFiles.Add(path);
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

void SvnXML::GetSvnInfo(const wxString& input, SvnInfo &svnInfo)
{
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
			if (node->GetName() == wxT("entry")) {
				node->GetPropVal(wxT("revision"), &svnInfo.m_revision);

				// Look for the URL
				wxXmlNode *child = node->GetChildren();
				while( child ) {
					if(child->GetName() == wxT("url")) {
						svnInfo.m_sourceUrl = child->GetNodeContent();
					}

					// Last commit info
					if( child->GetName() == wxT("commit") ) {
						wxXmlNode *gchild = child->GetChildren();
						while( gchild ) {
							if(gchild->GetName() == wxT("author")) {
								svnInfo.m_author = gchild->GetNodeContent();
							}

							if(gchild->GetName() == wxT("date")) {
								svnInfo.m_date = gchild->GetNodeContent();
							}
							gchild = gchild->GetNext();
						}
					}

					if( child->GetName() == wxT("repository") ) {
						wxXmlNode *gchild = child->GetChildren();
						while( gchild ) {
							if(gchild->GetName() == wxT("root")) {
								svnInfo.m_url = gchild->GetNodeContent();
								break;
							}
							gchild = gchild->GetNext();
						}
					}
					child = child->GetNext();
				}
			}
			node = node->GetNext();
		}
	}
	return;
}
