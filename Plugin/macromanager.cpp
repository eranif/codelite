//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : macromanager.cpp
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

#include "macromanager.h"

#include "IWorkspace.h"
#include "build_config.h"
#include "clFileSystemWorkspace.hpp"
#include "clWorkspaceManager.h"
#include "environmentconfig.h"
#include "globals.h"
#include "imanager.h"
#include "project.h"
#include "workspace.h"

#include <wx/regex.h>

MacroManager::MacroManager() {}

MacroManager::~MacroManager() {}

MacroManager* MacroManager::Instance()
{
    static MacroManager ms_instance;
    return &ms_instance;
}

wxString MacroManager::Expand(const wxString& expression, IManager* manager, const wxString& project,
                              const wxString& confToBuild)
{
    return DoExpand(expression, manager, project, true, confToBuild);
}

wxString MacroManager::Replace(const wxString& inString, const wxString& variableName, const wxString& replaceWith,
                               bool bIgnoreCase)
{
    size_t flags = wxRE_DEFAULT;
    if(bIgnoreCase) {
        flags |= wxRE_ICASE;
    }

    wxString strRe1;
    wxString strRe2;
    wxString strRe3;
    wxString strRe4;

    strRe1 << "\\$\\((" << variableName << ")\\)";
    strRe2 << "\\$\\{(" << variableName << ")\\}";
    strRe3 << "\\$(" << variableName << ")";
    strRe4 << "%(" << variableName << ")%";

    wxRegEx reOne(strRe1, flags);   // $(variable)
    wxRegEx reTwo(strRe2, flags);   // ${variable}
    wxRegEx reThree(strRe3, flags); // $variable
    wxRegEx reFour(strRe4, flags);  // %variable%

    wxString result = inString;
    if(reOne.Matches(result)) {
        reOne.ReplaceAll(&result, replaceWith);
    }

    if(reTwo.Matches(result)) {
        reTwo.ReplaceAll(&result, replaceWith);
    }

    if(reThree.Matches(result)) {
        reThree.ReplaceAll(&result, replaceWith);
    }

    if(reFour.Matches(result)) {
        reFour.ReplaceAll(&result, replaceWith);
    }
    return result;
}

bool MacroManager::FindVariable(const wxString& inString, wxString& name, wxString& fullname)
{
    size_t flags = wxRE_DEFAULT | wxRE_ICASE;

    wxString strRe1;
    wxString strRe2;
    wxString strRe3;
    wxString strRe4;

    strRe1 << "\\$\\(("
           << "[a-z_0-9]+"
           << ")\\)";
    strRe2 << "\\$\\{("
           << "[a-z_0-9]+"
           << ")\\}";
    strRe3 << "\\$("
           << "[a-z_0-9]+"
           << ")";
    strRe4 << "%("
           << "[a-z_0-9]+"
           << ")%";

    wxRegEx reOne(strRe1, flags);   // $(variable)
    wxRegEx reTwo(strRe2, flags);   // ${variable}
    wxRegEx reThree(strRe3, flags); // $variable
    wxRegEx reFour(strRe4, flags);  // %variable%

    if(reOne.Matches(inString)) {
        name = reOne.GetMatch(inString, 1);
        fullname = reOne.GetMatch(inString);
        return true;
    }

    if(reTwo.Matches(inString)) {
        name = reTwo.GetMatch(inString, 1);
        fullname = reTwo.GetMatch(inString);
        return true;
    }

    if(reThree.Matches(inString)) {
        name = reThree.GetMatch(inString, 1);
        fullname = reThree.GetMatch(inString);
        return true;
    }

    if(reFour.Matches(inString)) {
        name = reFour.GetMatch(inString, 1);
        fullname = reFour.GetMatch(inString);
        return true;
    }
    return false;
}

wxString MacroManager::ExpandNoEnv(const wxString& expression, const wxString& project, const wxString& confToBuild)
{
    return DoExpand(expression, NULL, project, false, confToBuild);
}

