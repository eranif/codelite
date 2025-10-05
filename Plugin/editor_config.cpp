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
#include "editor_config.h"

#include "ColoursAndFontsManager.h"
#include "cl_standard_paths.h"
#include "dirsaver.h"
#include "dirtraverser.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "precompiled_header.h"
#include "workspace.h"
#include "xmlutils.h"

#include <wx/ffile.h>
#include <wx/stdpaths.h>
#include <wx/xml/xml.h>

//-------------------------------------------------------------------------------------------

void SimpleLongValue::Serialize(Archive& arch) { arch.Write(wxT("m_value"), m_value); }

void SimpleLongValue::DeSerialize(Archive& arch) { arch.Read(wxT("m_value"), m_value); }

//-------------------------------------------------------------------------------------------

void SimpleStringValue::Serialize(Archive& arch) { arch.Write(wxT("m_value"), m_value); }

void SimpleStringValue::DeSerialize(Archive& arch) { arch.Read(wxT("m_value"), m_value); }

//-------------------------------------------------------------------------------------------

EditorConfig::EditorConfig()
    : m_transcation(false)
{
    m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig() { wxDELETE(m_doc); }

bool EditorConfig::DoLoadDefaultSettings()
{
    // try to load the default settings
    m_fileName = wxFileName(clStandardPaths::Get().GetDataDir(), "codelite.xml.default");
    m_fileName.AppendDir("config");
    clDEBUG() << "Loading default config:" << m_fileName << endl;
    if(!m_fileName.FileExists()) {
        return false;
    }

    return m_doc->Load(m_fileName.GetFullPath());
}

bool EditorConfig::Load()
{
    m_cacheLongValues.clear();
    m_cacheStringValues.clear();

    // first try to load the user's settings
    m_fileName = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/codelite.xml");
    wxString localFileName = m_fileName.GetFullPath();

    {
        // Make sure that the directory exists
        wxLogNull noLog;
        wxMkdir(m_fileName.GetPath());
        wxMkdir(clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("lexers"));
    }

    bool userSettingsLoaded(false);
    bool loadSuccess(false);

    if(!m_fileName.FileExists()) {
        clDEBUG() << "User configuration file:" << m_fileName << "does not exist. Loading defaults" << endl;
        loadSuccess = DoLoadDefaultSettings();

        if(loadSuccess) {
            // Copy the content of the default codelite.xml file into the user's local file
            wxCopyFile(m_fileName.GetFullPath(), localFileName);
        }

    } else {
        userSettingsLoaded = true;
        clDEBUG() << "Found user configuration file:" << m_fileName << endl;
        loadSuccess = m_doc->Load(m_fileName.GetFullPath());
    }

    if(!loadSuccess) {
        return false;
    }

    // Check the codelite-version for this file
    wxString version;
    bool found = m_doc->GetRoot()->GetAttribute(wxT("Version"), &version);
    if(userSettingsLoaded) {
        if(!found || (found && version != this->m_version)) {
            if(DoLoadDefaultSettings() == false) {
                return false;
            }
        }
    }

    // load CodeLite lexers
    // LoadLexers(false);

    // make sure that the file name is set to .xml and not .default
    m_fileName = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/codelite.xml");
    return true;
}

LexerConf::Ptr_t EditorConfig::GetLexerForFile(const wxString& filename)
{
    return ColoursAndFontsManager::Get().GetLexerForFile(filename);
}

LexerConf::Ptr_t EditorConfig::GetLexer(const wxString& lexerName)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer(lexerName);
    return lexer;
}

OptionsConfigPtr EditorConfig::GetOptions() const
{
    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
    // node can be null ...
    OptionsConfigPtr opts = std::make_shared<OptionsConfig>(node);

    // import legacy tab-width setting into opts
    long tabWidth = const_cast<EditorConfig*>(this)->GetInteger(wxT("EditorTabWidth"), -1);
    if(tabWidth != -1) {
        opts->SetTabWidth(tabWidth);
    }

    OptionsConfigPtr confOptions(opts);
    // Now let any local preferences overwrite the global equivalent
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        clCxxWorkspaceST::Get()->GetLocalWorkspace()->GetOptions(confOptions, wxEmptyString);
    }
    return confOptions;
}

