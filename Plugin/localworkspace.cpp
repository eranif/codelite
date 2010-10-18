//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : localoptions.cpp
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

#include "editor_config.h"
#include <wx/log.h>
#include "globals.h"
#include "optionsconfig.h"
#include <wx/fontmap.h>
#include <wx/utils.h>
#include "xmlutils.h"
#include "workspace.h"
#include "project.h"
#include "localworkspace.h"
#include "macros.h"

#include "wx_xml_compatibility.h"

//-----------------------------------------------------------------------------

LocalOptionsConfig::LocalOptionsConfig(OptionsConfigPtr opts, wxXmlNode *node)
{
	// Used for reading local values, which are merged into the passed OptionsConfigPtr only if valid
	if ( node ) {
		bool answer; wxString str; long l;
		if (XmlUtils::ReadBoolIfExists(node, wxT("DisplayFoldMargin"), answer) ) {
			opts->SetDisplayFoldMargin(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("DisplayBookmarkMargin"), answer) ) {
			opts->SetDisplayBookmarkMargin(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("HighlightCaretLine"), answer) ) {
			opts->SetHighlightCaretLine(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("EditorTrimEmptyLines"), answer) ) {
			opts->SetTrimLine(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("EditorAppendLf"), answer) ) {
			opts->SetAppendLF(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("ShowLineNumber"), answer) ) {
			opts->SetDisplayLineNumbers(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("IndentationGuides"), answer) ) {
			opts->SetShowIndentationGuidelines(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("IndentUsesTabs"), answer) ) {
			opts->SetIndentUsesTabs(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("HideChangeMarkerMargin"), answer) ) {
			opts->SetHideChangeMarkerMargin(answer);
		}
		if (XmlUtils::ReadLongIfExists(node, wxT("IndentWidth"), l) ) {
			opts->SetIndentWidth(l);
		}
		if (XmlUtils::ReadLongIfExists(node, wxT("TabWidth"), l) ) {
			opts->SetTabWidth(l);
		}
		if (XmlUtils::ReadLongIfExists(node, wxT("ShowWhitespaces"), l) ) {
			opts->SetShowWhitspaces(l);
		}
		if (XmlUtils::ReadStringIfExists(node, wxT("EOLMode"), str) ) {
			opts->SetEolMode(str);
		}
		if (XmlUtils::ReadStringIfExists(node, wxT("FileFontEncoding"), str) ) {
			opts->SetFileFontEncoding(str);
		}
	}
}

LocalOptionsConfig::LocalOptionsConfig(LocalOptionsConfigPtr opts, wxXmlNode *node)
{
	// Used for reading local values, which are stored in the passed empty LocalOptionsConfigPtr only if valid
	// This is the same code as the previous ctor,except opts is a *Local*OptionsConfigPtr
	// This duplication is ugly, but not as ugly as any alternatives I could think of :(
	// (The main problem is that e.g. LocalOptionsConfig::SetDisplayFoldMargin has a different prototype to OptionsConfig::SetDisplayFoldMargin)
	if ( node ) {
		bool answer; wxString str; long l;
		if (XmlUtils::ReadBoolIfExists(node, wxT("DisplayFoldMargin"), answer) ) {
			opts->SetDisplayFoldMargin(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("DisplayBookmarkMargin"), answer) ) {
			opts->SetDisplayBookmarkMargin(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("HighlightCaretLine"), answer) ) {
			opts->SetHighlightCaretLine(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("EditorTrimEmptyLines"), answer) ) {
			opts->SetTrimLine(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("EditorAppendLf"), answer) ) {
			opts->SetAppendLF(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("ShowLineNumber"), answer) ) {
			opts->SetDisplayLineNumbers(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("IndentationGuides"), answer) ) {
			opts->SetShowIndentationGuidelines(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("IndentUsesTabs"), answer) ) {
			opts->SetIndentUsesTabs(answer);
		}
		if (XmlUtils::ReadBoolIfExists(node, wxT("HideChangeMarkerMargin"), answer) ) {
			opts->SetHideChangeMarkerMargin(answer);
		}
		if (XmlUtils::ReadLongIfExists(node, wxT("IndentWidth"), l) ) {
			opts->SetIndentWidth(l);
		}
		if (XmlUtils::ReadLongIfExists(node, wxT("TabWidth"), l) ) {
			opts->SetTabWidth(l);
		}
		if (XmlUtils::ReadLongIfExists(node, wxT("ShowWhitespaces"), l) ) {
			opts->SetShowWhitespaces(l);
		}
		if (XmlUtils::ReadStringIfExists(node, wxT("EOLMode"), str) ) {
			opts->SetEolMode(str);
		}
		if (XmlUtils::ReadStringIfExists(node, wxT("FileFontEncoding"), str) ) {
			opts->SetFileFontEncoding(str);
		}
	}
}

