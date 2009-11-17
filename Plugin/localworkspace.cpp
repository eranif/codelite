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
#include "optionsconfig.h"
#include <wx/fontmap.h>
#include <wx/utils.h>
#include "xmlutils.h"
#include "macros.h"

#include "workspace.h"
#include "project.h"
#include "localworkspace.h"

void LocalWorkspace::GetOptions( OptionsConfigPtr options, const wxString& projectname )
{
    // First, a SanityCheck(). This protects against a change of workspace, or none, and calls Create() if needed
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

/*bool LocalWorkspace::SetWorkspaceOptions()
{
    // First, a SanityCheck(). This protects against a change of workspace, or none, and calls Create() if needed
    if ( !SanityCheck() ) {
        return false;
    }
	
    // Start by getting the global settings
	OptionsConfigPtr higherOptions = EditorConfigST::Get()->GetOptions();

	wxXmlNode* lwsnode = GetLocalWorkspaceOptionsNode();
    // Don't check lwsnode: it'll be NULL if there are currently no local options
    
    EditorSettingsLocal dlg(higherOptions, lwsnode, pLevel_workspace, NULL);
    if (dlg.ShowModal() == wxID_OK) {
        SaveWorkspaceOptions(dlg.GetLocalOpts());
        return true;
    }

	return false;
}

bool LocalWorkspace::SetProjectOptions( const wxString& projectname )
{
    // First, a SanityCheck(). This protects against a change of workspace, or none, and calls Create() if needed
    if ( !SanityCheck() ) {
        return false;
    }
    
	// Start by getting the global settings
	OptionsConfigPtr higherOptions = EditorConfigST::Get()->GetOptions();

	wxXmlNode* lwsnode = GetLocalWorkspaceOptionsNode();
	wxXmlNode* lpnode = GetLocalProjectOptionsNode(projectname);
    // Don't check the nodes: they'll be NULL if there are currently no local options
    
    // Merge any local workspace options with the global ones inside 'options'
    LocalOptionsConfig wsOC(higherOptions, lwsnode);

    EditorSettingsLocal dlg(higherOptions, lpnode, pLevel_project, NULL);
    if (dlg.ShowModal() == wxID_OK) {
        SaveProjectOptions(dlg.GetLocalOpts(), projectname);
        return true;
    }

    return false;
}
*/

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

wxString LocalWorkspace::GetUserName()
{
    wxString squashedname, name = wxGetUserName();
    
    // The wx doc says that 'name' may now be e.g. "Mr. John Smith"
    // So try to make it more suitable to be an extension
    name.MakeLower();
	name.Replace(wxT(" "), wxT("_"));
	for (size_t i=0; i<name.Len(); ++i) {
		wxChar ch = name.GetChar(i);
		if( (ch < wxT('a') || ch > wxT('z')) && ch != wxT('_')){
			// Non [a-z_] character: skip it
		} else {
			squashedname << ch;
		}
	}

	return (squashedname.IsEmpty() ? wxString(wxT("someone")) : squashedname);
}

bool LocalWorkspace::SanityCheck()
{    
/*    // First check there's a workspace open. If not, abort
    if (!ManagerST::Get()->IsWorkspaceOpen()) {
        return false;
    }*/
    wxString WorkspaceFullPath = WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath();
    if (WorkspaceFullPath.IsEmpty()) {
        return false;
    }
    
    // Next, that it's the same workspace as any previous Create()
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
    wxString fullpath = WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath() + wxT('.') + GetUserName();
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

