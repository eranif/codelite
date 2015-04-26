#include "WSImporter.h"
#include "VisualCppImporter.h"
#include "workspace.h"

WSImporter::WSImporter() {
	AddImporter(std::make_shared<VisualCppImporter>());
}

WSImporter::~WSImporter() {
}

void WSImporter::AddImporter(std::shared_ptr<GenericImporter> importer) {
	importers.push_back(importer);
}

void WSImporter::Load(const wxString& filename, const wxString& defaultCompiler) {
	this->filename = filename;
	this->defaultCompiler = defaultCompiler;
}

bool WSImporter::Import(wxString& errMsg) {
	for(std::shared_ptr<GenericImporter> importer : importers) {
		if (importer->OpenWordspace(filename, defaultCompiler)) {
			if (importer->isSupportedWorkspace()) {
				GenericWorkspace gworskspace = importer->PerformImport();
				wxString errMsgLocal;
				if (!WorkspaceST::Get()->CreateWorkspace(gworskspace.name, gworskspace.path, errMsgLocal)) {
					return false;
				}
				
				for (GenericProject project : gworskspace.projects) {
					GenericCfgType cfgType = project.cfgType;
					
					wxString projectType;
					wxString errMsg;
					switch (cfgType) {
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
					
					if (!WorkspaceST::Get()->CreateProject(project.name, project.path, projectType, true, errMsg)) {
						return false;
					}
					
					ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project.name, errMsg);
					ProjectSettingsPtr le_settings(new ProjectSettings(NULL));
					
					le_settings->RemoveConfiguration(wxT("Debug"));
					le_settings->SetProjectType(projectType);
					
					BuildMatrixPtr matrix = proj->GetWorkspace()->GetBuildMatrix();
					
					for (GenericProjectCfg cfg : project.cfgs) {
						BuildConfigPtr le_conf(new BuildConfig(NULL));
						le_conf->SetName(cfg.name);
						
						if (!cfg.includePath.IsEmpty())
							le_conf->SetIncludePath(cfg.includePath);
						
						if (!cfg.libraries.IsEmpty())
							le_conf->SetLibraries(cfg.libraries);
							
						if (!cfg.libPath.IsEmpty())
							le_conf->SetLibPath(cfg.libPath);
						
						if (!cfg.preprocessor.IsEmpty()) 
							le_conf->SetPreprocessor(cfg.preprocessor);
						
						if (!cfg.intermediateDirectory.IsEmpty())
							le_conf->SetIntermediateDirectory(cfg.intermediateDirectory);
							
						if (!cfg.outputFilename.IsEmpty())
							le_conf->SetOutputFileName(cfg.outputFilename);
						
						le_conf->SetCompilerType(defaultCompiler);
						
						WorkspaceConfiguration::ConfigMappingList cml;
						ConfigMappingEntry cme(project.name, cfg.name);
						cml.push_back(cme);
						
						WorkspaceConfigurationPtr wsconf = new WorkspaceConfiguration(NULL);
						wsconf->SetName(cfg.name);
						wsconf->SetConfigMappingList(cml);

						matrix->SetConfiguration(wsconf);
						
						wxString buildConfigType;
						
						switch (cfg.type) {
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
					}
					
					// Delete default configuration
					matrix->RemoveConfiguration("Debug");
					matrix->RemoveConfiguration("Release");
					
					proj->GetWorkspace()->SetBuildMatrix(matrix);
					
					proj->SetSettings(le_settings);
					
					proj->BeginTranscation();
					
					// Delete default virtual directory
					proj->DeleteVirtualDir("include");
					proj->DeleteVirtualDir("src");
					
					for (GenericProjectFile file : project.files) {
						proj->CreateVirtualDir(file.vpath);
						proj->AddFile(file.name, file.vpath);
					}
					
					proj->CommitTranscation();
				}
				
				return true;
			}
		}
	}
	
	return false;
}
