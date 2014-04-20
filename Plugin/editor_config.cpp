//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editor_config.cpp
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
#include "precompiled_header.h"
#include "dirsaver.h"
#include <wx/stdpaths.h>
#include "editor_config.h"
#include <wx/xml/xml.h>
#include "xmlutils.h"
#include "dirtraverser.h"
#include <wx/ffile.h>
#include "globals.h"
#include "wx_xml_compatibility.h"
#include "event_notifier.h"
#include "cl_standard_paths.h"

//-------------------------------------------------------------------------------------------
SimpleLongValue::SimpleLongValue()
{
}

SimpleLongValue::~SimpleLongValue()
{
}

void SimpleLongValue::Serialize(Archive &arch)
{
    arch.Write(wxT("m_value"), m_value);
}

void SimpleLongValue::DeSerialize(Archive &arch)
{
    arch.Read(wxT("m_value"), m_value);
}

//-------------------------------------------------------------------------------------------
SimpleStringValue::SimpleStringValue()
{
}

SimpleStringValue::~SimpleStringValue()
{
}

void SimpleStringValue::Serialize(Archive &arch)
{
    arch.Write(wxT("m_value"), m_value);
}

void SimpleStringValue::DeSerialize(Archive &arch)
{
    arch.Read(wxT("m_value"), m_value);
}

//-------------------------------------------------------------------------------------------

EditorConfig::EditorConfig()
    : m_transcation(false)
    , m_activeThemeLexers(NULL)
{
    m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig()
{
    delete m_doc;
    std::map<wxString, LexersInfo*>::iterator iter = m_lexers.begin();
    for(; iter != m_lexers.end(); iter++) {
        delete iter->second;
    }
    m_lexers.clear();
}

bool EditorConfig::DoLoadDefaultSettings()
{
    //try to load the default settings
    m_fileName = wxFileName(m_installDir + wxT("/config/codelite.xml.default"));
    m_fileName.MakeAbsolute();

    if ( !m_fileName.FileExists() ) {
        return false;
    }

    return m_doc->Load(m_fileName.GetFullPath());
}

bool EditorConfig::Load()
{
    //first try to load the user's settings
    m_fileName = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/codelite.xml");
    wxString localFileName = m_fileName.GetFullPath();

    {
        // Make sure that the directory exists
        wxLogNull noLog;
        wxMkdir(m_fileName.GetPath());
        wxMkdir(clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("lexers"));
    }

    bool userSettingsLoaded(false);
    bool loadSuccess       (false);

    if (!m_fileName.FileExists()) {
        loadSuccess = DoLoadDefaultSettings();

        if (loadSuccess) {
            // Copy the content of the default codelite.xml file into the user's local file
            wxCopyFile(m_fileName.GetFullPath(), localFileName);
        }

    } else {
        userSettingsLoaded = true;
        loadSuccess = m_doc->Load(m_fileName.GetFullPath());
    }

    if ( !loadSuccess ) {
        return false;
    }

    // Check the codelite-version for this file
    wxString version;
    bool found = m_doc->GetRoot()->GetPropVal(wxT("Version"), &version);
    if ( userSettingsLoaded ) {
        if(!found || (found && version != this->m_version)) {
            if(DoLoadDefaultSettings() == false) {
                return false;
            }
        }
    }

    // load CodeLite lexers
    LoadLexers(false);

    // make sure that the file name is set to .xml and not .default
    m_fileName = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/codelite.xml");
    return true;
}

void EditorConfig::SaveLexers()
{
    std::map<wxString, LexersInfo*>::iterator iter = m_lexers.begin();
    for (; iter != m_lexers.end(); iter++) {
        wxFileName fn(iter->second->filename);
        wxString userLexer( clStandardPaths::Get().GetUserDataDir() +
                            wxFileName::GetPathSeparator() +
                            wxT("lexers") +
                            wxFileName::GetPathSeparator() +
                            fn.GetFullName());

        wxXmlDocument doc;
        wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Lexers"));
        node->AddProperty(wxT("Theme"), iter->second->theme);
        node->AddProperty(wxT("OutputView_Fg_Colour"), iter->second->outputpane_fg_colour);
        node->AddProperty(wxT("OutputView_Bg_Colour"), iter->second->outputpane_bg_colour);
        doc.SetRoot( node );

        std::map<wxString, LexerConfPtr>::iterator it = iter->second->lexers.begin();
        for(; it != iter->second->lexers.end(); it++) {
            node->AddChild(it->second->ToXml());
        }
        doc.Save( userLexer );
    }

    wxString nodeName = wxT("Lexers");
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(nodeName);
    EventNotifier::Get()->AddPendingEvent( evt );
}

wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexerName)
{
    wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
    if ( lexersNode ) {
        return XmlUtils::FindNodeByName(lexersNode, wxT("Lexer"), lexerName);
    }
    return NULL;
}

