//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : VisualCppImporter.h
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

#ifndef VISUALCPPIMPORTER_H
#define VISUALCPPIMPORTER_H

#include "GenericImporter.h"

#include <wx/filename.h>
#include <wx/string.h>
#include <wx/xml/xml.h>

class VisualCppImporter : public GenericImporter
{
public:
    virtual bool OpenWorkspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    void ConvertToLinuxStyle(wxString& filepath);
    void GenerateFromVC6(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProjectVC6(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    void GenerateFromVC7_11(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProjectVC7(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    void GenerateFromProjectVC11(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    wxString ExtractProjectCfgName(const wxString& parentCondition, const wxString& elemCondition);
    wxString ReplaceDefaultEnvVars(const wxString& str);
    void AddFilesVC7(wxXmlNode* filterChild, GenericProjectPtr genericProject,
                     std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap, wxString preVirtualPath);
    void AddFilesVC11(wxXmlNode* itemGroupChild, GenericProjectPtr genericProject,
                      std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap);
    GenericProjectFilePtr FindProjectFileByName(GenericProjectPtr genericProject, wxString filename);
    wxFileName wsInfo;
    wxString extension;
    long version;
};

#endif // VISUALCPPIMPORTER_H
