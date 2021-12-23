//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : vcimporter.h
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
#ifndef VCIMPORTER_H
#define VCIMPORTER_H

#include "codelite_exports.h"
#include "map"
#include "project.h"
#include "project_settings.h"
#include "wx/string.h"

#include <wx/txtstrm.h>
#include <wx/wfstream.h>

struct VcProjectData {
    wxString name;
    wxString id;
    wxString filepath;
    wxArrayString deps;
};

class WXDLLIMPEXP_SDK VcImporter
{
    wxString m_fileName;
    bool m_isOk;
    wxFileInputStream* m_is;
    wxTextInputStream* m_tis;
    std::map<wxString, VcProjectData> m_projects;
    wxString m_compiler;
    wxString m_compilerLowercase;

public:
    VcImporter(const wxString& fileName, const wxString& defaultCompiler);
    virtual ~VcImporter();
    bool Import(wxString& errMsg);

private:
    // read line, skip empty lines
    bool ReadLine(wxString& line);
    bool OnProject(const wxString& firstLine, wxString& errMsg);
    void RemoveGershaim(wxString& str);
    void CreateWorkspace();
    void CreateProjects();
    bool ConvertProject(VcProjectData& data);
    void AddConfiguration(ProjectSettingsPtr settings, wxXmlNode* config);
    void CreateFiles(wxXmlNode* parent, wxString vdPath, ProjectPtr proj);
    wxArrayString SplitString(const wxString& s);
};

#endif // VCIMPORTER_H