LexerConfPtr EditorConfig::GetLexerForFile(const wxString& filename)
{
    if ( !m_activeThemeLexers )
        return NULL;
    
    wxFileName fnFileName(filename);
    wxString fileNameLowercase = fnFileName.GetFullName();
    fileNameLowercase.MakeLower();
    
    ThemeLexersMap::iterator iter = m_activeThemeLexers->lexers.begin();
    for(; iter != m_activeThemeLexers->lexers.end(); ++iter) {
        wxString fileSpec = iter->second->GetFileSpec();
        fileSpec.MakeLower();
        
        wxArrayString specs = ::wxStringTokenize(fileSpec, ";", wxTOKEN_STRTOK);
        for(size_t i=0; i<specs.GetCount(); ++i) {
            if ( ::wxMatchWild(specs.Item(i), fileNameLowercase) ) {
                // found the lexer for this file extension
                return iter->second;
            }
        }
    }
    return GetLexer("Text");
}

LexerConfPtr EditorConfig::GetLexer(const wxString &lexerName)
{
    if ( !m_activeThemeLexers )
        return NULL;
    
    wxString lexerNameLowercase = lexerName;
    lexerNameLowercase.MakeLower();
    
    std::map<wxString, LexerConfPtr>::const_iterator iter = m_activeThemeLexers->lexers.find( lexerNameLowercase );
    if(iter == m_activeThemeLexers->lexers.end())
        return NULL;

    return iter->second;
}

wxString EditorConfig::GetCurrentOutputviewFgColour() const
{
    if (m_activeThemeLexers == NULL || m_activeThemeLexers->outputpane_fg_colour.IsEmpty()) {
        return DrawingUtils::GetTextCtrlTextColour().GetAsString(wxC2S_HTML_SYNTAX | wxC2S_CSS_SYNTAX);
    }

    return m_activeThemeLexers->outputpane_fg_colour;
}

wxString EditorConfig::GetCurrentOutputviewBgColour() const
{
    if (m_activeThemeLexers == NULL || m_activeThemeLexers->outputpane_bg_colour.IsEmpty()) {
        return DrawingUtils::GetTextCtrlBgColour().GetAsString(wxC2S_HTML_SYNTAX | wxC2S_CSS_SYNTAX);
    }

    return m_activeThemeLexers->outputpane_bg_colour;
}

EditorConfig::ConstIterator EditorConfig::LexerEnd()
{
    return m_activeThemeLexers->lexers.end();
}

EditorConfig::ConstIterator EditorConfig::LexerBegin()
{
    return m_activeThemeLexers->lexers.begin();
}

OptionsConfigPtr EditorConfig::GetOptions() const
{
    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
    // node can be null ...
    OptionsConfigPtr opts = new OptionsConfig(node);

    // import legacy tab-width setting into opts
    long tabWidth(opts->GetTabWidth());
    if (const_cast<EditorConfig*>(this)->GetLongValue(wxT("EditorTabWidth"), tabWidth)) {
        opts->SetTabWidth(tabWidth);
    }

    return opts;
}