LocalOptionsConfig::LocalOptionsConfig()
{
	// Used to create an empty instance to save-to-xml any local preferences
	// Any invalid ones aren't saved: the global choice will be used automatically

	// All the members are validVars, which auto-set to invalid, so no need to do anything here
}

wxXmlNode* LocalOptionsConfig::ToXml(wxXmlNode* parent /*=NULL*/, const wxString& nodename /*=wxT("Options")*/) const
{
	wxXmlNode *n = new wxXmlNode(parent, wxXML_ELEMENT_NODE, nodename);

	if (DisplayFoldMarginIsValid()) {
		n->AddProperty(wxT("DisplayFoldMargin"),         BoolToString(m_localdisplayFoldMargin.GetDatum()));
	}
	if (DisplayBookmarkMarginIsValid()) {
		n->AddProperty(wxT("DisplayBookmarkMargin"),     BoolToString(m_localdisplayBookmarkMargin.GetDatum()));
	}
	if (HighlightCaretLineIsValid()) {
		n->AddProperty(wxT("HighlightCaretLine"),        BoolToString(m_localhighlightCaretLine.GetDatum()));
	}
	if (TrimLineIsValid()) {
		n->AddProperty(wxT("EditorTrimEmptyLines"),      BoolToString(m_localTrimLine.GetDatum()));
	}
	if (AppendLFIsValid()) {
		n->AddProperty(wxT("EditorAppendLf"),            BoolToString(m_localAppendLF.GetDatum()));
	}
	if (DisplayLineNumbersIsValid()) {
		n->AddProperty(wxT("ShowLineNumber"),            BoolToString(m_localdisplayLineNumbers.GetDatum()));
	}
	if (ShowIndentationGuidelinesIsValid()) {
		n->AddProperty(wxT("IndentationGuides"),         BoolToString(m_localshowIndentationGuidelines.GetDatum()));
	}
	if (IndentUsesTabsIsValid()) {
		n->AddProperty(wxT("IndentUsesTabs"),            BoolToString(m_localindentUsesTabs.GetDatum()));
	}
	if (HideChangeMarkerMarginIsValid()) {
		n->AddProperty(wxT("HideChangeMarkerMargin"),    BoolToString(m_localhideChangeMarkerMargin.GetDatum()));
	}

	if (EolModeIsValid()) {
		n->AddProperty(wxT("EOLMode"),                	 m_localeolMode.GetDatum());
	}

	wxString tmp;
	if (IndentWidthIsValid()) {
		tmp << m_localindentWidth.GetDatum();
		n->AddProperty(wxT("IndentWidth"), tmp);
	}

    tmp.clear();
	if (TabWidthIsValid()) {
		tmp << m_localtabWidth.GetDatum();
		n->AddProperty(wxT("TabWidth"), tmp);
	}

	tmp.clear();
	if (ShowWhitespacesIsValid()) {
		tmp << m_localshowWhitspaces.GetDatum();
		n->AddProperty(wxT("ShowWhitespaces"), tmp);
	}

	tmp.clear();
	if (FileFontEncodingIsValid()) {
		tmp = wxFontMapper::GetEncodingName(m_localfileFontEncoding.GetDatum());
		n->AddProperty(wxT("FileFontEncoding"), tmp);
	}

	return n;
}

