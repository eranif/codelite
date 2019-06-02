//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : externaltoolsdata.h
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

#ifndef __externaltoolsdata__
#define __externaltoolsdata__

#include "serialized_object.h"
#include <vector>
#include <unordered_map>

#define MAX_TOOLS 20

class ToolInfo : public SerializedObject
{
    wxString m_id;
    wxString m_path;
    wxString m_wd;
    wxString m_name;
    wxString m_icon16;
    wxString m_icon24;
    bool m_captureOutput;
    bool m_saveAllFiles;
    size_t m_flags;

public:
    ToolInfo();
    ~ToolInfo();

    enum {
        kCallOnFileSave = (1 << 0),
    };
    typedef std::unordered_map<wxString, ToolInfo> Map_t;
    
protected:
    void EnableFlag(int flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

public:
    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);

    void SetId(const wxString& id) { this->m_id = id; }
    void SetPath(const wxString& path) { this->m_path = path; }
    void SetWd(const wxString& wd) { this->m_wd = wd; }
    const wxString& GetId() const { return m_id; }
    const wxString& GetPath() const { return m_path; }
    const wxString& GetWd() const { return m_wd; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
    void SetIcon16(const wxString& icon16) { this->m_icon16 = icon16; }
    void SetIcon24(const wxString& icon24) { this->m_icon24 = icon24; }
    const wxString& GetIcon16() const { return m_icon16; }
    const wxString& GetIcon24() const { return m_icon24; }

    void SetCaptureOutput(const bool& captureOutput) { this->m_captureOutput = captureOutput; }
    const bool& GetCaptureOutput() const { return m_captureOutput; }

    void SetSaveAllFiles(const bool& saveAllFiles) { this->m_saveAllFiles = saveAllFiles; }
    const bool& GetSaveAllFiles() const { return m_saveAllFiles; }

    bool IsCallOnFileSave() const { return (m_flags & kCallOnFileSave); }
    void SetCallOnFileSave(bool b) { EnableFlag(kCallOnFileSave, b); }
};

class ExternalToolsData : public SerializedObject
{
    std::vector<ToolInfo> m_tools;

public:
    ExternalToolsData();
    virtual ~ExternalToolsData();

    const std::vector<ToolInfo>& GetTools() const;
    void SetTools(const std::vector<ToolInfo>& tools);

    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);
};

#endif // __externaltoolsdata__