void EditorConfig::SetOptions(OptionsConfigPtr opts)
{
    // remove legacy tab-width setting
    wxXmlNode *child = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("ArchiveObject"), wxT("EditorTabWidth"));
    if (child) {
        m_doc->GetRoot()->RemoveChild(child);
        delete child;
    }

    // locate the current node
    wxString nodeName = wxT("Options");
    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if ( node ) {
        m_doc->GetRoot()->RemoveChild(node);
        delete node;
    }
    m_doc->GetRoot()->AddChild(opts->ToXml());

    DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(nodeName);
    EventNotifier::Get()->AddPendingEvent( evt );
}

void EditorConfig::SetTagsDatabase(const wxString &dbName)
{
    wxString nodeName = wxT("TagsDatabase");
    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if ( node ) {
        XmlUtils::UpdateProperty(node, wxT("Path"), dbName);
    } else {
        //create new node
        node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
        node->AddProperty(wxT("Path"), dbName);
        m_doc->GetRoot()->AddChild(node);
    }

    DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(nodeName);
    EventNotifier::Get()->AddPendingEvent( evt );
}

wxString EditorConfig::GetTagsDatabase() const
{
    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("TagsDatabase"));
    if (node) {
        return XmlUtils::ReadString(node, wxT("Path"));
    } else {
        return wxEmptyString;
    }
}

void EditorConfig::GetRecentItems(wxArrayString &files, const wxString nodeName)
{
    if (nodeName.IsEmpty()) {
        return;
    }
    //find the root node
    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if (node) {
        wxXmlNode *child = node->GetChildren();
        while (child) {
            if (child->GetName() == wxT("File")) {
                wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
                // wxXmlDocument Saves/Loads items in reverse order, so prepend, not add.
                if( wxFileExists( fileName ) ) files.Insert(fileName, 0);
            }
            child = child->GetNext();
        }
    }
}

void EditorConfig::SetRecentItems(const wxArrayString &files, const wxString nodeName)
{
    if (nodeName.IsEmpty()) {
        return;
    }
    //find the root node
    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if (node) {
        wxXmlNode *root = m_doc->GetRoot();
        root->RemoveChild(node);
        delete node;
    }
    //create new entry in the configuration file
    node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
    m_doc->GetRoot()->AddChild(node);
    for (size_t i=0; i<files.GetCount(); i++) {
        wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
        child->AddProperty(wxT("Name"), files.Item(i));
        node->AddChild(child);
    }

    //save the data to disk
    DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(nodeName);
    EventNotifier::Get()->AddPendingEvent( evt );
}

bool EditorConfig::WriteObject(const wxString &name, SerializedObject *obj)
{
    if(!XmlUtils::StaticWriteObject(m_doc->GetRoot(), name, obj))
        return false;

    //save the archive
    bool res = DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(name);
    EventNotifier::Get()->AddPendingEvent( evt );
    return res;
}

bool EditorConfig::ReadObject(const wxString &name, SerializedObject *obj)
{
    //find the object node in the xml file
    return XmlUtils::StaticReadObject(m_doc->GetRoot(), name, obj);
}

wxString EditorConfig::GetRevision() const
{
    return XmlUtils::ReadString(m_doc->GetRoot(), wxT("Revision"), wxEmptyString);
}

void EditorConfig::SetRevision(const wxString &rev)
{
    wxXmlNode *root = m_doc->GetRoot();
    if (!root) {
        return;
    }

    XmlUtils::UpdateProperty(root, wxT("Revision"), rev);
    DoSave();
}


void EditorConfig::SaveLongValue(const wxString &name, long value)
{
    SimpleLongValue data;
    data.SetValue(value);
    WriteObject(name, &data);
}


