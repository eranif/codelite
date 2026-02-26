//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : environmentconfig.cpp
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
#include "environmentconfig.h"

#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "archive.h"
#include "envvarlist.h"
#include "macromanager.h"
#include "workspace.h"
#include "xmlutils.h"

#include <algorithm>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/utils.h>
#include <wx/window.h>

const wxString __NO_SUCH_ENV__ = wxT("__NO_SUCH_ENV__");

static EnvironmentConfig* ms_instance = NULL;

//------------------------------------------------------------------------------

EnvironmentConfig::EnvironmentConfig()
    : m_envApplied(0)
{
}

EnvironmentConfig* EnvironmentConfig::Instance()
{
    if (ms_instance == 0) {
        ms_instance = new EnvironmentConfig();
    }
    return ms_instance;
}

void EnvironmentConfig::Release()
{
    if (ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

wxString EnvironmentConfig::GetRootName() { return wxT("EnvironmentVariables"); }

bool EnvironmentConfig::Load()
{
    bool loaded = ConfigurationToolBase::Load(wxT("config/environment_variables.xml"));
    if (loaded) {
        // make sure that we are using the new format
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("ArchiveObject"));
        if (node) {
            node = XmlUtils::FindFirstByTagName(node, wxT("StringMap"));
            if (node) {

                // this is an old version, convert it to the new format
                EnvVarList vars;
                wxStringMap_t envSets;
                wxString content;

                wxXmlNode* child = node->GetChildren();
                while (child) {
                    if (child->GetName() == wxT("MapEntry")) {
                        wxString key = child->GetAttribute(wxT("Key"), wxT(""));
                        wxString val = child->GetAttribute(wxT("Value"), wxT(""));
                        content << key << wxT("=") << val << wxT("\n");
                    }
                    child = child->GetNext();
                }
                envSets[wxT("Default")] = content.Trim().Trim(false);
                vars.SetEnvVarSets(envSets);
                SetSettings(vars);
            }
        }
    }
    return loaded;
}

wxString EnvironmentConfig::ExpandVariables(const wxString& in, bool applyEnvironment)
{
    EnvSetter* env = NULL;
    if (applyEnvironment) {
        env = new EnvSetter();
    }
    wxString expandedValue = DoExpandVariables(in);

    wxDELETE(env);
    return expandedValue;
}

void EnvironmentConfig::ApplyEnv(wxStringMap_t* overrideMap, const wxString& project, const wxString& config)
{
    // We lock the CS here and it will be released in UnApplyEnv
    // this is safe to call without Locker since the UnApplyEnv
    // will always be called after ApplyEnv (ApplyEnv and UnApplyEnv are
    // protected functions that can only be called from EnvSetter class
    // which always call UnApplyEnv in its destructor)
    m_cs.Enter();
    ++m_envApplied;

    if (m_envApplied > 1) {
        return;
    }

    // read the environments variables
    EnvVarList vars;
    ReadObject(wxT("Variables"), &vars);

    // get the active environment variables set
    EnvMap variables = vars.GetVariables(wxEmptyString, true, project, config);

    // if we have an "override map" place all the entries from the override map
    // into the global map before applying the environment
    if (overrideMap) {
        for (const auto& [key, value] : *overrideMap) {
            variables.Put(key, value);
        }
    }

    m_envSnapshot.clear();
    for (size_t i = 0; i < variables.GetCount(); i++) {

        wxString key, val;
        variables.Get(i, key, val);

        // keep old value before changing it
        wxString oldVal(wxEmptyString);
        if (wxGetEnv(key, &oldVal) == false) {
            oldVal = __NO_SUCH_ENV__;
        }

        // keep the old value, however, don't override it if it
        // already exists as it might cause the variable to grow in size...
        // Simple case:
        // PATH=$(PATH);\New\Path
        // PATH=$(PATH);\Another\New\Path
        // If we replace the value, PATH will contain the original PATH + \New\Path
        if (m_envSnapshot.count(key) == 0) {
            m_envSnapshot.insert(std::make_pair(key, oldVal));
        }

        // Incase this line contains other environment variables, expand them before setting this environment variable
        wxString newVal = DoExpandVariables(val);

        // set the new value
        wxSetEnv(key, newVal);
    }
}

void EnvironmentConfig::UnApplyEnv()
{
    --m_envApplied;
    if (m_envApplied == 0) {
        // loop over the old values and restore them
        for (const auto& [key, value] : m_envSnapshot) {
            if (value == __NO_SUCH_ENV__) {
                // Remove the environment completely
                ::wxUnsetEnv(key);
            } else {
                // Restore old value
                ::wxSetEnv(key, value);
            }
        }
        m_envSnapshot.clear();
    }
    m_cs.Leave();
}

EnvVarList EnvironmentConfig::GetSettings()
{
    EnvVarList vars;
    ReadObject(wxT("Variables"), &vars);
    return vars;
}

void EnvironmentConfig::SetSettings(EnvVarList& vars) { WriteObject(wxT("Variables"), &vars); }

wxString EnvironmentConfig::DoExpandVariables(const wxString& in)
{
    wxString result(in);
    wxString varName, text;

    DollarEscaper de(result);
    wxStringMap_t unresolvedVars;
    int counter(0);
    while (MacroManager::Instance()->FindVariable(result, varName, text)) {

        wxString replacement;
        if (varName == wxT("MAKE")) {
            // ignore this variable, since it is probably was passed here
            // by the makefile generator
            replacement = wxT("___MAKE___");

        } else {

            // search for an environment with this name
            if (!wxGetEnv(varName, &replacement)) {
                replacement << "__unresolved__var__" << ++counter;
                unresolvedVars.insert(std::make_pair(replacement, text));
            }
        }

        // don't allow recursive replacements
        if (replacement.Contains(text)) {
            break;
        }
        result.Replace(text, replacement);
    }

    // restore the ___MAKE___ back to $(MAKE)
    result.Replace(wxT("___MAKE___"), wxT("$(MAKE)"));

    // and restore all those unresolved variables
    for (const auto& [placeHolder, value] : unresolvedVars) {
        result.Replace(placeHolder, value);
    }
    return result;
}

wxArrayString EnvironmentConfig::GetActiveSetEnvNames(bool includeWorkspace, const wxString& project)
{
    // read the environments variables
    EnvVarList vars;
    ReadObject(wxT("Variables"), &vars);

    wxArrayString envnames;
    // get the active environment variables set
    EnvMap variables = vars.GetVariables(wxEmptyString, includeWorkspace, project, wxEmptyString);
    for (size_t i = 0; i < variables.GetCount(); ++i) {
        wxString key, val;
        variables.Get(i, key, val);
        envnames.Add(key);
    }
    return envnames;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

DollarEscaper::DollarEscaper(wxString& str)
    : m_str(str)
{
    m_str.Replace("$$", "@@ESC_DOLLAR@@");
}

DollarEscaper::~DollarEscaper()
{
    // we restore it to non escaped $ !!
    m_str.Replace("@@ESC_DOLLAR@@", "$");
}

// EnvSetter

EnvSetter::EnvSetter()
    : m_env(EnvironmentConfig::Instance())
{
    wxStringMap_t env_map;
    wxString project_name, config_name;
    if (clFileSystemWorkspace::Get().IsOpen() && !clFileSystemWorkspace::Get().IsRemote()) {
        auto env_list = clFileSystemWorkspace::Get().GetEnvironment();
        for (const auto& [k, v] : env_list) {
            env_map.insert({k, v});
        }

    } else if (clCxxWorkspaceST::Get()->IsOpen() && clCxxWorkspaceST::Get()->GetActiveProject()) {
        auto project = clCxxWorkspaceST::Get()->GetActiveProject();
        config_name = project->GetBuildConfiguration() ? project->GetBuildConfiguration()->GetName() : wxString{};
        project_name = project->GetName();
    }
    m_env->ApplyEnv(&env_map, project_name, config_name);
}
