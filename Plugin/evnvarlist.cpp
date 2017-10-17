//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : evnvarlist.cpp
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
#include "evnvarlist.h"
#include <wx/tokenzr.h>
#include <wx/log.h>
#include "workspace.h"
#include "macromanager.h"

EvnVarList::EvnVarList()
    : m_activeSet(wxT("Default"))
{
}

EvnVarList::~EvnVarList() {}

void EvnVarList::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_envVarSets"), m_envVarSets);
    arch.Read(wxT("m_activeSet"), m_activeSet);
}

void EvnVarList::Serialize(Archive& arch)
{
    arch.Write(wxT("m_envVarSets"), m_envVarSets);
    arch.Write(wxT("m_activeSet"), m_activeSet);
}

void EvnVarList::AddVariable(const wxString& setName, const wxString& name, const wxString& value)
{
    wxString newEntry, actualSetName;
    newEntry << name << wxT("=") << value;

    wxString currentValueStr = DoGetSetVariablesStr(setName, actualSetName);
    wxArrayString currentValues = wxStringTokenize(currentValueStr, wxT("\r\n"), wxTOKEN_STRTOK);

    if(currentValues.Index(newEntry) == wxNOT_FOUND) currentValues.Add(newEntry);

    currentValueStr.Clear();
    for(size_t i = 0; i < currentValues.GetCount(); i++) {
        currentValueStr << currentValues.Item(i) << wxT("\n");
    }

    if(currentValueStr.IsEmpty() == false) currentValueStr.RemoveLast();

    m_envVarSets[actualSetName] = currentValueStr;
}

void EvnVarList::InsertVariable(const wxString& setName, const wxString& name, const wxString& value)
{
    wxString actualSetName;

    DoGetSetVariablesStr(setName, actualSetName);

    EnvMap set = GetVariables(actualSetName, false, wxEmptyString, wxEmptyString);
    if(!set.Contains(name)) {
        set.Put(name, value);
    }
    m_envVarSets[actualSetName] = set.String();
}

EnvMap EvnVarList::GetVariables(const wxString& setName, bool includeWorkspaceEnvs, const wxString& projectName,
                                const wxString& configName)
{
    EnvMap variables;
    wxString actualSetName;

    wxString currentValueStr = DoGetSetVariablesStr(setName, actualSetName);

    if(includeWorkspaceEnvs && !clCxxWorkspaceST::Get()->GetName().IsEmpty()) {
        currentValueStr.Trim().Trim(false);
        currentValueStr << wxT("\n");
        currentValueStr << clCxxWorkspaceST::Get()->GetEnvironmentVariabels();

        if(projectName.IsEmpty() == false) {
            currentValueStr.Trim().Trim(false);
            BuildConfigPtr buildConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, configName);
            if(buildConf) {
                currentValueStr << wxT("\n");
                currentValueStr << buildConf->GetEnvvars();
            }
        }
    }

    wxArrayString currentValues = wxStringTokenize(currentValueStr, wxT("\r\n"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < currentValues.GetCount(); i++) {
        wxString entry = currentValues.Item(i);

        // remove any comment from the line
        int where = entry.Find(wxT("#"));
        if(where != wxNOT_FOUND) {
            entry = entry.Left(where);
        }

        entry.Trim().Trim(false);
        if(entry.IsEmpty()) {
            continue;
        }

        wxString varname = entry.BeforeFirst(wxT('='));
        wxString varvalue = entry.AfterFirst(wxT('='));

        // Expand macros (which are not environment variables)
        varvalue = MacroManager::Instance()->ExpandNoEnv(varvalue, projectName, configName);
        variables.Put(varname, varvalue);
    }
    return variables;
}

wxString EvnVarList::DoGetSetVariablesStr(const wxString& setName, wxString& selectedSetName)
{
    wxString currentValueStr;

    selectedSetName = setName;
    wxStringMap_t::iterator iter = m_envVarSets.find(setName);
    if(iter != m_envVarSets.end())
        currentValueStr = iter->second;
    else {
        iter = m_envVarSets.find(m_activeSet);
        if(iter != m_envVarSets.end()) {
            currentValueStr = iter->second;
            selectedSetName = m_activeSet;
        } else {
            selectedSetName = wxT("Default");
            iter = m_envVarSets.find(selectedSetName);
            if(iter != m_envVarSets.end()) currentValueStr = iter->second;
        }
    }
    return currentValueStr;
}

bool EvnVarList::IsSetExist(const wxString& setName) { return m_envVarSets.find(setName) != m_envVarSets.end(); }

// Env Map helper class
EnvMap::EnvMap() {}

EnvMap::~EnvMap() {}

bool EnvMap::Get(const wxString& key, wxString& val)
{
    int where = m_keys.Index(key);
    if(where == wxNOT_FOUND) return false;

    val = m_values.Item(where);
    return true;
}

void EnvMap::Put(const wxString& key, const wxString& val)
{
    // We allow duplicate entries to exists
    m_keys.Add(key);
    m_values.Add(val);
}

void EnvMap::Clear()
{
    m_keys.Clear();
    m_values.Clear();
}

size_t EnvMap::GetCount() { return m_keys.GetCount(); }

bool EnvMap::Get(size_t index, wxString& key, wxString& val)
{
    if(index >= m_keys.GetCount()) return false;

    key = m_keys.Item(index);
    val = m_values.Item(index);
    return true;
}

bool EnvMap::Contains(const wxString& key) { return m_keys.Index(key) != wxNOT_FOUND; }

wxString EnvMap::String()
{
    wxString s;
    for(size_t i = 0; i < m_keys.GetCount(); i++) {
        s << m_keys.Item(i) << wxT("=") << m_values.Item(i) << wxT("\n");
    }

    if(s.IsEmpty() == false) s.RemoveLast();

    return s;
}