bool EditorConfig::GetLongValue(const wxString &name, long &value)
{
    SimpleLongValue data;
    if (ReadObject(name, &data)) {
        value = data.GetValue();
        return true;
    }
    return false;
}

wxString EditorConfig::GetStringValue(const wxString &key)
{
    SimpleStringValue data;
    ReadObject(key, &data);
    return data.GetValue();
}

void EditorConfig::SaveStringValue(const wxString &key, const wxString &value)
{
    SimpleStringValue data;
    data.SetValue(value);
    WriteObject(key, &data);
}

void EditorConfig::UpgradeUserLexer(const wxString& userLexer, const wxString& installedLexer)
{
    wxXmlDocument userdoc(userLexer), srcdoc(installedLexer);
    if(userdoc.IsOk() && srcdoc.IsOk()) {
        wxXmlNode* userroot = userdoc.GetRoot();
        wxXmlNode* srcroot = srcdoc.GetRoot();
        if (userroot && srcroot) {
            bool dirty(false);

            // Iterate through the lexers, looking for novelties
            wxXmlNode* child = srcroot->GetChildren();
            while (child) {
                wxString name = child->GetAttribute("Name", "");
                wxXmlNode* usernode = XmlUtils::FindNodeByName(userroot, "Lexer", name);
                if (!usernode) {
                    usernode = XmlUtils::FindNodeByName(userroot, "Lexer", name.Lower());
                    if (usernode) {
                        // Lexers used to have lower-case names. Upgrade the spelling for compatibility; it shouldn't break anything
                        wxXmlAttribute* attrib = usernode->GetAttributes();
                        while (attrib) {
                            if (attrib->GetName() == "Name") {
                                attrib->SetValue(name);
                                dirty = true;
                                break;
                            }
                            attrib = attrib->GetNext();
                        }
                    }
                }

                if (!usernode) {
                        // Found a missing lexer, so copy it across
                        userroot->AddChild(new wxXmlNode(*child));
                        dirty = true;
                } else {
                    // This lexer is already present, but check for any missing attributes
                    wxXmlNode* properties = XmlUtils::FindFirstByTagName(child, "Properties");
                    wxXmlNode* userproperties = XmlUtils::FindFirstByTagName(usernode, "Properties");
                    if (properties && userproperties) {
                        wxXmlNode* property = properties->GetChildren();
                        while (property) {
                             if (!XmlUtils::FindNodeByName(userproperties, "Property", property->GetAttribute("Name", ""))) {
                                // Found a missing attribute
                                userproperties->AddChild(new wxXmlNode(*property));
                                dirty = true;
                             }
                            property = property->GetNext();
                        }
                    }
                }
                child = child->GetNext();
            }

            if (dirty) {
                userdoc.Save(userLexer);
            }
        }
    }
}