void LocalOptionsConfig::SetFileFontEncoding(const wxString& strFileFontEncoding)
{
	m_localfileFontEncoding.Set(wxFontMapper::Get()->CharsetToEncoding(strFileFontEncoding, false));

	if (wxFONTENCODING_SYSTEM == m_localfileFontEncoding.GetDatum()) {
		m_localfileFontEncoding.Set(wxFONTENCODING_UTF8);
	}
}

void LocalWorkspace::GetOptions( OptionsConfigPtr options, const wxString& projectname )
{
    // First, a SanityCheck(). This protects against a change of workspace, and calls Create() if needed
    if ( !SanityCheck() ) {
        return;
    }

	// Get any workspace-wide preferences, then any project ones
    wxXmlNode* lwsnode = GetLocalWorkspaceOptionsNode();
    if (lwsnode) {
        // Any local workspace options will replace the global ones inside 'options'
        LocalOptionsConfig wsOC(options, lwsnode);
    }

    wxXmlNode* lpnode = GetLocalProjectOptionsNode(projectname);
    if (lpnode) {
        LocalOptionsConfig pOC(options, lpnode);
    }

	// This space intentionally left blank :p  Maybe, one day there'll be individual-editor options too
}

bool LocalWorkspace::SetWorkspaceOptions(LocalOptionsConfigPtr opts)
{
    // Stored as:
	//	<Workspace>
	//		<LocalWorkspaceOptions something="on" something_else="off"/>
    //	</Workspace>

	if ( !SanityCheck() ) {
        return false;
    }

	wxXmlNode* oldOptions = GetLocalWorkspaceOptionsNode();
	if (oldOptions) {
		m_doc.GetRoot()->RemoveChild(oldOptions);
		delete oldOptions;
	}
	m_doc.GetRoot()->AddChild(opts->ToXml(NULL, wxT("LocalWorkspaceOptions")));
    return SaveXmlFile();
}

bool LocalWorkspace::SetProjectOptions(LocalOptionsConfigPtr opts, const wxString& projectname)
{
    // Stored as:
	//	<Project Name="foo">
	//		<Options something="on" something_else="off"/>
    //	</Project>

	if ( !SanityCheck() ) {
        return false;
    }

	// If the project node doesn't exist, create it
	wxXmlNode* project = XmlUtils::FindNodeByName(m_doc.GetRoot(), wxT("Project"), projectname);
	if (!project) {
		project = new wxXmlNode(m_doc.GetRoot(),wxXML_ELEMENT_NODE, wxT("Project"));
		project->AddProperty(wxT("Name"), projectname);
	}

	wxXmlNode* oldOptions = XmlUtils::FindFirstByTagName(project, wxT("Options"));
	if (oldOptions) {
		project->RemoveChild(oldOptions);
		delete oldOptions;
	}
	project->AddChild(opts->ToXml(NULL, wxT("Options")));
    return SaveXmlFile();
}

bool LocalWorkspace::SaveXmlFile()
{
	return m_doc.Save(m_fileName.GetFullPath());
}

bool LocalWorkspace::SanityCheck()
{
	wxLogNull noLog;
    wxString WorkspaceFullPath = WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath();
    if (WorkspaceFullPath.IsEmpty()) {
        return false;
    }

    // Check that the current workspace is the same as any previous Create()
    // If so, and assuming m_doc is valid, there's nothing more to do
    if ( (WorkspaceFullPath == m_fileName.GetFullPath().BeforeLast(wxT('.'))) && m_doc.IsOk() ) {
        return true;
    }

    // If we're here, the class isn't correctly set up, so
    return Create();
}

