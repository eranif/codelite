#include "VisualCppImporter.h"
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>

#ifdef __WXMSW__
#define STATIC_LIBRARY_EXT wxT(".lib")
#define DYNAMIC_LIBRARY_EXT wxT(".dll")
#define EXECUTABLE_EXT wxT(".exe")
#else
#define STATIC_LIBRARY_EXT wxT(".a")
#define DYNAMIC_LIBRARY_EXT wxT(".so")
#define EXECUTABLE_EXT wxT("")
#endif

VisualCppImporter::VisualCppImporter() {
}

VisualCppImporter::~VisualCppImporter() {
}

bool VisualCppImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler) {
	wsInfo.Assign(filename);
	
	wxString compilerName = defaultCompiler.Lower();
	
	IsGccCompile = compilerName.Contains(wxT("gnu")) || compilerName.Contains(wxT("gcc")) || compilerName.Contains(wxT("g++")) | compilerName.Contains(wxT("mingw"));
	
	wxString extension = wsInfo.GetExt().Lower();
	
	bool isValidExt = extension == wxT("dsw") || extension == wxT("sln");
	bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;
	
	if (result) {
		fis = std::make_shared<wxFileInputStream>(filename);
		tis = std::make_shared<wxTextInputStream>(*fis);
		
		while(!fis->Eof()) {
			wxString line = tis->ReadLine();
			int index = line.Find(wxT("Format Version"));
			if (index != wxNOT_FOUND) {
				wxString value = line.Mid(index + 14).Trim().Trim(false);
				value.ToLong(&version);
				break;
			}
		}
		
		result = fis->IsOk();
	}
	
	return result;
}

bool VisualCppImporter::isSupportedWorkspace() {
	return version >= 6 && version <= 12;
}

GenericWorkspace VisualCppImporter::PerformImport() {
	GenericWorkspace genericWorkspace;
	switch(version) {
		case 6:
			GenerateFromVC6(genericWorkspace);
			break;
		case 7:
		case 8:
		case 9:
		case 10:
			GenerateFromVC7(genericWorkspace);
			break;
		case 11:
		case 12:
			GenerateFromVC11(genericWorkspace);
			break;
	}
	
	return genericWorkspace;
}

