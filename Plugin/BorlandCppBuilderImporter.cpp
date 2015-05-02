#include "BorlandCppBuilderImporter.h"
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>

BorlandCppBuilderImporter::BorlandCppBuilderImporter() {}

BorlandCppBuilderImporter::~BorlandCppBuilderImporter() {}

bool BorlandCppBuilderImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    wxString compilerName = defaultCompiler.Lower();

    IsGccCompile = compilerName.Contains(wxT("gnu")) || compilerName.Contains(wxT("gcc")) ||
                   compilerName.Contains(wxT("g++")) || compilerName.Contains(wxT("mingw"));

    wxString extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("bpr");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}

bool BorlandCppBuilderImporter::isSupportedWorkspace()
{
    wxXmlDocument projectDoc;
    if(projectDoc.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();
        if(root) {
            wxXmlNode* projectChild = root->GetChildren();
            if(projectChild) {
                wxXmlNode* macrosChild = projectChild->GetChildren();
                if(macrosChild && macrosChild->GetName() == wxT("VERSION")) {
                    wxString value = macrosChild->GetAttribute("value");
                    if(value == wxT("BCB.06.00")) return true;
                }
            }
        }
    }

    return false;
}

GenericWorkspacePtr BorlandCppBuilderImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
    genericProject->name = wsInfo.GetName();
    genericProject->path = wsInfo.GetPath();

    GenericProjectCfgPtr genericProjectCfgDebug = std::make_shared<GenericProjectCfg>();
    GenericProjectCfgPtr genericProjectCfgRelease = std::make_shared<GenericProjectCfg>();

    genericProjectCfgDebug->name = wxT("Debug");
    genericProjectCfgRelease->name = wxT("Release");

    genericProjectCfgDebug->intermediateDirectory = wxT("./Debug");
    genericProjectCfgRelease->intermediateDirectory = wxT("./Release");

    genericProject->cfgs.push_back(genericProjectCfgDebug);
    genericProject->cfgs.push_back(genericProjectCfgRelease);

    genericWorkspace->projects.push_back(genericProject);

    wxXmlDocument projectDoc;
    if(projectDoc.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();

        if(root) {
            wxXmlNode* projectChild = root->GetChildren();
            while(projectChild) {
                if(projectChild->GetName() == wxT("MACROS")) {
                    wxXmlNode* macrosChild = projectChild->GetChildren();
                    while(macrosChild) {
                        if(macrosChild->GetName() == wxT("INCLUDEPATH")) {
                            wxString projectIncludes = macrosChild->GetAttribute(wxT("value"));

                            genericProjectCfgDebug->includePath = projectIncludes;
                            genericProjectCfgRelease->includePath = projectIncludes;
                        }

                        if(macrosChild->GetName() == wxT("LIBPATH")) {
                            wxString projectLibs = macrosChild->GetAttribute(wxT("value"));

                            genericProjectCfgDebug->libPath = projectLibs;
                            genericProjectCfgRelease->libPath = projectLibs;
                        }

                        macrosChild = macrosChild->GetNext();
                    }
                }

                if(projectChild->GetName() == wxT("LINKER")) {
                    wxXmlNode* linkerChild = projectChild->GetChildren();
                    while(linkerChild) {
                        if(linkerChild->GetName() == wxT("ALLLIB")) {
                            wxString projectLibraries = linkerChild->GetAttribute(wxT("value"));

                            genericProjectCfgDebug->libraries = projectLibraries;
                            genericProjectCfgRelease->libraries = projectLibraries;
                        }

                        linkerChild = linkerChild->GetNext();
                    }
                }

                if(projectChild->GetName() == wxT("FILELIST")) {
                    wxXmlNode* fileListChild = projectChild->GetChildren();
                    while(fileListChild) {
                        if(fileListChild->GetName() == wxT("FILE")) {
                            wxString projectFilename = fileListChild->GetAttribute(wxT("FILENAME"));

                            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = projectFilename;
                            genericProject->files.push_back(genericProjectFile);
                        }

                        fileListChild = fileListChild->GetNext();
                    }
                }

                projectChild = projectChild->GetNext();
            }
        }
    }

    return genericWorkspace;
}
