//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_settings_config.cpp
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

#include "build_settings_config.h"
#include "conffilelocator.h"
#include "xmlutils.h"
#include <wx/ffile.h>
#include "wx_xml_compatibility.h"
#include "macros.h"
#include <event_notifier.h>
#include <cl_command_event.h>
#include "codelite_events.h"
#include <wx/sstream.h>
#include <globals.h>
#include "ICompilerLocator.h"
#include <algorithm>
#include "JSON.h"

BuildSettingsConfig::BuildSettingsConfig()
{
    m_doc = new wxXmlDocument();
    m_compilers.clear();
}

BuildSettingsConfig::~BuildSettingsConfig()
{
    wxDELETE(m_doc);
    m_compilers.clear();
}

bool BuildSettingsConfig::Load(const wxString& version, const wxString& xmlFilePath)
{
    bool loaded = false;
    m_version = version;
    if(xmlFilePath.IsEmpty()) {
        wxString initialSettings = ConfFileLocator::Instance()->Locate(wxT("config/build_settings.xml"));
        loaded = m_doc->Load(initialSettings);
        CHECK_PTR_RET_FALSE(m_doc->GetRoot());

        wxString xmlVersion = m_doc->GetRoot()->GetPropVal(wxT("Version"), wxEmptyString);
        if(xmlVersion != version) {
            loaded = m_doc->Load(ConfFileLocator::Instance()->GetDefaultCopy(wxT("config/build_settings.xml")));
        }
        m_fileName = ConfFileLocator::Instance()->GetLocalCopy(wxT("config/build_settings.xml"));

        if(loaded) { DoUpdateCompilers(); }
    } else {
        wxFileName xmlPath(xmlFilePath);
        loaded = m_doc->Load(xmlPath.GetFullPath());
        if(loaded) {
            DoUpdateCompilers();
            m_fileName = xmlPath;
        }
    }
    if(loaded) { SaveXmlFile(); }
    return loaded;
}

wxXmlNode* BuildSettingsConfig::GetCompilerNode(const wxString& name) const
{
    wxXmlNode* cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
    if(cmpsNode) {
        if(name.IsEmpty()) {
            // return the first compiler
            return XmlUtils::FindFirstByTagName(cmpsNode, wxT("Compiler"));
        } else {
            return XmlUtils::FindNodeByName(cmpsNode, wxT("Compiler"), name);
        }
    }
    return NULL;
}

void BuildSettingsConfig::SetCompiler(CompilerPtr cmp)
{
    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
    if(node) {
        wxXmlNode* oldCmp = NULL;
        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Compiler") && XmlUtils::ReadString(child, wxT("Name")) == cmp->GetName()) {
                oldCmp = child;
                break;
            }
            child = child->GetNext();
        }
        if(oldCmp) {
            node->RemoveChild(oldCmp);
            delete oldCmp;
        }
        node->AddChild(cmp->ToXml());

    } else {
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compilers"));
        m_doc->GetRoot()->AddChild(node);
        node->AddChild(cmp->ToXml());
    }

    SaveXmlFile();
    DoUpdateCompilers();
}

CompilerPtr BuildSettingsConfig::GetCompiler(const wxString& name) const
{
    if(!m_compilers.count(name)) {

        // no such compiler...
        return new Compiler(NULL);

    } else {

        return m_compilers.find(name)->second;
    }
}

CompilerPtr BuildSettingsConfig::GetFirstCompiler(BuildSettingsConfigCookie& cookie)
{
    wxXmlNode* cmps = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
    if(cmps) {
        cookie.parent = cmps;
        cookie.child = NULL;
        return GetNextCompiler(cookie);
    }
    return NULL;
}