void VisualCppImporter::GenerateFromVC6(GenericWorkspace& genericWorkspace) {
	genericWorkspace.name = wsInfo.GetName();
	genericWorkspace.path = wsInfo.GetPath();
	
	while(!fis->Eof()) {
		wxString line = tis->ReadLine();
		int index = line.Find(wxT("Project:"));
		if (index != wxNOT_FOUND) {
			int end = line.Find(wxT("- Package Owner")) - 1;
			wxString value = line.SubString(index + 8, end).Trim().Trim(false);
			value.Replace(wxT("\""), wxT(""));
			value.Replace(wxT(".\\"), wxT(""));
			
			wxStringTokenizer part(value, wxT("="));
			
			wxString projectName = part.GetNextToken().Trim().Trim(false);
			wxString projectFile = part.GetNextToken().Trim().Trim(false);
			
			wxFileName projectInfo(wsInfo.GetPath() + wxFileName::GetPathSeparator() + projectFile);
			wxFileInputStream projectFIS(projectInfo.GetFullPath());
			
			if (projectFIS.IsOk()) {
				GenericProject genericProject;
				genericProject.name = projectName;
				genericProject.path = projectInfo.GetPath();
				
				wxTextInputStream projectTIS(projectFIS);
				
				wxString virtualPath = wxT("");
				wxString outputFilename = wxT("");
				
				while(!projectFIS.Eof()) {
					line = projectTIS.ReadLine();
					
					index = line.Find(wxT("TARGTYPE"));
					if (index != wxNOT_FOUND) {
						index = line.find_last_of(wxT("\""));
						wxString projectType = line.Mid(index + 1).Trim().Trim(false);
						if (projectType == wxT("0x0104")) {
							genericProject.cfgType = GenericCfgType::STATIC_LIBRARY;
							outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
							outputFilename += STATIC_LIBRARY_EXT;
							if (IsGccCompile) 
								outputFilename.Replace(wxT("lib"), wxT("a"));
						}
						else if (projectType == wxT("0x0102")) {
							genericProject.cfgType = GenericCfgType::DYNAMIC_LIBRARY;
							outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
							outputFilename += DYNAMIC_LIBRARY_EXT;
						}
						else {
							genericProject.cfgType = GenericCfgType::EXECUTABLE;
							outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
							outputFilename += EXECUTABLE_EXT;
						}
					}
					
					index = line.Find(wxT("\"$(CFG)\" == "));
					if (index != wxNOT_FOUND) {
						wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
						projectCfgName.Replace(projectName + wxT(" - "), wxT(""));
						projectCfgName.Replace(wxT("\""), wxT(""));
						projectCfgName.Replace(wxT(" "), wxT("_"));
						
						GenericProjectCfg genericProjectCfg;
						genericProjectCfg.name = projectCfgName;
						genericProjectCfg.type = genericProject.cfgType;
						genericProjectCfg.outputFilename = outputFilename;
						
						while(!projectFIS.Eof()) {
							line = projectTIS.ReadLine();
							
							index = line.Find(wxT("PROP Intermediate_Dir"));
							if (index != wxNOT_FOUND) {
								wxString intermediateDirectory = line.Mid(index + 21).Trim().Trim(false);
								intermediateDirectory.Replace(wxT("\""), wxT(""));
								intermediateDirectory.Replace(wxT(" "), wxT("_"));
								intermediateDirectory.Replace(wxT("\\"), wxT("/"));
								genericProjectCfg.intermediateDirectory = wxT("./") + intermediateDirectory;
							}
							
							index = line.Find(wxT("ADD CPP"));
							if (index != wxNOT_FOUND) {
								wxString preprocessor = wxT("");
								for (size_t pos = 0; pos < line.Length(); pos++) {
									if (line.GetChar(pos) == wxT('/') && line.GetChar(pos + 1) == wxT('D')) {
										int count = 0;
										wxString word = wxT("");
										while(count < 2) {
											if (line.GetChar(pos) == wxT('\"')) {
												count++;
											}
											else if (count == 1) {
												word.Append(line.GetChar(pos));
											}
											
											pos++;
										}
										
										preprocessor += word + wxT(";");
									}
								}
								
								genericProjectCfg.preprocessor = preprocessor;
							}
							
							index = line.Find(wxT("ADD LINK32"));
							if (index != wxNOT_FOUND) {
								int begin = index  + 10;
								int end = line.Find(wxT("/")) - 1;
								wxString libraries = line.SubString(begin, end).Trim().Trim(false);
								libraries.Replace(wxT(" "), wxT(";"));
								if (IsGccCompile) 
									libraries.Replace(wxT(".lib"), wxT(""));
								
								genericProjectCfg.libraries = libraries;
								break;
							}
						}
						
						genericProject.cfgs.push_back(genericProjectCfg);
					}
				
					index = line.Find(wxT("Begin Group"));
					if (index != wxNOT_FOUND) {
						virtualPath = line.Mid(index + 11).Trim().Trim(false);
						virtualPath.Replace(wxT("\""), wxT(""));
						virtualPath.Replace(wxT(" "), wxT("_"));
						virtualPath.Replace(wxT("\\"), wxT(":"));
					}
					
					index = line.Find(wxT("SOURCE="));
					if (index != wxNOT_FOUND) {
						wxString filename = line.Mid(index + 7).Trim().Trim(false);
						filename.Replace(wxT("\\"), wxT("/"));
						
						GenericProjectFile genericProjectFile;
						genericProjectFile.name = filename;
						genericProjectFile.vpath = virtualPath;
						
						genericProject.files.push_back(genericProjectFile);
					}
				}
				
				genericWorkspace.projects.push_back(genericProject);
			}
		}
	}
}