bool LocalWorkspace::Create()
{
    m_doc = wxXmlDocument();
    // The idea is to make a name in the format foo.workspace.frodo
    wxString fullpath = WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath() + wxT('.') + clGetUserName();
	m_fileName = wxFileName(fullpath);
	m_fileName.MakeAbsolute();

    // Load any previous options. If none, create a blank entry
    m_doc.Load(m_fileName.GetFullPath());
	if ( !m_doc.IsOk() ) {
		wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Workspace"));
		m_doc.SetRoot(root);
	}
    return true;
}

wxXmlNode* LocalWorkspace::GetLocalWorkspaceOptionsNode() const
{
	return XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("LocalWorkspaceOptions"));
}

wxXmlNode* LocalWorkspace::GetLocalProjectOptionsNode(const wxString& projectname) const
{
	wxXmlNode* project = XmlUtils::FindNodeByName(m_doc.GetRoot(), wxT("Project"), projectname);
    return XmlUtils::FindFirstByTagName(project, wxT("Options"));
}

void LocalWorkspace::GetParserPaths(wxArrayString &inclduePaths, wxArrayString &excludePaths)
{
	if(!SanityCheck())
		return;

	wxXmlNode* workspaceInclPaths = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserPaths"));
	if( workspaceInclPaths ) {
		wxXmlNode *child = workspaceInclPaths->GetChildren();
		while(child) {
			if(child->GetName() == wxT("Exclude")) {
				wxString path = child->GetPropVal(wxT("Path"), wxT(""));
				path.Trim().Trim(false);
				if(path.IsEmpty() == false) {
					excludePaths.Add( path );
				}
			}

			else if(child->GetName() == wxT("Include")) {
				wxString path = child->GetPropVal(wxT("Path"), wxT(""));
				path.Trim().Trim(false);
				if(path.IsEmpty() == false) {
					inclduePaths.Add( path );
				}
			}

			child = child->GetNext();
		}
	}
}

void LocalWorkspace::SetParserPaths(const wxArrayString& inclduePaths, const wxArrayString& excludePaths)
{
	if(!SanityCheck())
		return;

	wxXmlNode* workspaceInclPaths = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserPaths"));
	if ( workspaceInclPaths ) {
		m_doc.GetRoot()->RemoveChild( workspaceInclPaths );
		delete workspaceInclPaths;
	}
	workspaceInclPaths = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("WorkspaceParserPaths"));
	for(size_t i=0; i<inclduePaths.GetCount(); i++) {
		wxXmlNode* child = new wxXmlNode(workspaceInclPaths, wxXML_ELEMENT_NODE, wxT("Include"));
		child->AddProperty(wxT("Path"), inclduePaths.Item(i));
	}

	for(size_t i=0; i<excludePaths.GetCount(); i++) {
		wxXmlNode* child = new wxXmlNode(workspaceInclPaths, wxXML_ELEMENT_NODE, wxT("Exclude"));
		child->AddProperty(wxT("Path"), excludePaths.Item(i));
	}
	SaveXmlFile();
}

wxString LocalWorkspace::GetActiveEnvironmentSet()
{
	if(!SanityCheck())
		return wxT("");

	wxXmlNode* envNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
	wxString setName;
	if( envNode ) {
		setName = envNode->GetPropVal(wxT("Name"), wxT(""));
	}
	return setName;
}

void LocalWorkspace::SetActiveEnvironmentSet(const wxString& setName)
{
	if(!SanityCheck())
		return;

	wxXmlNode* envNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
	if ( envNode ) {
		m_doc.GetRoot()->RemoveChild( envNode );
		delete envNode;
	}
	envNode = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("Environment"));
	envNode->AddProperty(wxT("Name"), setName);
	SaveXmlFile();
}

static LocalWorkspace* gs_LocalWorkspace = NULL;
void LocalWorkspaceST::Free()
{
	if(gs_LocalWorkspace) {
		delete gs_LocalWorkspace;
		gs_LocalWorkspace = NULL;
	}
}

LocalWorkspace* LocalWorkspaceST::Get()
{
	if(gs_LocalWorkspace == NULL) {
		gs_LocalWorkspace = new LocalWorkspace();
	}
	return gs_LocalWorkspace;
}

