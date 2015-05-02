#include "CodeBlocksImporter.h"
#include <wx/xml/xml.h>
#include <wx/filefn.h>

CodeBlocksImporter::CodeBlocksImporter() {}

CodeBlocksImporter::~CodeBlocksImporter() {}

bool CodeBlocksImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler) {
	wsInfo.Assign(filename);

    wxString compilerName = defaultCompiler.Lower();

    IsGccCompile = compilerName.Contains(wxT("gnu")) || compilerName.Contains(wxT("gcc")) ||
                   compilerName.Contains(wxT("g++")) || compilerName.Contains(wxT("mingw"));

    extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("cbp") || extension == wxT("workspace");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}

bool CodeBlocksImporter::isSupportedWorkspace() {
	wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
		wxString nodeName = root->GetName();
        if(root && (nodeName == wxT("CodeBlocks_workspace_file") || nodeName == wxT("CodeBlocks_project_file")))
			return true;
	}
	return false;
}

GenericWorkspacePtr CodeBlocksImporter::PerformImport() {
	GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();
	
	if (extension == wxT("cbp")) {
		GenerateFromProject(genericWorkspace, wsInfo.GetFullPath());
	}
	else if (extension == wxT("workspace")) {
		GenerateFromWorkspace(genericWorkspace);
	}
	
	return genericWorkspace;
}

void CodeBlocksImporter::GenerateFromProject(GenericWorkspacePtr genericWorkspace, const wxString& fullpath) {
	wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(fullpath)) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        if(root) {
			wxXmlNode* rootChild = root->GetChildren();
			
			while(rootChild) {
				if(rootChild->GetName() == wxT("Project")) {
					wxFileName projectInfo(fullpath);
					GenericProjectPtr genericProject = std::make_shared<GenericProject>();
					genericProject->path = projectInfo.GetPath();
					
					genericWorkspace->projects.push_back(genericProject);
					
					wxXmlNode* projectChild = rootChild->GetChildren();
					
					while(projectChild) {
						if(projectChild->GetName() == wxT("Option") && projectChild->HasAttribute(wxT("title"))) {
							genericProject->name = projectChild->GetAttribute(wxT("title"));
						}
						
						if(projectChild->GetName() == wxT("Build")) {
							wxXmlNode* buildChild = projectChild->GetChildren();
							
							while(buildChild) {
								if(buildChild->GetName() == wxT("Target") && buildChild->HasAttribute(wxT("title"))) {
									GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
									genericProjectCfg->name = buildChild->GetAttribute(wxT("title"));
									genericProject->cfgs.push_back(genericProjectCfg);
									
									wxXmlNode* targetChild = buildChild->GetChildren();
									while(targetChild) {
										if(targetChild->GetName() == wxT("Option") && targetChild->HasAttribute(wxT("type"))) {
											wxString projectType = targetChild->GetAttribute(wxT("type"));
											
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
											
											genericProjectCfg->outputFilename = outputFilename;
											genericProjectCfg->type = genericProject->cfgType;
										}
										
										if(targetChild->GetName() == wxT("Compiler")) {
											wxString compilerOptions = wxT(""), includePath = wxT("");
											
											wxXmlNode* compilerChild = targetChild->GetChildren();
											while(compilerChild) {
												if(compilerChild->GetName() == wxT("Add") && compilerChild->HasAttribute(wxT("option"))) {
													compilerOptions += compilerChild->GetAttribute(wxT("option")) + wxT(" ");
												}
												
												if(compilerChild->GetName() == wxT("Add") && compilerChild->HasAttribute(wxT("directory"))) {
													includePath += compilerChild->GetAttribute(wxT("directory")) + wxT(";");
												}
												
												compilerChild = compilerChild->GetNext();
											}
											
											genericProjectCfg->cCompilerOptions = compilerOptions;
											genericProjectCfg->cppCompilerOptions = compilerOptions;
											genericProjectCfg->includePath = includePath;
										}
										
										if(targetChild->GetName() == wxT("Linker")) {
											wxString linkerOptions = wxT(""), libPath = wxT(""), libraries = wxT("");
											
											wxXmlNode* linkerChild = targetChild->GetChildren();
											while(linkerChild) {
												if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("option"))) {
													linkerOptions += linkerChild->GetAttribute(wxT("option")) + wxT(" ");
												}
												
												if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("directory"))) {
													libPath += linkerChild->GetAttribute(wxT("directory")) + wxT(";");
												}
												
												if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("library"))) {
													libraries += linkerChild->GetAttribute(wxT("library")) + wxT(";");
												}
												
												linkerChild = linkerChild->GetNext();
											}
											
											genericProjectCfg->linkerOptions = linkerOptions;
											genericProjectCfg->libPath = libPath;
											genericProjectCfg->libraries = libraries;
										}
										
										targetChild = targetChild->GetNext();
									}
								}
								
								buildChild = buildChild->GetNext();
							}
						}
						
						if(projectChild->GetName() == wxT("Unit") && projectChild->HasAttribute(wxT("filename"))) {
							wxString vpath = wxT("");
							wxXmlNode* unitChild = projectChild->GetChildren();
							
							while(unitChild) {
								if(unitChild->GetName() == wxT("Option") && unitChild->HasAttribute(wxT("virtualFolder"))) {
									vpath = unitChild->GetAttribute(wxT("virtualFolder"));
								}
								unitChild = unitChild->GetNext();
							}
							
							wxString projectFilename = projectChild->GetAttribute(wxT("filename"));
							GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = projectFilename;
							genericProjectFile->vpath = vpath;
							genericProject->files.push_back(genericProjectFile);
						}
						
						projectChild = projectChild->GetNext();
					}
				}
				
				rootChild = rootChild->GetNext();
			}
		}
	}
}

void CodeBlocksImporter::GenerateFromWorkspace(GenericWorkspacePtr genericWorkspace) {
	wxCopyFile(wsInfo.GetFullPath(), wsInfo.GetFullPath() + wxT(".backup"));
	wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        if(root) {
			wxXmlNode* rootChild = root->GetChildren();
			
			while(rootChild) {
				if(rootChild->GetName() == wxT("Workspace")) {					
					wxXmlNode* workspaceChild = rootChild->GetChildren();
					
					while(workspaceChild) {
						if(workspaceChild->GetName() == wxT("Project") && workspaceChild->HasAttribute(wxT("filename"))) {
							wxString projectFilename = workspaceChild->GetAttribute(wxT("filename"));
							GenerateFromProject(genericWorkspace, wsInfo.GetPath() + wxFileName::GetPathSeparator() + projectFilename);
						}
						
						workspaceChild = workspaceChild->GetNext();
					}
				}
				
				rootChild = rootChild->GetNext();
			}
		}
	}
}

