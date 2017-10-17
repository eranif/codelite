//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : evnvarlist.h
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
#ifndef __evnvarlist__
#define __evnvarlist__

#include "serialized_object.h"
#include <wx/arrstr.h>
#include "macros.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EnvMap
{
    wxArrayString m_keys;
    wxArrayString m_values;

public:
    EnvMap();
    ~EnvMap();

    void Put(const wxString& key, const wxString& val);
    bool Get(const wxString& key, wxString& val);
    void Clear();
    size_t GetCount();
    bool Get(size_t index, wxString& key, wxString& val);
    bool Contains(const wxString& key);
    wxString String();
};

class WXDLLIMPEXP_SDK EvnVarList : public SerializedObject
{
    wxStringMap_t m_envVarSets;
    wxString m_activeSet;

protected:
    wxString DoGetSetVariablesStr(const wxString& setName, wxString& selectedSetName);

public:
    EvnVarList();
    virtual ~EvnVarList();

    void SetActiveSet(const wxString& activeSet)
    {
        if(activeSet != _("<Use Active Set>") && activeSet != USE_GLOBAL_SETTINGS) this->m_activeSet = activeSet;
    }

    const wxString& GetActiveSet() const { return m_activeSet; }
    void SetEnvVarSets(const wxStringMap_t& envVarSets) { this->m_envVarSets = envVarSets; }
    const wxStringMap_t& GetEnvVarSets() const { return m_envVarSets; }

    void AddVariable(const wxString& setName, const wxString& name, const wxString& value);

    /**
     * \brief inser variable into the set. If a variable is already exist with this name, replace it
     * \param setName
     * \param name
     * \param value
     */
    void InsertVariable(const wxString& setName, const wxString& name, const wxString& value);

    EnvMap GetVariables(const wxString& setName, bool includeWorkspaceEnvs, const wxString& projectName,
                        const wxString& configName);
    bool IsSetExist(const wxString& setName);

public:
    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);
};
#endif // __evnvarlist__