CompilerPtr BuildSettingsConfig::GetNextCompiler(BuildSettingsConfigCookie& cookie)
{
    if(cookie.parent == NULL) { return NULL; }

    if(cookie.child == NULL) { cookie.child = cookie.parent->GetChildren(); }

    while(cookie.child) {
        if(cookie.child->GetName() == wxT("Compiler")) {
            wxXmlNode* n = cookie.child;
            // advance the child to the next child and bail out
            cookie.child = cookie.child->GetNext();

            // incase we dont have more childs to iterate
            // reset the parent as well so the next call to GetNexeLexer() will fail
            if(cookie.child == NULL) { cookie.parent = NULL; }
            return new Compiler(n);
        }
        cookie.child = cookie.child->GetNext();
    }
    return NULL;
}

bool BuildSettingsConfig::IsCompilerExist(const wxString& name) const { return m_compilers.count(name); }

void BuildSettingsConfig::DeleteCompiler(const wxString& name)
{
    wxXmlNode* node = GetCompilerNode(name);
    if(node) {
        node->GetParent()->RemoveChild(node);
        delete node;
        SaveXmlFile();
        DoUpdateCompilers();
    }
}

void BuildSettingsConfig::SetBuildSystem(BuilderConfigPtr bs)
{
    // find the old setting
    wxXmlNode* node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("BuildSystem"), bs->GetName());
    if(node) {
        node->GetParent()->RemoveChild(node);
        delete node;
    }
    m_doc->GetRoot()->AddChild(bs->ToXml());
    SaveXmlFile();
    DoUpdateCompilers();
}

BuilderConfigPtr BuildSettingsConfig::GetBuilderConfig(const wxString& name)
{
    wxXmlNode* node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("BuildSystem"),
                                               name.IsEmpty() ? GetSelectedBuildSystem() : name);
    if(node) { return new BuilderConfig(node); }
    return NULL;
}

void BuildSettingsConfig::SaveBuilderConfig(BuilderPtr builder)
{
    // update configuration file
    BuilderConfigPtr bsptr(new BuilderConfig(NULL));
    bsptr->SetName(builder->GetName());
    bsptr->SetIsActive(builder->IsActive());
    SetBuildSystem(bsptr);
}

wxString BuildSettingsConfig::GetSelectedBuildSystem()
{
    wxString active("Default");

    wxXmlNode* node = m_doc->GetRoot()->GetChildren();
    while(node) {
        if(node->GetName() == wxT("BuildSystem")) {
            if(node->GetPropVal(wxT("Active"), wxT("")) == wxT("yes")) {
                active = node->GetPropVal(wxT("Name"), wxT(""));
                break;
            }
        }
        node = node->GetNext();
    }

    return active;
}

void BuildSettingsConfig::RestoreDefaults()
{
    // Delete the local copy of the build settings
    ConfFileLocator::Instance()->DeleteLocalCopy(wxT("config/build_settings.xml"));

    // free the XML dodcument loaded into the memory and allocate new one
    wxDELETE(m_doc);
    m_doc = new wxXmlDocument();

    // call Load again, this time the default settings will be loaded
    // since we just deleted the local settings
    Load(m_version);

    clCommandEvent event(wxEVT_COMPILER_LIST_UPDATED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void BuildSettingsConfig::DeleteAllCompilers(bool notify)
{
    // Delete all compilers
    wxXmlNode* node = GetCompilerNode("");
    while(node) {
        node->GetParent()->RemoveChild(node);
        wxDELETE(node);
        node = GetCompilerNode("");
    }
    SaveXmlFile();
    m_compilers.clear();

    if(notify) {
        clCommandEvent event(wxEVT_COMPILER_LIST_UPDATED);
        EventNotifier::Get()->AddPendingEvent(event);
    }
}

void BuildSettingsConfig::SetCompilers(const std::vector<CompilerPtr>& compilers)
{
    DeleteAllCompilers(false);

    wxXmlNode* cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
    if(cmpsNode) {
        for(size_t i = 0; i < compilers.size(); ++i) {
            cmpsNode->AddChild(compilers.at(i)->ToXml());
        }
    }
    SaveXmlFile();
    DoUpdateCompilers();

    clCommandEvent event(wxEVT_COMPILER_LIST_UPDATED);
    EventNotifier::Get()->AddPendingEvent(event);
}

wxArrayString BuildSettingsConfig::GetAllCompilersNames() const
{
    wxArrayString allCompilers;
    wxXmlNode* compilersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), "Compilers");
    if(compilersNode) {
        wxXmlNode* child = compilersNode->GetChildren();
        while(child) {
            if(child->GetName() == "Compiler") { allCompilers.Add(XmlUtils::ReadString(child, "Name")); }
            child = child->GetNext();
        }
    }
    return allCompilers;
}

