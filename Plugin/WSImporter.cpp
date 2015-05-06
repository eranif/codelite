#include "WSImporter.h"
#include "VisualCppImporter.h"
#include "DevCppImporter.h"
#include "BorlandCppBuilderImporter.h"
#include "CodeBlocksImporter.h"
#include "EnvVarImporterDlg.h"
#include "workspace.h"

WSImporter::WSImporter()
{
    AddImporter(std::make_shared<VisualCppImporter>());
    AddImporter(std::make_shared<DevCppImporter>());
    AddImporter(std::make_shared<BorlandCppBuilderImporter>());
    AddImporter(std::make_shared<CodeBlocksImporter>());
}

WSImporter::~WSImporter()
{
}

void WSImporter::AddImporter(std::shared_ptr<GenericImporter> importer)
{
    importers.push_back(importer);
}

void WSImporter::Load(const wxString& filename, const wxString& defaultCompiler)
{
    this->filename = filename;
    this->defaultCompiler = defaultCompiler;
}

bool WSImporter::Import(wxString& errMsg)
{
    for(std::shared_ptr<GenericImporter> importer : importers) {
        if(importer->OpenWordspace(filename, defaultCompiler)) {
            if(importer->isSupportedWorkspace()) {
                GenericWorkspacePtr gworskspace = importer->PerformImport();
                wxString errMsgLocal;
                bool showDlg = true;

                if(!WorkspaceST::Get()->CreateWorkspace(gworskspace->name, gworskspace->path, errMsgLocal))
                    return false;

                Workspace* clWorkspace = NULL;
                WorkspaceConfiguration::ConfigMappingList cmlDebug;
                WorkspaceConfiguration::ConfigMappingList cmlRelease;

                for(GenericProjectPtr project : gworskspace->projects) {
                    GenericCfgType cfgType = project->cfgType;
                    wxString projectType, errMsg;

                    switch(cfgType) {
                    case GenericCfgType::DYNAMIC_LIBRARY:
                        projectType = Project::DYNAMIC_LIBRARY;
                        break;
                    case GenericCfgType::STATIC_LIBRARY:
                        projectType = Project::STATIC_LIBRARY;
                        break;
                    case GenericCfgType::EXECUTABLE:
                    default:
                        projectType = Project::EXECUTABLE;
                        break;
                    }

                    if(!WorkspaceST::Get()->CreateProject(project->name, project->path, projectType, true, errMsg))
                        return false;

                    ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project->name, errMsg);
                    ProjectSettingsPtr le_settings(new ProjectSettings(NULL));

                    le_settings->RemoveConfiguration(wxT("Debug"));
                    le_settings->SetProjectType(projectType);

                    if(clWorkspace == NULL)
                        clWorkspace = proj->GetWorkspace();

                    for(GenericProjectCfgPtr cfg : project->cfgs) {
                        BuildConfigPtr le_conf(new BuildConfig(NULL));

                        if(showDlg &&
                           ContainsEnvVar({ cfg->includePath, cfg->libPath, cfg->libraries, cfg->preprocessor })) {
                            std::set<wxString> listEnvVar = GetListEnvVarName(
                                { cfg->includePath, cfg->libPath, cfg->libraries, cfg->preprocessor });
                                
                            for(GenericEnvVarsValueType envVar : cfg->envVars) {
                                listEnvVar.erase(envVar.first);
                            }

                            EnvVarImporterDlg envVarImporterDlg(
                                NULL, project->name, cfg->name, listEnvVar, le_conf, &showDlg);
                            envVarImporterDlg.ShowModal();
                        }
                        
                        wxString envVars = le_conf->GetEnvvars() + wxT("\n");
                        
                        for(GenericEnvVarsValueType envVar : cfg->envVars) {
                            envVars += envVar.first + wxT("=") + envVar.second;
                        }
                        
                        le_conf->SetEnvvars(envVars);

                        le_conf->SetName(cfg->name);

                        if(!cfg->includePath.IsEmpty())
                            le_conf->SetIncludePath(cfg->includePath);

                        if(!cfg->libraries.IsEmpty())
                            le_conf->SetLibraries(cfg->libraries);

                        if(!cfg->libPath.IsEmpty())
                            le_conf->SetLibPath(cfg->libPath);

                        if(!cfg->preprocessor.IsEmpty())
                            le_conf->SetPreprocessor(cfg->preprocessor);

                        if(!cfg->intermediateDirectory.IsEmpty())
                            le_conf->SetIntermediateDirectory(cfg->intermediateDirectory);

                        if(!cfg->outputFilename.IsEmpty())
                            le_conf->SetOutputFileName(cfg->outputFilename);
                        else
                            le_conf->SetOutputFileName(wxT("$(IntermediateDirectory)/$(ProjectName)"));

                        if(!cfg->cCompilerOptions.IsEmpty())
                            le_conf->SetCCompileOptions(cfg->cCompilerOptions);

                        if(!cfg->cppCompilerOptions.IsEmpty())
                            le_conf->SetCompileOptions(cfg->cppCompilerOptions);

                        if(!cfg->linkerOptions.IsEmpty())
                            le_conf->SetLinkOptions(cfg->linkerOptions);

                        if(!cfg->preCompiledHeader.IsEmpty())
                            le_conf->SetPrecompiledHeader(cfg->preCompiledHeader);

                        if(!cfg->command.IsEmpty())
                            le_conf->SetCommand(cfg->command);
                        else
                            le_conf->SetCommand(wxT("./$(ProjectName)"));

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

                        switch(cfg->type) {
                        case GenericCfgType::DYNAMIC_LIBRARY:
                            buildConfigType = Project::DYNAMIC_LIBRARY;
                            break;
                        case GenericCfgType::STATIC_LIBRARY:
                            buildConfigType = Project::STATIC_LIBRARY;
                            break;
                        case GenericCfgType::EXECUTABLE:
                        default:
                            buildConfigType = Project::EXECUTABLE;
                            break;
                        }

                        le_conf->SetProjectType(buildConfigType);
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
                        wxString vpath;

                        if(file->vpath.IsEmpty()) {
                            wxFileName fileInfo(file->name);
                            wxString ext = fileInfo.GetExt().Lower();

                            if(ext == wxT("h") || ext == wxT("hpp") || ext == wxT("hxx") || ext == wxT("hh") ||
                               ext == wxT("inl") || ext == wxT("inc")) {
                                vpath = wxT("include");
                            } else if(ext == wxT("c") || ext == wxT("cpp") || ext == wxT("cxx") || ext == wxT("cc")) {
                                vpath = wxT("src");
                            } else if(ext == wxT("s") || ext == wxT("asm")) {
                                vpath = wxT("src");
                            } else {
                                vpath = wxT("resource");
                            }
                        } else {
                            vpath = file->vpath;

                            if(file->vpath.Contains(wxT("\\"))) {
                                vpath.Replace(wxT("\\"), wxT(":"));
                            } else if(file->vpath.Contains(wxT("/"))) {
                                vpath.Replace(wxT("/"), wxT(":"));
                            }
                        }

                        proj->CreateVirtualDir(vpath);
                        proj->AddFile(file->name, vpath);
                    }

                    proj->CommitTranscation();
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

bool WSImporter::ContainsEnvVar(std::initializer_list<wxString> elems)
{
    for(wxString elem : elems) {
        if(elem.Contains("$(") && elem.Contains(")"))
            return true;
    }

    return false;
}

std::set<wxString> WSImporter::GetListEnvVarName(std::initializer_list<wxString> elems)
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
            list.insert(word);
            word = wxT("");
            app = false;
        } else if(app) {
            word += data.GetChar(pos);
        }
    }

    return list;
}