wxString MacroManager::DoExpand(const wxString& expression, IManager* manager, const wxString& project, bool applyEnv,
                                const wxString& confToBuild)
{
    wxString expandedString(expression);
    clCxxWorkspace* workspace = nullptr;
    // IWorkspace* workspace = clWorkspaceManager::Get().IsWorkspaceOpened();

    if(!manager) {
        manager = clGetManager();
    }

    wxString wspName;
    wxString wspConfig;
    wxString wspPath;

    if(clCxxWorkspaceST::Get()->IsOpen()) {
        wspName = clCxxWorkspaceST::Get()->GetName();
        wspConfig = clCxxWorkspaceST::Get()->GetSelectedConfig()
                        ? clCxxWorkspaceST::Get()->GetSelectedConfig()->GetName()
                        : wxString();

        wspPath = clCxxWorkspaceST::Get()->GetFileName().GetPath();
        workspace = clCxxWorkspaceST::Get();
    } else if(clFileSystemWorkspace::Get().IsOpen()) {
        wspName = clFileSystemWorkspace::Get().GetName();
        wspConfig = clFileSystemWorkspace::Get().GetSettings().GetSelectedConfig()
                        ? clFileSystemWorkspace::Get().GetSettings().GetSelectedConfig()->GetName()
                        : wxString();
        wspPath = clFileSystemWorkspace::Get().GetFileName().GetPath();
    }

    size_t retries = 0;
    wxString dummyname, dummfullname;
    while((retries < 5) && FindVariable(expandedString, dummyname, dummyname)) {
        ++retries;
        DollarEscaper de(expandedString);
        expandedString.Replace("$(WorkspaceName)", wspName);
        expandedString.Replace("$(WorkspaceConfiguration)", wspConfig);
        expandedString.Replace("$(WorkspacePath)", wspPath);
        if(workspace) {
            ProjectPtr proj = workspace->GetProject(project);
            if(proj) {
                wxString prjBuildWd;
                wxString prjRunWd;

                wxString project_name(proj->GetName());

                // make sure that the project name does not contain any spaces
                project_name.Replace(" ", "_");

                BuildConfigPtr bldConf = workspace->GetProjBuildConf(proj->GetName(), confToBuild);
                if(bldConf) {
                    bool isCustom = bldConf->IsCustomBuild();
                    expandedString.Replace("$(OutputDirectory)", bldConf->GetOutputDirectory());
                    expandedString.Replace("$(ProjectOutputFile)", bldConf->GetOutputFileName());
                    // An alias
                    expandedString.Replace("$(OutputFile)", bldConf->GetOutputFileName());

                    // When custom build project, use the working directory set in the
                    // custom build tab, otherwise use the project file's path
                    prjBuildWd = isCustom ? bldConf->GetCustomBuildWorkingDir() : proj->GetFileName().GetPath();
                    prjRunWd = bldConf->GetWorkingDirectory();
                }

                expandedString.Replace("$(ProjectWorkingDirectory)", prjBuildWd);
                expandedString.Replace("$(ProjectRunWorkingDirectory)", prjRunWd);
                expandedString.Replace("$(ProjectPath)", proj->GetFileName().GetPath());
                expandedString.Replace("$(WorkspacePath)", workspace->GetWorkspaceFileName().GetPath());
                expandedString.Replace("$(ProjectName)", project_name);

                if(bldConf) {
                    expandedString.Replace("$(IntermediateDirectory)", bldConf->GetIntermediateDirectory());
                    expandedString.Replace("$(ConfigurationName)", bldConf->GetName());
                    expandedString.Replace("$(OutDir)", bldConf->GetIntermediateDirectory());

                    // Compiler-related variables

                    wxString cFlags = bldConf->GetCCompileOptions();
                    cFlags.Replace(";", " ");
                    expandedString.Replace("$(CC)", bldConf->GetCompiler()->GetTool("CC"));
                    expandedString.Replace("$(CFLAGS)", cFlags);

                    wxString cxxFlags = bldConf->GetCompileOptions();
                    cxxFlags.Replace(";", " ");
                    expandedString.Replace("$(CXX)", bldConf->GetCompiler()->GetTool("CXX"));
                    expandedString.Replace("$(CXXFLAGS)", cxxFlags);

                    wxString ldFlags = bldConf->GetLinkOptions();
                    ldFlags.Replace(";", " ");
                    expandedString.Replace("$(LDFLAGS)", ldFlags);

                    wxString asFlags = bldConf->GetAssmeblerOptions();
                    asFlags.Replace(";", " ");
                    expandedString.Replace("$(AS)", bldConf->GetCompiler()->GetTool("AS"));
                    expandedString.Replace("$(ASFLAGS)", asFlags);

                    wxString resFlags = bldConf->GetResCompileOptions();
                    resFlags.Replace(";", " ");
                    expandedString.Replace("$(RES)", bldConf->GetCompiler()->GetTool("ResourceCompiler"));
                    expandedString.Replace("$(RESFLAGS)", resFlags);

                    expandedString.Replace("$(AR)", bldConf->GetCompiler()->GetTool("AR"));

                    expandedString.Replace("$(MAKE)", bldConf->GetCompiler()->GetTool("MAKE"));

                    expandedString.Replace("$(IncludePath)", bldConf->GetIncludePath());
                    expandedString.Replace("$(LibraryPath)", bldConf->GetLibPath());
                    expandedString.Replace("$(ResourcePath)", bldConf->GetResCmpIncludePath());
                    expandedString.Replace("$(LinkLibraries)", bldConf->GetLibraries());
                }

                if(expandedString.Find("$(ProjectFiles)") != wxNOT_FOUND) {
                    expandedString.Replace("$(ProjectFiles)", proj->GetFilesAsString(false));
                }

                if(expandedString.Find("$(ProjectFilesAbs)") != wxNOT_FOUND) {
                    expandedString.Replace("$(ProjectFilesAbs)", proj->GetFilesAsString(true));
                }
            }
        }

        if(manager) {
            IEditor* editor = manager->GetActiveEditor();
            if(editor) {
                wxFileName fn(editor->GetFileName());

                expandedString.Replace("$(CurrentFileName)", fn.GetName());

                wxString fpath(fn.GetPath());
                fpath.Replace("\\", "/");
                expandedString.Replace("$(CurrentFilePath)", fpath);
                expandedString.Replace("$(CurrentFileExt)", fn.GetExt());
                expandedString.Replace("$(CurrentFileFullName)", fn.GetFullName());

                wxString ffullpath(fn.GetFullPath());
                ffullpath.Replace("\\", "/");
                expandedString.Replace("$(CurrentFileFullPath)", ffullpath);
                expandedString.Replace("$(CurrentSelection)", editor->GetSelection());
                if(expandedString.Find("$(CurrentSelectionRange)") != wxNOT_FOUND) {
                    int start = editor->GetSelectionStart(), end = editor->GetSelectionEnd();

                    wxString output = wxString::Format("%i:%i", start, end);
                    expandedString.Replace("$(CurrentSelectionRange)", output);
                }
            }
        }

        // exapand common macros
        wxDateTime now = wxDateTime::Now();
        expandedString.Replace("$(User)", wxGetUserName());
        expandedString.Replace("$(Date)", now.FormatDate());

        if(manager && applyEnv) {
            expandedString.Replace("$(CodeLitePath)", manager->GetInstallDirectory());

            // Apply the environment and expand the variables
            EnvSetter es(NULL, NULL, project, confToBuild);
            expandedString = manager->GetEnv()->ExpandVariables(expandedString, false);
        } else if(applyEnv) {
            expandedString = EnvironmentConfig::Instance()->ExpandVariables(expandedString, false);
        }
    }
    return expandedString;
}
