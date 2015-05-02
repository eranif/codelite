#include "DevCppImporter.h"
#include <wx/tokenzr.h>

DevCppImporter::DevCppImporter() {}

DevCppImporter::~DevCppImporter() {}

bool DevCppImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    wxString compilerName = defaultCompiler.Lower();

    IsGccCompile = compilerName.Contains(wxT("gnu")) || compilerName.Contains(wxT("gcc")) ||
                   compilerName.Contains(wxT("g++")) || compilerName.Contains(wxT("mingw"));

    wxString extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("dev");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}

bool DevCppImporter::isSupportedWorkspace() { return true; }

GenericWorkspacePtr DevCppImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    fis = std::make_shared<wxFileInputStream>(wsInfo.GetFullPath());
    tis = std::make_shared<wxTextInputStream>(*fis);

    GenericProjectPtr genericProject;
    GenericProjectCfgPtr genericProjectCfgDebug;
    GenericProjectCfgPtr genericProjectCfgRelease;

    while(!fis->Eof()) {
        wxString line = tis->ReadLine();

        if(line.Contains(wxT("[Project]"))) {
            while(!line.IsEmpty()) {
                line = tis->ReadLine();
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

                    wxString outputFilename;
                    if(projectType == wxT("2")) {
                        genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                        outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
                        outputFilename += STATIC_LIBRARY_EXT;
                        if(IsGccCompile) outputFilename.Replace(wxT("lib"), wxT("a"));
                    } else if(projectType == wxT("3")) {
                        genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                        outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
                        outputFilename += DYNAMIC_LIBRARY_EXT;
                    } else {
                        genericProject->cfgType = GenericCfgType::EXECUTABLE;
                        outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
                        outputFilename += EXECUTABLE_EXT;
                    }

                    genericProjectCfgDebug->type = genericProject->cfgType;
                    genericProjectCfgRelease->type = genericProject->cfgType;

                    genericProjectCfgDebug->outputFilename = outputFilename;
                    genericProjectCfgRelease->outputFilename = outputFilename;
                }

                if(tagName == wxT("Includes")) {
                    wxString projectIncludes = tagValue;

                    genericProjectCfgDebug->includePath = projectIncludes;
                    genericProjectCfgRelease->includePath = projectIncludes;
                }

                if(tagName == wxT("Libs")) {
                    wxString projectLibs = tagValue;

                    genericProjectCfgDebug->libPath = projectLibs;
                    genericProjectCfgRelease->libPath = projectLibs;
                }

                if(tagName == wxT("Compiler")) {
                    wxString projectCompiler = tagValue;
                    projectCompiler.Replace(wxT(" "), wxT(";"));

                    genericProjectCfgDebug->cCompilerOptions = projectCompiler;
                    genericProjectCfgRelease->cCompilerOptions = projectCompiler;
                }

                if(tagName == wxT("CppCompiler")) {
                    wxString projectCppCompiler = tagValue;
                    projectCppCompiler.Replace(wxT(" "), wxT(";"));

                    genericProjectCfgDebug->cppCompilerOptions = projectCppCompiler;
                    genericProjectCfgRelease->cppCompilerOptions = projectCppCompiler;
                }

                if(tagName == wxT("Linker")) {
                    wxString projectLinker = tagValue;
                    projectLinker.Replace(wxT(" "), wxT(";"));

                    genericProjectCfgDebug->linkerOptions = projectLinker;
                    genericProjectCfgRelease->linkerOptions = projectLinker;
                }
            }
        }

        if(line.Contains(wxT("[Unit"))) {
            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();

            while(!line.IsEmpty()) {
                line = tis->ReadLine();

                wxStringTokenizer part(line, wxT("="));
                wxString tagName = part.GetNextToken().Trim().Trim(false);
                wxString tagValue = part.GetNextToken().Trim().Trim(false);

                if(tagName == wxT("FileName")) {
                    wxString fileName = tagValue;
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