void EditorConfig::LoadLexers(bool loadDefault)
{
    m_lexers.clear();

    wxString theme = GetStringValue(wxT("LexerTheme"));
    if (theme.IsEmpty()) {
        theme = wxT("Default");
        SaveStringValue(wxT("LexerTheme"), wxT("Default"));
    }

    //when this function is called, the working directory is located at the
    //startup directory
    DirSaver ds;
    wxSetWorkingDirectory(wxStandardPaths::Get().GetDataDir() + wxFileName::GetPathSeparator() + wxT("lexers"));

    wxString cwd = ::wxGetCwd();

    // load all lexer configuration files
    wxArrayString files;
    wxDir::GetAllFiles(cwd, &files,wxT("*.xml"), wxDIR_FILES);
    wxString path_( cwd + wxFileName::GetPathSeparator() );

    LexersInfo *pLexersInfo = NULL;
    m_lexers.clear();

    for (size_t i=0; i<files.GetCount(); i++) {
        wxString fileToLoad( files.Item(i) );

        //try to locate a file with the same name but with the user extension
        wxFileName fn(files.Item(i));

        wxString userLexer( clStandardPaths::Get().GetUserDataDir() +
                            wxFileName::GetPathSeparator() +
                            wxT("lexers") +
                            wxFileName::GetPathSeparator() +
                            fn.GetFullName());

        if ( wxFileName::FileExists( userLexer ) ) {
            if ( !loadDefault ) {
                // First try to merge any new lexers or new properties into the user's one, without overwriting any user-set preferences
                UpgradeUserLexer(userLexer, fileToLoad);
                fileToLoad = userLexer;

            } else {
                ::wxRemoveFile( userLexer );
            }
        }


        // Load the lexers
        wxXmlDocument doc(fileToLoad);
        if(doc.IsOk()) {
            // we found our lexers XML file
            wxXmlNode* root = doc.GetRoot();
            if(root) {
                wxString themeName    = root->GetPropVal(wxT("Theme"), wxT("Default"));
                wxString ovFgColour   = root->GetPropVal(wxT("OutputView_Fg_Colour"), wxT(""));
                wxString ovBgColour   = root->GetPropVal(wxT("OutputView_Bg_Colour"), wxT(""));
                pLexersInfo           = new LexersInfo;
                pLexersInfo->filename = fileToLoad;
                pLexersInfo->theme    = themeName;
                if (ovFgColour.IsEmpty()) {
                    pLexersInfo->outputpane_fg_colour = DrawingUtils::GetOutputPaneFgColour().GetAsString(wxC2S_HTML_SYNTAX | wxC2S_CSS_SYNTAX);
                } else {
                    pLexersInfo->outputpane_fg_colour = ovFgColour;
                }
                if (ovBgColour.IsEmpty()) {
                    pLexersInfo->outputpane_bg_colour = DrawingUtils::GetOutputPaneBgColour().GetAsString(wxC2S_HTML_SYNTAX | wxC2S_CSS_SYNTAX);
                } else {
                    pLexersInfo->outputpane_bg_colour = ovBgColour;
                }

                wxXmlNode *child = root->GetChildren();
                while(child) {
                    LexerConfPtr lexer(new LexerConf());
                    lexer->FromXml( child );

                    if(!lexer->GetName().IsEmpty()) {
                        pLexersInfo->lexers[lexer->GetName()] = lexer;
                    }
                    child = child->GetNext();
                }
                m_lexers[themeName] = pLexersInfo;
            }
        }
    }

    // Set the selected theme
    if(m_lexers.find(theme) == m_lexers.end()) {
        // the requested theme does not exist, we use the first that we can find
        if(m_lexers.empty()) {
            // Create an empty theme lexers map
            m_activeThemeLexers = new LexersInfo;
            return;
        }
        m_activeThemeLexers = m_lexers.begin()->second;

    } else {
        m_activeThemeLexers = m_lexers.find(theme)->second;

    }
}

void EditorConfig::Begin()
{
    m_transcation = true;
}

void EditorConfig::Save()
{
    m_transcation = false;
    DoSave();
}

bool EditorConfig::DoSave() const
{
    if (m_transcation) {
        return true;
    }
    return m_doc->Save(m_fileName.GetFullPath());
}

//--------------------------------------------------
// Simple rectangle class wrapper
//--------------------------------------------------
SimpleRectValue::SimpleRectValue()
{
}

SimpleRectValue::~SimpleRectValue()
{
}

void SimpleRectValue::DeSerialize(Archive& arch)
{
    wxPoint pos;
    wxSize size;

    arch.Read(wxT("TopLeft"), pos);
    arch.Read(wxT("Size"), size);

    m_rect = wxRect(pos, size);
}

void SimpleRectValue::Serialize(Archive& arch)
{
    arch.Write(wxT("TopLeft"), m_rect.GetTopLeft());
    arch.Write(wxT("Size"), m_rect.GetSize());
}

void EditorConfig::SetInstallDir(const wxString& instlDir)
{
    m_installDir = instlDir;
}

