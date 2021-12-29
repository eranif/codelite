#include "DevCppImporter.h"

#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

bool DevCppImporter::OpenWorkspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    wxString extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("dev");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}

bool DevCppImporter::isSupportedWorkspace()
{
    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();

        if(line.Contains(wxT("[Project]"))) {
            return true;
        }
    }

    return false;
}

GenericWorkspacePtr DevCppImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    GenericProjectPtr genericProject;
    GenericProjectCfgPtr genericProjectCfgDebug;
    GenericProjectCfgPtr genericProjectCfgRelease;

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();

        if(line.Contains(wxT("[Project]"))) {
            while(!line.IsEmpty()) {
                line = tis.ReadLine();
                wxStringTokenizer part(line, wxT("="));
                wxString tagName = part.GetNextToken().Trim().Trim(false);
                wxString tagValue = part.GetNextToken().Trim().Trim(false);

                if(tagName == wxT("Name")) {
                    wxString projectName = tagValue;

                    genericProject = std::make_shared<GenericProject>();
                    genericProject->name = projectName;
                    genericProject->path = wsInfo.GetPath();

                    genericProjectCfgDebug = std::make_shared<GenericProjectCfg>();
                    genericProjectCfgRelease = std::make_shared<GenericProjectCfg>();

                    genericProjectCfgDebug->name = wxT("Debug");
                    genericProjectCfgRelease->name = wxT("Release");

                    genericProjectCfgDebug->intermediateDirectory = wxT("./Debug");
                    genericProjectCfgRelease->intermediateDirectory = wxT("./Release");

                    genericProject->cfgs.push_back(genericProjectCfgDebug);
                    genericProject->cfgs.push_back(genericProjectCfgRelease);

                    genericWorkspace->projects.push_back(genericProject);
                }

                if(tagName == wxT("Type")) {
                    wxString projectType = tagValue;

                    if(projectType == wxT("2")) {
                        genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                    } else if(projectType == wxT("3")) {
                        genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                    } else {
                        genericProject->cfgType = GenericCfgType::EXECUTABLE;
                    }

                    genericProjectCfgDebug->type = genericProject->cfgType;
                    genericProjectCfgRelease->type = genericProject->cfgType;
                }

                if(tagName == wxT("Includes")) {
                    wxString projectIncludes = tagValue;
                    projectIncludes.Replace(wxT("\""), wxT(""));
                    projectIncludes.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->includePath = projectIncludes;
                    genericProjectCfgRelease->includePath = projectIncludes;
                }

                if(tagName == wxT("Libs")) {
                    wxString projectLibs = tagValue;
                    projectLibs.Replace(wxT("\""), wxT(""));
                    projectLibs.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->libPath = projectLibs;
                    genericProjectCfgRelease->libPath = projectLibs;
                }

                if(tagName == wxT("Compiler")) {
                    wxString projectCompiler = tagValue;
                    projectCompiler.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->cCompilerOptions = projectCompiler;
                    genericProjectCfgRelease->cCompilerOptions = projectCompiler;
                    genericProjectCfgDebug->cppCompilerOptions = projectCompiler;
                    genericProjectCfgRelease->cppCompilerOptions = projectCompiler;
                }

                if(tagName == wxT("CppCompiler")) {
                    wxString projectCppCompiler = tagValue;
                    projectCppCompiler.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->cppCompilerOptions = projectCppCompiler;
                    genericProjectCfgRelease->cppCompilerOptions = projectCppCompiler;
                }

                if(tagName == wxT("Linker")) {
                    wxString projectLinker = tagValue;
                    projectLinker.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->linkerOptions = projectLinker;
                    genericProjectCfgRelease->linkerOptions = projectLinker;
                }
            }
        }

        if(line.Contains(wxT("[Unit"))) {
            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();

            while(!line.IsEmpty()) {
                line = tis.ReadLine();

                wxStringTokenizer part(line, wxT("="));
                wxString tagName = part.GetNextToken().Trim().Trim(false);
                wxString tagValue = part.GetNextToken().Trim().Trim(false);

                if(tagName == wxT("FileName")) {
                    wxString fileName = tagValue;
                    fileName.Replace(wxT("\\"), wxT("/"));

                    genericProjectFile->name = fileName;
                }

                if(tagName == wxT("Folder")) {
                    wxString fileFolder = tagValue;
                    genericProjectFile->vpath = fileFolder;
                }
            }

            genericProject->files.push_back(genericProjectFile);
        }
    }

    return genericWorkspace;
}
