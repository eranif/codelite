//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_configuration_data.h
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

#ifndef PHPCONFIGURATIONDATA_H
#define PHPCONFIGURATIONDATA_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include <cl_config.h>
#include "phpoptions.h"

class PHPConfigurationData : public clConfigItem
{
protected:
    size_t m_xdebugPort;
    wxArrayString m_ccIncludePath;
    size_t m_flags;
    wxString m_xdebugIdeKey;
    wxString m_xdebugHost;
    wxString m_findInFilesMask;
    int m_workspaceType;
    size_t m_settersGettersFlags;
    PhpOptions m_phpOptions;

public:
    enum {
        kDontPromptForMissingFileMapping = (1 << 0),
    };

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
    PHPConfigurationData();

    PHPConfigurationData& Load();
    void Save();

    virtual ~PHPConfigurationData();

    PHPConfigurationData& SetWorkspaceType(int workspaceType)
    {
        this->m_workspaceType = workspaceType;
        return *this;
    }

    int GetWorkspaceType() const { return m_workspaceType; }
    PHPConfigurationData& EnableFlag(size_t flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
        return *this;
    }

    bool HasFlag(size_t flag) const { return m_flags & flag; }

    PHPConfigurationData& SetFindInFilesMask(const wxString& findInFilesMask)
    {
        this->m_findInFilesMask = findInFilesMask;
        return *this;
    }

    const wxString& GetFindInFilesMask() const { return m_findInFilesMask; }
    // ----------------------------------------------------
    // Setters
    // ----------------------------------------------------
    PHPConfigurationData& SetCcIncludePath(const wxArrayString& ccIncludePath)
    {
        this->m_ccIncludePath = ccIncludePath;
        return *this;
    }
    PHPConfigurationData& SetErrorReporting(const wxString& errorReporting)
    {
        m_phpOptions.SetErrorReporting(errorReporting);
        return *this;
    }
    PHPConfigurationData& SetIncludePaths(const wxArrayString& includePaths)
    {
        m_phpOptions.SetIncludePaths(includePaths);
        return *this;
    }
    PHPConfigurationData& SetPhpExe(const wxString& phpExe)
    {
        m_phpOptions.SetPhpExe(phpExe);
        return *this;
    }
    PHPConfigurationData& SetXdebugPort(size_t xdebugPort)
    {
        this->m_xdebugPort = xdebugPort;
        return *this;
    }
    PHPConfigurationData& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }
    PHPConfigurationData& SetXdebugIdeKey(const wxString& xdebugIdeKey)
    {
        this->m_xdebugIdeKey = xdebugIdeKey;
        return *this;
    }

    // ----------------------------------------------------
    // Getters
    // ----------------------------------------------------

    const wxArrayString& GetCcIncludePath() const { return m_ccIncludePath; }
    wxArrayString& GetCcIncludePath() { return m_ccIncludePath; }

    const wxString& GetErrorReporting() const { return m_phpOptions.GetErrorReporting(); }
    const wxArrayString& GetIncludePaths() const { return m_phpOptions.GetIncludePaths(); }
    wxString GetIncludePathsAsString() const;
    wxString GetCCIncludePathsAsString() const;
    const wxString& GetPhpExe() const { return m_phpOptions.GetPhpExe(); }
    size_t GetXdebugPort() const { return m_xdebugPort; }

    size_t GetFlags() const { return m_flags; }
    const wxString& GetXdebugIdeKey() const { return m_xdebugIdeKey; }
    PHPConfigurationData& SetXdebugHost(const wxString& xdebugHost)
    {
        this->m_xdebugHost = xdebugHost;
        return *this;
    }
    const wxString& GetXdebugHost() const { return m_xdebugHost; }
    PHPConfigurationData& SetSettersGettersFlags(size_t settersGettersFlags)
    {
        this->m_settersGettersFlags = settersGettersFlags;
        return *this;
    }
    size_t GetSettersGettersFlags() const { return m_settersGettersFlags; }
};

#endif // PHPCONFIGURATIONDATA_H