void BuildSettingsConfig::DoUpdateCompilers()
{
    m_compilers.clear();
    wxArrayString compilers = GetAllCompilersNames();
    for(size_t i = 0; i < compilers.GetCount(); ++i) {
        CompilerPtr pCompiler(new Compiler(GetCompilerNode(compilers.Item(i))));
        m_compilers.insert(std::make_pair(compilers.Item(i), pCompiler));
    }
}

bool BuildSettingsConfig::SaveXmlFile()
{
    // Store this information for later retrieval
    wxArrayString compilers = GetAllCompilersNames();
    JSON root(cJSON_Array);
    JSONItem e = root.toElement();
    for(size_t i = 0; i < compilers.size(); ++i) {
        CompilerPtr cmp = GetCompiler(compilers[i]);
        if(!cmp) { continue; }
        JSONItem o = JSONItem::createObject();
        o.addProperty("name", cmp->GetName()).addProperty("paths", cmp->GetDefaultIncludePaths());
        e.arrayAppend(o);
    }
    wxFileName compilersFile(clStandardPaths::Get().GetUserDataDir(), "compilers_paths.json");
    root.save(compilersFile);
    return ::SaveXmlToFile(m_doc, m_fileName.GetFullPath());
}

static BuildSettingsConfig* gs_buildSettingsInstance = NULL;
void BuildSettingsConfigST::Free()
{
    if(gs_buildSettingsInstance) {
        delete gs_buildSettingsInstance;
        gs_buildSettingsInstance = NULL;
    }
}

BuildSettingsConfig* BuildSettingsConfigST::Get()
{
    if(gs_buildSettingsInstance == NULL) gs_buildSettingsInstance = new BuildSettingsConfig;
    return gs_buildSettingsInstance;
}

CompilerPtr BuildSettingsConfig::GetDefaultCompiler(const wxString& compilerFamilty) const
{
#ifdef __WXMSW__
    wxString DEFAULT_COMPILER = COMPILER_FAMILY_MINGW;
#else
    wxString DEFAULT_COMPILER = COMPILER_FAMILY_GCC;
#endif

    CompilerPtr defaultComp;
    wxString family = compilerFamilty.IsEmpty() ? DEFAULT_COMPILER : compilerFamilty;

    std::unordered_map<wxString, CompilerPtr>::const_iterator iter = m_compilers.begin();
    for(; iter != m_compilers.end(); ++iter) {
        if(iter->second->GetCompilerFamily() == family) {
            if(!defaultComp) {
                // keep the first one, just incase
                defaultComp = iter->second;
            }
            if(iter->second->IsDefault()) { return iter->second; }
        }
    }
    return defaultComp;
}

CompilerPtrVec_t BuildSettingsConfig::GetAllCompilers(const wxString& family) const
{
    CompilerPtrVec_t all;
    std::for_each(m_compilers.begin(), m_compilers.end(), [&](const std::pair<wxString, CompilerPtr>& p) {
        if(!family.IsEmpty() && p.second->GetCompilerFamily() == family) {
            all.push_back(p.second);
        } else if(family.IsEmpty()) {
            all.push_back(p.second);
        }
    });
    return all;
}

std::unordered_map<wxString, wxArrayString> BuildSettingsConfig::GetCompilersGlobalPaths() const
{
    std::unordered_map<wxString, wxArrayString> M;
    wxArrayString compilers = GetAllCompilersNames();
    for(const wxString& name : compilers) {
        CompilerPtr cmp = GetCompiler(name);
        if(!cmp) { continue; }
        M.insert({ name, cmp->GetDefaultIncludePaths() });
    }
    return M;
}
