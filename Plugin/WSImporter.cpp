#include "WSImporter.h"

#include "BorlandCppBuilderImporter.h"
#include "CodeBlocksImporter.h"
#include "DevCppImporter.h"
#include "EnvVarImporterDlg.h"
#include "VisualCppImporter.h"
#include "workspace.h"

#include <wx/tokenzr.h>

WSImporter::WSImporter()
{
    AddImporter(std::make_shared<VisualCppImporter>());
    AddImporter(std::make_shared<DevCppImporter>());
    AddImporter(std::make_shared<BorlandCppBuilderImporter>());
    AddImporter(std::make_shared<CodeBlocksImporter>());
}

WSImporter::~WSImporter() {}

void WSImporter::AddImporter(std::shared_ptr<GenericImporter> importer) { importers.push_back(importer); }

void WSImporter::Load(const wxString& filename, const wxString& defaultCompiler)
{
    this->filename = filename;
    this->defaultCompiler = defaultCompiler;
}

bool WSImporter::Import(wxString& errMsg)
{
    wxString compileName = defaultCompiler.Lower();
    bool isGccCompile = compileName.Contains(wxT("gnu")) || compileName.Contains(wxT("gcc")) ||
                        compileName.Contains(wxT("g++")) || compileName.Contains(wxT("mingw"));

    for(std::shared_ptr<GenericImporter> importer : importers) {
        if(importer->OpenWorkspace(filename, defaultCompiler)) {
            if(importer->isSupportedWorkspace()) {
                GenericWorkspacePtr gworskspace = importer->PerformImport();
                wxString errMsgLocal;
                bool showDlg = true;

                if(!clCxxWorkspaceST::Get()->CreateWorkspace(gworskspace->name, gworskspace->path, errMsgLocal))
                    return false;

                clCxxWorkspace* clWorkspace = NULL;
                WorkspaceConfiguration::ConfigMappingList cmlDebug;
                WorkspaceConfiguration::ConfigMappingList cmlRelease;

                for(GenericProjectPtr project : gworskspace->projects) {
                    GenericCfgType cfgType = project->cfgType;
                    wxString projectType, errMsg;

                    switch(cfgType) {
                    case GenericCfgType::DYNAMIC_LIBRARY:
                        projectType = PROJECT_TYPE_DYNAMIC_LIBRARY;
                        break;
                    case GenericCfgType::STATIC_LIBRARY:
                        projectType = PROJECT_TYPE_STATIC_LIBRARY;
                        break;
                    case GenericCfgType::EXECUTABLE:
                    default:
                        projectType = PROJECT_TYPE_EXECUTABLE;
                        break;
                    }

                    if(!clCxxWorkspaceST::Get()->CreateProject(project->name, project->path, projectType, "", true,
                                                               errMsg))
                        return false;

                    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project->name, errMsg);
                    ProjectSettingsPtr le_settings(new ProjectSettings(NULL));

                    le_settings->RemoveConfiguration(wxT("Debug"));
                    le_settings->SetProjectType(projectType);

                    if(clWorkspace == NULL)
                        clWorkspace = proj->GetWorkspace();

                    for(GenericProjectCfgPtr cfg : project->cfgs) {
                        BuildConfigPtr le_conf(new BuildConfig(NULL));

                        wxString outputFileName = wxT("");

                        switch(cfgType) {
                        case GenericCfgType::DYNAMIC_LIBRARY:
                            outputFileName = wxT("$(IntermediateDirectory)/$(ProjectName)");
                            outputFileName += DYNAMIC_LIBRARY_EXT;
                            break;
                        case GenericCfgType::STATIC_LIBRARY:
                            outputFileName = wxT("$(IntermediateDirectory)/$(ProjectName)");
                            outputFileName += STATIC_LIBRARY_EXT;
                            if(isGccCompile && outputFileName.Contains(wxT(".lib"))) {
                                outputFileName.Replace(wxT(".lib"), wxT(".a"));
                            }
                            break;
                        case GenericCfgType::EXECUTABLE:
                            outputFileName = wxT("$(IntermediateDirectory)/$(ProjectName)");
                            outputFileName += EXECUTABLE_EXT;
                            break;
                        }

                        std::vector<wxString> envVarElems;

                        le_conf->SetName(cfg->name);

                        if(!cfg->includePath.IsEmpty()) {
                            envVarElems.push_back(cfg->includePath);
                            le_conf->SetIncludePath(cfg->includePath);
                        }

                        if(!cfg->libraries.IsEmpty()) {
                            envVarElems.push_back(cfg->libraries);
                            if(isGccCompile && cfg->libraries.Contains(wxT(".lib"))) {
                                cfg->libraries.Replace(wxT(".lib"), wxT(".a"));
                            }
                            le_conf->SetLibraries(cfg->libraries);
                        }

                        if(!cfg->libPath.IsEmpty()) {
                            envVarElems.push_back(cfg->libPath);
                            le_conf->SetLibPath(cfg->libPath);
                        }

                        if(!cfg->preprocessor.IsEmpty()) {
                            envVarElems.push_back(cfg->preprocessor);
                            le_conf->SetPreprocessor(cfg->preprocessor);
                        }

                        if(!cfg->intermediateDirectory.IsEmpty())
                            le_conf->SetIntermediateDirectory(cfg->intermediateDirectory);

                        if(!cfg->outputFilename.IsEmpty()) {
                            if(isGccCompile && cfg->outputFilename.Contains(wxT(".lib"))) {
                                cfg->outputFilename.Replace(wxT(".lib"), wxT(".a"));
                            }
                            le_conf->SetOutputFileName(cfg->outputFilename);
                        } else
                            le_conf->SetOutputFileName(outputFileName);

                        if(!cfg->cCompilerOptions.IsEmpty())
                            le_conf->SetCCompileOptions(cfg->cCompilerOptions);

                        if(!cfg->cppCompilerOptions.IsEmpty())
                            le_conf->SetCompileOptions(cfg->cppCompilerOptions);

                        if(!cfg->linkerOptions.IsEmpty())
                            le_conf->SetLinkOptions(cfg->linkerOptions);

                        if(!cfg->preCompiledHeader.IsEmpty())
                            le_conf->SetPrecompiledHeader(cfg->preCompiledHeader);

                        wxString outputFileNameCommand, outputFileNameWorkingDirectory;
                        if(!cfg->outputFilename.IsEmpty()) {
                            wxFileName outputFileNameInfo(cfg->outputFilename);
                            outputFileNameCommand = wxT("./") + outputFileNameInfo.GetFullName();
                            outputFileNameWorkingDirectory = outputFileNameInfo.GetPath();
                            outputFileNameWorkingDirectory.Replace(wxT("\\"), wxT("/"));
                        }

                        if(!cfg->command.IsEmpty())
                            le_conf->SetCommand(cfg->command);
                        else if(!outputFileNameCommand.IsEmpty())
                            le_conf->SetCommand(outputFileNameCommand);
                        else
                            le_conf->SetCommand(wxT("./$(ProjectName)"));

                        if(!cfg->workingDirectory.IsEmpty())
                            le_conf->SetWorkingDirectory(cfg->workingDirectory);
                        else if(!outputFileNameWorkingDirectory.IsEmpty())
                            le_conf->SetWorkingDirectory(outputFileNameWorkingDirectory);
                        else
                            le_conf->SetWorkingDirectory(wxT("./$(IntermediateDirectory)"));

                        le_conf->SetCompilerType(defaultCompiler);

                        wxString cfgName = cfg->name.Lower();

                        if(cfgName.Contains(wxT("debug"))) {
                            ConfigMappingEntry cme(project->name, cfg->name);
                            cmlDebug.push_back(cme);
                        } else if(cfgName.Contains(wxT("release"))) {
                            ConfigMappingEntry cme(project->name, cfg->name);
                            cmlRelease.push_back(cme);
                        } else {
                            ConfigMappingEntry cme(project->name, cfg->name);
                            cmlDebug.push_back(cme);
                            cmlRelease.push_back(cme);
                        }

                        wxString buildConfigType;

                        switch(cfgType) {
                        case GenericCfgType::DYNAMIC_LIBRARY:
                            buildConfigType = PROJECT_TYPE_DYNAMIC_LIBRARY;
                            break;
                        case GenericCfgType::STATIC_LIBRARY:
                            buildConfigType = PROJECT_TYPE_STATIC_LIBRARY;
                            break;
                        case GenericCfgType::EXECUTABLE:
                        default:
                            buildConfigType = PROJECT_TYPE_EXECUTABLE;
                            break;
                        }

                        le_conf->SetProjectType(buildConfigType);

                        if(showDlg) {
                            if(envVarElems.size() > 0 && ContainsEnvVar(envVarElems)) {
                                std::set<wxString> listEnvVar = GetListEnvVarName(envVarElems);

                                for(GenericEnvVarsValueType envVar : cfg->envVars) {
                                    listEnvVar.erase(envVar.first);
                                }

                                if(listEnvVar.size() > 0) {
                                    EnvVarImporterDlg envVarImporterDlg(NULL, project->name, cfg->name, listEnvVar,
                                                                        le_conf, &showDlg);
                                    envVarImporterDlg.ShowModal();
                                }
                            }
                        }

                        wxString envVars =
                            !le_conf->GetEnvvars().IsEmpty() ? le_conf->GetEnvvars() + wxT("\n") : wxT("");

                        for(GenericEnvVarsValueType envVar : cfg->envVars) {
                            envVars += envVar.first + wxT("=") + envVar.second + wxT("\n");
                        }

                        le_conf->SetEnvvars(envVars);

                        BuildCommandList preBuildCommandList;
                        BuildCommandList postBuildCommandList;

                        for(wxString preBuildCmd : cfg->preBuildCommands) {
                            BuildCommand preBuildCommand;
                            preBuildCommand.SetCommand(preBuildCmd);
                            preBuildCommand.SetEnabled(true);

                            preBuildCommandList.push_back(preBuildCommand);
                        }

                        for(wxString postBuildCmd : cfg->postBuildCommands) {
                            BuildCommand postBuildCommand;
                            postBuildCommand.SetCommand(postBuildCmd);
                            postBuildCommand.SetEnabled(true);

                            postBuildCommandList.push_back(postBuildCommand);
                        }

                        le_conf->SetPreBuildCommands(preBuildCommandList);
                        le_conf->SetPostBuildCommands(postBuildCommandList);

                        if(cfg->enableCustomBuild) {
                            le_conf->EnableCustomBuild(cfg->enableCustomBuild);
                            le_conf->SetCustomBuildCmd(cfg->customBuildCmd);
                            le_conf->SetCustomCleanCmd(cfg->customCleanCmd);
                            le_conf->SetCustomRebuildCmd(cfg->customRebuildCmd);
                        }

                        le_settings->SetBuildConfiguration(le_conf);

                        if(!project->deps.IsEmpty())
                            proj->SetDependencies(project->deps, cfg->name);
                    }

                    proj->SetSettings(le_settings);

                    proj->BeginTranscation();

                    // Delete default virtual directory
                    proj->DeleteVirtualDir("include");
                    proj->DeleteVirtualDir("src");

                    for(GenericProjectFilePtr file : project->files) {
                        wxString vpath = GetVPath(file->name, file->vpath);

                        wxString vDir = wxT("");
                        wxStringTokenizer vDirList(vpath, wxT(":"));
                        while(vDirList.HasMoreTokens()) {
                            wxString vdName = vDirList.NextToken();
                            vDir += vdName;
                            proj->CreateVirtualDir(vDir);
                            vDir += wxT(":");
                        }

                        wxFileName fileNameInfo(project->path + wxFileName::GetPathSeparator() + file->name);
                        fileNameInfo.Normalize(wxPATH_NORM_DOTS);
                        proj->AddFile(fileNameInfo.GetFullPath(), vpath);
                    }

                    proj->CommitTranscation();

                    for(GenericProjectCfgPtr cfg : project->cfgs) {
                        for(GenericProjectFilePtr excludeFile : cfg->excludeFiles) {
                            wxFileName excludeFileNameInfo(project->path + wxFileName::GetPathSeparator() +
                                                           excludeFile->name);
                            excludeFileNameInfo.Normalize(wxPATH_NORM_DOTS);
                            proj->AddExcludeConfigForFile(excludeFileNameInfo.GetFullPath());
                        }
                    }
                }

                if(clWorkspace) {
                    BuildMatrixPtr clMatrix = clWorkspace->GetBuildMatrix();

                    WorkspaceConfigurationPtr wsconf = clMatrix->GetConfigurationByName(wxT("Debug"));
                    if(wsconf) {
                        wsconf->SetConfigMappingList(cmlDebug);
                    }

                    wsconf = clMatrix->GetConfigurationByName(wxT("Release"));
                    if(wsconf) {
                        wsconf->SetConfigMappingList(cmlRelease);
                    }

                    clWorkspace->SetBuildMatrix(clMatrix);
                }

                return true;
            }
        }
    }

    return false;
}

