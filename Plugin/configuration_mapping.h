#ifndef CONFIGURATION_MAPPING_H
#define CONFIGURATION_MAPPING_H

#include "wx/string.h"
#include "wx/xml/xml.h"
#include "list"
#include "smart_ptr.h"

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif 

class WXDLLIMPEXP_LE_SDK ConfigMappingEntry {
public:
	wxString m_project;
	wxString m_name;
	
public:
	ConfigMappingEntry(const wxString &project, const wxString &name)
		: m_project(project), m_name(name) 
	{}

	ConfigMappingEntry()
		: m_project(wxEmptyString), m_name(wxEmptyString)
	{}

	virtual ~ConfigMappingEntry()
	{}
};

class WXDLLIMPEXP_LE_SDK WorkspaceConfiguration {
public:
	typedef std::list<ConfigMappingEntry> ConfigMappingList;

private:
	wxString m_name;
	ConfigMappingList m_mappingList;
	bool m_isSelected;

public:
	WorkspaceConfiguration();
	WorkspaceConfiguration(wxXmlNode *node);
	WorkspaceConfiguration(const wxString &name, bool selected);
	virtual ~WorkspaceConfiguration();
	wxXmlNode *ToXml() const;
	
	void SetSelected(bool selected) { m_isSelected = selected; }
	bool IsSelected() const { return m_isSelected; }
	const wxString &GetName() const{ return m_name; }
	const ConfigMappingList &GetMapping() const { return m_mappingList; }
	void SetConfigMappingList(const ConfigMappingList &mapList) { m_mappingList = mapList; }
	void SetName(const wxString &name){ m_name = name; }
};

typedef SmartPtr<WorkspaceConfiguration> WorkspaceConfigurationPtr;

class WXDLLIMPEXP_LE_SDK BuildMatrix {
	std::list<WorkspaceConfigurationPtr> m_configurationList;

protected:
	WorkspaceConfigurationPtr FindConfiguration(const wxString &name) const;

public:
	BuildMatrix(wxXmlNode *node);
	virtual ~BuildMatrix();
	wxXmlNode *ToXml() const;
	const std::list<WorkspaceConfigurationPtr>& GetConfigurations() const { return m_configurationList; };
	void RemoveConfiguration(const wxString &configName);
	void SetConfiguration(WorkspaceConfigurationPtr conf);
	wxString GetProjectSelectedConf(const wxString &configName, const wxString &project) const;
	wxString GetSelectedConfigurationName() const;
	void SetSelectedConfigurationName(const wxString &name);
	WorkspaceConfigurationPtr GetConfigurationByName(const wxString &name) const;
};

typedef SmartPtr<BuildMatrix> BuildMatrixPtr;

#endif // CONFIGURATION_MAPPING_H