void EditorConfig::SetOptions(OptionsConfigPtr opts)
{
    // remove legacy tab-width setting
    wxXmlNode* child = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("ArchiveObject"), wxT("EditorTabWidth"));
    if(child) {
        m_doc->GetRoot()->RemoveChild(child);
        delete child;
    }

    // locate the current node
    wxString nodeName = wxT("Options");
    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if(node) {
        m_doc->GetRoot()->RemoveChild(node);
        delete node;
    }
    m_doc->GetRoot()->AddChild(opts->ToXml());

    DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(nodeName);
    EventNotifier::Get()->AddPendingEvent(evt);
}

int clSortStringsFunc(const wxString& first, const wxString& second)
{
    wxFileName f1(first);
    wxFileName f2(second);
    return f2.GetFullName().CmpNoCase(f1.GetFullName());
}

void EditorConfig::GetRecentItems(wxArrayString& files, const wxString& nodeName)
{
    if(nodeName.IsEmpty()) {
        return;
    }

    if(m_cacheRecentItems.count(nodeName)) {
        files = m_cacheRecentItems.find(nodeName)->second;
        // files.Sort(clSortStringsFunc);
        return;
    }

    // find the root node
    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if(node) {
        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == wxT("File")) {
                wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
                // wxXmlDocument Saves/Loads items in reverse order, so prepend, not add.
                if(wxFileExists(fileName))
                    files.Insert(fileName, 0);
            }
            child = child->GetNext();
        }
    }
    // files.Sort(clSortStringsFunc);
}

void EditorConfig::SetRecentItems(const wxArrayString& files, const wxString& nodeName)
{
    if(nodeName.IsEmpty()) {
        return;
    }

    // find the root node
    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
    if(node) {
        wxXmlNode* root = m_doc->GetRoot();
        root->RemoveChild(node);
        delete node;
    }
    // create new entry in the configuration file
    node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
    m_doc->GetRoot()->AddChild(node);
    for(size_t i = 0; i < files.GetCount(); i++) {
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
        child->AddAttribute(wxT("Name"), files.Item(i));
        node->AddChild(child);
    }

    // Update the cache
    if(m_cacheRecentItems.count(nodeName)) {
        m_cacheRecentItems.erase(nodeName);
    }
    m_cacheRecentItems.insert(std::make_pair(nodeName, files));

    // save the data to disk
    DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(nodeName);
    EventNotifier::Get()->AddPendingEvent(evt);
}

bool EditorConfig::WriteObject(const wxString& name, SerializedObject* obj)
{
    if(!XmlUtils::StaticWriteObject(m_doc->GetRoot(), name, obj))
        return false;

    // save the archive
    bool res = DoSave();
    wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
    evt.SetString(name);
    EventNotifier::Get()->AddPendingEvent(evt);
    return res;
}

bool EditorConfig::ReadObject(const wxString& name, SerializedObject* obj)
{
    // find the object node in the xml file
    return XmlUtils::StaticReadObject(m_doc->GetRoot(), name, obj);
}

void EditorConfig::SetInteger(const wxString& name, long value)
{
    SimpleLongValue data;
    data.SetValue(value);
    WriteObject(name, &data);
    m_cacheLongValues[name] = value;
}

long EditorConfig::GetInteger(const wxString& name, long defaultValue)
{
    // Check the cache first
    std::map<wxString, long>::iterator iter = m_cacheLongValues.find(name);
    if(iter != m_cacheLongValues.end())
        return iter->second;

    SimpleLongValue data;
    if(!ReadObject(name, &data)) {
        return defaultValue;
    }

    // update the cache
    m_cacheLongValues[name] = data.GetValue();
    return data.GetValue();
}

wxString EditorConfig::GetString(const wxString& key, const wxString& defaultValue)
{
    // Check the cache first
    std::map<wxString, wxString>::iterator iter = m_cacheStringValues.find(key);
    if(iter != m_cacheStringValues.end())
        return iter->second;

    SimpleStringValue data;
    if(!ReadObject(key, &data)) {
        return defaultValue;
    }

    m_cacheStringValues[key] = data.GetValue();
    return data.GetValue();
}

void EditorConfig::SetString(const wxString& key, const wxString& value)
{
    SimpleStringValue data;
    data.SetValue(value);
    WriteObject(key, &data);
    // update the cache
    m_cacheStringValues[key] = value;
}