void VisualCppImporter::GenerateFromVC7(GenericWorkspace& genericWorkspace) {
	genericWorkspace.name = wsInfo.GetName();
	genericWorkspace.path = wsInfo.GetPath();
	
	while(!fis->Eof()) {
		wxString line = tis->ReadLine();
		
		int index = line.Find(wxT("Project("));
		if (index != wxNOT_FOUND) {
			wxStringTokenizer projectToken(line, wxT("="));
			projectToken.GetNextToken();
			projectToken.SetString(projectToken.GetNextToken(), wxT(","));
			wxString projectName = projectToken.GetNextToken().Trim().Trim(false);
			projectName.Replace(wxT("\""), wxT(""));
			wxString projectFile = projectToken.GetNextToken().Trim().Trim(false);
			projectFile.Replace(wxT("\""), wxT(""));
			
			wxFileName projectInfo(wsInfo.GetPath() + wxFileName::GetPathSeparator() + projectFile);
			
			GenericProject genericProject;
			genericProject.name = projectName;
			genericProject.path = projectInfo.GetPath();
			
			wxXmlDocument projectDoc;
			if (projectDoc.Load(projectInfo.GetFullPath())) {
				wxXmlNode* root = projectDoc.GetRoot();
				
				wxXmlNode* vspChild = root->GetChildren();
				while(vspChild) {
					if(vspChild->GetName() == wxT("Configurations")) {
						wxXmlNode* confChild = vspChild->GetChildren();
						
						while(confChild) {
							if(confChild->GetName() == wxT("Configuration")) {
								wxString projectCfgName = confChild->GetAttribute(wxT("Name"));
								projectCfgName.Replace(wxT("|"), wxT("_"));
								wxString configurationType = confChild->GetAttribute(wxT("ConfigurationType"));
								wxString intermediateDirectory = confChild->GetAttribute(wxT("IntermediateDirectory"));
								
								GenericProjectCfg genericProjectCfg;
								genericProjectCfg.name = projectCfgName;
								
								intermediateDirectory.Replace(wxT(" "), wxT("_"));
								intermediateDirectory.Replace(wxT("\\"), wxT("/"));
								genericProjectCfg.intermediateDirectory = intermediateDirectory;
								
								wxString outputFilename;
								
								if (configurationType == wxT("4")) {
									genericProjectCfg.type = GenericCfgType::STATIC_LIBRARY;
									outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
									outputFilename += STATIC_LIBRARY_EXT;
									if (IsGccCompile) 
										outputFilename.Replace(wxT("lib"), wxT("a"));
								}
								else if (configurationType == wxT("2")) {
									genericProjectCfg.type = GenericCfgType::DYNAMIC_LIBRARY;
									outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
									outputFilename += DYNAMIC_LIBRARY_EXT;
								}
								else {
									genericProjectCfg.type = GenericCfgType::EXECUTABLE;
									outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
									outputFilename += EXECUTABLE_EXT;
								}
								
								genericProjectCfg.outputFilename = outputFilename;
								
								wxXmlNode* toolChild = confChild->GetChildren();
						
								while(toolChild) {
									if(toolChild->GetName() == wxT("Tool")) {
										if (toolChild->GetAttribute(wxT("Name")) == wxT("VCCLCompilerTool")) {
											wxString preprocessorDefinitions = toolChild->GetAttribute(wxT("PreprocessorDefinitions"));
											preprocessorDefinitions.Replace(wxT(","), wxT(";"));
											genericProjectCfg.preprocessor = preprocessorDefinitions;
										}
										
										if (toolChild->GetAttribute(wxT("Name")) == wxT("VCLinkerTool")) {
											wxString additionalDependencies = toolChild->GetAttribute(wxT("AdditionalDependencies"));
											additionalDependencies.Replace(wxT(" "), wxT(";"));
											if (IsGccCompile) 
												additionalDependencies.Replace(wxT(".lib"), wxT(""));
									
											genericProjectCfg.libraries = additionalDependencies;
										}
									}
									
									toolChild = toolChild->GetNext();
								}
								
								
								if (genericProject.cfgType != genericProjectCfg.type)
									genericProject.cfgType = genericProjectCfg.type;
								
								genericProject.cfgs.push_back(genericProjectCfg);
							}
							
							confChild = confChild->GetNext();
						}
					}
					
					if(vspChild->GetName() == wxT("Files")) {
						wxXmlNode* filterChild = vspChild->GetChildren();
						
						while(filterChild) {
							if(filterChild->GetName() == wxT("Filter")) {
								wxString virtualPath = filterChild->GetAttribute(wxT("Name"));
								virtualPath.Replace(wxT("\\"), wxT(":"));
								
								wxXmlNode* fileChild = filterChild->GetChildren();
								
								while(fileChild) {
									if(fileChild->GetName() == wxT("File")) {
										wxString filename = fileChild->GetAttribute(wxT("RelativePath"));
										filename.Replace(wxT("\\"), wxT("/"));
										
										GenericProjectFile genericProjectFile;
										genericProjectFile.name = filename;
										genericProjectFile.vpath = virtualPath;
	
										genericProject.files.push_back(genericProjectFile);
									}
									
									fileChild = fileChild->GetNext();
								}
							}
							
							filterChild = filterChild->GetNext();
						}
					}
					
					vspChild = vspChild->GetNext();
				}
			}
			
			genericWorkspace.projects.push_back(genericProject);
		}
	}
}


