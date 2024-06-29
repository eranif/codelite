//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : environmentconfig.h
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
#pragma once

#include "build_config.h"
#include "clEnvironment.hpp"
#include "configurationtoolbase.h"
#include "envvarlist.h"
#include "macros.h"
#include "project.h"

#include <wx/thread.h>
#include <wx/utils.h>

class EnvSetter;

/// A helper class that makes sure that escaped $ signs
/// are kept unharmed
class WXDLLIMPEXP_SDK DollarEscaper
{
    wxString& m_str;

public:
    DollarEscaper(wxString& str);

    ~DollarEscaper();
};

class WXDLLIMPEXP_SDK EnvironmentConfig : public ConfigurationToolBase
{
    // Allow access to Apply/UnApply Env
    friend class EnvSetter;

    wxStringMap_t m_envSnapshot;
    wxCriticalSection m_cs;
    int m_envApplied;

protected:
    wxString DoExpandVariables(const wxString& in);
    void ApplyEnv(wxStringMap_t* overrideMap, const wxString& project, const wxString& config);
    void UnApplyEnv();

public:
    static EnvironmentConfig* Instance();
    static void Release();
    bool Load();
    wxString ExpandVariables(const wxString& in, bool applyEnvironment);
    EnvVarList GetSettings();
    void SetSettings(EnvVarList& vars);
    /**
     * @brief return a list of the environment variables as defined by the user
     * in the global environment table + workspace + project (this function only returns the names of the variable, not
     * its value)
     */
    wxArrayString GetActiveSetEnvNames(bool includeWorkspace = true, const wxString& project = wxEmptyString);

private:
    EnvironmentConfig();
    virtual ~EnvironmentConfig();

public:
    virtual wxString GetRootName();
};

class EnvSetter
{
public:
    explicit EnvSetter(wxStringMap_t* om = NULL)
        : m_env(EnvironmentConfig::Instance())
    {
        m_env->ApplyEnv(om, wxEmptyString, wxEmptyString);
    }

    explicit EnvSetter(const clEnvList_t* envlist)
        : m_env(EnvironmentConfig::Instance())
    {
        // convert envlist -> wxStringMap_t
        wxStringMap_t overrideMap;
        if (envlist) {
            overrideMap.reserve(envlist->size());
            for (const auto& [name, value] : *envlist) {
                overrideMap.insert({ name, value });
            }
        }
        m_env->ApplyEnv(&overrideMap, wxEmptyString, wxEmptyString);
    }

    EnvSetter(ProjectPtr proj)
        : m_env(EnvironmentConfig::Instance())
    {
        wxString projname = proj->GetName();
        wxString buildConfName;
        BuildConfigPtr buildConf = proj->GetBuildConfiguration();
        if (buildConf) {
            buildConfName = buildConf->GetName();
        }
        m_env->ApplyEnv(NULL, projname, buildConfName);
    }

    explicit EnvSetter(Project* proj)
        : m_env(EnvironmentConfig::Instance())
    {
        wxString projname = proj->GetName();
        wxString buildConfName;
        BuildConfigPtr buildConf = proj->GetBuildConfiguration();
        if (buildConf) {
            buildConfName = buildConf->GetName();
        }
        m_env->ApplyEnv(NULL, projname, buildConfName);
    }

    explicit EnvSetter(EnvironmentConfig* conf, wxStringMap_t* om = NULL)
        : m_env(EnvironmentConfig::Instance())
    {
        wxUnusedVar(conf);
        m_env->ApplyEnv(om, wxEmptyString, wxEmptyString);
    }

    explicit EnvSetter(EnvironmentConfig* conf, wxStringMap_t* om, const wxString& project, const wxString& buildConfig)
        : m_env(EnvironmentConfig::Instance())
    {
        wxUnusedVar(conf);
        m_env->ApplyEnv(om, project, buildConfig);
    }

    explicit EnvSetter(const wxString& var, const wxString& value)
        : m_env(NULL)
    {
        m_envName = var;
        // keep old value
        m_restoreOldValue = ::wxGetEnv(m_envName, &m_oldEnvValue);
        ::wxSetEnv(m_envName, value);
    }

    ~EnvSetter()
    {
        if (m_env) {
            m_env->UnApplyEnv();
            m_env = NULL;
        }

        if (m_restoreOldValue) {
            // restore old env variable value
            ::wxSetEnv(m_envName, m_oldEnvValue);

        } else if (!m_envName.IsEmpty()) {
            // we applied a single env variable without old value
            // uset it
            ::wxUnsetEnv(m_envName);
        }
    }

private:
    EnvironmentConfig* m_env = nullptr;
    wxString m_envName;
    wxString m_oldEnvValue;
    bool m_restoreOldValue = false;
};