void EditorConfig::Begin() { m_transcation = true; }

void EditorConfig::Save()
{
    m_transcation = false;
    DoSave();
}

bool EditorConfig::DoSave() const
{
    if(m_transcation) {
        return true;
    }

    // Notify that the editor configuration was modified
    wxCommandEvent event(wxEVT_EDITOR_CONFIG_CHANGED);
    EventNotifier::Get()->AddPendingEvent(event);
    return ::SaveXmlToFile(m_doc, m_fileName.GetFullPath());
}

//--------------------------------------------------
// Simple rectangle class wrapper
//--------------------------------------------------

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

void EditorConfig::SetInstallDir(const wxString& instlDir) { m_installDir = instlDir; }

bool EditorConfig::GetPaneStickiness(const wxString& caption)
{
    if(caption == _("Build")) {
        return GetOptions()->GetHideOutputPaneNotIfBuild();

    } else if(caption == _("Search")) {
        return GetOptions()->GetHideOutputPaneNotIfSearch();
    } else if(caption == _("Replace")) {
        return GetOptions()->GetHideOutputPaneNotIfReplace();
    } else if(caption == _("References")) {
        return GetOptions()->GetHideOutputPaneNotIfReferences();
    } else if(caption == _("Output")) {
        return GetOptions()->GetHideOutputPaneNotIfOutput();
    } else if(caption == _("Debug")) {
        return GetOptions()->GetHideOutputPaneNotIfDebug();
    } else if(caption == _("Trace")) {
        return GetOptions()->GetHideOutputPaneNotIfTrace();
    } else if(caption == _("Tasks")) {
        return GetOptions()->GetHideOutputPaneNotIfTasks();
    } else if(caption == _("BuildQ")) {
        return GetOptions()->GetHideOutputPaneNotIfBuildQ();
    } else if(caption == _("CppCheck")) {
        return GetOptions()->GetHideOutputPaneNotIfCppCheck();
    } else if(caption == _("Subversion")) {
        return GetOptions()->GetHideOutputPaneNotIfSvn();
    } else if(caption == _("CScope")) {
        return GetOptions()->GetHideOutputPaneNotIfCscope();
    } else if(caption == _("Git")) {
        return GetOptions()->GetHideOutputPaneNotIfGit();
    } else if(caption == _("MemCheck")) {
        return GetOptions()->GetHideOutputPaneNotIfMemCheck();
    }

    // How did we get here?
    return false;
}

void EditorConfig::SetPaneStickiness(const wxString& caption, bool stickiness)
{
    OptionsConfigPtr options = GetOptions();
    if(caption == _("Build")) {
        options->SetHideOutputPaneNotIfBuild(stickiness);
    } else if(caption == _("Search")) {
        options->SetHideOutputPaneNotIfSearch(stickiness);
    } else if(caption == _("Replace")) {
        options->SetHideOutputPaneNotIfReplace(stickiness);
    } else if(caption == _("References")) {
        options->SetHideOutputPaneNotIfReferences(stickiness);
    } else if(caption == _("Output")) {
        options->SetHideOutputPaneNotIfOutput(stickiness);
    } else if(caption == _("Debug")) {
        options->SetHideOutputPaneNotIfDebug(stickiness);
    } else if(caption == _("Trace")) {
        options->SetHideOutputPaneNotIfTrace(stickiness);
    } else if(caption == _("Tasks")) {
        options->SetHideOutputPaneNotIfTasks(stickiness);
    } else if(caption == _("BuildQ")) {
        options->SetHideOutputPaneNotIfBuildQ(stickiness);
    } else if(caption == _("CppCheck")) {
        options->SetHideOutputPaneNotIfCppCheck(stickiness);
    } else if(caption == _("Subversion")) {
        options->SetHideOutputPaneNotIfSvn(stickiness);
    } else if(caption == _("Cscope")) {
        options->SetHideOutputPaneNotIfCscope(stickiness);
    } else if(caption == _("Git")) {
        options->SetHideOutputPaneNotIfGit(stickiness);
    } else if(caption == _("MemCheck")) {
        options->SetHideOutputPaneNotIfMemCheck(stickiness);
    } else {
        return;
    }

    SetOptions(options);
    Save();
}

// Singleton
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
