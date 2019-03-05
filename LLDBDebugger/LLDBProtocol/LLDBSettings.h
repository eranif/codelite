//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBSettings.h
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

#ifndef LLDBSETTINGS_H
#define LLDBSETTINGS_H

#include "LLDBEnums.h"
#include <wx/string.h>
#include "JSON.h"

class LLDBSettings
{
    size_t m_arrItems;
    size_t m_stackFrames;
    size_t m_flags;
    wxString m_types;
    wxString m_proxyIp;
    int m_proxyPort;
    wxString m_lastLocalFolder;
    wxString m_lastRemoteFolder;
    wxString m_debugserver;

public:
    LLDBSettings();
    virtual ~LLDBSettings();

    void SetLastLocalFolder(const wxString& lastLocalFolder) { this->m_lastLocalFolder = lastLocalFolder; }
    void SetLastRemoteFolder(const wxString& lastRemoteFolder) { this->m_lastRemoteFolder = lastRemoteFolder; }
    const wxString& GetLastLocalFolder() const { return m_lastLocalFolder; }
    const wxString& GetLastRemoteFolder() const { return m_lastRemoteFolder; }
    bool IsRaiseWhenBreakpointHit() const { return m_flags & kLLDBOptionRaiseCodeLite; }

    bool HasFlag(int flag) const { return m_flags & flag; }

    void EnableFlag(int flag, bool enable)
    {
        if(enable) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    void SetMaxArrayElements(size_t maxArrayElements) { this->m_arrItems = maxArrayElements; }
    void SetMaxCallstackFrames(size_t maxCallstackFrames) { this->m_stackFrames = maxCallstackFrames; }
    void SetTypes(const wxString& types) { this->m_types = types; }
    size_t GetMaxArrayElements() const { return m_arrItems; }
    size_t GetMaxCallstackFrames() const { return m_stackFrames; }
    const wxString& GetTypes() const { return m_types; }

    bool IsUsingRemoteProxy() const { return HasFlag(kLLDBOptionUseRemoteProxy); }

    void SetUseRemoteProxy(bool b) { EnableFlag(kLLDBOptionUseRemoteProxy, b); }

    void SetProxyIp(const wxString& proxyIp) { this->m_proxyIp = proxyIp; }
    void SetProxyPort(int proxyPort) { this->m_proxyPort = proxyPort; }
    const wxString& GetProxyIp() const { return m_proxyIp; }
    int GetProxyPort() const { return m_proxyPort; }
    LLDBSettings& Load();
    LLDBSettings& Save();

    wxString GetTcpConnectString() const { return (wxString() << m_proxyIp << ":" << m_proxyPort); }

    // Helpers
    static wxString LoadPerspective();
    static void SavePerspective(const wxString& perspective);

    // Serialization API
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);
    LLDBSettings& SetArrItems(size_t arrItems)
    {
        this->m_arrItems = arrItems;
        return *this;
    }
    LLDBSettings& SetDebugserver(const wxString& debugserver)
    {
        this->m_debugserver = debugserver;
        return *this;
    }
    LLDBSettings& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }
    LLDBSettings& SetStackFrames(size_t stackFrames)
    {
        this->m_stackFrames = stackFrames;
        return *this;
    }
    size_t GetArrItems() const { return m_arrItems; }
    const wxString& GetDebugserver() const { return m_debugserver; }
    size_t GetFlags() const { return m_flags; }
    size_t GetStackFrames() const { return m_stackFrames; }
};

#endif // LLDBSETTINGS_H