bool WSImporter::ContainsEnvVar(std::vector<wxString> elems)
{
    for(wxString elem : elems) {
        if(elem.Contains("$(") && elem.Contains(")"))
            return true;
    }

    return false;
}

std::set<wxString> WSImporter::GetListEnvVarName(std::vector<wxString> elems)
{
    bool app = false;
    wxString word = wxT(""), data = wxT("");
    std::set<wxString> list;

    for(wxString elem : elems) {
        if(!elem.IsEmpty()) {
            data += elem;
        }
    }

    const int length = data.length();

    for(int pos = 0; pos < length; pos++) {
        if(data.GetChar(pos) == wxT('$') && data.GetChar(pos + 1) == wxT('(')) {
            app = true;
            pos++;
        } else if(data.GetChar(pos) == wxT(')')) {
            if(!word.IsEmpty()) {
                list.insert(word);
                word = wxT("");
                app = false;
            }
        } else if(app) {
            word += data.GetChar(pos);
        }
    }

    return list;
}

wxString WSImporter::GetVPath(const wxString& filename, const wxString& virtualPath)
{
    wxString vpath;
    if(virtualPath.IsEmpty()) {
        wxFileName fileInfo(filename);
        wxString ext = fileInfo.GetExt().Lower();

        if(ext == wxT("h") || ext == wxT("hpp") || ext == wxT("hxx") || ext == wxT("hh") || ext == wxT("inl") ||
           ext == wxT("inc")) {
            vpath = wxT("include");
        } else if(ext == wxT("c") || ext == wxT("cpp") || ext == wxT("cxx") || ext == wxT("cc")) {
            vpath = wxT("src");
        } else if(ext == wxT("s") || ext == wxT("S") || ext == wxT("asm")) {
            vpath = wxT("src");
        } else {
            vpath = wxT("resource");
        }
    } else {
        vpath = virtualPath;

        if(vpath.Contains(wxT("\\"))) {
            vpath.Replace(wxT("\\"), wxT(":"));
        } else if(vpath.Contains(wxT("/"))) {
            vpath.Replace(wxT("/"), wxT(":"));
        }
    }

    return vpath;
}