wxArrayString EditorConfig::GetLexersThemes()
{
    wxArrayString themes;
    std::map<wxString, LexersInfo*>::iterator iter = m_lexers.begin();
    for(; iter != m_lexers.end(); iter++) {
        themes.Add( iter->first );
    }
    return themes;
}

bool EditorConfig::GetPaneStickiness(const wxString& caption)
{
    if (caption == _("Build")) {
        return GetOptions()->GetHideOutputPaneNotIfBuild();
    } else if (caption == _("Search")) {
        return GetOptions()->GetHideOutputPaneNotIfSearch();
    } else if (caption == _("Replace")) {
        return GetOptions()->GetHideOutputPaneNotIfReplace();
    } else if (caption == _("References")) {
        return GetOptions()->GetHideOutputPaneNotIfReferences();
    } else if (caption == _("Output")) {
        return GetOptions()->GetHideOutputPaneNotIfOutput();
    } else if (caption == _("Debug")) {
        return GetOptions()->GetHideOutputPaneNotIfDebug();
    } else if (caption == _("Trace")) {
        return GetOptions()->GetHideOutputPaneNotIfTrace();
    } else if (caption == _("Tasks")) {
        return GetOptions()->GetHideOutputPaneNotIfTasks();
    } else if (caption == _("BuildQ")) {
        return GetOptions()->GetHideOutputPaneNotIfBuildQ();
    } else if (caption == _("CppCheck")) {
        return GetOptions()->GetHideOutputPaneNotIfCppCheck();
    } else if (caption == _("Subversion")) {
        return GetOptions()->GetHideOutputPaneNotIfSvn();
    } else if (caption == _("CScope")) {
        return GetOptions()->GetHideOutputPaneNotIfCscope();
    } else if (caption == _("git")) {
        return GetOptions()->GetHideOutputPaneNotIfGit();
    }

    // How did we get here?
    return false;
}

void EditorConfig::SetPaneStickiness(const wxString& caption, bool stickiness)
{
    OptionsConfigPtr options = GetOptions();
    if (caption == _("Build")) {
        options->SetHideOutputPaneNotIfBuild(stickiness);
    } else if (caption == _("Search")) {
        options->SetHideOutputPaneNotIfSearch(stickiness);
    } else if (caption == _("Replace")) {
        options->SetHideOutputPaneNotIfReplace(stickiness);
    } else if (caption == _("References")) {
        options->SetHideOutputPaneNotIfReferences(stickiness);
    } else if (caption == _("Output")) {
        options->SetHideOutputPaneNotIfOutput(stickiness);
    } else if (caption == _("Debug")) {
        options->SetHideOutputPaneNotIfDebug(stickiness);
    } else if (caption == _("Trace")) {
        options->SetHideOutputPaneNotIfTrace(stickiness);
    } else if (caption == _("Tasks")) {
        options->SetHideOutputPaneNotIfTasks(stickiness);
    } else if (caption == _("BuildQ")) {
        options->SetHideOutputPaneNotIfBuildQ(stickiness);
    } else if (caption == _("CppCheck")) {
        options->SetHideOutputPaneNotIfCppCheck(stickiness);
    } else if (caption == _("Subversion")) {
        options->SetHideOutputPaneNotIfSvn(stickiness);
    } else if (caption == _("Cscope")) {
        options->SetHideOutputPaneNotIfCscope(stickiness);
    } else if (caption == _("git")) {
        options->SetHideOutputPaneNotIfGit(stickiness);
    } else {
        return;
    }

    SetOptions(options);
    Save();
}

static EditorConfig* gs_EditorConfig = NULL;
void EditorConfigST::Free()
{
    if(gs_EditorConfig) {
        delete gs_EditorConfig;
        gs_EditorConfig = NULL;
    }
}

EditorConfig* EditorConfigST::Get()
{
    if(gs_EditorConfig == NULL) {
        gs_EditorConfig = new EditorConfig;
    }
    return gs_EditorConfig;
}