void VisualCppImporter::GenerateFromVC11(GenericWorkspace& genericWorkspace) {
	genericWorkspace.name = wsInfo.GetName();
	genericWorkspace.path = wsInfo.GetPath();
	
	while(!fis->Eof()) {
		wxString line = tis->ReadLine();
		
		int index = line.Find(wxT("Project("));
		if (index != wxNOT_FOUND) {
			wxStringTokenizer projectToken(line, wxT("="));
			projectToken.GetNextToken();
			projectToken.SetString(projectToken.GetNextToken(), wxT(","));
			wxString projectName = projectToken.GetNextToken().Trim().Trim(false);
			projectName.Replace(wxT("\""), wxT(""));
			wxString projectFile = projectToken.GetNextToken().Trim().Trim(false);
			projectFile.Replace(wxT("\""), wxT(""));
			
			wxFileName projectInfo(wsInfo.GetPath() + wxFileName::GetPathSeparator() + projectFile);
			
			GenericProject genericProject;
			genericProject.name = projectName;
			genericProject.path = projectInfo.GetPath();
			
			wxXmlDocument projectDoc;
			if (projectDoc.Load(projectInfo.GetFullPath())) {
				wxXmlNode* root = projectDoc.GetRoot();
				wxXmlNode* projectChild = root->GetChildren();
				
				while(projectChild) {
					if (projectChild->GetName() == wxT("ItemDefinitionGroup")) {
						wxString projectCfgName = projectChild->GetAttribute("Condition");
						projectCfgName.Replace(wxT("'$(Configuration)|$(Platform)'=='"), wxT(""));
						projectCfgName.Replace(wxT("'"), wxT(""));
						projectCfgName.Replace(wxT("|"), wxT("_"));
						
						GenericProjectCfg genericProjectCfg;
						genericProjectCfg.name = projectCfgName;
						
						wxXmlNode* itemDefinitionGroupChild = projectChild->GetChildren();
						
						while(itemDefinitionGroupChild) {
							if (itemDefinitionGroupChild->GetName() == wxT("ClCompile")) { 
								wxXmlNode* clcompileChild = itemDefinitionGroupChild->GetChildren();
								
								while(clcompileChild) {
									if (clcompileChild->GetName() == wxT("PreprocessorDefinitions")) {
										wxString preprocessorDefinitions = clcompileChild->GetNodeContent();
										preprocessorDefinitions.Replace(wxT("%(PreprocessorDefinitions)"), wxT(""));
										genericProjectCfg.preprocessor = preprocessorDefinitions;
									}
									
									if (clcompileChild->GetName() == wxT("AdditionalIncludeDirectories")) {
										wxString additionalIncludeDirectories = clcompileChild->GetNodeContent();
										additionalIncludeDirectories.Replace(wxT("%(AdditionalIncludeDirectories)"), wxT(""));
										additionalIncludeDirectories.Replace(wxT("\\"), wxT("/"));
										genericProjectCfg.includePath = additionalIncludeDirectories;
									}
									
									clcompileChild = clcompileChild->GetNext();
								}
							}
							
							if (itemDefinitionGroupChild->GetName() == wxT("Link")) { 
								wxXmlNode* linkChild = itemDefinitionGroupChild->GetChildren();
								
								while(linkChild) {
									if (linkChild->GetName() == wxT("AdditionalDependencies")) {
										wxString additionalDependencies = linkChild->GetNodeContent();
										additionalDependencies.Replace(wxT("%(AdditionalDependencies)"), wxT(""));
										if (IsGccCompile) 
											additionalDependencies.Replace(wxT(".lib"), wxT(""));
											
										genericProjectCfg.libraries = additionalDependencies;
									}
									
									linkChild = linkChild->GetNext();
								}
							}
							
							itemDefinitionGroupChild = itemDefinitionGroupChild->GetNext();
						}
						
						if (genericProject.cfgType != genericProjectCfg.type)
							genericProject.cfgType = genericProjectCfg.type;
									
						genericProject.cfgs.push_back(genericProjectCfg);
					}
					
					projectChild = projectChild->GetNext();
				}
			}
			
			wxXmlDocument filterDoc;
			if (filterDoc.Load(projectInfo.GetFullPath() + wxT(".filters"))) {
				wxXmlNode* root = filterDoc.GetRoot();
				wxXmlNode* itemGroupChild = root->GetChildren();
				
				while(itemGroupChild) {
					wxXmlNode* itemChild = itemGroupChild->GetChildren();
				
					while(itemChild) {
						if (itemChild->GetName() == wxT("ClInclude") ||
							itemChild->GetName() == wxT("ClCompile") || 
							itemChild->GetName() == wxT("None") ||
							itemChild->GetName() == wxT("Text") ||
							itemChild->GetName() == wxT("ResourceCompile")) {
							wxString virtualPath = "";
							wxString filename = itemChild->GetAttribute("Include");
							filename.Replace(wxT("\\"), wxT("/"));
							
							wxXmlNode* filterChild = itemChild->GetChildren();
							if (filterChild) {
								if (filterChild->GetName() == wxT("Filter")) {
									wxString content = filterChild->GetNodeContent();
									if (!content.IsEmpty()) {
										content.Replace(wxT(" "), wxT("_"));
										content.Replace(wxT("\\"), wxT(":"));
										virtualPath = content;
									}
								}
							}
							
							GenericProjectFile genericProjectFile;
							genericProjectFile.name = filename;
							genericProjectFile.vpath = virtualPath;

							genericProject.files.push_back(genericProjectFile);
						}
						
						itemChild = itemChild->GetNext();
					}
					
					itemGroupChild = itemGroupChild->GetNext();
				}
			}
			
			genericWorkspace.projects.push_back(genericProject);
		}
	}
}